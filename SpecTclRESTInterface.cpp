#include "SpecTclRESTInterface.h"
#include "GlobalSettings.h"
#include "GSlice.h"
#include "GateEditComHandler.h"

#include <QString>
#include <QUrl>

#include <TLine.h>
#include <iostream>

SpecTclRESTInterface::SpecTclRESTInterface()
    : m_pGateEditCmd(new GateEditComHandler)
{

}

void SpecTclRESTInterface::addGate(const GSlice &slice)
{
    // at the moment editing and creating use the same command
    editGate(slice);
}

void SpecTclRESTInterface::editGate(const GSlice &slice)
{

    QString server = GlobalSettings::getServer();

    QString cmd = server + "/spectcl/gate/edit";
    cmd += QString("?name=") + slice.getName();
    cmd += QString("&type=s&low=") + QString::number(slice.getXLow());
    cmd += QString("&high=") + QString::number(slice.getXHigh());
    cmd += QString("&parameter=") + slice.getParameter();
    std::cout << cmd.toStdString() << std::endl;

    m_pGateEditCmd->makeRequest(QUrl(cmd));
}


void SpecTclRESTInterface::addGate(const TCutG &slice)
{
    editGate(slice);
}

void SpecTclRESTInterface::editGate(const TCutG &slice)
{}
