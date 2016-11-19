#ifndef MAIN_H
#define MAIN_H

#include <CmdLineOptions.h>

#include <QHostAddress>
#include <QList>

namespace Viewer {

class Main
{
private:
    CmdLineOptions m_opts;

public:
    Main();

    int operator()(int argc, char* argv[]);

    bool serverIsOnThisMachine(QString hostName);

    bool listsShareContent(QList<QHostAddress> list0, QList<QHostAddress> list1);
};


} // end Viewer namespace

#endif // MAIN_H
