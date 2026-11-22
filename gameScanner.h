#pragma once

#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QStringList>
#include <QFile>
#include <QSet>
#include <QRegularExpression>

struct GameData {
    QFileInfo fileInfo;
    QString   consoleType;
    QString   coverImagePath;
};

Q_DECLARE_METATYPE(GameData)

class GameScanner : public QObject {
    Q_OBJECT

public:
    explicit GameScanner(const QString& savedPath, const QString& globalCoversPath, QObject *parent = nullptr) 
        : QObject(parent), m_savedPath(savedPath), m_globalCoversPath(globalCoversPath) {}

signals:
    void gameFound(const GameData &game);
    void finished();

public slots:
    void startScan() {
        QDir gamesDir(m_savedPath);
        
        QStringList filters;
        filters << "*.chd"  << "*.bin"  << "*.pbp"  << "*.iso" << "*.pkg"    << "*.rar"
                << "*.cso"  << "*.vpk"  << "*.nes"  << "*.unf" << "*.sfc"    << "*.smc"
                << "*.z64"  << "*.n64"  << "*.v64"  << "*.gcm" << "*.gcz"    << "*.wbfs"
                << "*.wad"  << "*.wua"  << "*.rpx"  << "*.wud" << "*.gbc"    << "*.gb"
                << "*.gba"  << "*.nds"  << "*.dsi"  << "*.3ds" << "*.cia"    << "*.cci"
                << "*.xci"  << "*.nsp"  << "*.xex"  << "*.god" << "*.xvc"    << "*.xiso"
                << "*.stfs" << "*.xpkg" << "*.appx" << "*.zip" << "*.tar.gz" << "*.tgz"
                << "*.7z";

        QSet<QString> availableCovers;
        if (!m_globalCoversPath.isEmpty()) {
            QDir coversDir(m_globalCoversPath);
            QStringList existingFiles = coversDir.entryList(QStringList() << "*.png" << "*.jpg" << "*.jpeg", QDir::Files);
            for (const QString& fName : existingFiles) {
                availableCovers.insert(fName.toLower());
            }
        }

        QDirIterator it(gamesDir.absolutePath(), filters, QDir::Files, QDirIterator::Subdirectories);
        
        const qint64 KB = 1024;
        const qint64 MB = 1024 * KB;

        QString coversDir = QDir::cleanPath(m_globalCoversPath) + "/";
        QRegularExpression vitaTitleIdRegex("pcs[abdeghjnkmtu]\\d{5}");

        while (it.hasNext()) {
            it.next();
            QFileInfo fileInfo = it.fileInfo();
            
            if (fileInfo.isDir()) continue;

            QString fileName = fileInfo.fileName().toLower();
            QString extension = fileInfo.suffix().toLower();
            QString absolutePath = fileInfo.absolutePath().toLower();
            qint64 fileSize = fileInfo.size();

            if (extension == "cue" || extension == "txt" || extension == "rap") continue; 

            bool isPspEboot = (fileName == "eboot.pbp");
            bool isVitaEboot = (fileName == "eboot.bin");

            if (fileName.contains("eboot") && !isPspEboot && !isVitaEboot) continue;
            
            if (fileName.contains("track") || fileName.contains("bios") || fileName.contains("gamedata")) continue;

            if (fileName.startsWith("._") || fileName == "desktop.ini") continue;
            
            if (absolutePath.contains("/bios")    || absolutePath.contains("/rap")     || 
                absolutePath.contains("/updates") || absolutePath.contains("/add-ons") || absolutePath.contains("dlc")) continue;

            QString consoleType = "";
            qint64 minSize = 0;

            if (absolutePath.contains("/playstation portable") || absolutePath.contains("/psp") || extension == "pbp") {
                if ((absolutePath.contains("/psp") || absolutePath.contains("/playstation portable")) && extension == "bin") {
                    continue; 
                }
                consoleType = "PSP"; 
                minSize = 1 * MB;
            }
            else if (vitaTitleIdRegex.match(absolutePath).hasMatch() || absolutePath.contains("/playstation vita") || absolutePath.contains("/psvita") || absolutePath.contains("/psv")) {
                 if (vitaTitleIdRegex.match(absolutePath).hasMatch() && !isVitaEboot && extension != "vpk" && extension != "pkg") {
                    continue; 
                }
                consoleType = "PSV"; 
                minSize = 1 * MB;
            } 
            else if (absolutePath.contains("/playstation 1") || absolutePath.contains("/ps1")) {
                consoleType = "PS1"; minSize = 5 * MB; 
            } else if (absolutePath.contains("/playstation 2") || absolutePath.contains("/ps2")) {
                consoleType = "PS2"; minSize = 10 * MB;
            } else if (absolutePath.contains("/playstation 3") || absolutePath.contains("/ps3")) {
                consoleType = "PS3"; minSize = 20 * MB;
            } else if (absolutePath.contains("/playstation 4") || absolutePath.contains("/ps4")) {
                consoleType = "PS4"; minSize = 50 * MB;
            } else if (absolutePath.contains("/playstation 5") || absolutePath.contains("/ps5")) {
                consoleType = "PS5"; minSize = 100 * MB;
            } else if (absolutePath.contains("/xbox 360")) {
                consoleType = "XBOX 360"; minSize = 50 * MB;
            } else if (absolutePath.contains("/xbox")) {
                consoleType = "XBOX"; minSize = 10 * MB;
            } else if (absolutePath.contains("/gameboy advanced") || absolutePath.contains("/gba")) {
                consoleType = "GBA"; minSize = 64 * KB;
            } else if (absolutePath.contains("/nintendo 3ds") || absolutePath.contains("/n3ds")) {
                consoleType = "N3DS"; minSize = 4 * MB;
            } else if (absolutePath.contains("/nintendo 64") || absolutePath.contains("/n64")) {
                consoleType = "N64"; minSize = 1 * MB;
            } else if (absolutePath.contains("/nintendo ds") || absolutePath.contains("/nds")) {
                consoleType = "NDS"; minSize = 512 * KB;
            } else if (absolutePath.contains("/nintendo entertainment system") || absolutePath.contains("/nes")) {
                consoleType = "NES"; minSize = 8 * KB;
            } else if (absolutePath.contains("/super nintendo entertainment system") || absolutePath.contains("/snes")) {
                consoleType = "SNES"; minSize = 200 * KB;
            } else if (absolutePath.contains("/nintendo switch") || absolutePath.contains("/ns") || absolutePath.contains("/ns1")) {
                consoleType = "NS"; minSize = 5 * MB;
            } else if (absolutePath.contains("/wiiu")) {
                consoleType = "WiiU"; minSize = 10 * MB;
            } else if (absolutePath.contains("/wii")) {
                consoleType = "Wii"; minSize = 10 * MB;
            } else {
                continue;
            }

            if (fileSize < minSize) continue; 

            // COVER RESOLUTION BLOCK
            QString baseName = fileInfo.baseName().toLower();
            
            // If the file is literally called "eboot", pull the name of the folder it is inside
            if (baseName == "eboot") {
                QDir parentDir(fileInfo.absolutePath());
                baseName = parentDir.dirName().toLower();
                
                // If the parent folder is a generic system folder like "game", step up one more layer
                if (baseName == "game" || baseName == "app") {
                    QDir grandParentDir = parentDir;
                    if (grandParentDir.cdUp()) {
                        baseName = grandParentDir.dirName().toLower();
                    }
                }
            }

            QString coverPath = "";
            if (availableCovers.contains(baseName + ".png")) {
                coverPath = QDir::cleanPath(coversDir + baseName + ".png");
            } else if (availableCovers.contains(baseName + ".jpg")) {
                coverPath = QDir::cleanPath(coversDir + baseName + ".jpg");
            } else if (availableCovers.contains(baseName + ".jpeg")) {
                coverPath = QDir::cleanPath(coversDir + baseName + ".jpeg");
            }

            emit gameFound({fileInfo, consoleType, coverPath});
        }
        emit finished();
    }

private:
    QString m_savedPath;
    QString m_globalCoversPath;
};
