/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2015.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Jeromy Tompkins
         NSCL
         Michigan State University
         East Lansing, MI 48824-1321
*/


#ifndef DISPLAYINTERFACE_H
#define DISPLAYINTERFACE_H

#include <daqdatatypes.h>
#include <histotypes.h>
#include <xamineDataTypes.h>

#include <string>
#include <vector>
#include <utility>

class CHistogrammer;
class CDisplay;
class CSpectrum;
class CSpectrumFit;
class CDisplayGate;
class CGateContainer;

// Display binding management types:
typedef std::vector<std::string>   DisplayBindings;
typedef DisplayBindings::iterator  DisplayBindingsIterator;

class CDisplayInterface
{
    typedef std::pair<int, std::string> BoundFitline;
    typedef std::list<BoundFitline>     FitlineList;
    typedef std::vector<FitlineList>    FitlineBindings;

private:
    CHistogrammer* m_pSorter;
    CDisplay*      m_pDisplay;
    DisplayBindings     m_DisplayBindings;     // Display id to spectrum name map.
    std::vector<CSpectrum*> m_boundSpectra;        // Spectrum if bound.
    FitlineBindings     m_FitlineBindings;     // Fitlines bound to displayer.

public:
    CDisplayInterface();
    CDisplayInterface(const CDisplayInterface&);
    virtual ~CDisplayInterface();

    const DisplayBindings&  getDisplayBindings() const {
      return m_DisplayBindings;
    }
    void setDisplayBindings (const DisplayBindings& am_DisplayBindings) {
      m_DisplayBindings = am_DisplayBindings;
    }

    DisplayBindingsIterator DisplayBindingsBegin();
    DisplayBindingsIterator DisplayBindingsEnd();
    Int_t findDisplayBinding(std::string name);
    UInt_t DisplayBindingsSize();

    UInt_t BindToDisplay(const std::string& rsName);
    UInt_t UnBindFromDisplay(UInt_t nSpec);
    void addFit(CSpectrumFit& fit);
    void deleteFit(CSpectrumFit& fit);
    void updateStatistics();

    CDisplayGate* GateToXamineGate(UInt_t nBindingId, CGateContainer& rGate);
    void AddGateToBoundSpectra(CGateContainer& rGate);
    void RemoveGateFromBoundSpectra(CGateContainer& rGate);
    std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum);
    CSpectrum* DisplayBinding(UInt_t xid);

    int GetEventFd();


    CDisplay *getDisplay() const;
    void setDisplay(CDisplay *pDisplay);
};

#endif // DISPLAYINTERFACE_H
