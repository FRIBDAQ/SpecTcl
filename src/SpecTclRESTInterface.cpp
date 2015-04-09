#include "SpecTclRESTInterface.h"
#include "GlobalSettings.h"
#include "GSlice.h"
#include "GateEditComHandler.h"
#include "GateEditRequest.h"

#include <QString>
#include <QUrl>

#include <TLine.h>
#include <iostream>

using namespace std;

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
    GateEditRequest req(slice);

    m_pGateEditCmd->makeRequest(req.toUrl());
}


void SpecTclRESTInterface::addGate(const GGate &gate)
{
    editGate(gate);
}

void SpecTclRESTInterface::editGate(const GGate &gate)
{

  GateEditRequest req(gate);

  cout << req.toUrl().toString().toStdString() << endl;
  m_pGateEditCmd->makeRequest(req.toUrl());

//    QString server = GlobalSettings::getServer();

//    QString cmd = server + "/spectcl/gate/edit";
//    cmd += QString("?name=") + slice.getName();
//    cmd += QString("&type=s&low=") + QString::number(slice.getXLow());
//    cmd += QString("&high=") + QString::number(slice.getXHigh());
//    cmd += QString("&parameter=") + slice.getParameter();
//    std::cout << cmd.toStdString() << std::endl;

//    m_pGateEditCmd->makeRequest(QUrl(cmd));

}
