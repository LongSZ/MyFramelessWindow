#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    mLocation = this->geometry();
    mIsMax = false;
    mDrag = false;

    ui->widgetTitle->installEventFilter(this);

    setStyleSheet("QMainWindow{color:#E8E8E8;background:#43CD80;}\
                  QWidget#widgetTitle{background:#363636;}\
                  QLabel#labelTitle{color:white;padding:8px 0px 5px;}\
                  QPushButton#btnMin,QPushButton#btnMax,QPushButton#btnExit{\
                          border-radius:0px;\
                          color: #F0F0F0;\
                          background-color:rgba(0,0,0,0);\
                          border-style:none;\
                      }\
                  QPushButton#btnMin:hover,QPushButton#btnMax:hover{\
                          background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(25, 134, 199, 0), stop:1 #5CACEE);\
                      }\
                  QPushButton#btnExit:hover{\
                          background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(238, 0, 0, 128), stop:1 rgba(238, 44, 44, 255));\
                      }");
}

MainWindow::~MainWindow() {
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e) {
    if (obj == ui->widgetTitle) {
        if(e->type() == QEvent::MouseButtonDblClick) {
            on_btnMax_clicked();
            return true;
        }
    }
    return QObject::eventFilter(obj, e);
}

void MainWindow::mousePressEvent(QMouseEvent *e) { //鼠标按下事件
    if (e->button() == Qt::LeftButton) {
        mDrag = true;
        mDragPos = e->globalPosition().toPoint() - pos();
        e->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) { //鼠标移动事件
    if (mDrag && (e->buttons() & Qt::LeftButton)) {
        move(e->globalPosition().toPoint() - mDragPos);
        e->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e) { //鼠标释放事件
    mDrag = false;
}

void MainWindow::on_btnMin_clicked() {
    showMinimized();
}

void MainWindow::on_btnMax_clicked() {
    if (mIsMax) {
        setGeometry(mLocation);
        ui->btnMax->setIcon(QIcon(":/image/max1.png"));
        ui->btnMax->setToolTip(QStringLiteral("最大化"));
    } else {
        mLocation = geometry();
        setGeometry(screen()->availableGeometry());
        ui->btnMax->setIcon(QIcon(":/image/max2.png"));
        ui->btnMax->setToolTip(QStringLiteral("还原"));
    }
    mIsMax = !mIsMax;
}

void MainWindow::on_btnExit_clicked() {
    qApp->exit();
}
