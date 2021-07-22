#include "myframelessmainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    //This has the app draw at HiDPI scaling on HiDPI displays, usually two pixels for every one logical pixel
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    //This has QPixmap images use the @2x images when available
    //See this bug for more details on how to get this right: https://bugreports.qt.io/browse/QTBUG-44486#comment-327410
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QApplication a(argc, argv);
    
    MyFramelessMainWindow w;
    w.show();
    
    return a.exec();
}
