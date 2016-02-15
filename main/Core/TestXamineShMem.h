#ifndef TESTXAMINESHMEM_H
#define TESTXAMINESHMEM_H


#include "XamineSharedMemory.h"
#include "XamineGate.h"

#include <string>
#include <map>

class CSpectrum;


struct BoundSpectrum {
    std::string s_info;
    int         s_slot;
};


class CTestXamineShMem : public CXamineSharedMemory
{
private:
    int m_slotIndex;

    std::map<CSpectrum*, BoundSpectrum>    m_map;
    std::map<int, CXamineGate>             m_gateMap;

public:
    CTestXamineShMem();

    const std::map<CSpectrum*, BoundSpectrum>& boundSpectra() const;
    const std::map<int, CXamineGate>& getGates() const;

    virtual void setManaged(bool value);

    virtual bool isManaged() const;

    virtual void attach();
    virtual void detach();

    virtual size_t getSize() const;

    virtual UInt_t addSpectrum(CSpectrum& , CHistogrammer& );
    virtual void removeSpectrum(UInt_t slot, CSpectrum& );

    virtual DisplayBindings getDisplayBindings() const;

    virtual CXamineSpectrum& operator[](UInt_t n);
    virtual DisplayBindingsIterator displayBindingsBegin();
    virtual DisplayBindingsIterator displayBindingsEnd();

    virtual UInt_t displayBindingsSize() const;
    virtual Int_t findDisplayBinding(std::string name);
    virtual Int_t findDisplayBinding(CSpectrum& rSpectrum);

    virtual void addFit(CSpectrumFit& fit);
    virtual void deleteFit(CSpectrumFit& fit);

    virtual std::string createTitle(CSpectrum& rSpectrum,
                            UInt_t maxLength,
                            CHistogrammer &rSorter);
    virtual UInt_t getTitleSize()  const;
    virtual void setTitle(std::string name, UInt_t slot);
    virtual void setInfo(CSpectrum& rSpectrum, std::string name);
    virtual void setInfo(std::string  name, UInt_t slot);

    virtual void addGate (CXamineGate& rGate);
    virtual void removeGate(CSpectrum& rSpectrum, CGateContainer& rGate) ;
    virtual void removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType) ;
    virtual CXamineGates* GetGates (UInt_t nSpectrum) ;

    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y);
    virtual void setOverflows(unsigned slot, unsigned x, unsigned y);
    virtual void clearStatistics(unsigned slot);
};

#endif // TESTXAMINESHMEM_H
