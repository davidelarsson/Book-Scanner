#-------------------------------------------------
#
# Project created by QtCreator 2014-07-20T04:03:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BookScannerServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
#    listeningdialog.cpp \
    helpdialog.cpp \
    configuredialog.cpp \
    debugdialog.cpp \
    server.cpp

HEADERS  += mainwindow.h \
#    listeningdialog.h \
    helpdialog.h \
    configuredialog.h \
    debugdialog.h \
    server.h

FORMS    += mainwindow.ui \
#    listeningdialog.ui \
    helpdialog.ui \
    configuredialog.ui \
    debugdialog.ui
