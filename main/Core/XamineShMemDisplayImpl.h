#ifndef XAMINESHMEMDISPLAYIMPL_H
#define XAMINESHMEMDISPLAYIMPL_H

#include "Display.h"

#include <Xamine.h>

#include <string>

#include <histotypes.h>
#include "XamineSpectrumIterator.h"

#include "DisplayFactory.h"

class CXamineGate;
class CXamineGates;		// Forward reference.
class CXamineEvent;
class CXamineSpectrum;
class CXamineButton;
class CXamineSharedMemory;
class CSpectrum;

class CXamineShMemDisplayImpl
{
    CXamineSharedMemory*          m_pMemory;
    std::vector<CSpectrum*>       m_boundSpectra;

public:
    CXamineShMemDisplayImpl(UInt_t nBytes);
    ~CXamineShMemDisplayImpl();

    CXamineSharedMemory* getSharedMemory() { return m_pMemory; }
    const CXamineSharedMemory* getSharedMemory() const { return m_pMemory; }

    void detach();
    void attach();

    void addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);
    void removeSpectrum(UInt_t nSpec, CSpectrum &rSpectrum);
    void removeSpectrum(CSpectrum &rSpectrum);
    SpectrumContainer getBoundSpectra() const;

    void addFit(CSpectrumFit& fit);
    void deleteFit(CSpectrumFit& fit);

    void updateStatistics();

    std::vector<CGateContainer> getAssociatedGates(const std::string& spectrumName,
                                                  CHistogrammer& rSorter);
    CSpectrum* getSpectrum(UInt_t xid);

    bool spectrumBound(CSpectrum *pSpectrum);

    DisplayBindings  getDisplayBindings() const;

    std::string createTitle(CSpectrum& rSpectrum,
                            UInt_t maxLength,
                            CHistogrammer &rSorter);
    UInt_t getTitleSize()  const;
    void setTitle(CSpectrum& rSpectrum, std::string name);
    void setTitle(std::string name, UInt_t slot);
    void setInfo(CSpectrum& rSpectrum, std::string name);
    void setInfo(std::string  name, UInt_t slot);

    // Spectrum statistics.

    void setOverflows(unsigned slot, unsigned x, unsigned y);
    void setUnderflows(unsigned slot, unsigned x, unsigned y);
    void clearStatistics(unsigned slot);

    void addGate (CSpectrum& rSpectrum, CGateContainer& rGate)  ;
    void removeGate(CSpectrum& rSpectrum, CGateContainer& rGate);
    void removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType);
    CXamineGates* GetGates (UInt_t nSpectrum);

private:
    void ThrowGateStatus(Int_t nStatus, const CXamineGate& rGate,
                         const std::string& doing);
};

#endif // XAMINESHMEMDISPLAYIMPL_H
