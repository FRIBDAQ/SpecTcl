//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins 
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#include "SpecTclRESTInterface.h"
#include "GlobalSettings.h"
#include "GSlice.h"
#include "GateEditComHandler.h"
#include "GateEditRequest.h"
#include "GateDeleteRequest.h"
#include "CommonResponseHandler.h"
#include "GateListRequestHandler.h"
#include "HistogramList.h"
#include "QRootCanvas.h"
#include "CanvasOps.h"

#include <QString>
#include <QTimer>
#include <QUrl>
#include <QRegExp>

#include <TLine.h>
#include <TCanvas.h>
#include <TList.h>
#include <TH1.h>

#include <iostream>

using namespace std;


namespace Viewer
{


SpecTclRESTInterface::SpecTclRESTInterface()
    : SpecTclInterface(),
    m_pGateList(new MasterGateList),
    m_pHistList(new HistogramList),
    m_pGateEditCmd(new GateEditComHandler),
    m_pCommonHandler(new CommonResponseHandler),
    m_pGateListCmd(new GateListRequestHandler),
    m_pHistListCmd(new ListRequestHandler),
    m_pHistContentCmd(new ContentRequestHandler(m_pHistList.get())),
    pollGates(false),
    pollHistInfo(false)
{

  connect(m_pGateListCmd.get(), SIGNAL(parseCompleted(std::vector<SpJs::GateInfo*>)),
      this, SLOT(onGateListReceived(std::vector<SpJs::GateInfo*>)));

  connect(m_pHistListCmd.get(), SIGNAL(parseCompleted(std::vector<SpJs::HistInfo>)),
      this, SLOT(onHistogramListReceived(std::vector<SpJs::HistInfo>)));

  connect(m_pHistContentCmd.get(), SIGNAL(parsingComplete(HistogramBundle*)),
          this, SLOT(onHistogramContentUpdated(HistogramBundle*)));

  std::cout << "SpecTclRESTInterface::SpecTclRESTInterface()" << std::endl;
}

SpecTclRESTInterface::~SpecTclRESTInterface()
{
    std::cout << "SpecTclRESTInterface::~SpecTclRESTInterface()" << std::endl;
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

  m_pCommonHandler->makeRequest(req.toUrl());
}

void SpecTclRESTInterface::addGate(const GGate &gate)
{
    editGate(gate);
}

void SpecTclRESTInterface::editGate(const GGate &gate)
{

  GateEditRequest req(gate);

  m_pGateEditCmd->makeRequest(req.toUrl());
}

void SpecTclRESTInterface::deleteGate(const GGate &slice)
{
  GateDeleteRequest req(slice);

  m_pCommonHandler->makeRequest(req.toUrl());
}

void SpecTclRESTInterface::deleteGate(const QString& name) 
{
  GateDeleteRequest req(name);

  m_pCommonHandler->makeRequest(req.toUrl());

  // we always want to follow immediately with a gate list request.
  // SpecTcl is a single threaded application so that there is no way
  // this request will be handled prior to the gate deletion
  listGates();

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
//  std::cout << "SpecTclRESTInterface::onGateListReceived()" << std::endl;

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
//      std::cout << "gates changed!" << std::endl;
//      std::cout << "========" << std::endl;

      m_pHistList->synchronize(*m_pGateList);

      // tell the world that things have changed.
      emit gateListChanged();

  }

  // schedule the next update
  QTimer::singleShot(GlobalSettings::getPollInterval(), this, SLOT(listGates()));

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

void SpecTclRESTInterface::requestHistContentUpdate(QRootCanvas* pCanvas)
{

  Q_ASSERT( pCanvas != nullptr );

  // update all histograms in this canvas
  requestHistContentUpdate(pCanvas->getCanvas());
}

void SpecTclRESTInterface::requestHistContentUpdate(TVirtualPad* pPad)
{

  Q_ASSERT( pPad != nullptr );

    auto histNames = CanvasOps::extractAllHistNames(*pPad);
    for (auto& name : histNames) {
        requestHistContentUpdate(name);
    }
}

void SpecTclRESTInterface::requestHistContentUpdate(const QString& name)
{
  auto host = GlobalSettings::getServerHost();
  auto port = GlobalSettings::getServerPort();

  QString trimmedName = name.left(name.lastIndexOf(QRegExp("_copy$")));

  QString reqUrl("http://%1:%2/spectcl/spectrum/contents?name=%3");
  auto reqUrlTmp = reqUrl.arg(host).arg(port).arg(trimmedName);

  m_pHistContentCmd->get(QUrl(reqUrlTmp));
}

void
SpecTclRESTInterface::onHistogramListReceived(std::vector<SpJs::HistInfo> hists)
{

  if (! pollHistInfo) {
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
  QTimer::singleShot(GlobalSettings::getPollInterval(), this, SLOT(listHistogramInfo()));

}

void
SpecTclRESTInterface::onHistogramContentUpdated(HistogramBundle *pBundle)
{

    emit histogramContentUpdated(pBundle);
}

} // end of namespace

