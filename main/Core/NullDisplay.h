#ifndef NULLDISPLAY_H
#define NULLDISPLAY_H

#include "Display.h"
#include "DisplayFactory.h"


/*!
 * \brief The CNullDisplay class
 *
 * This is intended to be a display that does nothing but doesn't break anything
 * when the rest of SpecTcl tries to interact with it.
 */
class CNullDisplay : public CDisplay
{
public:
    CNullDisplay();
    CNullDisplay(const CNullDisplay& rhs);

    virtual CNullDisplay* clone() const;

    virtual int operator==(const CDisplay&);

    virtual void start();
    virtual void stop();
    virtual bool isAlive();
    virtual void restart();

    virtual void addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);
    virtual void   removeSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);

    virtual void addFit(CSpectrumFit& fit);
    virtual void deleteFit(CSpectrumFit& fit);

    virtual void updateStatistics();

    virtual std::vector<CGateContainer> getAssociatedGates(const std::string& rSpectrum,
                                                       CHistogrammer& rSorter);

    SpectrumContainer getBoundSpectra() const;
    virtual CSpectrum* getSpectrum(UInt_t xid);
    virtual bool spectrumBound(CSpectrum* pSpectrum);
    virtual UInt_t DisplayBindingsSize() const;

    virtual std::string createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer& rSorter);
    virtual void setInfo(CSpectrum& rSpec, std::string name);
    virtual void setTitle(CSpectrum& rSpec, std::string name);
    virtual UInt_t getTitleSize() const;

    virtual void addGate(CSpectrum& rSpectrum, CGateContainer& rGate);
    virtual void removeGate(CSpectrum &rSpectrum, CGateContainer &rGate);
    virtual void setOverflows(unsigned slot, unsigned x, unsigned y);
    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y);
};


class CNullDisplayCreator : public CDisplayCreator
{
    CNullDisplay* create();
};

#endif // NULLDISPLAY_H
