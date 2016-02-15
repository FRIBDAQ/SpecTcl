#ifndef XAMINESHAREDMEMORY_H
#define XAMINESHAREDMEMORY_H

#include <daqdatatypes.h>
#include <histotypes.h>


#include <vector>
#include <string>

class CSpectrumFit;
class CSpectrum;
class CXamineSpectrum;
class CHistogrammer;
class CGateContainer;
class CXamineGate;
class CXamineGates;

using DisplayBindings         = std::vector<std::string>;
using DisplayBindingsIterator = DisplayBindings::iterator;
using SpectrumContainer       = std::vector<CSpectrum*>;

class CXamineSharedMemory
{

public:
    virtual void setManaged(bool value) = 0;

    virtual bool isManaged() const = 0;

    virtual void attach() = 0;
    virtual void detach() = 0;

    virtual size_t getSize() const = 0;

    virtual UInt_t addSpectrum(CSpectrum& , CHistogrammer& ) = 0;
    virtual void removeSpectrum(UInt_t slot, CSpectrum& ) = 0;

    virtual DisplayBindings getDisplayBindings() const = 0;

    virtual CXamineSpectrum& operator[](UInt_t n) = 0;
    virtual DisplayBindingsIterator displayBindingsBegin() = 0;
    virtual DisplayBindingsIterator displayBindingsEnd() = 0;

    virtual UInt_t displayBindingsSize() const = 0;
    virtual Int_t findDisplayBinding(std::string name) = 0;
    virtual Int_t findDisplayBinding(CSpectrum& rSpectrum) = 0;

    virtual void addFit(CSpectrumFit& fit) = 0;
    virtual void deleteFit(CSpectrumFit& fit) = 0;

    virtual std::string createTitle(CSpectrum& rSpectrum,
                            UInt_t maxLength,
                            CHistogrammer &rSorter) = 0;
    virtual UInt_t getTitleSize()  const = 0;
    virtual void setTitle(std::string name, UInt_t slot) = 0;
    virtual void setInfo(CSpectrum& rSpectrum, std::string name) = 0;
    virtual void setInfo(std::string  name, UInt_t slot) = 0;

    virtual void addGate (CXamineGate& rGate) = 0;
    virtual void removeGate(CSpectrum& rSpectrum, CGateContainer& rGate)  = 0;
    virtual void removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType)  = 0;
    virtual CXamineGates* GetGates (UInt_t nSpectrum)  = 0;

    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y) = 0;
    virtual void setOverflows(unsigned slot, unsigned x, unsigned y) = 0;
    virtual void clearStatistics(unsigned slot) = 0;

};

#endif // XAMINESHAREDMEMORY_H
