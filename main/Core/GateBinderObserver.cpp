#include "GateBinderObserver.h"
#include "Display.h"
#include "DisplayInterface.h"
#include "Histogrammer.h"
#include "GateMediator.h"
#include <assert.h>
#include <iostream>

CGateBinderObserver::CGateBinderObserver(CDisplayInterface &rInterface, CHistogrammer &rSorter)
    : m_pInterface(&rInterface), m_pSorter(&rSorter)
{

}

void CGateBinderObserver::onAdd(std::string name, CGateContainer &rGate)
{

    CDisplay* pDisp = m_pInterface->getCurrentDisplay();

    // The mediator tells us whether the spectrum can display the gate
    //const DisplayBindings& bindings = pDisp->getDisplayBindings();
    SpectrumContainer spectra = pDisp->getBoundSpectra();
    for(UInt_t nId = 0; nId < spectra.size(); nId++) {
        CSpectrum* pSpec = spectra[nId];
        if(pSpec != NULL) { // Spectrum bound.

            CGateMediator DisplayableGate(rGate, pSpec);
            if(DisplayableGate()) {
                pDisp->addGate(*pSpec, rGate);
            }
        }
    }
}


void CGateBinderObserver::onRemove(std::string name, CGateContainer &rGate)
{
    // This function is quite simple since gates entered in Xamine on our
    // behalf will have this id and name.   Therefore we just need
    // to remove gates with id == nGateId from all spectra bound.
    //
    // Note that CXamine::RemoveGate throws on error, and therefore
    // we must catch and ignore exceptions at the removal.

    CDisplay* pDisp = m_pInterface->getCurrentDisplay();

    SpectrumContainer boundSpectra = pDisp->getBoundSpectra();
    UInt_t nSpectra = boundSpectra.size();
    for(UInt_t nId = 0; nId < nSpectra; nId++) {
        CSpectrum* pSpectrum = boundSpectra.at(nId);
        if(pSpectrum != NULL) {
            try {
                pDisp->removeGate(*pSpectrum, rGate);
            }
            catch(...) {		// Ignore exceptions.
            }
        }
    }
}

void CGateBinderObserver::onChange(std::string name, CGateContainer &rGate)
{
    // assumes that the new gate and old gate have the same index
    onRemove(name, rGate);
    onAdd(name, rGate);
}
