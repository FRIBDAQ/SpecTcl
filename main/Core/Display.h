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
typedef std::vector<CSpectrum*>    SpectrumContainer;

/*!
 * \brief The CDisplay class
 *
 * This is the interface for a display used in the SpecTcl. The Xamine
 * display is a concrete implementation of this class. The rest of
 * SpecTcl does not care about the type of display that is being used
 * as long as it implements this interface.
 */
class CDisplay
{
public:
    typedef std::pair<int, std::string> BoundFitline;
    typedef std::list<BoundFitline>     FitlineList;
    typedef std::vector<FitlineList>    FitlineBindings;

public:
    virtual ~CDisplay();

//    virtual int operator==(const CDisplay&) = 0;

    /*!
     * \brief clone
     *
     * Virtual copy constructor. The ownership transfers to the caller.
     *
     * \return a new display object
     */
    virtual CDisplay* clone() const = 0;

    /*!
     * \brief start
     *
     * This method should be used to cause the display to start functioning
     */
    virtual void start() = 0;

    /*!
     * \brief stop
     *
     * This should cause the display to stop functioning.
     */
    virtual void stop() = 0;

    /*!
     * \brief isAlive
     *
     * Checks whether the display is running or not.
     *
     * \return boolean
     * \retval 0 - display is not running
     * \retval 1 - display is running
     */
    virtual bool isAlive() = 0;

    /*!
     * \brief restart
     *
     * Causes the display to stop and then start. There may be some logic that
     * needs to be done between the stop and start depending on the display.
     */
    virtual void restart() = 0;

    /*!
     * \brief getDisplayBindings
     *
     * This returns a snapshot of the display  bindings at the time the method is called.
     * The display should maintain a list of the graphical entities that are bound to it
     * and this method should return said list.
     *
     * \return DisplayBindings - the names of graphical entities bound to the display
     */
    virtual SpectrumContainer getBoundSpectra() const = 0;

    /*!
     * \brief addSpectrum
     *
     *  Adds a spectrum to the list of graphical entities managed by the display.
     *  Binding the spectrum may involve processing the display to create a displayable
     *  version of the spectrum. This is should succeed or else throw an exception.
     *
     * \param rSpectrum - the spectrum to bind
     * \param rSorter   - the histogrammer to gain access to parameter, gate, spectrum dicts
     *
     */
    virtual void addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter) = 0;

    /*!
     * \brief removeSpectrum
     *
     * Accomplishes the opposite of the addSpectrum method. A spectrum is removed from the
     * list of display binding. THe spectrum is required because in  the Xamine display,
     * the memory associated with bin contents is swapped out.
     *
     * \param rSpectrum - SpecTcl spectrum being unbound
     *
     * \todo Get rid of any need for the first version of this.
     */
    virtual void removeSpectrum(CSpectrum& rSpectrum) = 0;

    /*!
     * \brief spectrumExists
     *
     * Look up the display binding by the spectrum name
     *
     * \param pSpectrum - address of a spectrum
     *
     * \return display binding
     * \retval >=0 if binding exists
     * \retval -1  binding does not exist
     */
    virtual bool spectrumBound(CSpectrum* pSpectrum) = 0;

    /*!
     * \brief addFit
     *
     * Similarly to the addGate, the addFit method binds a fit to a display.
     *
     * \param fit - the fit
     */
    virtual void addFit(CSpectrumFit& fit) = 0;

    /*!
     * \brief deleteFit
     *
     * Removes a fit from the display
     *
     * \param fit - fit to remove
     */
    virtual void deleteFit(CSpectrumFit& fit) = 0;


    /*!
     * \brief addGate
     *
     * Add a display gate to the display
     *
     * \param rGate - the gate to be added
     */
    virtual void addGate(CSpectrum& rSpectrum, CGateContainer& rGate) = 0;

    /*!
     * \brief removeGate
     *
     * Remove the gate from the spectrum identified.
     *
     * \param rSpectrum - Spectrum to remove gate from
     * \param rGate     - the gate to remove
     */
    virtual void removeGate(CSpectrum& rSpectrum, CGateContainer& rGate) = 0;


    /*!
     * \brief getAssociatedGates
     *
     * Retrieves the list of all displayable gates for a certain spectrum. The histogrammer is necessary
     * to have access to the gate and spectrum dictionaries.
     *
     * \param spectrumName - the name of the spectrum
     * \param rSorter      - the histogrammer
     *
     * \return vector of gates that can be displayed on the spectrum
     */
    virtual std::vector<CGateContainer> getAssociatedGates(const std::string& spectrumName,
                                                           CHistogrammer& rSorter) = 0;


    /*!
     * \brief createTitle
     *
     * This creates a title for a certain spectrum. The display has responsibility for
     * doing this because it alone knows the space that it has to create the title in.
     *
     * \param rSpectrum - The spectrum the title will apply to
     * \param maxLength - Max length of the title
     * \param rSorter   - histogrammer to gain access to the dictionaries
     * \return
     */
    virtual std::string createTitle(CSpectrum& rSpectrum, UInt_t maxLength,
                                    CHistogrammer& rSorter) = 0;

    /*!
     * \brief setInfo
     *
     * Sets the info line for a spectrum identified by slot
     *
     * \param rSpectrum the spectrum to assign to
     * \param name - the text to use
     */
    virtual void setInfo(CSpectrum& rSpectrum, std::string name) = 0;

    /*!
     * \brief setTitle
     *
     * Sets the title of the spectrum at a certain slot (i.e. display binding)
     *
     * \param rSpectrum - the spectrum to apply this to
     * \param name - text to set the title to
     */
    virtual void setTitle(CSpectrum& rSpectrum, std::string name) = 0;

    /*!
     * \brief getTitleSize
     *
     * Retrieves the number of characters that are allowed to be in the title
     *
     * \return integer
     */
    virtual UInt_t getTitleSize() const = 0;

    /*!
     * \brief updateStatistics
     *
     * Causes the statistics for the display to be updated.
     */
    virtual void updateStatistics() = 0;
};

#endif // DISPLAY_H
