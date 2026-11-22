#include "mainWindow.h"
#include "toggleTheme.h"
#include "messageBox.h"

#include <QPixmapCache>
#include <QDebug>
#include <QFileInfo>
#include <QThread>
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDir>
#include <QDirIterator>
#include <QFileInfoList>
#include <QFileDialog>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QMouseEvent>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QStackedWidget>
#include <QListWidget>
#include <QWidget>
#include <QCheckBox>
#include <QScrollBar>
#include <QPainterPath>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#endif

mainWindow::mainWindow(QWidget *parent) : QMainWindow(parent) {
    qRegisterMetaType<GameData>("GameData");

    this->setObjectName("myMainWindow"); 

    setWindowTitle("FOX Game Library V.1.0.0");
    setWindowIcon(QIcon(""));
    resize(800, 600);

    menubar();

    this->styleSheetDark(true);

    QWidget *rootContainer = new QWidget(this);
    QHBoxLayout *rootLayout = new QHBoxLayout(rootContainer);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QWidget *sidebarContainer = new QWidget(rootContainer);
    sidebarContainer->setObjectName("panel");
    sidebarContainer->setFixedWidth(100);

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebarContainer);
    sidebarLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    sidebarLayout->setContentsMargins(10, 10, 10, 10);
    sidebarLayout->setSpacing(10);

    QPushButton *home        = new QPushButton(sidebarContainer);
    QPushButton *playstation = new QPushButton(sidebarContainer);
    QPushButton *xbox        = new QPushButton(sidebarContainer);
    QPushButton *nintendo    = new QPushButton(sidebarContainer);

    home        ->setIcon(QIcon(":/assets/home.svg"));
    playstation ->setIcon(QIcon(":/assets/sony.svg"));
    xbox        ->setIcon(QIcon(":/assets/xbox.svg"));
    nintendo    ->setIcon(QIcon(":/assets/nintendo-switch.svg"));

    QSize iconSize(32, 32);

    home        ->setIconSize(iconSize);
    playstation ->setIconSize(iconSize);
    xbox        ->setIconSize(iconSize);
    nintendo    ->setIconSize(iconSize);

    sidebarLayout->addWidget(home);
    sidebarLayout->addWidget(playstation);
    sidebarLayout->addWidget(xbox);
    sidebarLayout->addWidget(nintendo);

    QStackedWidget *tabs = new QStackedWidget(rootContainer);
    tabs->setObjectName("tabs");

    tabs->addWidget(homePage());
    tabs->addWidget(playstationPage());
    tabs->addWidget(xboxPage());
    tabs->addWidget(nintendoPage());

    connect(home,        &QPushButton::clicked, this, [this, tabs]() { tabs->setCurrentIndex(0); });
    connect(playstation, &QPushButton::clicked, this, [this, tabs]() { tabs->setCurrentIndex(1); });
    connect(xbox,        &QPushButton::clicked, this, [this, tabs]() { tabs->setCurrentIndex(2); });
    connect(nintendo,    &QPushButton::clicked, this, [this, tabs]() { tabs->setCurrentIndex(3); });

    rootLayout->addWidget(sidebarContainer);
    rootLayout->addWidget(tabs, 1);

    this->setCentralWidget(rootContainer);

        QString savedPath = getDirFromJson("gameDir");
    if (!savedPath.isEmpty()) {
        this->startGameScanner();
    } else {
        MsgBox::show(this, MsgBox::Info, "Setup Directory", "Welcome! Go to System -> Game Directory to select your game folder.");
    }
}

void mainWindow::styleSheetDark(bool isDarkMode) {
    if (!isDarkMode) return;
    this->setStyleSheet(R"(
        #myMainWindow {
            background-color: #000000;
        }
        #panel {
            background-color: #121212;
            border-right: 1px solid #2d2d2d;
        }
        QPushButton {
            background-color: transparent;
            border: none;
            padding: 8px;
            border-radius: 6px;
            color: #ffffff;
        }
        QPushButton:hover {
            background-color: #252525;
        }
        QPushButton:pressed {
            background-color: #333333;
        }
            QDialogButtonBox {
    /* Layout style 5 forces buttons to center perfectly regardless of the OS */
    button-layout: 5; 
}

QMessageBox QPushButton {
    min-width: 80px;
    padding: 6px 16px;
    margin: 0 5px; /* Adds even spacing on both sides of the buttons */
}
    /* Add these lines to your theme setups so card styling is parsed exactly ONCE */
QPushButton#gameCardWidget {
    background-color: #1e1e1e; 
    border: 1px solid #2d2d2d; 
    border-radius: 10px;
}
QPushButton#gameCardWidget:hover {
    border-color: #007acc; 
}
QLabel#noImagePlaceholder {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2a2a2a, stop:1 #1e1e1e);
    color: #555555;
    font-weight: bold;
    font-size: 12px;
    border: 1px dashed #3a3a3a;
    border-radius: 10px;
}
QLabel#gameCardTitle {
    color: white; 
    font-size: 13px; 
    font-weight: bold; 
    background-color: rgba(0, 0, 0, 160); 
    padding: 6px; 
    border-radius: 4px;
}
QLabel[console="PS"]   { color: #5c9eff; font-size: 11px; font-weight: bold; background: transparent; }
QLabel[console="XBOX"] { color: #5cd65c; font-size: 11px; font-weight: bold; background: transparent; }
QLabel[console="NIN"]  { color: #ff5c5c; font-size: 11px; font-weight: bold; background: transparent; }

QLabel#pageTitle {
    font-size: 20px;
    font-weight: bold;
    color: white;
}

/* ScrollArea outer frame + internal viewport + content container */
QScrollArea#libraryScrollArea,
QScrollArea#libraryScrollArea > QWidget > QWidget#libraryScrollContent,
QScrollArea#libraryScrollArea QWidget#qt_scrollarea_viewport {
    border: none;
    background: transparent;
}
    )");
}

void mainWindow::styleSheetLight(bool isLightMode) {
    if (isLightMode) return; 
    this->setStyleSheet(R"(
        #myMainWindow {
            background-color: #000000;
        }
        #panel {
            background-color: #121212;
            border-right: 1px solid #e0e0e0;
        }
        QPushButton {
            background-color: transparent;
            border: none;
            padding: 8px;
            border-radius: 6px;
            color: #000000;
        }
        QPushButton:hover {
            background-color: #e5e5e5;
        }
        QPushButton:pressed {
            background-color: #d5d5d5;
        }
        QDialogButtonBox {
            button-layout: 5; 
        }

        QMessageBox QPushButton {
            min-width: 80px;
            padding: 6px 16px;
            margin: 0 5px;
        }
        QPushButton#gameCardWidget {
            background-color: #1e1e1e; 
            border: 1px solid #2d2d2d; 
            border-radius: 10px;
        }
        QPushButton#gameCardWidget:hover {
            border-color: #007acc; 
        }
        QLabel#noImagePlaceholder {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2a2a2a, stop:1 #1e1e1e);
            color: #555555;
            font-weight: bold;
            font-size: 12px;
            border: 1px dashed #3a3a3a;
            border-radius: 10px;
        }
        QLabel#gameCardTitle {
            color: white; 
            font-size: 13px; 
            font-weight: bold; 
            background-color: rgba(0, 0, 0, 160); 
            padding: 6px; 
            border-radius: 4px;
        }
            QLabel[console="PS"]   { color: #5c9eff; font-size: 11px; font-weight: bold; background: transparent; }
            QLabel[console="XBOX"] { color: #5cd65c; font-size: 11px; font-weight: bold; background: transparent; }
            QLabel[console="NIN"]  { color: #ff5c5c; font-size: 11px; font-weight: bold; background: transparent; }
    QLabel#pageTitle {
    font-size: 20px;
    font-weight: bold;
    color: white;
}

/* ScrollArea outer frame + internal viewport + content container */
QScrollArea#libraryScrollArea,
QScrollArea#libraryScrollArea > QWidget > QWidget#libraryScrollContent,
QScrollArea#libraryScrollArea QWidget#qt_scrollarea_viewport {
    border: none;
    background: transparent;
}
    )");
}

bool mainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
        
        SwitchButton* toggleThemeSwitch = watched->findChild<SwitchButton*>("themeSwitch");
        if (toggleThemeSwitch) {
            toggleThemeSwitch->setChecked(!toggleThemeSwitch->isChecked());
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void mainWindow::menubar() {
    QMenuBar *menuBar = new QMenuBar(this);
    this->setMenuBar(menuBar);

    QMenu *system = new QMenu(tr("System"), this);
    QMenu *help   = new QMenu(tr("Help"),   this);

    help  ->setIcon(QIcon(""));

    QAction *gameDirectory = new QAction(tr("Game Directory"), this);
    QAction *boxArt        = new QAction(tr("Box Art"),        this);
    QAction *boxArtHelp    = new QAction(tr("Box Art Guide"),  this);
    QAction *reload        = new QAction(tr("reload"),         this);
    QAction *update        = new QAction(tr("Update"),         this);
    QAction *uninstall     = new QAction(tr("Uninstall"),      this);

    reload->setIcon(QIcon(""));
    update->setIcon(QIcon(""));

    gameDirectory->setObjectName("gameDir");
    boxArt       ->setObjectName("coverArtDir");
    boxArtHelp   ->setObjectName("boxArtHelp");
    update       ->setObjectName("updateBtn");
    uninstall    ->setObjectName("uninstallBtn");

    connect(gameDirectory, &QAction::triggered, this, &mainWindow::dirButton);
    connect(boxArt,        &QAction::triggered, this, &mainWindow::dirButton);
    connect(boxArtHelp,    &QAction::triggered, this, [this]() { MsgBox::show(this, MsgBox::Info, "Help", "Put your images inside the Box Art folder."); });
    connect(reload,        &QAction::triggered, this, &mainWindow::reload);
    connect(update, &QAction::triggered, this, [this]() {
        #ifdef Q_OS_WIN
            QString updaterName = "Updater.exe";
        #else
            QString updaterName = "Updater";
        #endif

            QString updaterPath = QDir(QCoreApplication::applicationDirPath()).filePath(updaterName);

            if (!QFileInfo::exists(updaterPath)) {
                MsgBox::show(this, MsgBox::Error, "Error", "Updater executable was not found.");
                return;
            }

        #ifdef Q_OS_WIN
            std::wstring wPath = QDir::toNativeSeparators(updaterPath).toStdWString();
            HINSTANCE res = ShellExecuteW(nullptr, L"runas", wPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            if (reinterpret_cast<intptr_t>(res) > 32) {
                QCoreApplication::quit();
            }
        #else
            if (QProcess::startDetached(updaterPath, {})) {
                QCoreApplication::quit();
            } else {
                MsgBox::show(this, MsgBox::Error, "Error", "Failed to launch updater.");
            }
        #endif
        });
    connect(uninstall, &QAction::triggered, this, [this]() {
        #ifdef Q_OS_WIN
                QString uninstallerName = "Uninstaller.exe";
        #else
                QString uninstallerName = "Uninstaller";
        #endif

                QString uninstallerPath = QDir(QCoreApplication::applicationDirPath()).filePath(uninstallerName);

                if (!QFileInfo::exists(uninstallerPath)) {
                    MsgBox::show(this, MsgBox::Error, "Error", "Uninstaller executable was not found.");
                    return;
                }

        #ifdef Q_OS_WIN
                std::wstring wPath = QDir::toNativeSeparators(uninstallerPath).toStdWString();
                HINSTANCE res = ShellExecuteW(nullptr, L"runas", wPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                if (reinterpret_cast<intptr_t>(res) > 32) {
                    QCoreApplication::quit();
                }
        #else
                if (QProcess::startDetached(uninstallerPath, {})) {
                    QCoreApplication::quit();
                } else {
                    MsgBox::show(this, MsgBox::Error, "Error", "Failed to launch uninstaller.");
                }
        #endif
        });

    system->addAction(gameDirectory);
    system->addAction(boxArt);

    help->addAction(boxArtHelp);

    menuBar->addMenu(system);
    menuBar->addMenu(help);
    menuBar->addAction(reload);
    menuBar->addAction(update);
    menuBar->addAction(uninstall);

    QWidget* toggleThemeContainer = new QWidget(menuBar);
    toggleThemeContainer->setFixedSize(50, 20);
    toggleThemeContainer->setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout* toggleThemeLayout = new QHBoxLayout(toggleThemeContainer);
    toggleThemeLayout->setContentsMargins(0, 2, 10, 2);
    toggleThemeLayout->setSpacing(0);
    toggleThemeLayout->setAlignment(Qt::AlignCenter);

    SwitchButton* toggleThemeSwitch = new SwitchButton(toggleThemeContainer);
    toggleThemeSwitch->setFixedSize(40, 20); 
    toggleThemeSwitch->setChecked(true);

    toggleThemeLayout   ->addWidget(toggleThemeSwitch);
    toggleThemeContainer->setLayout(toggleThemeLayout);
    menuBar             ->setCornerWidget(toggleThemeContainer, Qt::TopRightCorner);
    toggleThemeContainer->installEventFilter(this);
    toggleThemeSwitch   ->setObjectName("themeSwitch");

    {
        QSignalBlocker blocker(toggleThemeSwitch);
        toggleThemeSwitch->setChecked(true);
    }

    this->styleSheetDark(true);

    connect(toggleThemeSwitch, &SwitchButton::toggled, this, [this](bool isDarkMode) {
        if (isDarkMode) {
            this->styleSheetDark(true);
        } else {
            this->styleSheetLight(false);
        } 
    });
}

void mainWindow::dirButton() {
    QObject *senderObject = sender();
    if (!senderObject) return;

    QString buttonName = senderObject->objectName();
    QString path;
    QString configKey;

    if (buttonName == "gameDir") {
        path = QFileDialog::getExistingDirectory(this, tr("Select game folder"), QDir::homePath());
        configKey = "gameDir";
    }
    else if (buttonName == "coverArtDir") {
        path = QFileDialog::getExistingDirectory(this, tr("Select valid images folder"), QDir::homePath());
        configKey = "coverArtDir";
    }

    if (!path.isEmpty()) {
        QString nativePath = QDir::toNativeSeparators(path);
        QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(appDataDir); 
        QString jsonFilePath = QDir(appDataDir).filePath("config.json"); 
        QJsonObject configObject;

        QFile loadFile(jsonFilePath);
        if (loadFile.open(QIODevice::ReadOnly)) {
            QByteArray docData = loadFile.readAll();
            QJsonDocument loadDoc = QJsonDocument::fromJson(docData);
            configObject = loadDoc.object();
            loadFile.close();
        }

        configObject[configKey] = nativePath;

        QFile saveFile(jsonFilePath);
        if (saveFile.open(QIODevice::WriteOnly)) {
            QJsonDocument saveDoc(configObject);
            saveFile.write(saveDoc.toJson(QJsonDocument::Indented));
            saveFile.close();

            if (configKey == "gameDir") {
                this->reload();
            }
        }
    }
}

QString mainWindow::getDirFromJson(const QString &configKey) {
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString jsonFilePath = QDir(appDataDir).filePath("config.json");

    QFile loadFile(jsonFilePath);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        return ""; 
    }

    QByteArray docData = loadFile.readAll();
    loadFile.close();

    QJsonDocument loadDoc = QJsonDocument::fromJson(docData);
    QJsonObject configObject = loadDoc.object();

    if (configObject.contains(configKey) && configObject[configKey].isString()) {
        QString savedPath = configObject[configKey].toString();

        return QDir::toNativeSeparators(savedPath); 
    }

    return "";
}

void mainWindow::startGameScanner() {
    QString savedPath = getDirFromJson("gameDir");
    QString globalCoversPath = getDirFromJson("coverArtDir");
    QThread *workerThread = new QThread(this);
    GameScanner *scanner = new GameScanner(savedPath, globalCoversPath);
    scanner->moveToThread(workerThread);

    connect(workerThread, &QThread    ::started,   scanner,      &GameScanner::startScan);
    connect(scanner,      &GameScanner::gameFound, this,         &mainWindow ::processDiscoveredGame);
    connect(scanner,      &GameScanner::finished,  workerThread, &QThread    ::quit);
    connect(scanner,      &GameScanner::finished,  scanner,      &QObject    ::deleteLater);
    connect(workerThread, &QThread    ::finished,  workerThread, &QObject    ::deleteLater);

    workerThread->start();
}

void mainWindow::reload() {
    auto clearGridLayout = [](QGridLayout* grid) {
        if (!grid) return;
        
        QLayoutItem* item;
        while ((item = grid->takeAt(0)) != nullptr) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }

            delete item;
        }
    };

    clearGridLayout(this->homeGrid);
    clearGridLayout(this->psGrid);
    clearGridLayout(this->xboxGrid);
    clearGridLayout(this->nintendoGrid);

    this->homeRow      = 0; this->homeCol      = 0;
    this->psRow        = 0; this->psCol        = 0;
    this->xboxRow      = 0; this->xboxCol      = 0;
    this->nintendoRow  = 0; this->nintendoCol  = 0;

    this->scannedGamePaths.clear(); 

    this->startGameScanner();
}

QWidget* mainWindow::createLibraryPage(const QString &pageTitleText, QGridLayout *&outGrid) {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(20, 20, 20, 20);

    QLabel *title = new QLabel(pageTitleText, page);
    title->setObjectName("pageTitle");
    title->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->addWidget(title);

    QScrollArea *scrollArea = new QScrollArea(page);
    scrollArea->setObjectName("libraryScrollArea");
    scrollArea->setWidgetResizable(true);
    scrollArea->horizontalScrollBar()->setSingleStep(15); 
    scrollArea->verticalScrollBar()->setSingleStep(15);
    
    QWidget *scrollContent = new QWidget();
    scrollContent->setObjectName("libraryScrollContent");
    
    outGrid = new QGridLayout(scrollContent);
    outGrid->setSpacing(20);
    outGrid->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    outGrid->setColumnStretch(0, 1);
    outGrid->setColumnStretch(columnCount + 1, 1);

    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);
    return page;
}

QWidget* mainWindow::homePage() {
    return createLibraryPage(tr("Welcome Home Dashboard"), homeGrid);
}

QWidget* mainWindow::nintendoPage() {
    return createLibraryPage(tr("Nintendo Game Library"), nintendoGrid);
}

QWidget* mainWindow::playstationPage() {
    return createLibraryPage(tr("PlayStation Game Library"), psGrid);
}

QWidget* mainWindow::xboxPage() {
    return createLibraryPage(tr("Xbox Game Library"), xboxGrid);
}

void mainWindow::processDiscoveredGame(const GameData &game) {
    
    QString gamePath = game.fileInfo.absoluteFilePath();

    if (this->scannedGamePaths.contains(gamePath)) {
        return; 
    }
    this->scannedGamePaths.insert(gamePath);

    QString cacheKey = "masked_thumb_" + gamePath;
    QPixmap optimizedThumb;

    if (!QPixmapCache::find(cacheKey, &optimizedThumb)) {
        QSize cardSize(200, 300);
        QPixmap rawImage;

        if (!game.coverImagePath.isEmpty()) {
            rawImage.load(game.coverImagePath);
        }

        if (!rawImage.isNull()) {
            optimizedThumb = QPixmap(cardSize);
            optimizedThumb.fill(Qt::transparent); 

            QPainter painter(&optimizedThumb);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setRenderHint(QPainter::SmoothPixmapTransform); 

            QPainterPath path;
            path.addRoundedRect(QRectF(0, 0, cardSize.width(), cardSize.height()), 10, 10);
            painter.setClipPath(path);

            QPixmap scaledImage = rawImage.scaled(cardSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            
            int sourceX = (scaledImage.width() - cardSize.width()) / 2;
            int sourceY = (scaledImage.height() - cardSize.height()) / 2;
            painter.drawPixmap(0, 0, scaledImage, sourceX, sourceY, cardSize.width(), cardSize.height());
            painter.end();
        } else {
            optimizedThumb = QPixmap();
        }

        QPixmapCache::insert(cacheKey, optimizedThumb);
    }

    createGameCard(homeGrid, homeRow, homeCol, game, optimizedThumb);
}

void mainWindow::createGameCard(QGridLayout *targetGrid, int &row, int &col, const GameData &game, const QPixmap &preBakedThumb) {

    if (!targetGrid) return;
         
    QWidget *scrollContent = targetGrid->parentWidget();
    QPushButton *gameCard = new QPushButton(scrollContent);
    gameCard->setFixedSize(200, 300);
    gameCard->setObjectName("gameCardWidget");

    QLabel *coverLabel = new QLabel(gameCard);
    coverLabel->setGeometry(1, 1, gameCard->width() - 2, gameCard->height() - 2);
    coverLabel->setAlignment(Qt::AlignCenter);

    if (preBakedThumb.width() > 0 && preBakedThumb.height() > 0) {
        coverLabel->setPixmap(preBakedThumb.scaled(coverLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        coverLabel->setText("");
        coverLabel->setObjectName("gameCardCover");
    } else {
        coverLabel->setPixmap(QPixmap());
        coverLabel->setText("NO IMAGE");
        coverLabel->setObjectName("noImagePlaceholder");
    }
    coverLabel->lower();

    QVBoxLayout *cardLayout = new QVBoxLayout(gameCard);
    cardLayout->setContentsMargins(12, 12, 12, 12);
    cardLayout->addStretch(); 

    QLabel *gameTitle = new QLabel(game.fileInfo.baseName(), gameCard);
    gameTitle->setWordWrap(true);
    gameTitle->setAlignment(Qt::AlignCenter);
    gameTitle->setObjectName("gameCardTitle");
    cardLayout->addWidget(gameTitle);

    connect(gameCard, &QPushButton::clicked, this, [game]() {
        qDebug() << "Launching -> System:" << game.consoleType << "| Path:" << game.fileInfo.absoluteFilePath();
    });

    targetGrid->addWidget(gameCard, row, col + 1);

    col++;
    if (col >= columnCount) {
        col = 0;
        row++;
    }
}
