/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


#ifndef NULLDISPLAY_H
#define NULLDISPLAY_H

#include "Display.h"
#include "DisplayFactory.h"
#include "XamineSharedMemory.h"
#include <memory>



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

    virtual UInt_t addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);
    virtual UInt_t removeSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);

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
private:
    std::shared_ptr<CXamineSharedMemory> m_pSharedMem;
public:
    CNullDisplayCreator();
    CNullDisplay* create();
    void setSharedMemory(std::shared_ptr<CXamineSharedMemory> pShMem) { m_pSharedMem = pShMem; }
    std::weak_ptr<CXamineSharedMemory> getDisplayBytes() const { return m_pSharedMem; }
};

#endif // NULLDISPLAY_H
