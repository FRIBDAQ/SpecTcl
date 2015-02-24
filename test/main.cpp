#include <QCoreApplication>
#include <QtNetwork>
#include <QTimer>
#include "test.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTimer::singleShot(1000,&a,SLOT(quit()));

    Test t;
    t.get(QUrl("http://daqdev-wheezy.nscl.msu.edu:8000/spectcl/spectrum/list"));
    return a.exec();
}
