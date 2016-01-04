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
            // JRT - not sure if this is needed because the fit dict is an associative
            // container... leaving it here for now. Could potentially be removed after
            // successful testing.
            iFit = Fits.begin();
        }
        else {
            iFit++;
        }
    }
}
