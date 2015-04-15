#include "SpecTclRESTInterface.h"
#include "GlobalSettings.h"
#include "GSlice.h"
#include "GateEditComHandler.h"
#include "GateEditRequest.h"
#include "GateDeleteRequest.h"
#include "CommonResponseHandler.h"

#include <QString>
#include <QUrl>

#include <TLine.h>
#include <iostream>

using namespace std;

SpecTclRESTInterface::SpecTclRESTInterface()
    : m_pGateEditCmd(new GateEditComHandler),
    m_pCommonHandler(new CommonResponseHandler)
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

void SpecTclRESTInterface::deleteGate(const GSlice &slice)
{
  GateDeleteRequest req(slice);

  cout << req.toUrl().toString().toStdString() << endl;
  m_pCommonHandler->makeRequest(req.toUrl());
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
}

void SpecTclRESTInterface::deleteGate(const GGate &slice)
{
  GateDeleteRequest req(slice);

  cout << req.toUrl().toString().toStdString() << endl;
  m_pCommonHandler->makeRequest(req.toUrl());
}

void SpecTclRESTInterface::deleteGate(const QString& name) 
{
  GateDeleteRequest req(name);

  cout << req.toUrl().toString().toStdString() << endl;
  m_pCommonHandler->makeRequest(req.toUrl());

}
