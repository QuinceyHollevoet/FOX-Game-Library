#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QThread>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QStringList args = app.arguments();
    QString currentExe = QCoreApplication::applicationFilePath();
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

#ifdef Q_OS_WIN
    QString tempExeName = "fox_uninstaller_temp.exe";
#else
    QString tempExeName = "fox_uninstaller_temp";
#endif

    // =========================================================================
    // PHASE 1: Copy uninstaller to OS temp folder and run from there
    // =========================================================================
    if (!args.contains("--run-from-temp")) {
        auto reply = QMessageBox::warning(
            nullptr,
            "Uninstall FOX Game Library",
            "Are you sure you want to completely uninstall FOX Game Library and remove its files?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

        if (reply != QMessageBox::Yes) {
            return 0;
        }

        // Determine the root folder to delete
        QDir targetDir(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MAC
        // On macOS, executable is inside "FOX Game Library.app/Contents/MacOS"
        if (targetDir.dirName() == "MacOS") {
            targetDir.cdUp(); // Contents
            targetDir.cdUp(); // FOX Game Library.app
        }
#endif

        QString tempExe = QDir(tempDir).filePath(tempExeName);
        QFile::remove(tempExe);

        if (QFile::copy(currentExe, tempExe)) {
            QFile::setPermissions(tempExe, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
            QProcess::startDetached(tempExe, {"--run-from-temp", targetDir.absolutePath()});
        } else {
            QMessageBox::critical(nullptr, "Error", "Failed to prepare uninstaller in temp directory.");
        }
        return 0;
    }

    // =========================================================================
    // PHASE 2: Delete the application folder & self-delete from temp
    // =========================================================================
    int idx = args.indexOf("--run-from-temp") + 1;
    if (idx >= args.size()) return 1;

    QString targetPath = args.at(idx);
    QDir targetDir(targetPath);

    // Safety check to prevent deleting root directories
    if (targetPath.isEmpty() || targetDir.isRoot() || targetPath.length() < 5) {
        return 1;
    }

    // Wait 1.5 seconds for the main app to close completely
    QThread::msleep(1500);

    std::error_code ec;
    fs::remove_all(fs::path(targetPath.toStdWString()), ec);

    if (!ec) {
        QMessageBox::information(nullptr, "Uninstalled", "FOX Game Library has been completely removed.");
    } else {
        QMessageBox::warning(
            nullptr,
            "Permission Error",
            "Some files could not be removed. Try running the uninstaller with administrator/sudo privileges."
        );
    }

    // Clean up the temporary uninstaller binary
#ifdef Q_OS_WIN
    QString nativeExe = QDir::toNativeSeparators(currentExe);
    QProcess::startDetached("cmd.exe", {"/c", "ping 127.0.0.1 -n 2 > nul & del /f /q \"" + nativeExe + "\""});
#else
    QProcess::startDetached("/bin/sh", {"-c", "sleep 2 && rm -f \"" + currentExe + "\""});
#endif

    return 0;
}
