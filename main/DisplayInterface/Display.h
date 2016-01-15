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


#ifndef DISPLAY_H
#define DISPLAY_H

#include <daqdatatypes.h>
#include <histotypes.h>

#include <string>
#include <vector>
#include <list>

class CDisplayGate;
class CSpectrum;
class CSpectrumFit;
class CGateContainer;
class CHistogrammer;

// Display binding management types:
typedef std::vector<std::string>   DisplayBindings;
typedef DisplayBindings::iterator  DisplayBindingsIterator;

class CDisplay
{
public:
    typedef std::pair<int, std::string> BoundFitline;
    typedef std::list<BoundFitline>     FitlineList;
    typedef std::vector<FitlineList>    FitlineBindings;

public:
    virtual ~CDisplay();

    virtual int operator==(const CDisplay&) = 0;

    virtual CDisplay* clone() const = 0;

    /*!
     * \brief Start
     *
     * This method should be used to cause the display to start functioning
     */
    virtual void Start() = 0;

    /*!
     * \brief Stop
     *
     * This should cause the display to stop functioning.
     */
    virtual void Stop() = 0;

    /*!
     * \brief isAlive
     *
     * Checks whether the display is running or not.
     *
     * \return boolean
     * \retval 0 - display is not running
     * \retval 1 - display is running
     */
    virtual Bool_t isAlive() = 0;

    /*!
     * \brief Restart
     *
     * Causes the display to stop and then start. There may be some logic that
     * needs to be done between the stop and start depending on the display.
     */
    virtual void Restart() = 0;

    /*!
     * \brief getDisplayBindings
     *
     * This returns a snapshot of the display  bindings at the time the method is called.
     * The display should maintain a list of the graphical entities that are bound to it
     * and this method should return said list.
     *
     * \return DisplayBindings - the names of graphical entities bound to the display
     */
    virtual DisplayBindings getDisplayBindings() const = 0;

    virtual UInt_t BindToDisplay(CSpectrum& rSpectrum, CHistogrammer& rSorter) = 0;
    virtual void   UnBindFromDisplay(UInt_t nSpec, CSpectrum& rSpectrum) = 0;

    virtual void addFit(CSpectrumFit& fit) = 0;
    virtual void deleteFit(CSpectrumFit& fit) = 0;

    virtual void updateStatistics() = 0;

    virtual std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum, CHistogrammer& rSorter) = 0;

    virtual CSpectrum* DisplayBinding(UInt_t xid) = 0;
    virtual Int_t FindDisplayBinding(std::string name) = 0;
    virtual UInt_t DisplayBindingsSize() const = 0;

    virtual std::string createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer& rSorter) = 0;
    virtual void setInfo(std::string name, UInt_t slot) = 0;
    virtual void setTitle(std::string name, UInt_t slot) = 0;
    virtual UInt_t getTitleSize() const = 0;

    virtual CDisplayGate* GateToDisplayGate(CSpectrum& rSpectrum, CGateContainer& rGate) = 0;
    virtual void EnterGate(CDisplayGate& rGate) = 0;
    virtual void RemoveGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType) = 0;

    virtual void setOverflows(unsigned slot, unsigned x, unsigned y) = 0;
    virtual void setUnderflows(unsigned slot, unsigned x, unsigned y) = 0;
};

#endif // DISPLAY_H
