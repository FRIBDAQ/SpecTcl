#include "mainwindow.h"
#include <TQApplication.h>
#include <TQRootApplication.h>
#include "GlobalSettings.h"

int main(int argc, char *argv[])
{
  QApplication::setGraphicsSystem("native");

  TQApplication a("app", &argc, argv);
  TQRootApplication b(argc, argv, 0);

  MainWindow w;
  w.show();

  GlobalSettings::getInstance()->setValue("/server/hostname","localhost");
  GlobalSettings::getInstance()->setValue("/server/port",8000);

  return b.exec();
}
