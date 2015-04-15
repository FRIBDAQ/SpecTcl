#ifndef SPECTCLRESTINTERFACE_H
#define SPECTCLRESTINTERFACE_H

#include "SpecTclInterface.h"
#include <memory>

class TCutG;
class GSlice;
class GateEditComHandler;
class CommonResponseHandler;

/*! \brief Implementation of the SpecTcl facade for the REST server plugin
 *
 */
class SpecTclRESTInterface : public SpecTclInterface
{
public:
    SpecTclRESTInterface();

    void addGate(const GSlice& slice);
    void editGate(const GSlice& slice);
    void deleteGate(const GSlice& slice);

    void addGate(const GGate& slice);
    void editGate(const GGate& slice);
    void deleteGate(const GGate& slice);

    void deleteGate(const QString& name);
private:
    std::unique_ptr<GateEditComHandler> m_pGateEditCmd;
    std::unique_ptr<CommonResponseHandler> m_pCommonHandler;
};

#endif // SPECTCLRESTINTERFACE_H
