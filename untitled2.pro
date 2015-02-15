#-------------------------------------------------
#
# Project created by QtCreator 2015-02-13T20:55:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled2
TEMPLATE = app

QMAKE_CXX = "g++ -std=c++11"

SOURCES += main.cpp\
        QRootCanvas.cpp\
        QRootDialog.cpp\
        QRootApplication.cpp\
        QRootWindow.cpp\
        mainwindow.cpp

HEADERS  += QRootCanvas.h QRootDialog.h QRootApplication.h QRootWindow.h mainwindow.h

FORMS    += mainwindow.ui

!exists ($$(ROOTSYS)/include/rootcint.pri) {
   message("The Rootcint.pri was not found")
}
exists ($$(ROOTSYS)/include/rootcint.pri) {
  include ($$(ROOTSYS)/include/rootcint.pri)
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/root/root-5.34.07/lib/release/ -lQtRoot
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/root/root-5.34.07/lib/debug/ -lQtRoot
else:unix: LIBS += -L$$PWD/../../../../usr/local/root/root-5.34.07/lib/ -lQtRoot -lQtGSI

INCLUDEPATH += $$PWD/../../../../usr/local/root/root-5.34.07/include
DEPENDPATH += $$PWD/../../../../usr/local/root/root-5.34.07/include

unix|win32: LIBS += -lX11
