#pragma once
#include <QMainWindow>
#include <QGridLayout>
#include <QStackedWidget>
#include <QFileInfo>
#include "gameScanner.h"

class mainWindow : public QMainWindow {
    Q_OBJECT

public:
    mainWindow(QWidget *parent = nullptr); ~mainWindow() override = default;
    QWidget* createLibraryPage(const QString &pageTitleText, QGridLayout *&outGrid);
    QWidget* homePage();
    QWidget* playstationPage();
    QWidget* xboxPage();
    QWidget* nintendoPage();

private slots:
    void styleSheetDark(bool isDarkMode = true);
    void styleSheetLight(bool isLightMode = false);
    void dirButton();
    void reload();
    void processDiscoveredGame(const GameData &game);
    QString getDirFromJson(const QString &configKey);

private:
    void createGameCard(QGridLayout *targetGrid, int &row, int &col, const GameData &game, const QPixmap &preBakedThumb);
    void menubar();
    void startGameScanner();
    QSet<QString> scannedGamePaths;

    QStackedWidget *mainStackedWidget;
    QGridLayout    *homeGrid;
    QGridLayout    *psGrid;
    QGridLayout    *xboxGrid;
    QGridLayout    *nintendoGrid;

    int homeRow     = 0, homeCol     = 0;
    int psRow       = 0, psCol       = 0;
    int xboxRow     = 0, xboxCol     = 0;
    int nintendoRow = 0, nintendoCol = 0;

    const int columnCount = 10;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
};