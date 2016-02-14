#include "TestXamineShMem.h"

CTestXamineShMem::CTestXamineShMem()
{
}

void CTestXamineShMem::setManaged(bool value) {}

bool CTestXamineShMem::isManaged() const {}

void CTestXamineShMem::attach() {}
void CTestXamineShMem::detach() {}

size_t CTestXamineShMem::getSize() const {}

UInt_t CTestXamineShMem::addSpectrum(CSpectrum& , CHistogrammer& ) {}
void CTestXamineShMem::removeSpectrum(UInt_t slot, CSpectrum& ) {}

DisplayBindings CTestXamineShMem::getDisplayBindings() const {}

CXamineSpectrum& CTestXamineShMem::operator[](UInt_t n) {}
DisplayBindingsIterator CTestXamineShMem::displayBindingsBegin() {}
DisplayBindingsIterator CTestXamineShMem::displayBindingsEnd() {}

UInt_t CTestXamineShMem::displayBindingsSize() const {}
Int_t CTestXamineShMem::findDisplayBinding(std::string name) {}
Int_t CTestXamineShMem::findDisplayBinding(CSpectrum& rSpectrum) {}

void CTestXamineShMem::addFit(CSpectrumFit& fit) {}
void CTestXamineShMem::deleteFit(CSpectrumFit& fit) {}

std::string CTestXamineShMem::createTitle(CSpectrum& rSpectrum,
                        UInt_t maxLength,
                        CHistogrammer &rSorter) {}
UInt_t CTestXamineShMem::getTitleSize()  const {}
void CTestXamineShMem::setTitle(std::string name, UInt_t slot) {}
void CTestXamineShMem::setInfo(CSpectrum& rSpectrum, std::string name) {}
void CTestXamineShMem::setInfo(std::string  name, UInt_t slot) {}


std::vector<CGateContainer> CTestXamineShMem::getAssociatedGates(const std::string& spectrumName,
                                           CHistogrammer& rSorter) {}

void CTestXamineShMem::addGate (CXamineGate& rGate) {}
void CTestXamineShMem::removeGate(CSpectrum& rSpectrum, CGateContainer& rGate)  {}
void CTestXamineShMem::removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType)  {}
CXamineGates* CTestXamineShMem::GetGates (UInt_t nSpectrum)  {}

void CTestXamineShMem::setUnderflows(unsigned slot, unsigned x, unsigned y) {}
void CTestXamineShMem::setOverflows(unsigned slot, unsigned x, unsigned y) {}
void CTestXamineShMem::clearStatistics(unsigned slot) {}
