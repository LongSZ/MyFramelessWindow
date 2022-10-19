#include "myframelessmainwindow.h"
#include "ui_myframelessmainwindow.h"

MyFramelessMainWindow::MyFramelessMainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyFramelessMainWindow) {
    ui->setupUi(this);

    initUI("MyFramelessMainWindow");
}

MyFramelessMainWindow::~MyFramelessMainWindow() {
    delete ui;
}

void MyFramelessMainWindow::initUI(const QString &name) {
    _reenableParent = false;

    //Create a native window and give it geometry values * devicePixelRatio for HiDPI support
    p_ParentWinNativeWindow = new WinNativeWindow(1  * window()->devicePixelRatio()
            , 1 * window()->devicePixelRatio()
            , 1 * window()->devicePixelRatio()
            , 1 * window()->devicePixelRatio()
            , name.isEmpty() ? QString(QApplication::applicationName()) : name);

    //If you want to set a minimize size for your app, do so here
    //p_ParentWinNativeWindow->setMinimumSize(1024 * window()->devicePixelRatio(), 768 * window()->devicePixelRatio());

    //If you want to set a maximum size for your app, do so here
    //p_ParentWinNativeWindow->setMaximumSize(1024 * window()->devicePixelRatio(), 768 * window()->devicePixelRatio());

    //Save the native window handle for shorthand use
    m_ParentNativeWindowHandle = p_ParentWinNativeWindow->hWnd;
    //Q_ASSERT(m_ParentNativeWindowHandle);

    //Create the child window & embed it into the native one
    if (m_ParentNativeWindowHandle) {
        setWindowFlags(Qt::FramelessWindowHint);
        setProperty("_q_embedded_native_parent_handle", (WId)m_ParentNativeWindowHandle);
        SetWindowLong((HWND)winId(), GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

        SetParent((HWND)winId(), m_ParentNativeWindowHandle);
        QEvent e(QEvent::EmbeddingControl);
        QApplication::sendEvent(this, &e);
    }

    //Pass along our window handle & widget pointer to WinFramelessWidget so we can exchange messages
    p_ParentWinNativeWindow->childWindow = (HWND)winId();
    p_ParentWinNativeWindow->childWidget = this;

    //Update the BORDERWIDTH value if needed for HiDPI displays
    BORDERWIDTH = BORDERWIDTH * window()->devicePixelRatio();

    //Send the parent native window a WM_SIZE message to update the widget size
    SendMessage(m_ParentNativeWindowHandle, WM_SIZE, 0, 0);

    //init window size and pos
    //QDesktopWidget *desktop = QApplication::desktop();
    QScreen *screen = window()->screen();
    QRect availableRc = screen->availableGeometry();
    int windowXPos, windowYPos, windowWidth, windowHeight;
    windowWidth = 1200;
    windowHeight = 800;
    windowXPos = availableRc.x() + (availableRc.width() - windowWidth) / 2;
    windowYPos = availableRc.y() + (availableRc.height() - windowHeight) / 2;
    this->setGeometry(windowXPos, windowYPos, windowWidth, windowHeight);
}

void MyFramelessMainWindow::setTitle(const QString &title) {
    if (m_ParentNativeWindowHandle) {
        LPCWSTR _cn = (LPCWSTR)title.utf16();
        SetWindowTextW(m_ParentNativeWindowHandle, _cn);
    }
}

void MyFramelessMainWindow::show() {
    ShowWindow(m_ParentNativeWindowHandle, true);
    saveFocus();
    QWidget::show();
}

void MyFramelessMainWindow::center() {
    const QWidget *child = findChild<QWidget*>();
    if (child && !child->isWindow()) {
        qWarning("FramelessWnd::center: Call this function only for FramelessWnd with toplevel children");
    }
    RECT r;
    GetWindowRect(m_ParentNativeWindowHandle, &r);
    setGeometry((r.right - r.left) / 2 + r.left, (r.bottom - r.top) / 2 + r.top, 0, 0);
}

void MyFramelessMainWindow::showCentered() {
    center();
    show();
}

void MyFramelessMainWindow::setGeometry(int x, int y, int w, int h) {
    p_ParentWinNativeWindow->setGeometry(x * window()->devicePixelRatio()
                                         , y * window()->devicePixelRatio()
                                         , w * window()->devicePixelRatio()
                                         , h * window()->devicePixelRatio());
}

HWND MyFramelessMainWindow::getParentWindow() {
    return m_ParentNativeWindowHandle;
}

void MyFramelessMainWindow::childEvent(QChildEvent *e) {
    QObject *obj = e->child();
    if (obj->isWidgetType()) {
        if (e->added()) {
            if (obj->isWidgetType()) {
                obj->installEventFilter(this);
            }
        } else if (e->removed() && _reenableParent) {
            _reenableParent = false;
            EnableWindow(m_ParentNativeWindowHandle, true);
            obj->removeEventFilter(this);
        }
    }
    QWidget::childEvent(e);
}

bool MyFramelessMainWindow::nativeEvent(const QByteArray &, void *message, long *result) {
    MSG *msg = (MSG *)message;

    if (msg->message == WM_SETFOCUS) {
        Qt::FocusReason reason;
        if (::GetKeyState(VK_LBUTTON) < 0 || ::GetKeyState(VK_RBUTTON) < 0)
            reason = Qt::MouseFocusReason;
        else if (::GetKeyState(VK_SHIFT) < 0)
            reason = Qt::BacktabFocusReason;
        else
            reason = Qt::TabFocusReason;
        QFocusEvent e(QEvent::FocusIn, reason);
        QApplication::sendEvent(this, &e);
    }

    //Only close if safeToClose clears()
    if (msg->message == WM_CLOSE) {
        if (true /* put your check for it if it safe to close your app here */) { //eg, does the user need to save a document
            //Safe to close, so hide the parent window
            ShowWindow(m_ParentNativeWindowHandle, false);

            //And then quit
            QApplication::quit();
        } else {
            *result = 0; //Set the message to 0 to ignore it, and thus, don't actually close
            return true;
        }
    }

    //Double check WM_SIZE messages to see if the parent native window is maximized
    if (msg->message == WM_SIZE) {
//        if (maximizeBtn)
//        {
        //Get the window state
        WINDOWPLACEMENT wp;
        GetWindowPlacement(m_ParentNativeWindowHandle, &wp);

        //If we're maximized,
        if (wp.showCmd == SW_MAXIMIZE) {
            //Maximize button should show as Restore
            //maximizeBtn->setChecked(true);
            ui->pushButtonRestore->setVisible(true);
            ui->pushButtonMaximum->setVisible(false);
        } else {
            //Maximize button should show as Maximize
            //maximizeBtn->setChecked(false);
            ui->pushButtonMaximum->setVisible(true);
            ui->pushButtonRestore->setVisible(false);
        }
//        }
    }

    //Pass NCHITTESTS on the window edges as determined by BORDERWIDTH & TOOLBARHEIGHT through to the parent native window
    if (msg->message == WM_NCHITTEST) {
        RECT WindowRect;
        int x, y;

        GetWindowRect(msg->hwnd, &WindowRect);
        x = GET_X_LPARAM(msg->lParam) - WindowRect.left;
        y = GET_Y_LPARAM(msg->lParam) - WindowRect.top;
        TOOLBARHEIGHT = ui->title_bar->height();
        if (x >= BORDERWIDTH && x <= WindowRect.right - WindowRect.left - BORDERWIDTH && y >= BORDERWIDTH && y <= TOOLBARHEIGHT) {
            if (ui->title_bar) {
                //If the mouse is over top of the toolbar area BUT is actually positioned over a child widget of the toolbar,
                //Then we don't want to enable dragging. This allows for buttons in the toolbar, eg, a Maximize button, to keep the mouse interaction
                if (QApplication::widgetAt(QCursor::pos()) != ui->title_bar)
                    return false;
            }

            //The mouse is over the toolbar area & is NOT over a child of the toolbar, so pass this message
            //through to the native window for HTCAPTION dragging
            *result = HTTRANSPARENT;
            return true;

        } else if (x < BORDERWIDTH && y < BORDERWIDTH) {
            *result = HTTRANSPARENT;
            return true;
        } else if (x > WindowRect.right - WindowRect.left - BORDERWIDTH && y < BORDERWIDTH) {
            *result = HTTRANSPARENT;
            return true;
        } else if (x > WindowRect.right - WindowRect.left - BORDERWIDTH && y > WindowRect.bottom - WindowRect.top - BORDERWIDTH) {
            *result = HTTRANSPARENT;
            return true;
        } else if (x < BORDERWIDTH && y > WindowRect.bottom - WindowRect.top - BORDERWIDTH) {
            *result = HTTRANSPARENT;
            return true;
        } else if (x < BORDERWIDTH) {
            *result = HTTRANSPARENT;
            return true;
        } else if (y < BORDERWIDTH) {
            *result = HTTRANSPARENT;
            return true;
        } else if (x > WindowRect.right - WindowRect.left - BORDERWIDTH) {
            *result = HTTRANSPARENT;
            return true;
        } else if (y > WindowRect.bottom - WindowRect.top - BORDERWIDTH) {
            *result = HTTRANSPARENT;
            return true;
        }

        return false;
    }

    return false;
}

bool MyFramelessMainWindow::eventFilter(QObject *o, QEvent *e) {
    QWidget *w = (QWidget*)o;

    switch (e->type()) {
    case QEvent::WindowDeactivate:
        if (w->isModal() && w->isHidden())
            BringWindowToTop(m_ParentNativeWindowHandle);
        break;

    case QEvent::Hide:
        if (_reenableParent) {
            EnableWindow(m_ParentNativeWindowHandle, true);
            _reenableParent = false;
        }
        resetFocus();

        if (w->testAttribute(Qt::WA_DeleteOnClose) && w->isWindow())
            deleteLater();
        break;

    case QEvent::Show:
        if (w->isWindow()) {
            saveFocus();
            hide();
            if (w->isModal() && !_reenableParent) {
                EnableWindow(m_ParentNativeWindowHandle, false);
                _reenableParent = true;
            }
        }
        break;

    case QEvent::Close: {
        ::SetActiveWindow(m_ParentNativeWindowHandle);
        if (w->testAttribute(Qt::WA_DeleteOnClose))
            deleteLater();
        break;
    }
    default:
        break;
    }

    return QWidget::eventFilter(o, e);
}

void MyFramelessMainWindow::focusInEvent(QFocusEvent *e) {
    QWidget *candidate = this;

    switch (e->reason()) {
    case Qt::TabFocusReason:
    case Qt::BacktabFocusReason:
        while (!(candidate->focusPolicy() & Qt::TabFocus)) {
            candidate = candidate->nextInFocusChain();
            if (candidate == this) {
                candidate = 0;
                break;
            }
        }
        if (candidate) {
            candidate->setFocus(e->reason());
            if (e->reason() == Qt::BacktabFocusReason || e->reason() == Qt::TabFocusReason) {
                candidate->setAttribute(Qt::WA_KeyboardFocusChange);
                candidate->window()->setAttribute(Qt::WA_KeyboardFocusChange);
            }
            if (e->reason() == Qt::BacktabFocusReason)
                QWidget::focusNextPrevChild(false);
        }
        break;
    default:
        break;
    }
}

bool MyFramelessMainWindow::focusNextPrevChild(bool next) {
    QWidget *curFocus = focusWidget();
    if (!next) {
        if (!curFocus->isWindow()) {
            QWidget *nextFocus = curFocus->nextInFocusChain();
            QWidget *prevFocus = 0;
            QWidget *topLevel = 0;
            while (nextFocus != curFocus) {
                if (nextFocus->focusPolicy() & Qt::TabFocus) {
                    prevFocus = nextFocus;
                    topLevel = 0;
                }
                nextFocus = nextFocus->nextInFocusChain();
            }

            if (!topLevel) {
                return QWidget::focusNextPrevChild(false);
            }
        }
    } else {
        QWidget *nextFocus = curFocus;
        while (1 && nextFocus != 0) {
            nextFocus = nextFocus->nextInFocusChain();
            if (nextFocus->focusPolicy() & Qt::TabFocus) {
                return QWidget::focusNextPrevChild(true);
            }
        }
    }

    ::SetFocus(m_ParentNativeWindowHandle);

    return true;
}

void MyFramelessMainWindow::resetFocus() {
    if (_prevFocus)
        ::SetFocus(_prevFocus);
    else
        ::SetFocus(getParentWindow());
}

void MyFramelessMainWindow::saveFocus() {
    if (!_prevFocus)
        _prevFocus = ::GetFocus();
    if (!_prevFocus)
        _prevFocus = getParentWindow();
}

void MyFramelessMainWindow::on_pushButtonMinimum_clicked() {
    SendMessage(m_ParentNativeWindowHandle, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void MyFramelessMainWindow::on_pushButtonMaximum_clicked() {
    SendMessage(m_ParentNativeWindowHandle, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
}

void MyFramelessMainWindow::on_pushButtonRestore_clicked() {
    SendMessage(m_ParentNativeWindowHandle, WM_SYSCOMMAND, SC_RESTORE, 0);
}

void MyFramelessMainWindow::on_pushButtonClose_clicked() {
    //Safe to close, so hide the parent window
    ShowWindow(m_ParentNativeWindowHandle, false);

    //And then quit
    QApplication::quit();
}
