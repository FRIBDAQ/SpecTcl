#include "GateBinderObserver.h"
#include "Display.h"
#include "DisplayInterface.h"
#include "Histogrammer.h"
#include "GateMediator.h"
#include <assert.h>

CGateBinderObserver::CGateBinderObserver(CDisplayInterface &rInterface, CHistogrammer &rSorter)
    : m_pInterface(&rInterface), m_pSorter(&rSorter)
{

}

void CGateBinderObserver::onAdd(std::string name, CGateContainer &rGate)
{
    CDisplay* pDisp = m_pInterface->getCurrentDisplay();

    // The mediator tells us whether the spectrum can display the gate
    const DisplayBindings& bindings = pDisp->getDisplayBindings();
    for(UInt_t nId = 0; nId < bindings.size(); nId++) {
        if(bindings[nId] != "") { // Spectrum bound.
            CSpectrum* pSpec = m_pSorter->FindSpectrum(bindings[nId]);
            assert(pSpec != (CSpectrum*)kpNULL); // Bound spectra must exist!!.
            CGateMediator DisplayableGate(rGate, pSpec);
            if(DisplayableGate()) {
                CDisplayGate* pDisplayed = pDisp->GateToDisplayGate(*pSpec, rGate);
                if(pDisplayed)
                    pDisp->EnterGate(*pDisplayed);
                delete pDisplayed;
            }
        }
    }
}


void CGateBinderObserver::onRemove(std::string name, CGateContainer &rGate)
{
}

void CGateBinderObserver::onChange(std::string name, CGateContainer &rGate)
{
}
