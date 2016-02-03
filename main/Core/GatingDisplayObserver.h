#ifndef GATINGDISPLAYOBSERVER_H
#define GATINGDISPLAYOBSERVER_H

#include "GatingObserver.h"

class CDisplayInterface;

class CGatingDisplayObserver : public CGatingObserver
{
private:
    CDisplayInterface* m_pInterface;

public:
    CGatingDisplayObserver(CDisplayInterface* pInterface);

    void onApply(const CGateContainer &rGate, CSpectrum &rSpectrum, CHistogrammer& rSorter);

    void onRemove(const CGateContainer &rGate, CSpectrum &rSpectrum, CHistogrammer& rSorter);
};

#endif // GATINGDISPLAYOBSERVER_H
