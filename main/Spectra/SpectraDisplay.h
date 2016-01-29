#ifndef SPECTRADISPLAY_H
#define SPECTRADISPLAY_H

#include <Display.h>

class CSpectraDisplay : public CDisplay
{
private:
    SpectrumContainer m_boundSpectra;

public:
    CSpectraDisplay();
    CSpectraDisplay(const CSpectraDisplay& rhs);

    virtual ~CSpectraDisplay();
    virtual int operator==(const CDisplay& rhs);

    virtual CSpectraDisplay* clone() const;

    virtual void start();
    virtual void stop();
    virtual bool isAlive();
    virtual void restart();

    virtual SpectrumContainer getBoundSpectra() const;
    virtual void addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter);

    virtual void removeSpectrum(UInt_t nSpec, CSpectrum &rSpectrum);
    virtual void removeSpectrum(CSpectrum& rSpectrum);

    virtual CSpectrum* getSpectrum(UInt_t xid);

    virtual bool spectrumBound(CSpectrum *pSpectrum);

    virtual void addFit(CSpectrumFit &fit);
    virtual void deleteFit(CSpectrumFit &fit);
    virtual void addGate(CSpectrum &rSpectrum, CGateContainer &rGate);
    virtual void removeGate(CSpectrum &rSpectrum, CGateContainer &rGate);
    virtual std::vector<CGateContainer> getAssociatedGates(const std::string &spectrumName,
                                                           CHistogrammer &rSorter);

    virtual UInt_t DisplayBindingsSize() const;

    virtual std::string createTitle(CSpectrum &rSpectrum, UInt_t maxLength, CHistogrammer &rSorter);
    virtual void setTitle(CSpectrum &rSpectrum, std::string name);
    virtual void setInfo(CSpectrum &rSpectrum, std::string name);
    virtual UInt_t getTitleSize() const;

    virtual void updateStatistics();
    virtual void setOverflows(unsigned slot, unsigned x, unsigned y);
    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y);


};

#endif // SPECTRADISPLAY_H
