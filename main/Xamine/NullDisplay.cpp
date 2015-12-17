#include "NullDisplay.h"

#include "GateContainer.h"
#include "Spectrum.h"

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

void CNullDisplay::Start() {}
void CNullDisplay::Stop() {}

// By definition, null displays are alive. They never turn off.
Bool_t CNullDisplay::isAlive() { return true;}
void CNullDisplay::Restart() {}

UInt_t CNullDisplay::BindToDisplay(CSpectrum &rSpectrum)
{ return 0;}

void CNullDisplay::UnBindFromDisplay(UInt_t nSpec, CSpectrum &rSpectrum) {}

void CNullDisplay::addFit(CSpectrumFit &fit) {}
void CNullDisplay::deleteFit(CSpectrumFit &fit) {}

void CNullDisplay::updateStatistics() {}

void CNullDisplay::AddGateToBoundSpectra(CGateContainer &rGate) {}
void CNullDisplay::RemoveGateFromBoundSpectra(CGateContainer &rGate) {}

std::vector<CGateContainer> CNullDisplay::GatesToDisplay(const std::string &rSpectrum)
{
    return std::vector<CGateContainer>();
}

CSpectrum* CNullDisplay::DisplayBinding(UInt_t xid) { return static_cast<CSpectrum*>(kpNULL); }

DisplayBindingsIterator CNullDisplay::DisplayBindingsBegin() { return DisplayBindingsIterator(); }
DisplayBindingsIterator CNullDisplay::DisplayBindingsEnd() { return DisplayBindingsIterator(); }
UInt_t CNullDisplay::DisplayBindingsSize() { return 0; }
Int_t CNullDisplay::FindDisplayBinding(std::string name) { return -1;}

void CNullDisplay::setInfo(std::string name, UInt_t slot) {}
void CNullDisplay::setTitle(std::string name, UInt_t slot) {}
UInt_t CNullDisplay::getTitleSize() const { return 0; }

void CNullDisplay::EnterGate(CDisplayGate &rGate) {}
CXamineGates* CNullDisplay::GetGates(UInt_t nSpectrum) { return static_cast<CXamineGates*>(kpNULL); }
void CNullDisplay::RemoveGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType) {}

void CNullDisplay::FreeSpectrum(UInt_t nSpectrum) {}
void CNullDisplay::setOverflows(unsigned slot, unsigned x, unsigned y) {}
void CNullDisplay::setUnderflows(unsigned slot, unsigned x, unsigned y) {}

