#ifndef SPECTRALOCALDISPLAY_H
#define SPECTRALOCALDISPLAY_H

#include <Display.h>
#include <memory>

class CXamineSharedMemory;

namespace Spectra
{

class CSpectraProcess;

class CSpectraLocalDisplay : public CDisplay
{

private:

    std::unique_ptr<CSpectraProcess>     m_pProcess;
    std::unique_ptr<CXamineSharedMemory> m_pMemory;

public:
    CSpectraLocalDisplay(size_t nBytes);
    CSpectraLocalDisplay(const CSpectraLocalDisplay& rhs);

    virtual ~CSpectraLocalDisplay();
    virtual int operator==(const CDisplay& rhs);

    virtual CSpectraLocalDisplay* clone() const;

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

    void startRESTServer();
    void stopRESTServer();




};

}
#endif // SPECTRADISPLAY_H
