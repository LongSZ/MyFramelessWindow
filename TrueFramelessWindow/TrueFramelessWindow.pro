#-------------------------------------------------
#
# TrueFramelessWindow.pro
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrueFramelessWindow
TEMPLATE = app

SOURCES += main.cpp \
    widget.cpp \
    qwinwidget.cpp \
    winnativewindow.cpp

HEADERS  += widget.h \
    qwinwidget.h \
    winnativewindow.h
