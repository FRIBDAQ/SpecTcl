//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#ifndef XAMINESHMEMDISPLAYIMPL_H
#define XAMINESHMEMDISPLAYIMPL_H

#include "Display.h"

#include <Xamine.h>


#include <histotypes.h>
#include "XamineSpectrumIterator.h"
#include "DisplayFactory.h"

#include <string>
#include <memory>

class CXamineGate;
class CXamineGates;		// Forward reference.
class CXamineEvent;
class CXamineSpectrum;
class CXamineButton;
class CXamineSharedMemory;
class CSpectrum;

/*!
 * \brief The CXamineShMemDisplayImpl class
 *
 * Both CXamine and CSpectraLocalDisplay need to do just about the same thing to
 * implement the CDisplay interface in terms of a CXamineSharedMemory.
 * Basically, they need to translate between SpecTcl-side information and
 * Xamine shared memory information. A lot of this is translation between
 * a SpecTcl-side object and the slot it is associated  with in shared
 * memory. In any case, this implementation class exists so that the CXamine
 * and CSpectraLocalDisplay classes can delegate to an object that implements the
 * common logic.
 *
 */
class CXamineShMemDisplayImpl
{
    std::shared_ptr<CXamineSharedMemory> m_pMemory;
    std::vector<CSpectrum*>              m_boundSpectra;

public:
    /*!
     * \brief Constructor
     *
     * \param pMemory   shared memory to manipulate
     */
    CXamineShMemDisplayImpl(std::shared_ptr<CXamineSharedMemory> pMemory);

    /*!
     * \brief Shallow copy
     * \param rhs   object whose state will be copied
     */
    CXamineShMemDisplayImpl( const CXamineShMemDisplayImpl& rhs );

    /*!
     * Empty implementation
     */
    ~CXamineShMemDisplayImpl();

    /*!
     * \brief Obtain a pointer to the shared memory
     *
     *  The weak pointer means that the object can expire. It is essentially
     *  a non-owning reference to the object.
     *
     * \return  weak pointer to the shared memory
     */
    std::weak_ptr<CXamineSharedMemory> getSharedMemory() const { return m_pMemory; }

    // The following two methods delegate directly to m_pMemory
    /*!
     * \brief Detach from shared memory
     *
     * After the user is done with the shared memory, it is important to detach
     * from it so that there is no resource leak.
     *
     */
    void detach();

    /*!
     * \brief Attach to the shared memory
     *
     *  Before the majority of the methods in this class can be successfully called,
     * the shared memory region must be attached to.
     */
    void attach();

    /*!
     * \brief Add spectrum to shared memory
     *
     * \param rSpectrum  the spectrum to bind
     * \param rSorter    access to the dictionaries
     *
     * \throw  CDictionaryException - if spectrum of given name does not exist.
     * \throw  CErrnoException      - may be thrown by routines we call.
     *
     * The spectrum is added to the shared memory by the shared memory
     * addSpectrum method. Next the info is set using the createTitle of this
     * class to produce a very verbose informative string about the histogram.
     * The associated gates are then added to the shared memory as well as the
     * fits.
     */
    void addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);

    /*!
     * \brief Remove the spectrum from shared memory
     *
     * \param nSpec         index of spectrum in shared memory
     * \param rSpectrum     SpecTcl-side spectrum
     *
     * If the spectrum is bound, it is removed using the removeSpectrum
     * method of the shared memory object we are dealing with.
     */
    void removeSpectrum(UInt_t nSpec, CSpectrum &rSpectrum);

    /*!
     * \brief Remove the spectrum from shared memory
     *
     * \param rSpectrum     referenece to the spectrum
     */
    void removeSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter);

    /*!
     * \brief Retrieve the list of spectra that are bound to the shared memory
     *
     * \return  copied list of bound spectra
     */
    SpectrumContainer getBoundSpectra() const;

    /*!
     * \brief Update the underflow and overflows for all bound spectra
     */
    void updateStatistics();

    /*!
     * \brief Check whether a spectrum is bound
     *
     * \param pSpectrum     pointer to spectrum being inquired of
     *
     * \return boolean
     * \retval true - spectrum is bound
     * \retval false - otherwise
     */
    bool spectrumBound(CSpectrum *pSpectrum);

    /*!
     * \return list of spectrum names that are bound
     */
    DisplayBindings  getDisplayBindings() const;

    /*!
     * \brief Generate a verbose title that contains the informaton
     *        about the spectrum
     *
     * \param rSpectrum     reference to the spectrum
     * \param maxLength     max allowed length for title
     * \param rSorter       access to dictionaries
     *
     * \return the generated title
     */
    std::string createTitle(CSpectrum& rSpectrum,
                            UInt_t maxLength,
                            CHistogrammer &rSorter);

    /*!
     * \return Max length of title string supported by shared memory
     */
    UInt_t getTitleSize()  const;

    /*!
     * \brief Set title for the spectrum in shared memory
     *
     * \param rSpectrum     reference to target spectrum
     * \param name          the new title
     *
     * This looks up the slot for the spectrum in shared memory first.
     *
     * \throw std::runtime_error if spectrum not bound already
     */
    void setTitle(CSpectrum& rSpectrum, std::string name);

    /*!
     * \brief Set title for a spectrum in shared memory
     *
     * \param name          the new title
     * \param slot          slot index of spectrum in shared memory
     */
    void setTitle(std::string name, UInt_t slot);

    /*!
     * \brief Set the info for a spectrum in shared memory
     *
     * \param rSpectrum     reference to target spectrum
     * \param name          the new information string
     *
     */
    void setInfo(CSpectrum& rSpectrum, std::string name);

    /*!
     * \brief Set the info for a spectrum in shared memory
     *
     * \param name          the new information string
     * \param slot          slot index of spectrum in shared memory
     */
    void setInfo(std::string  name, UInt_t slot);

    // Spectrum statistics.

    /*!
     * \brief Set the overflows for a specific spectrum
     *
     * \param slot   slot index of spectrum in shared memory
     * \param x      overflow value of x axis
     * \param y      overflow value of y axis
     */
    void setOverflows(unsigned slot, unsigned x, unsigned y);

    /*!
     * \brief Set the underflows for a specific spectrum
     *
     * \param slot   slot index of spectrum in shared memory
     * \param x      underflow value of x axis
     * \param y      underflow value of y axis
     */
    void setUnderflows(unsigned slot, unsigned x, unsigned y);


    /*!
     * \brief Set the overflow and underflow values to 0 for spectrum
     * \param slot  slot index of spectrum in shared memory
     */
    void clearStatistics(unsigned slot);

};

#endif // XAMINESHMEMDISPLAYIMPL_H
