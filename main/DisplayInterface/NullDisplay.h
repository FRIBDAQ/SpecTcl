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

    virtual UInt_t BindToDisplay(CSpectrum& rSpectrum);
    virtual void   UnBindFromDisplay(UInt_t nSpec, CSpectrum& rSpectrum);

    virtual void addFit(CSpectrumFit& fit);
    virtual void deleteFit(CSpectrumFit& fit);

    virtual void updateStatistics();

    virtual void AddGateToBoundSpectra(CGateContainer& rGate);
    virtual void RemoveGateFromBoundSpectra(CGateContainer& rGate);

    virtual std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum);

    virtual CSpectrum* DisplayBinding(UInt_t xid);
    virtual DisplayBindingsIterator DisplayBindingsBegin();
    virtual DisplayBindingsIterator DisplayBindingsEnd();
    virtual UInt_t DisplayBindingsSize();
    virtual Int_t FindDisplayBinding(std::string name);

    virtual void setInfo(std::string name, UInt_t slot);
    virtual void setTitle(std::string name, UInt_t slot);
    virtual UInt_t getTitleSize() const;

    virtual void EnterGate(CDisplayGate& rGate);
    virtual void RemoveGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType);

    virtual void FreeSpectrum(UInt_t nSpectrum);

    virtual void setOverflows(unsigned slot, unsigned x, unsigned y);
    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y);
};


class CNullDisplayCreator : public CDisplayCreator
{
    CNullDisplay* create();
};

#endif // NULLDISPLAY_H
