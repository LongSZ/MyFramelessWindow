#include <QApplication>

#include "qwinwidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    //A common feature is to save your app's geometry on close such that you can draw in the same place on relaunch
    //Thus this project supports specifying the X/Y/Width/Height in a cross-platform manner
    int windowXPos, windowYPos, windowWidth, windowHeight;
    windowXPos = 100;
    windowYPos = 100;
    windowWidth = 1024;
    windowHeight = 768;

    //On Windows, the widget needs to be encapsulated in a native window for frameless rendering
    //In this case, QWinWidget #includes "Widget.h", creates it, and adds it to a layout
    QWinWidget w;
    w.setGeometry(windowXPos, windowYPos, windowWidth, windowHeight);
    w.show();

    return app.exec();
}
