#include "mainwindow.h"
#include <TQApplication.h>
#include <TQRootApplication.h>
#include <TQRootCanvas.h>
#include <TH1.h>

int main(int argc, char *argv[])
{
    TQApplication a("app",&argc, argv);
    TQRootApplication b(argc, argv,0);

    MainWindow w;
    w.show();

    return b.exec();
}
