#include "TestXamineShMem.h"
#include "Spectrum.h"
#include "GateContainer.h"



CTestXamineShMem::CTestXamineShMem()
    : m_slotIndex(0)
{
}

const std::map<CSpectrum*, BoundSpectrum>& CTestXamineShMem::boundSpectra() const
{
    return m_map;
}

const std::map<int, CXamineGate>& CTestXamineShMem::getGates() const
{
    return m_gateMap;
}

void CTestXamineShMem::setManaged(bool value) {}

bool CTestXamineShMem::isManaged() const {}

void CTestXamineShMem::attach() {}
void CTestXamineShMem::detach() {}

size_t CTestXamineShMem::getSize() const {}

UInt_t CTestXamineShMem::addSpectrum(CSpectrum& spec, CHistogrammer& sorter) {

    BoundSpectrum bs;
    bs.s_info = spec.getName();
    bs.s_slot = m_slotIndex;
    m_map[&spec] = bs;

    m_slotIndex++;
}

void CTestXamineShMem::removeSpectrum(UInt_t slot, CSpectrum& ) {}

DisplayBindings CTestXamineShMem::getDisplayBindings() const {}

CXamineSpectrum& CTestXamineShMem::operator[](UInt_t n) {}
DisplayBindingsIterator CTestXamineShMem::displayBindingsBegin() {}
DisplayBindingsIterator CTestXamineShMem::displayBindingsEnd() {}

UInt_t CTestXamineShMem::displayBindingsSize() const { return 0;}
Int_t CTestXamineShMem::findDisplayBinding(std::string name) {return 0; }
Int_t CTestXamineShMem::findDisplayBinding(CSpectrum& rSpectrum) { return 0;}

void CTestXamineShMem::addFit(CSpectrumFit& fit) {}
void CTestXamineShMem::deleteFit(CSpectrumFit& fit) {}

std::string CTestXamineShMem::createTitle(CSpectrum& rSpectrum,
                        UInt_t maxLength,
                                          CHistogrammer &rSorter) { return std::string(); }
UInt_t CTestXamineShMem::getTitleSize()  const { return 128;}
void CTestXamineShMem::setTitle(std::string name, UInt_t slot) {}
void CTestXamineShMem::setInfo(CSpectrum& rSpectrum, std::string name) {}
void CTestXamineShMem::setInfo(std::string  name, UInt_t slot) {}


void CTestXamineShMem::addGate (CXamineGate& rGate) {
    auto candidate = std::make_pair(rGate.getId(), CXamineGate(rGate));
    m_gateMap.insert(m_gateMap.begin(), candidate);
}

void CTestXamineShMem::removeGate(CSpectrum& rSpectrum, CGateContainer& rGate)  {
    auto it = m_gateMap.find(rGate.getNumber());
    if (it != m_gateMap.end()) {
        m_gateMap.erase(it);
    }
}

void CTestXamineShMem::removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType)  {
    auto it = m_gateMap.find(nSpectrum);
    if (it != m_gateMap.end()) {
        m_gateMap.erase(it);
    }
}

CXamineGates* CTestXamineShMem::GetGates (UInt_t nSpectrum)  {

}

void CTestXamineShMem::setUnderflows(unsigned slot, unsigned x, unsigned y) {}
void CTestXamineShMem::setOverflows(unsigned slot, unsigned x, unsigned y) {}
void CTestXamineShMem::clearStatistics(unsigned slot) {}
