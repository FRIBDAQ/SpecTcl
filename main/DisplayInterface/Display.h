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

    virtual int operator==(const CDisplay&) = 0;

    /*!
     * \brief clone
     *
     * Virtual copy constructor. The ownership transfers to the caller.
     *
     * \return a new display object
     */
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

    /*!
     * \brief BindToDisplay
     *
     *  Adds a spectrum to the list of graphical entities managed by the display.
     *  Binding the spectrum may involve processing the display to create a displayable
     *  version of the spectrum.
     *
     * \param rSpectrum - the spectrum to bind
     * \param rSorter   - the histogrammer to gain access to parameter, gate, spectrum dicts
     * \return integer - display binding
     */
    virtual UInt_t BindToDisplay(CSpectrum& rSpectrum, CHistogrammer& rSorter) = 0;

    /*!
     * \brief UnBindFromDisplay
     *
     * Accomplishes the opposite of the BindToDisplay method. A spectrum is removed from the
     * list of display binding. THe spectrum is required because in  the Xamine display,
     * the memory associated with bin contents is swapped out.
     *
     * \param nSpec     - display binding of the spectrum to remove
     * \param rSpectrum - SpecTcl spectrum being unbound
     */
    virtual void   UnBindFromDisplay(UInt_t nSpec, CSpectrum& rSpectrum) = 0;

    /*!
     * \brief addFit
     *
     * Similarly to the BindToDisplay, the addFit method binds a fit to a display.
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
     * \brief updateStatistics
     *
     * Causes the statistics for the display to be updated.
     */
    virtual void updateStatistics() = 0;

    /*!
     * \brief GatesToDisplay
     *
     * Retrieves the list of all displayable gates for a certain spectrum. The histogrammer is necessary
     * to have access to the gate and spectrum dictionaries.
     *
     * \param spectrumName - the name of the spectrum
     * \param rSorter      - the histogrammer
     *
     * \return vector of gates that can be displayed on the spectrum
     */
    virtual std::vector<CGateContainer> GatesToDisplay(const std::string& spectrumName,
                                                       CHistogrammer& rSorter) = 0;

    /*!
     * \brief DisplayBinding
     *
     * Retrieve the spectrum by means of the display binding.
     *
     * \param xid
     *
     * \return CSpectrum*
     * \retval pointer to the spectrum
     * \retval NULL if xid out of range or does not map to spectrum
     */
    virtual CSpectrum* DisplayBinding(UInt_t xid) = 0;

    /*!
     * \brief FindDisplayBinding
     *
     * Look up the display binding by the spectrum name
     *
     * \param name - name of spectrum
     *
     * \return display binding
     * \retval >=0 if binding exists
     * \retval -1  binding does not exist
     */
    virtual Int_t FindDisplayBinding(std::string name) = 0;

    /*!
     * \brief DisplayBindingsSize
     * \return the number of display bindings that have been created for all time
     */
    virtual UInt_t DisplayBindingsSize() const = 0;

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
    virtual std::string createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer& rSorter) = 0;

    /*!
     * \brief setInfo
     *
     * Sets the info line for a spectrum identified by slot
     *
     * \param name - the text to use
     * \param slot - slot (i.e. display binding) containing spectrum of interest
     */
    virtual void setInfo(std::string name, UInt_t slot) = 0;

    /*!
     * \brief setTitle
     *
     * Sets the title of the spectrum at a certain slot (i.e. display binding)
     *
     * \param name - text to set the title to
     * \param slot - slot identifying the spectrum
     */
    virtual void setTitle(std::string name, UInt_t slot) = 0;

    /*!
     * \brief getTitleSize
     *
     * Retrieves the number of characters that are allowed to be in the title
     *
     * \return integer
     */
    virtual UInt_t getTitleSize() const = 0;

//    /*!
//     * \brief GateToDisplayGate
//     *
//     * Convert a SpecTcl gate into a gate that can be displayed on the given display.
//     * If the spectrum is not bound to the display, then this will blow up via an
//     * assert().
//     *
//     * \param rSpectrum - the spectrum to create the gate for
//     * \param rGate     - the gate to add
//     *
//     * \retval kpNULL -- if the gate not convertable
//     *
//     * \note the gate is dynamically allocated and therefore must be deleted by the client
//     */
//    virtual CDisplayGate* GateToDisplayGate(CSpectrum& rSpectrum, CGateContainer& rGate) = 0;

    /*!
     * \brief EnterGate
     *
     * Add a display gate to the display
     *
     * \param rGate - the gate to be added
     */
    virtual void EnterGate(CSpectrum& rSpectrum, CGateContainer& rGate) = 0;

    /*!
     * \brief RemoveGate
     *
     * Remove the gate from the spectrum identified.
     *
     * \param nSpectrum - Number of spectra from which remove the gate
     * \param nId       - Identification of the spectrum
     * \param eType     - Type of the gate to remove
     */
    virtual void RemoveGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType) = 0;

    /*!
     * \brief setOverflows
     *
     * Updates the values of the overflows in the display
     *
     * \param slot - identifies the spectrum
     * \param x    - overflow for the x axis
     * \param y    - overflow for the y axis
     *
     *
     */
    virtual void setOverflows(unsigned slot, unsigned x, unsigned y) = 0;

    /*!
     * \brief setUnderflows
     *
     * Updates the values of the underflows in the display
     *
     * \param slot - identifies the spectrum
     * \param x    - underflow for the x axis
     * \param y    - underflow for the y axis
     *
     *
     */
     virtual void setUnderflows(unsigned slot, unsigned x, unsigned y) = 0;
};

#endif // DISPLAY_H
