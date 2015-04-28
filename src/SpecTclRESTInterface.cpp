#include "SpecTclRESTInterface.h"
#include "GlobalSettings.h"
#include "GSlice.h"
#include "GateEditComHandler.h"
#include "GateEditRequest.h"
#include "GateDeleteRequest.h"
#include "CommonResponseHandler.h"
#include "GateListRequestHandler.h"
#include "HistogramList.h"

#include <QString>
#include <QTimer>
#include <QUrl>

#include <TLine.h>
#include <iostream>

using namespace std;

namespace Viewer
{

SpecTclRESTInterface::SpecTclRESTInterface()
    : SpecTclInterface(),
    m_pGateList(new GateList),
    m_pHistList(new HistogramList),
    m_pGateEditCmd(new GateEditComHandler),
    m_pCommonHandler(new CommonResponseHandler),
    m_pGateListCmd(new GateListRequestHandler),
    m_pHistListCmd(new ListRequestHandler),
    pollGates(false),
    pollHistInfo(false)
{

  connect(m_pGateListCmd.get(), SIGNAL(parseCompleted(std::vector<SpJs::GateInfo*>)),
      this, SLOT(onGateListReceived(std::vector<SpJs::GateInfo*>)));

  connect(m_pGateListCmd.get(), SIGNAL(parseCompleted(std::vector<SpJs::HistInfo*>)),
      this, SLOT(onHistogramListReceived(std::vector<SpJs::HistInfo*>)));
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


void SpecTclRESTInterface::listGates() 
{
  m_pGateListCmd->get(); 
}

void SpecTclRESTInterface::listHistogramInfo()
{
  m_pHistListCmd->get();
}

void 
SpecTclRESTInterface::onGateListReceived(std::vector<SpJs::GateInfo*> gates)
{

  if (! pollGates) {
      // free the gates... they have done their job
      for (auto ptr : gates) { delete ptr; }

      return;
  }

  // synchronize our list of gates to the list that we are being passed
  // from SpecTcl
  bool gatesChanged = m_pGateList->synchronize(gates);

  // only update everything else if something actually changed.
  if (gatesChanged) {

      // now update the histograms so that we know they only reference gates
      // that exist after the synchronization
      m_pHistList->synchronize(*m_pGateList);

      // tell the world that things have changed.
      emit gateListChanged();

  }

  // schedule the next update
  QTimer::singleShot(1000, this, SLOT(listGates()));

  // free the gates... they have done their job
  for (auto ptr : gates) { delete ptr; }

}

void SpecTclRESTInterface::enableGatePolling(bool enable)
{
  // don't double schedule... only start the polling if it has not already been
  // started.
  if (pollGates != enable) {
      pollGates = enable;
      if (enable) {
          listGates();
        }
  }

}

void SpecTclRESTInterface::enableHistogramInfoPolling(bool enable)
{
  // don't double schedule... only start the polling if it has not already been
  // started.
  if (pollHistInfo != enable) {
      pollHistInfo = enable;
      if (enable) {
          listHistogramInfo();
      }
  }

}

void
SpecTclRESTInterface::onHistogramListReceived(std::vector<SpJs::HistInfo*> hists)
{

  if (! pollHistInfo) {
      // free the gates... they have done their job
      for (auto ptr : hists) { delete ptr; }

      return;
  }

  // synchronize our list of gates to the list that we are being passed
  // from SpecTcl
  bool histInfoChanged = m_pHistList->update(hists);

  // only update everything else if something actually changed.
  if (histInfoChanged) {

      // tell the world that things have changed.
      emit histogramListChanged();

  }

  // schedule the next update
  QTimer::singleShot(1000, this, SLOT(listHistogramInfo()));

  // free the gates... they have done their job
  for (auto ptr : hists) { delete ptr; }

}


} // end of namespace

