#ifndef SPECTRADISPLAY_H
#define SPECTRADISPLAY_H

#include <Display.h>

class CXamineSharedMemory;

namespace Spectra
{

class CSpectraDisplay : public CDisplay
{

private:
    CXamineSharedMemory *m_pMemory;

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

    virtual void removeSpectrum(CSpectrum& rSpectrum);


    virtual bool spectrumBound(CSpectrum *pSpectrum);

    virtual void addFit(CSpectrumFit &fit);
    virtual void deleteFit(CSpectrumFit &fit);
    virtual void addGate(CSpectrum &rSpectrum, CGateContainer &rGate);
    virtual void removeGate(CSpectrum &rSpectrum, CGateContainer &rGate);
    virtual std::vector<CGateContainer> getAssociatedGates(const std::string &spectrumName,
                                                           CHistogrammer &rSorter);

    virtual std::string createTitle(CSpectrum &rSpectrum, UInt_t maxLength, CHistogrammer &rSorter);
    virtual void setTitle(CSpectrum &rSpectrum, std::string name);
    virtual void setInfo(CSpectrum &rSpectrum, std::string name);
    virtual UInt_t getTitleSize() const;

    virtual void updateStatistics();


};

}
#endif // SPECTRADISPLAY_H
