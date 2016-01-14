#include "GatingDisplayObserver.h"

#include "Spectrum.h"
#include "GateContainer.h"
#include "Histogrammer.h"
#include "Display.h"
#include "DisplayInterface.h"

#include <iostream>

CGatingDisplayObserver::CGatingDisplayObserver(CDisplayInterface *pInterface)
    : m_pInterface(pInterface)
{

}

void CGatingDisplayObserver::onApply(const CGateContainer &rGate,
                                     CSpectrum &rSpectrum,
                                     CHistogrammer &rSorter)
{

    CDisplay* pDisplay = m_pInterface->getCurrentDisplay();
    Int_t b =  pDisplay->FindDisplayBinding(rSpectrum.getName());
      if(b >= 0) {
        pDisplay->setTitle(rSpectrum.getName(), b);
        pDisplay->setInfo(pDisplay->createTitle(rSpectrum,
                                                pDisplay->getTitleSize(),
                                                rSorter),
                            b);
      }
}


void CGatingDisplayObserver::onRemove(const CGateContainer &rGate,
                                      CSpectrum &rSpectrum, CHistogrammer &rSorter)
{

}
