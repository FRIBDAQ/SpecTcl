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
    Bool_t                        m_fManaged;  //  Set TRUE if memory management started.
    UInt_t                        m_nBytes;    //  Size of shared memory region.

    DisplayBindings               m_DisplayBindings;
    FitlineBindings               m_FitlineBindings;     // Fitlines bound to displayer.

    static int              m_nextFitlineId;       // Next Xamine fitline id.

public:
    CXamineSharedMemory();
    CXamineSharedMemory(size_t nBytes);
    ~CXamineSharedMemory();


  void setManaged(bool value) {
      m_fManaged = value;
  }

  bool isManaged() const {
      return m_fManaged;
  }

  void attach();
  void detach();

    std::string getMemoryName();

    UInt_t getSize() const {
        return m_nBytes;
    }

    void mapMemory (const std::string& rsName,
                   UInt_t nBytes=knDefaultSpectrumSize)  ;

    UInt_t addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);
    void removeSpectrum(UInt_t slot, CSpectrum& rSpectrum);

    DisplayBindings  getDisplayBindings() const;

    CXamineSpectrum&  operator[](UInt_t n);
    CXamineSpectrumIterator begin();
    CXamineSpectrumIterator end();

    DisplayBindingsIterator displayBindingsBegin();
    DisplayBindingsIterator displayBindingsEnd();
    UInt_t displayBindingsSize() const;
    Int_t findDisplayBinding(std::string name);
    Int_t findDisplayBinding(CSpectrum& rSpectrum);
    bool spectrumBound(CSpectrum &rSpectrum);

    void addFit(CSpectrumFit& fit);
    void deleteFit(CSpectrumFit& fit);

    std::string createTitle(CSpectrum& rSpectrum,
                            UInt_t maxLength,
                            CHistogrammer &rSorter);
    UInt_t getTitleSize()  const;
    void setTitle(std::string name, UInt_t slot);
    void setInfo(CSpectrum& rSpectrum, std::string name);
    void setInfo(std::string  name, UInt_t slot);


    std::vector<CGateContainer> getAssociatedGates(const std::string& spectrumName,
                                               CHistogrammer& rSorter);

    void addGate (CXamineGate& rGate)  ;
    void removeGate(CSpectrum& rSpectrum, CGateContainer& rGate);
    void removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType);
    CXamineGates* GetGates (UInt_t nSpectrum)  ;

    void setUnderflows(unsigned slot, unsigned x, unsigned y);
    void setOverflows(unsigned slot, unsigned x, unsigned y);
    void clearStatistics(unsigned slot);

private:
    Address_t defineSpectrum (CXamineSpectrum& rSpectrum)  ;
    void freeSpectrum (UInt_t nSpectrum)  ;

    Xamine_gatetype      MapFromGate_t(GateType_t type);

    void ThrowGateStatus(Int_t nStatus, const CXamineGate& rGate,
                         const std::string& doing);

protected:
  volatile Xamine_shared* getXamineMemory() const
  {
    return m_pMemory;
  }

  void setXamineMemory (Xamine_shared* pSharedMem)
  {
    m_pMemory = pSharedMem;
  }

};

#endif // XAMINESHAREDMEMORY_H
