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

#ifndef PRODUCTIONXAMINESHMEM_H
#define PRODUCTIONXAMINESHMEM_H

#include "XamineSharedMemory.h"
#include <Xamine.h>
#include "XamineSpectrumIterator.h"

#include <vector>
#include <list>
#include <utility>
#include <string>

//
//  forward references to classes:
//
class CHistogrammer;
class CXamineGate;
class CXamineGates;		// Forward reference.
class CXamineEvent;
class CXamineSpectrum;
class CXamineButton;
class CSpectrum;
class CSpectrumFit;
class CGateContainer;

/*!
 * \brief The CProductionXamineShMem class
 *
 * Implementation of CXamineSHaredMemory that is intended to be used for
 * production environments. It uses the api provided by libXamine to
 * create shared memory and then manipulate it in ways that a client is
 * expected to do.
 *
 * In the shared memory region that this manages, the memory is broken up to
 * slots. These slots are indexed and an index uniquely identifies a spectrum.
 */
class CProductionXamineShMem : public CXamineSharedMemory
{
public:
    typedef std::pair<int, std::string> BoundFitline;
    typedef std::list<BoundFitline>     FitlineList;
    typedef std::vector<FitlineList>    FitlineBindings;

private:

    volatile Xamine_shared*       m_pMemory;
    Bool_t                        m_fManaged;  //  Set TRUE if memory management started.
    UInt_t                        m_nBytes;    //  Size of shared memory region.

    DisplayBindings               m_DisplayBindings;
    FitlineBindings               m_FitlineBindings;     // Fitlines bound to displayer.

    static int              m_nextFitlineId;       // Next Xamine fitline id.

public:
    CProductionXamineShMem();
    CProductionXamineShMem(size_t nBytes);
    ~CProductionXamineShMem();


  void setManaged(bool value) {
      m_fManaged = value;
  }

  bool isManaged() const {
      return m_fManaged;
  }

  /*!
   * \brief Attach to the shared memory
   *
   * \throws CErrnoException if it is unable to attach to the shared memory
   */
  void attach();

  /*!
   * \brief Detach from the shared memory
   */
  void detach();

  /*!
   * \brief Makes a call to Xamine_GetMemoryName.
   *
    * \return the name of the shared memory
    */
   std::string getMemoryName() const;

   /*!
    * \brief Maps to a pre-existing shared memory region which communicates
    * with Xamine.
    *
    *  \param   rsNam    Name of the shared memory region
    *  \param   nBytes   Number of bytes of spectrum memory
    *                    must match value in shared memory or
    *                    map fails.
    *
    * \throws CErrnoException on failure
    */
   void mapMemory(const std::string& rsName, UInt_t nBytes);

   /*!
    * \return the number of bytes gauranteed in the shared memory region
    */
   size_t getSize() const {
        return m_nBytes;
    }

    /*!
     * \brief Adds spectrum to shared memory
     *
     * \param rSpectrum     the spectrum
     * \param rSorter       access to the dictionaries
     *
     * Space is allocated in shared memory for the spectrum and then
     * the contents of the spectrum are copied in. The spectrum then
     * swaps its memory with the shared memory. It is directly bound to the
     * shared memory after this point. The gates associated with it are copied
     * into shared memory as are the fits.
     *
     * \return  slot the spectrum is bound to
     */
    UInt_t addSpectrum(CSpectrum& rSpectrum, CHistogrammer& rSorter);

    /*!
     * \brief Remove the spectrum from shared memory
     *
     * \param slot      identifier of spectrum in shared memory
     * \param rSpectrum the SpecTcl-side spectrum
     *
     * The spectrum is deallocated from shared memory.
     * The gates are deallocated from shared memory.
     * The contents of the spectrum in shared memory are copied over.
     *
     *
     */
    void removeSpectrum(UInt_t slot, CSpectrum& rSpectrum);

    /*!
     * \brief Index operator for looking up a spectrum by slot number
     * \param n     slot number
     * \return spectrum stored in the slot
     */
    CXamineSpectrum&  operator[](UInt_t n);

    //////////////////////////////////////////////////////////////////////////
    // Iterator access

    CXamineSpectrumIterator begin();
    CXamineSpectrumIterator end();

    //////////////////////////////////////////////////////////////////////////

    /*!
     * \return list of names bound to the shared memory
     */
    DisplayBindings  getDisplayBindings() const;

    /*!
     * \brief Lookup a bound spectrum by name
     *
     * \param name  name of the spectrum
     * \return slot of the spectrum in shared memory
     */
    Int_t findDisplayBinding(std::string name);

    /*!
     * \brief Lookup a buound spectrum by spectrum
     * \param rSpectrum - the spectrum to find
     * \return slot of the spectrum in shared memory
     */
    Int_t findDisplayBinding(CSpectrum& rSpectrum);


    /*!
     * \brief adds a fit to the Xamine bindings.
     *
     * We keep track of
     * these fits in m_fitlineBindings.  This is a vector of lists.
     * The index of each vector element is the Xamine 'display slot' fitlines
     * are bound to. Each element is a list of pairs.  Each pair is the fitline
     * id and fitline name.
     * \param fit   reference to the fit to add.
    */
    void addFit(CSpectrumFit& fit);

    /*!
     * \brief Removes fit from Xamine bindings
     *
     * \param fit   reference to fit to remove
     */
    void deleteFit(CSpectrumFit& fit);

    /*!
     * \brief Base implementation of createTitle
     *
     * \param rSpectrum     reference to spectrum in question
     * \param maxLength     max length of title
     * \param rSorter       access to the dictionaries
     *
     * This is actually so simple it just returns the name of the spectrum.
     * \return  title
     */
    std::string createTitle(CSpectrum& rSpectrum,
                            UInt_t maxLength,
                            CHistogrammer &rSorter);

    /*!
     * \return Number of bytes shared memory has to store any spectrum title
     */
    UInt_t getTitleSize()  const;

    /*!
     * \brief Set the title for a specific spectrum
     *
     * \param name  the new title
     * \param slot  slot index of the spectrum in shared mem
     */
    void setTitle(std::string name, UInt_t slot);

    /*!
     * \brief Set the info for the specific spectrum in shared memory
     *
     * \param rSpectrum     spectrum whose title will be manipulated
     * \param name          the new information string to use
     */
    void setInfo(CSpectrum& rSpectrum, std::string name);

    /*!
     * \brief Set the info for the specific spectrum in shared memory
     *
     * \param name  the new informatoin string to use
     * \param slot  slot referencing the spectrum in shared memory
     */
    void setInfo(std::string  name, UInt_t slot);

    /*!
     * \brief Add graphical gate to shared memory
     * \param rGate the gate to add
     *
     * The gate is converted into an array of points and then it is copied into
     * the slot available for the spectrum. The spectrum that is associated with
     * is stored as part of the gate passed in.
     */
    void addGate (CXamineGate& rGate);

    /*!
     * \brief Remove gate for shared memory
     *
     * \param nSpectrum
     * \param nId
     * \param eType
     *
     * This doesnt do anything but potentially throw an exception if eType
     * is not valid.
     */
    void removeGate (UInt_t nSpectrum, UInt_t nId, GateType_t eType);

    /*!
     * \brief Retrieves the list of gates associated with the spectrum
     *
     * \param nSpectrum     the spectrum number whose gates are requested
     *
     * Ownership of the returned object is passed to the caller.
     *
     * \return a new CXamineGate object associated with the spectrum id
     */
    CXamineGates* GetGates (UInt_t nSpectrum)  ;

    /*!
     * \brief Write the underflow statistics for a specific spectrum
     *
     * \param slot      the spectrum identifier
     * \param x         underflow for x axis
     * \param y         underflow for y axis
     */
    void setUnderflows(unsigned slot, unsigned x, unsigned y);

    /*!
     * \brief Write the overflow statistics for a specific spectrum
     *
     * \param slot      the spectrum identifier
     * \param x         overflow for x axis
     * \param y         overflow for y axis
     */
    void setOverflows(unsigned slot, unsigned x, unsigned y);

    /*!
     * \brief Reset the statistics for a specific spectrum
     *
     * \param slot      the spectrum identifier
     */
    void clearStatistics(unsigned slot);

private:
    Address_t defineSpectrum (CXamineSpectrum& rSpectrum)  ;
    void freeSpectrum (UInt_t nSpectrum)  ;

    Xamine_gatetype      MapFromGate_t(GateType_t type);

    void ThrowGateStatus(Int_t nStatus, const CXamineGate& rGate,
                         const std::string& doing);

protected:
  volatile Xamine_shared* getXamineMemory() const
  {
    return m_pMemory;
  }

  void setXamineMemory (Xamine_shared* pSharedMem)
  {
    m_pMemory = pSharedMem;
  }

};

#endif // XAMINESHAREDMEMORY_H
