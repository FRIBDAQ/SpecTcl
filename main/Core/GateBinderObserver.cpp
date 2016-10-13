//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

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
