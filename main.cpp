#include "mainwindow.h"
#include <QApplication>
#include <QPixmapCache>

int main(int argc, char *argv[]) {
    qputenv("QSG_RENDER_TIMING", "1");

    QApplication app(argc, argv);
    QPixmapCache::setCacheLimit(512 * 1024);
    mainWindow w;
    w.show();

    return app.exec();
}
