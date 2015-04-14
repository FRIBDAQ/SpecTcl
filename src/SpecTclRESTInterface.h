#ifndef SPECTCLRESTINTERFACE_H
#define SPECTCLRESTINTERFACE_H

#include "SpecTclInterface.h"
#include <memory>

class TCutG;
class GSlice;
class GateEditComHandler;

/*! \brief Implementation of the SpecTcl facade for the REST server plugin
 *
 */
class SpecTclRESTInterface : public SpecTclInterface
{
public:
    SpecTclRESTInterface();

    void addGate(const GSlice& slice);
    void editGate(const GSlice& slice);

    void addGate(const GGate& slice);
    void editGate(const GGate& slice);

private:
    std::unique_ptr<GateEditComHandler> m_pGateEditCmd;
};

#endif // SPECTCLRESTINTERFACE_H
