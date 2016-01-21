#ifndef XAMINESHAREDMEMORY_H
#define XAMINESHAREDMEMORY_H

#include <histotypes.h>
#include <Xamine.h>
#include "XamineSpectrumIterator.h"

#include <vector>
#include <list>
#include <utility>
#include <string>

//
//  forward references to classes:
//
class CHistogrammer;
class CXamineGate;
class CXamineGates;		// Forward reference.
class CXamineEvent;
class CXamineSpectrum;
class CXamineButton;
class CSpectrum;
class CSpectrumFit;
class CGateContainer;

typedef std::vector<std::string>  DisplayBindings;
typedef DisplayBindings::iterator DisplayBindingsIterator;
typedef std::vector<CSpectrum*>   SpectrumContainer;

class CXamineSharedMemory
{
public:
    typedef std::pair<int, std::string> BoundFitline;
    typedef std::list<BoundFitline>     FitlineList;
    typedef std::vector<FitlineList>    FitlineBindings;

private:

    volatile Xamine_shared*       m_pMemory;
    Bool_t         m_fManaged;  //  Set TRUE if memory management started.
    UInt_t         m_nBytes;    //  Size of shared memory region.

    std::vector<CSpectrum*>       m_boundSpectra;
    DisplayBindings               m_DisplayBindings;
    FitlineBindings         m_FitlineBindings;     // Fitlines bound to displayer.

    static int              m_nextFitlineId;       // Next Xamine fitline id.

public:
    CXamineSharedMemory();
    CXamineSharedMemory(size_t nBytes);

public:
  volatile Xamine_shared* getXamineMemory() const
  {
    return m_pMemory;
  }

  // Mutator (for derivec classes):

public:
  void setXamineMemory (Xamine_shared* pSharedMem)
  {
    m_pMemory = pSharedMem;
  }

  void setManaged(bool value) {
      m_fManaged = value;
  }

  bool isManaged() const {
      return m_fManaged;
  }

    std::string GetMemoryName();

    void MapMemory (const std::string& rsName,
                   UInt_t nBytes=knDefaultSpectrumSize)  ;

    UInt_t addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);

    void removeSpectrum(CSpectrum& rSpectrum);
    void removeSpectrum(UInt_t slot, CSpectrum& rSpectrum);
    CSpectrum* getSpectrum(UInt_t xid);

    DisplayBindings  getDisplayBindings() const;
    SpectrumContainer getBoundSpectra() const;

    CXamineSpectrum&  operator[](UInt_t n);
    CXamineSpectrumIterator begin();
    CXamineSpectrumIterator end();

    DisplayBindingsIterator DisplayBindingsBegin();
    DisplayBindingsIterator DisplayBindingsEnd();
    UInt_t DisplayBindingsSize() const;
    Int_t FindDisplayBinding(std::string name);
    Int_t FindDisplayBinding(CSpectrum& rSpectrum);
    bool spectrumBound(CSpectrum &rSpectrum);

    void addFit(CSpectrumFit& fit);
    void deleteFit(CSpectrumFit& fit);

    std::string createTitle(CSpectrum& rSpectrum,
                            UInt_t maxLength,
                            CHistogrammer &rSorter);
    UInt_t getTitleSize()  const;
    void setTitle(CSpectrum& rSpectrum, std::string name);
    void setTitle(std::string name, UInt_t slot);
    void setInfo(CSpectrum& rSpectrum, std::string name);
    void setInfo(std::string  name, UInt_t slot);


    std::vector<CGateContainer> getAssociatedGates(const std::string& spectrumName,
                                               CHistogrammer& rSorter);

    void addGate (CSpectrum& rSpectrum, CGateContainer& rGate)  ;
    void addGate (CXamineGate& rGate)  ;
    void removeGate(CSpectrum& rSpectrum, CGateContainer& rGate);
    void removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType);
    CXamineGates* GetGates (UInt_t nSpectrum)  ;

    void setUnderflows(unsigned slot, unsigned x, unsigned y);
    void setOverflows(unsigned slot, unsigned x, unsigned y);
    void clearStatistics(unsigned slot);

private:
    Address_t DefineSpectrum (CXamineSpectrum& rSpectrum)  ;
    void FreeSpectrum (UInt_t nSpectrum)  ;

    Xamine_gatetype      MapFromGate_t(GateType_t type);

    void ThrowGateStatus(Int_t nStatus, const CXamineGate& rGate,
                         const std::string& doing);

    CXamineGate* GateToXamineGate(CSpectrum& rSpectrum, CGateContainer& rGate);
    bool flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam);

};

#endif // XAMINESHAREDMEMORY_H
