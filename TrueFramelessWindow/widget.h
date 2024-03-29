#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>
#include <QPushButton>
#include <QToolBar>

class Widget : public QMainWindow {
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);

    //If you want to have Max/Min/Close buttons, look at how QWinWidget uses these
    QPushButton* maximizeButton = nullptr;
    QPushButton* minimizeButton = nullptr;
    QPushButton* closeButton = nullptr;

    //If you want to enable dragging the window when the mouse is over top of, say, a QToolBar,
    //then look at how QWinWidget uses this
    QToolBar* toolBar = nullptr;
};

#endif // WIDGET_H
