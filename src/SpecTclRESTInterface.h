#ifndef SPECTCLRESTINTERFACE_H
#define SPECTCLRESTINTERFACE_H

#include "SpecTclInterface.h"

#include <memory>
#include <vector>

namespace SpJs {
  class GateInfo;
}

class GSlice;
#include "GateEditComHandler.h"
#include "CommonResponseHandler.h"
#include "GateListRequestHandler.h"
#include "GateList.h"

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

    GateList* getGateList() { return m_pGateList.get(); }

public slots:
    void listGates();
    void onGateListReceived(std::vector<SpJs::GateInfo*> gates);

private:
    std::unique_ptr<GateList> m_pGateList;
    std::unique_ptr<GateEditComHandler> m_pGateEditCmd;
    std::unique_ptr<CommonResponseHandler> m_pCommonHandler;
    std::unique_ptr<GateListRequestHandler> m_pGateListCmd;
    bool pollGates;
};

#endif // SPECTCLRESTINTERFACE_H
