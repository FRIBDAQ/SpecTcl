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
    const DisplayBindings& bindings = pDisp->getDisplayBindings();
    std::cout << "number of bindings = " << bindings.size() << std::endl;
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
    // Removes a gate that is just about to be destroyed from
    // the appropriate set of Xamine bound spectra.
    //
    // Formal Paramters:
    //    CGateContainer& rGate:
    //       Reference to the container which holds the gate about to be
    //       destroyed.  Note that for most purposes, a gate container
    //       can be treated as if it was a pointer to a gate.
    //
    UInt_t nGateId = rGate.getNumber();
    GateType_t eType;
    if(rGate->Type() == "c" || rGate->Type() == "gc") {
      eType = kgContour2d;
    }
    else if(rGate->Type() == "b" || rGate->Type() == "gb") {
      eType = kgBand2d;
    }
    else if (rGate->Type() == "s" || rGate->Type() == "gs") {
      eType = kgCut1d;
    }
    else {
      return;			// Non -primitive gates won't be displayed.
    }

    // This function is quite simple since gates entered in Xamine on our
    // behalf will have this id and name.   Therefore we just need
    // to remove gates with id == nGateId from all spectra bound.
    //
    // Note that CXamine::RemoveGate throws on error, and therefore
    // we must catch and ignore exceptions at the removal.

    CDisplay* pDisp = m_pInterface->getCurrentDisplay();

    DisplayBindings bindings = pDisp->getDisplayBindings();
    UInt_t nBindings = bindings.size();
    for(UInt_t nId = 0; nId < nBindings; nId++) {
      if(bindings.at(nId) != "") {
        try {
              pDisp->RemoveGate(nId, nGateId, eType);
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
