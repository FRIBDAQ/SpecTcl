#ifndef TESTDISPLAY_H
#define TESTDISPLAY_H

#include "Display.h"
#include "DisplayFactory.h"

class CSpectrum;

class CTestDisplay : public CDisplay
{
    bool                    m_alive;
    DisplayBindings         m_DisplayBindings;
    std::vector<CSpectrum*> m_boundSpectra;        // Spectrum if bound.
    FitlineBindings         m_FitlineBindings;     // Fitlines bound to displayer.

    static int              m_nextFitlineId;       // Next Xamine fitline id.


public:
    CTestDisplay();

    virtual ~CTestDisplay();

    virtual int operator==(const CDisplay&);

    virtual CTestDisplay* clone() const;

    virtual void Start();
    virtual void Stop();
    virtual Bool_t isAlive();
    virtual void Restart();

    DisplayBindings  getDisplayBindings() const;
    void setDisplayBindings (const DisplayBindings& am_DisplayBindings);

    virtual UInt_t BindToDisplay(CSpectrum& rSpectrum, CHistogrammer& rSorter);
    virtual void   UnBindFromDisplay(UInt_t nSpec, CSpectrum& rSpectrum);

    virtual void addFit(CSpectrumFit& fit);
    virtual void deleteFit(CSpectrumFit& fit);

    virtual void updateStatistics();

    virtual std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum,
                                                       CHistogrammer& rSorter);

    virtual CSpectrum* DisplayBinding(UInt_t xid);
    DisplayBindingsIterator DisplayBindingsBegin();
    DisplayBindingsIterator DisplayBindingsEnd();
    UInt_t DisplayBindingsSize() const;
    virtual Int_t FindDisplayBinding(std::string name);

    virtual std::string createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer &);
    virtual void setInfo(std::string name, UInt_t slot);
    virtual void setTitle(std::string name, UInt_t slot);
    virtual UInt_t getTitleSize() const;

    virtual CDisplayGate* GateToDisplayGate(CSpectrum &rSpectrum, CGateContainer &rGate);
    virtual void EnterGate(CDisplayGate& rGate);
    virtual void RemoveGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType);

    virtual void setOverflows(unsigned slot, unsigned x, unsigned y);
    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y);

};



class CTestDisplayCreator : public CDisplayCreator
{
public:
    CTestDisplay* create();
};

#endif // TESTDISPLAY_H
