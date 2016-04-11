#include "NullDisplay.h"

#include "GateContainer.h"
#include "Spectrum.h"

// Functions for CNullDisplayCreator
//////////////////////////////////////////////////////////////////////////

CNullDisplay* CNullDisplayCreator::create()
{
    return new CNullDisplay();
}

// Functions for CNullDisplay
//////////////////////////////////////////////////////////////////////////

CNullDisplay::CNullDisplay()
{
}

CNullDisplay::CNullDisplay(const CNullDisplay &rhs)
{
}

CNullDisplay* CNullDisplay::clone() const
{
    return new CNullDisplay(*this);
}

// All null displays are identical by definition.
int CNullDisplay::operator==(const CDisplay& disp)
{ return 1; }

void CNullDisplay::start() {}
void CNullDisplay::stop() {}

// By definition, null displays are alive. They never turn off.
bool CNullDisplay::isAlive() { return true;}
void CNullDisplay::restart() {}

void CNullDisplay::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
{}

void CNullDisplay::removeSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter) {}

void CNullDisplay::addFit(CSpectrumFit &fit) {}
void CNullDisplay::deleteFit(CSpectrumFit &fit) {}

void CNullDisplay::updateStatistics() {}

std::vector<CGateContainer> CNullDisplay::getAssociatedGates(const std::string &rSpectrum, CHistogrammer &rSorter)
{
    return std::vector<CGateContainer>();
}

SpectrumContainer CNullDisplay::getBoundSpectra() const
{
    return SpectrumContainer();
}

CSpectrum* CNullDisplay::getSpectrum(UInt_t xid) { return static_cast<CSpectrum*>(kpNULL); }
bool CNullDisplay::spectrumBound(CSpectrum* pSpectrum) { return false;}
UInt_t CNullDisplay::DisplayBindingsSize() const { return 0; }

std::string CNullDisplay::createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer &rSorter) { return "null";}
void CNullDisplay::setInfo(CSpectrum& rSpec, std::string name) {}
void CNullDisplay::setTitle(CSpectrum& rSpec, std::string name) {}
UInt_t CNullDisplay::getTitleSize() const { return 0; }

void CNullDisplay::addGate(CSpectrum &rSpectrum, CGateContainer &rGate) {}
void CNullDisplay::removeGate(CSpectrum &rSpectrum, CGateContainer& rGate) {}

void CNullDisplay::setOverflows(unsigned slot, unsigned x, unsigned y) {}
void CNullDisplay::setUnderflows(unsigned slot, unsigned x, unsigned y) {}

