#include <QApplication>
#include <QMessageBox>
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QThread>
#include <QEventLoop>

// Replace YOUR_USERNAME and YOUR_REPO with your actual GitHub details
const QString GITHUB_API_URL = "https://github.com/QuinceyHollevoet/FOX-Game-Library";

#ifndef APP_VERSION
const QString APP_VERSION = "1.0.0";
#endif

bool extractArchive(const QString &archivePath, const QString &destDir) {
    QProcess extractor;
#ifdef Q_OS_WIN
    extractor.start("tar", {"-xf", archivePath, "-C", destDir});
#elif defined(Q_OS_MAC)
    extractor.start("ditto", {"-xk", archivePath, destDir});
#else
    extractor.start("unzip", {"-o", archivePath, "-d", destDir});
#endif
    extractor.waitForFinished(-1);
    return (extractor.exitCode() == 0);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QStringList args = app.arguments();
    QString currentExe = QCoreApplication::applicationFilePath();
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

#ifdef Q_OS_WIN
    QString tempExeName = "fox_updater_temp.exe";
#else
    QString tempExeName = "fox_updater_temp";
#endif

    // =========================================================================
    // PHASE 1: Copy Updater to %TEMP% / /tmp and re-launch from there
    // =========================================================================
    if (!args.contains("--run-from-temp")) {
        QDir targetDir(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MAC
        if (targetDir.dirName() == "MacOS") {
            targetDir.cdUp(); // Contents
            targetDir.cdUp(); // FOX Game Library.app
            targetDir.cdUp(); // Folder containing .app
        }
#endif

        QString tempExe = QDir(tempDir).filePath(tempExeName);
        QFile::remove(tempExe);

        if (QFile::copy(currentExe, tempExe)) {
            QFile::setPermissions(tempExe, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);

            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef Q_OS_LINUX
            QString appDir = QCoreApplication::applicationDirPath();
            env.insert("LD_LIBRARY_PATH", appDir + ":" + appDir + "/lib:" + env.value("LD_LIBRARY_PATH"));
#endif
            QProcess proc;
            proc.setProgram(tempExe);
            proc.setArguments({"--run-from-temp", targetDir.absolutePath()});
            proc.setProcessEnvironment(env);
            proc.startDetached();
        }
        return 0;
    }

    // =========================================================================
    // PHASE 2: Running from Temp -> Query GitHub Releases API, download, install
    // =========================================================================
    int idx = args.indexOf("--run-from-temp") + 1;
    if (idx >= args.size()) return 1;
    QString targetDir = args.at(idx);

    QNetworkAccessManager net;

    // 1. Ask GitHub API for the latest release (GitHub requires a User-Agent header)
    QNetworkRequest apiRequest{QUrl(GITHUB_API_URL)};
    apiRequest.setRawHeader("User-Agent", "FoxGameLibrary-Updater");
    apiRequest.setRawHeader("Accept", "application/vnd.github+json");

    QNetworkReply *metaReply = net.get(apiRequest);
    QEventLoop loop;
    QObject::connect(metaReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (metaReply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(nullptr, "Update Check Failed", "Could not reach GitHub:\n" + metaReply->errorString());
        return 1;
    }

    QJsonObject json = QJsonDocument::fromJson(metaReply->readAll()).object();
    metaReply->deleteLater();

    // Strip leading 'v' from tag (e.g., "v1.0.1" -> "1.0.1")
    QString latestVersion = json["tag_name"].toString();
    if (latestVersion.startsWith('v', Qt::CaseInsensitive)) {
        latestVersion.remove(0, 1);
    }

    if (latestVersion.isEmpty() || latestVersion == APP_VERSION) {
        QMessageBox::information(nullptr, "Up to Date", "FOX Game Library is already up to date (v" + QString(APP_VERSION) + ").");
        return 0;
    }

    // 2. Find the matching OS .zip file inside GitHub's "assets" list
#ifdef Q_OS_WIN
    QString targetKeyword = "windows";
#elif defined(Q_OS_MAC)
    QString targetKeyword = "mac";
#else
    QString targetKeyword = "linux";
#endif

    QString downloadUrl;
    QJsonArray assets = json["assets"].toArray();
    for (const QJsonValue &val : assets) {
        QJsonObject asset = val.toObject();
        QString fileName = asset["name"].toString().toLower();
        if (fileName.contains(targetKeyword) && fileName.endsWith(".zip")) {
            downloadUrl = asset["browser_download_url"].toString();
            break;
        }
    }

    if (downloadUrl.isEmpty()) {
        QMessageBox::warning(nullptr, "Update Error", "No release archive found for your operating system (" + targetKeyword + ").");
        return 1;
    }

    auto reply = QMessageBox::question(
        nullptr,
        "Update Available",
        "Version " + latestVersion + " is available (current: " + QString(APP_VERSION) + ").\n\nDownload and install now?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes) return 0;

    // 3. Download the .zip archive (enabling redirects for GitHub CDN)
    QProgressDialog progress("Downloading update v" + latestVersion + "...", "Cancel", 0, 100);
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    QNetworkRequest zipRequest{QUrl(downloadUrl)};
    zipRequest.setRawHeader("User-Agent", "FoxGameLibrary-Updater");
    zipRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply *zipReply = net.get(zipRequest);
    QObject::connect(zipReply, &QNetworkReply::downloadProgress, [&](qint64 received, qint64 total) {
        if (total > 0) {
            progress.setMaximum(static_cast<int>(total));
            progress.setValue(static_cast<int>(received));
        }
    });
    QObject::connect(&progress, &QProgressDialog::canceled, zipReply, &QNetworkReply::abort);
    QObject::connect(zipReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (zipReply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(nullptr, "Download Failed", zipReply->errorString());
        return 1;
    }

    QString zipPath = QDir(tempDir).filePath("fox_update.zip");
    QFile zipFile(zipPath);
    if (!zipFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, "Error", "Failed to save update archive.");
        return 1;
    }
    zipFile.write(zipReply->readAll());
    zipFile.close();
    zipReply->deleteLater();

    // 4. Wait 1.5s to ensure main app is closed, then extract over targetDir
    QThread::msleep(1500);

    if (!extractArchive(zipPath, targetDir)) {
        QMessageBox::critical(nullptr, "Install Failed", "Failed to extract update files. Check folder permissions.");
        return 1;
    }

    QFile::remove(zipPath);
    QMessageBox::information(nullptr, "Update Complete", "FOX Game Library has been updated to v" + latestVersion + "!");

    // 5. Relaunch the updated FOX Game Library
#ifdef Q_OS_WIN
    QString mainExe = QDir(targetDir).filePath("FOX Game Library.exe");
    QProcess::startDetached(mainExe, {});
    QString nativeSelf = QDir::toNativeSeparators(currentExe);
    QProcess::startDetached("cmd.exe", {"/c", "ping 127.0.0.1 -n 2 > nul & del /f /q \"" + nativeSelf + "\""});
#elif defined(Q_OS_MAC)
    QString appBundle = QDir(targetDir).filePath("FOX Game Library.app");
    QProcess::startDetached("open", {appBundle});
    QProcess::startDetached("/bin/sh", {"-c", "sleep 2 && rm -f \"" + currentExe + "\""});
#else
    QString mainExe = QDir(targetDir).filePath("FOX Game Library");
    QProcess::startDetached(mainExe, {});
    QProcess::startDetached("/bin/sh", {"-c", "sleep 2 && rm -f \"" + currentExe + "\""});
#endif

    return 0;
}
