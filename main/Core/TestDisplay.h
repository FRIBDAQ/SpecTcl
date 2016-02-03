#ifndef TESTDISPLAY_H
#define TESTDISPLAY_H

#include "Display.h"
#include "DisplayFactory.h"
#include <set>

class CSpectrum;

class CTestDisplay : public CDisplay
{
    bool                     m_alive;
    std::set<CSpectrum*>     m_boundSpectra;        // Spectrum if bound.
    std::set<CSpectrumFit*>  m_fits;

public:
    CTestDisplay();

    virtual ~CTestDisplay();

    virtual int operator==(const CDisplay&);

    virtual CTestDisplay* clone() const;

    virtual void start();
    virtual void stop();
    virtual bool isAlive();
    virtual void restart();

    SpectrumContainer getBoundSpectra() const;

    virtual void addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);
    virtual void removeSpectrum(CSpectrum& rSpectrum);

    virtual void addFit(CSpectrumFit& fit);
    virtual void deleteFit(CSpectrumFit& fit);

    virtual void updateStatistics();

    virtual std::vector<CGateContainer> getAssociatedGates(const std::string& rSpectrum,
                                                       CHistogrammer& rSorter);

    virtual bool spectrumBound(CSpectrum *pSpectrum);

    virtual std::string createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer &);
    virtual void setInfo(CSpectrum& rSpectrum, std::string name);
    virtual void setTitle(CSpectrum& rSpectrum, std::string name);
    virtual UInt_t getTitleSize() const;

    virtual void addGate(CSpectrum& rSpectrum, CGateContainer& rGate);
    virtual void removeGate(CSpectrum &rSpectrum, CGateContainer &rGate);

};



class CTestDisplayCreator : public CDisplayCreator
{

public:
    CTestDisplay* create();

};

#endif // TESTDISPLAY_H
