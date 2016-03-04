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

#include "SpectrumDictionaryFitObserver.h"
#include "CFitDictionary.h"
#include "Spectrum.h"
#include "CSpectrumFit.h"

SpectrumDictionaryFitObserver::SpectrumDictionaryFitObserver()
{
}

void
SpectrumDictionaryFitObserver::onAdd(std::string name, CSpectrum *&item)
{}

void SpectrumDictionaryFitObserver::onRemove(std::string name, CSpectrum *&item)
{
    // I don't like doing this here but I'm really not sure where else to do
    // it.. in an ideal world I'd have observers on the spectrum dictionary
    // and the deletion of a spectrum would trigger the deletion of
    // the corresponding fits... however that sort of internal restruturing
    // is a 4.0 thing.

    CFitDictionary&          Fits(CFitDictionary::getInstance());
    CFitDictionary::iterator iFit  = Fits.begin();
    while (iFit != Fits.end()) {
        CSpectrumFit* pFit = iFit->second;
        if (pFit->getName() == name) {

            Fits.erase(iFit);	// This will also trigger remove from Xamine.
            delete pFit;

            // The iterator has been invalidated potentially so start again:
            //
            // JRT comment  starts: - not sure if this is needed because the fit dict is an associative
            // container... leaving it here for now. Could potentially be removed after
            // successful testing.
            iFit = Fits.begin();
        }
        else {
            iFit++;
        }
    }
}
