#ifndef NULLDISPLAY_H
#define NULLDISPLAY_H

#include "Display.h"
#include "DisplayFactory.h"



class CNullDisplay : public CDisplay
{
public:
    CNullDisplay();
    CNullDisplay(const CNullDisplay& rhs);

    virtual CNullDisplay* clone() const;

    virtual int operator==(const CDisplay&);

    virtual void Start();
    virtual void Stop();
    virtual Bool_t isAlive();
    virtual void Restart();

    virtual UInt_t BindToDisplay(CSpectrum& rSpectrum, CHistogrammer& rSorter);
    virtual void   UnBindFromDisplay(UInt_t nSpec, CSpectrum& rSpectrum);

    virtual void addFit(CSpectrumFit& fit);
    virtual void deleteFit(CSpectrumFit& fit);

    virtual void updateStatistics();

    virtual std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum);

    DisplayBindings getDisplayBindings() const;
    void setDisplayBindings(const DisplayBindings &am_DisplayBindings);
    virtual CSpectrum* DisplayBinding(UInt_t xid);
    virtual Int_t FindDisplayBinding(std::string name);
    virtual UInt_t DisplayBindingsSize() const;

    virtual std::string createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer& rSorter);
    virtual void setInfo(std::string name, UInt_t slot);
    virtual void setTitle(std::string name, UInt_t slot);
    virtual UInt_t getTitleSize() const;

    virtual CDisplayGate* GateToDisplayGate(CSpectrum &rSpectrum, CGateContainer &rGate);
    virtual void EnterGate(CDisplayGate& rGate);
    virtual void RemoveGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType);
    virtual void setOverflows(unsigned slot, unsigned x, unsigned y);
    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y);
};


class CNullDisplayCreator : public CDisplayCreator
{
    CNullDisplay* create();
};

#endif // NULLDISPLAY_H
