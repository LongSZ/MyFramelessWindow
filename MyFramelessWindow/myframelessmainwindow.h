#ifndef MYFRAMELESSMAINWINDOW_H
#define MYFRAMELESSMAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QApplication>
#include <QEvent>
#include <QFocusEvent>
#include <qt_windows.h>
#include <QWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "winnativewindow.h"

#include <iostream>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class MyFramelessMainWindow;
}
QT_END_NAMESPACE

class MyFramelessMainWindow : public QWidget {
    Q_OBJECT

public:
    MyFramelessMainWindow(QWidget *parent = nullptr);
    ~MyFramelessMainWindow();

    void initUI(const QString &name);

    void setTitle(const QString &title);
    void show();
    void center();
    void showCentered();
    void setGeometry(int x, int y, int w, int h);
    HWND getParentWindow();

protected:
    void childEvent(QChildEvent *e);
    bool nativeEvent(const QByteArray &, void *message, long *result);
    bool eventFilter(QObject *o, QEvent *e);
    void focusInEvent(QFocusEvent *e);
    bool focusNextPrevChild(bool next);

private slots:
    void on_pushButtonMinimum_clicked();

    void on_pushButtonMaximum_clicked();

    void on_pushButtonRestore_clicked();

    void on_pushButtonClose_clicked();

private:
    Ui::MyFramelessMainWindow *ui;

    void resetFocus();
    void saveFocus();

    WinNativeWindow *p_ParentWinNativeWindow;
    HWND m_ParentNativeWindowHandle;
    int BORDERWIDTH = 6;	//Adjust this as you wish for # of pixels on the edges to show resize handles
    int TOOLBARHEIGHT = 80; //Adjust this as you wish for # of pixels from the top to allow dragging the window
    HWND _prevFocus;
    bool _reenableParent;
};
#endif // MYFRAMELESSMAINWINDOW_H
