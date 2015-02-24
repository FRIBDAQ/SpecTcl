#-------------------------------------------------
#
# Project created by QtCreator 2015-02-13T20:55:03
#
#-------------------------------------------------

QT       += core gui network
CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled2
TEMPLATE = app

QMAKE_CXX = "g++ -std=c++11"

SOURCES += main.cpp\
        QRootCanvas.cpp\
        QRootDialog.cpp\
        QRootApplication.cpp\
        QRootWindow.cpp\
        mainwindow.cpp\
        ConnectServer.cpp \
        HistogramList.cpp \
        RequestHandler.cpp \
        SpectrumViewer.cpp \
        GlobalSettings.cpp \
    HistogramView.cpp \
    ListRequestHandler.cpp \
    TGo4CreateNewHistogram.cpp

HEADERS  += QRootCanvas.h QRootDialog.h QRootApplication.h QRootWindow.h mainwindow.h ConnectServer.h \
    SpectrumViewer.h \
    Singleton.h \ 
    HistogramList.h \
    GlobalSettings.h \
    RequestHandler.h \
    HistogramView.h \
    ListRequestHandler.h \
    TGo4CreateNewHistogram.h

FORMS    += mainwindow.ui ConnectServer.ui \
    SpectrumViewer.ui \
    HistogramList.ui \
    HistogramView.ui \
    TGo4CreateNewHistogram.ui

!exists ($$(ROOTSYS)/include/root/rootcint.pri) {
   message("The Rootcint.pri was not found")
}
exists ($$(ROOTSYS)/include/root/rootcint.pri) {
  include ($$(ROOTSYS)/include/root/rootcint.pri)
}

unix:INCLUDEPATH += $$(ROOTSYS)/include/root
unix:DEPENDPATH += $$(ROOTSYS)/include/root

unix|win32: LIBS += -lX11

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/root/root-5.34-10/lib/root/release/ -lQtGSI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/root/root-5.34-10/lib/root/debug/ -lQtGSI
else:symbian: LIBS += -lQtGSI
else:unix: LIBS += -L$$PWD/../../../../../../usr/local/root/root-5.34-10/lib/root/ -lQtGSI

INCLUDEPATH += $$PWD/../../../../../../usr/local/root/root-5.34-10/lib/root
DEPENDPATH += $$PWD/../../../../../../usr/local/root/root-5.34-10/lib/root

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += jsoncpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../json/SpecTclJson/oot/lib/release/ -lLibSpecTclJson
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../json/SpecTclJson/oot/lib/debug/ -lLibSpecTclJson
else:symbian: LIBS += -lLibSpecTclJson
else:unix: LIBS += -L$$PWD/../../../json/SpecTclJson/oot/lib/ -lLibSpecTclJson

INCLUDEPATH += $$PWD/../../../json/SpecTclJson/oot/include
DEPENDPATH += $$PWD/../../../json/SpecTclJson/oot/include

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../json/SpecTclJson/oot/lib/release/LibSpecTclJson.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../json/SpecTclJson/oot/lib/debug/LibSpecTclJson.lib
else:unix:!symbian: PRE_TARGETDEPS += $$PWD/../../../json/SpecTclJson/oot/lib/libLibSpecTclJson.a
