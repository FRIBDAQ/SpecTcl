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

    if(pDisplay->spectrumBound(&rSpectrum)) {

        //Int_t b =  pDisplay->FindDisplayBinding(rSpectrum.getName());

        pDisplay->setTitle(rSpectrum, rSpectrum.getName());
        pDisplay->setInfo(rSpectrum,
                          pDisplay->createTitle(rSpectrum,
                                                pDisplay->getTitleSize(),
                                                rSorter));
      }
}


void CGatingDisplayObserver::onRemove(const CGateContainer &rGate,
                                      CSpectrum &rSpectrum, CHistogrammer &rSorter)
{

}
