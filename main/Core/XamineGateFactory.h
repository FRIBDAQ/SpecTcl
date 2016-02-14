#ifndef XAMINEGATEFACTORY_H
#define XAMINEGATEFACTORY_H


#include "daqdatatypes.h"

class CProductionXamineShMem;
class CXamineGate;
class CSpectrum;
class CGateContainer;

class CXamineGateFactory
{
private:
    CProductionXamineShMem* m_pMemory;

public:
    CXamineGateFactory(CProductionXamineShMem* pSharedMem);

    CXamineGate* fromSpecTclGate(CSpectrum &rSpectrum, CGateContainer& rGate);

private:
    bool flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam);
};

#endif // XAMINEGATEFACTORY_H
