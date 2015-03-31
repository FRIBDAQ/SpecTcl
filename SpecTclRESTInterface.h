#ifndef SPECTCLRESTINTERFACE_H
#define SPECTCLRESTINTERFACE_H

#include "SpecTclInterface.h"
#include <memory>

class TCutG;
class GSlice;
class GateEditComHandler;

class SpecTclRESTInterface : public SpecTclInterface
{
public:
    SpecTclRESTInterface();

    void addGate(const GSlice& slice);
    void editGate(const GSlice& slice);

    void addGate(const TCutG& slice);
    void editGate(const TCutG& slice);

private:
    std::unique_ptr<GateEditComHandler> m_pGateEditCmd;
};

#endif // SPECTCLRESTINTERFACE_H
