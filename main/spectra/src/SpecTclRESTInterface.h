//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
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
#ifndef SPECTCLRESTINTERFACE_H
#define SPECTCLRESTINTERFACE_H

#include "SpecTclInterface.h"
#include "GateEditComHandler.h"
#include "CommonResponseHandler.h"
#include "GateListRequestHandler.h"
#include "GateList.h"
#include "HistogramList.h"
#include "ListRequestHandler.h"
#include "ContentRequestHandler.h"

#include <memory>
#include <vector>

namespace SpJs {
  class GateInfo;
}

class TPad;

namespace Viewer
{

class GSlice;
class QRootCanvas;

/*! \brief Implementation of the SpecTcl facade for the REST server plugin
 *
 */
class SpecTclRESTInterface : public SpecTclInterface
{
  Q_OBJECT
public:
    SpecTclRESTInterface();
    virtual ~SpecTclRESTInterface();

    void addGate(const GSlice& slice);
    void editGate(const GSlice& slice);
    void deleteGate(const GSlice& slice);

    void addGate(const GGate& slice);
    void editGate(const GGate& slice);
    void deleteGate(const GGate& slice);

    void deleteGate(const QString& name);

    void enableGatePolling(bool enable);
    void enableHistogramInfoPolling(bool enable);

    GateList* getGateList() { return m_pGateList.get(); }
    HistogramList* getHistogramList() { return m_pHistList.get(); }

    void requestHistContentUpdate(QRootCanvas *pView);
    void requestHistContentUpdate(TPad *pPad);
    void requestHistContentUpdate(const QString& hName);

public slots:
    void listGates();
    void listHistogramInfo();
    void onGateListReceived(std::vector<SpJs::GateInfo*> gates);
    void onHistogramListReceived(std::vector<SpJs::HistInfo> hists);
    void onHistogramContentUpdated(HistogramBundle* pBundle);

private:
    std::unique_ptr<GateList> m_pGateList;
    std::unique_ptr<HistogramList> m_pHistList;
    std::unique_ptr<GateEditComHandler> m_pGateEditCmd;
    std::unique_ptr<CommonResponseHandler> m_pCommonHandler;
    std::unique_ptr<GateListRequestHandler> m_pGateListCmd;
    std::unique_ptr<ListRequestHandler> m_pHistListCmd;
    std::unique_ptr<ContentRequestHandler> m_pHistContentCmd;
    bool pollGates;
    bool pollHistInfo;
};

} // end of namespace

#endif // SPECTCLRESTINTERFACE_H
