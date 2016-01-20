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

    virtual void start();
    virtual void stop();
    virtual Bool_t isAlive();
    virtual void restart();

    SpectrumContainer getBoundSpectra() const;

    virtual UInt_t addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);
    virtual void   removeSpectrum(UInt_t nSpec, CSpectrum& rSpectrum);
    virtual void   removeSpectrum(CSpectrum& rSpectrum);

    virtual void addFit(CSpectrumFit& fit);
    virtual void deleteFit(CSpectrumFit& fit);

    virtual void updateStatistics();

    virtual std::vector<CGateContainer> getAssociatedGates(const std::string& rSpectrum,
                                                       CHistogrammer& rSorter);

    virtual CSpectrum* getSpectrum(UInt_t xid);
    DisplayBindingsIterator DisplayBindingsBegin();
    DisplayBindingsIterator DisplayBindingsEnd();
    UInt_t DisplayBindingsSize() const;
    virtual bool spectrumBound(CSpectrum& rSpectrum);
    Int_t FindDisplayBinding(const std::string& spectrumName);

    virtual std::string createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer &);
    virtual void setInfo(CSpectrum& rSpectrum, std::string name);
    virtual void setTitle(CSpectrum& rSpectrum, std::string name);
    virtual UInt_t getTitleSize() const;

    virtual void addGate(CSpectrum& rSpectrum, CGateContainer& rGate);
    virtual void removeGate(CSpectrum &rSpectrum, CGateContainer &rGate);

    virtual void setOverflows(unsigned slot, unsigned x, unsigned y);
    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y);

};



class CTestDisplayCreator : public CDisplayCreator
{
public:
    CTestDisplay* create();
};

#endif // TESTDISPLAY_H
