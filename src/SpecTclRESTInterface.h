#ifndef SPECTCLRESTINTERFACE_H
#define SPECTCLRESTINTERFACE_H

#include "SpecTclInterface.h"
#include "GateEditComHandler.h"
#include "CommonResponseHandler.h"
#include "GateListRequestHandler.h"
#include "GateList.h"
#include "HistogramList.h"
#include "ListRequestHandler.h"

#include <memory>
#include <vector>

namespace SpJs {
  class GateInfo;
}

namespace Viewer
{

class GSlice;

/*! \brief Implementation of the SpecTcl facade for the REST server plugin
 *
 */
class SpecTclRESTInterface : public SpecTclInterface
{
  Q_OBJECT
public:
    SpecTclRESTInterface();

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

public slots:
    void listGates();
    void listHistogramInfo();
    void onGateListReceived(std::vector<SpJs::GateInfo*> gates);
    void onHistogramListReceived(std::vector<SpJs::HistInfo*> hists);

private:
    std::unique_ptr<GateList> m_pGateList;
    std::unique_ptr<HistogramList> m_pHistList;
    std::unique_ptr<GateEditComHandler> m_pGateEditCmd;
    std::unique_ptr<CommonResponseHandler> m_pCommonHandler;
    std::unique_ptr<GateListRequestHandler> m_pGateListCmd;
    std::unique_ptr<ListRequestHandler> m_pHistListCmd;
    bool pollGates;
    bool pollHistInfo;
};

} // end of namespace

#endif // SPECTCLRESTINTERFACE_H
