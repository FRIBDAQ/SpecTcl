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

#include "ProductionXamineShMem.h"

#include "dispshare.h"
#include "XamineGate.h"
#include "XamineGates.h"
#include "XamineGateException.h"
#include "XamineButtonException.h"
#include "XamineEvent.h"
#include "XamineSpectrum.h"
#include "Xamine1D.h"
#include "Xamine2D.h"
#include "XamineSpectrumIterator.h"
#include "XamineNoPrompt.h"
#include "XamineConfirmPrompt.h"
#include "XamineFilePrompt.h"
#include "XaminePointsPrompt.h"
#include "XamineTextPrompt.h"
#include "XamineSpectrumPrompt.h"

#include "Spectrum.h"
#include <Histogrammer.h>
#include <Dictionary.h>
#include <DictionaryException.h>
#include <CFitDictionary.h>
#include <Histogrammer.h>
#include <CSpectrumFit.h>
#include <XamineGates.h>
#include <PointlistGate.h>
#include <Cut.h>
#include <Gamma2DW.h>
#include <GateMediator.h>

#include "ErrnoException.h"
#include <stdexcept>
#include <iostream>
#include <assert.h>

using namespace std;

//
// External References:
//
extern "C" {
  static Xamine_shared* Xamine_memory;	   // Pointer to shared memory.
}
extern   int            Xamine_newgates;  // fd for events.


int CProductionXamineShMem::m_nextFitlineId(1); // Next fitline id assigned.

CProductionXamineShMem::CProductionXamineShMem()
    :
     m_pMemory(nullptr),
     m_fManaged(kfFALSE),
     m_nBytes(0),
     m_DisplayBindings(),
     m_FitlineBindings()
{
    m_pMemory = Xamine_memory;
}

/////////////////////////////////////////////////////////////////////////
//
// Function:
//    CXamineSharedMemory::CXamineSharedMemory(UInt_t nBytes)
// Operation Type:
//    Parameterized Constructor.
//
CProductionXamineShMem::CProductionXamineShMem(size_t nBytes) :
    m_pMemory(nullptr),
    m_fManaged(kfFALSE),
    m_nBytes(nBytes),
    m_DisplayBindings(),
    m_FitlineBindings()
{
    attach();
}

CProductionXamineShMem::~CProductionXamineShMem()
{
    detach();
    m_pMemory = nullptr;
}

void CProductionXamineShMem::attach()
{
    // abort if we are already attached
    if (m_pMemory) return;

    if(!Xamine_CreateSharedMemory(m_nBytes,
                                 (volatile Xamine_shared**)&m_pMemory)) {
      throw CErrnoException("Failed to create Xamine shared memory!!");
    }
    m_fManaged = kfFALSE;
}

void CProductionXamineShMem::detach()
{
    Xamine_DetachSharedMemory();
}

//
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    std::string GetMemoryName (  )
//  Operation Type:
//     Selector
//
std::string
CProductionXamineShMem::getMemoryName() const
{
// Returns the name of the shared memory
// segment created for communication with
// Xamine.
//
  char name[33];
  std::string sName;

  Xamine_GetMemoryName(name);
  sName = name;
  return sName;

}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void MapMemory ( const std::string& rsName,
//                     UInt_t nBytes=knDefaultSpectrumSize )
//  Operation Type:
//     mutator.
//
void
CProductionXamineShMem::mapMemory(const std::string& rsName, UInt_t nBytes)
{
// Maps to a pre-existing shared memory region which communicates
// with Xamine.
//
// Formal Parameters:
//     const std::string&   rsName:
//           Name of the shared memory region
//     UInt_t nBytes:
//           Number of bytes of spectrum memory
//           must match value in shared memory or
//           map fails.

  m_nBytes = nBytes;

  if(!Xamine_MapMemory((char*)(rsName.c_str()), nBytes,
               (volatile Xamine_shared**)&m_pMemory)) {
    throw CErrnoException("Failed to map Xamine shared memory!");
  }

}


UInt_t CProductionXamineShMem::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
{

    // From the spectrum we must construct an Xamine spectrum which desribes
    // what we're trying to create.  There are two variables to worry about
    //  Dimensionality (determines the kind of Xamine spectrum to be created)
    //  and StorageType (determines how to construct it among other things).
    //  asserts are used to enforce restrictions on the types of spectra
    //  supported by Xamine.
      UInt_t nSpectrum;
      CXamineSpectrum* pXSpectrum(0);
      try {
          switch(rSpectrum.Dimensionality()) {
          case 1:			// 1-d spectrum.
          {
              Bool_t           fWord = rSpectrum.StorageType() == keWord;
              pXSpectrum   = new CXamine1D(getXamineMemory(),
                                           rSpectrum.getName(),
                                           rSpectrum.Dimension(0),
                                           rSpectrum.GetLow(0),
                                           rSpectrum.GetHigh(0),
                                           rSpectrum.GetUnits(0),
                                           fWord);
              break;
          }
          case 2:			// 2-d spectrum.
          {
              // 2d spectra can now have any data type so:

              int dataType;
              switch (rSpectrum.StorageType()) {
              case keWord:			// 2dW
                  dataType = 0;
                  break;
              case keByte:		// 2db
                  dataType = 1;
                  break;
              case keLong:			// (was 2).  2dL
                  dataType = 2;
                  break;
              default:
                  throw string("Invalid 2d spectrum type");
              }

              pXSpectrum = new CXamine2D(getXamineMemory(),
                                         rSpectrum.getName(),
                                         rSpectrum.Dimension(0),
                                         rSpectrum.Dimension(1),
                                         rSpectrum.GetLow(0),
                                         rSpectrum.GetLow(1),
                                         rSpectrum.GetHigh(0),
                                         rSpectrum.GetHigh(1),
                                         rSpectrum.GetUnits(0),
                                         rSpectrum.GetUnits(1),
                                         dataType);

              break;
          }
          default:			// Unrecognized dimensionality.
                  assert(kfFALSE);
          }
          // pXSpectrum points to a spectrum which can be 'defined' in Xamine:
          // pSpectrum  points to a spectrum dictionary entry.
          // What's left to do is:
          //   Define the spectrum to Xamine (allocating slot and storage).
          //   Replace the spectrum's storage with Xamine's.
          //   Enter the slot/name correspondence in the m_DisplayBindings table.
          //

          Address_t pStorage           = defineSpectrum(*pXSpectrum);
          nSpectrum                    = pXSpectrum->getSlot();

          rSpectrum.ReplaceStorage(pStorage, kfFALSE);
          while(m_DisplayBindings.size() <= nSpectrum) {
              m_DisplayBindings.push_back("");
          }

          m_DisplayBindings[nSpectrum] = rSpectrum.getName();
          delete pXSpectrum;		// Destroy the XamineSpectrum.
      }
      catch (...) {		// In case of throw after CXamine2D created.
          delete pXSpectrum;
          throw;
      }

      return nSpectrum;
}


void CProductionXamineShMem::removeSpectrum(UInt_t nSpec, CSpectrum &rSpectrum)
{
    // Unbinds the spectrum which is
    // attached to the specified Displayer spectrum number.
    //
    // Formal Parameters:
    //    UInt_t nSpec:
    //       Display spectrum id to unbind.

    CXamineSpectrum  Spec(getXamineMemory(), nSpec);
    if(Spec.getSpectrumType() != undefined) { // No-op if spectrum not defined


      CSpectrum*       pSpectrum = &rSpectrum;
      //
      //  What we need to do is:
      //    0. Remove the gates which are being displayed.
      //    1. Provide local storage for the spectrum data.
      //    2. Remove the spectrum from the bindings table.
      //    3. Tell Xamine to free the slot and spectrum.
      //

      // Deal with the gates:

      CXamineGates* pGates = GetGates(nSpec);
      CDisplayGateVectorIterator pGateIterator = pGates->begin();
      while(pGateIterator != pGates->end()) {
        UInt_t   nGateId   = pGateIterator->getId();
        GateType_t eGateType = pGateIterator->getGateType();
        removeGate(nSpec, nGateId, eGateType);
        pGateIterator++;
      }

      delete pGates;

      // Deal with the spectrum:

      pSpectrum->ReplaceStorage(new char[static_cast<unsigned>(pSpectrum->StorageNeeded())],
                                kfTRUE);
      m_DisplayBindings[nSpec] = "";
      freeSpectrum(nSpec);

    }
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void EnterGate (CDisplayGate& rGate )
//  Operation Type:
//     mutator
//
void CProductionXamineShMem::addGate(CXamineGate& rGate)
{
// Adds a gate graphical object to the
// display subsystem.
//
//  Formal Parameters:
//     const DisplayGate&  rGate:
//        Refers to the gate to enter.
//

  // We need to first create a points array for the Xamine_EnterGate
  // function:

  if(rGate.size() > GROBJ_MAXPTS) {
    throw CRangeError(0, GROBJ_MAXPTS, rGate.size(),
              "CXamine::EnterGate - Creating points array");
  }

  Xamine_point  points[GROBJ_MAXPTS];
  Xamine_point* pPoints = points;
  for(PointIterator p = rGate.begin(); p != rGate.end(); p++, pPoints++) {
    pPoints->x = p->X();
    pPoints->y = p->Y();
  }

  int nStatus = Xamine_EnterGate(rGate.getSpectrum()+1,
                 rGate.getId(),
                 MapFromGate_t(rGate.getGateType()),
                 (char*)(rGate.getName().c_str()),
                 (int)rGate.size(), points);
  ThrowGateStatus(nStatus, rGate,
          "Xamine::EnterGate -- Failed to enter gate");
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void RemoveGate ( UInt_t nSpectrum, UInt_t nId, GateType_t eType )
//  Operation Type:
//     Mutator
//
void
CProductionXamineShMem::removeGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType)
{
// Removes a specified gate from the spectrum.
// The gate is defined by its spectrum, id, and type.
//
// Formal Parameters:
//      UInt_t nSpectrum:
//           Number of spectrum from which to remove gate.
//      UInt_t nId:
//            Identification of the spectrum.
//      GateType_t eType:
//            Type of the gate to remove (See EnterGate for
//             details).
//

  // Map GateType_t to Xamine gate types;

  Xamine_gatetype GateType;
  switch(eType) {
  case kgCut1d:
    GateType = Xamine_cut1d;
    break;
  case kgContour2d:
    GateType = Xamine_contour2d;
    break;
  case kgBand2d:
    GateType = Xamine_band;
    break;
  case kgFitline:
    GateType = fitline;
    break;
  default:			// range error from kgCut1d ... kgBand2d
    throw CRangeError(kgCut1d, kgBand2d, eType,
              "Xamine::RemoveGate -- Mapping gate types");
  }

 int nStatus = Xamine_RemoveGate(nSpectrum+1, nId, GateType);

 msg_object msg;
 msg.spectrum = nSpectrum;
 msg.id       = nId;
 msg.type     = GateType;
 msg.hasname  = kfFALSE;
 msg.npts     = 0;
 ThrowGateStatus(nStatus, CXamineGate(msg),
         "Xamine::RemoveGate - Removing the gate");

}


//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CXamineGates GetGates ( UInt_t nSpectrum )
//  Operation Type:
//     Selector
//
CXamineGates*
CProductionXamineShMem::GetGates(UInt_t nSpectrum)
{
// Retrieves a set of gates from Xamine.
// Copies of the gates described are retrieved into
//  an XamineGates object.  From there, iterators
// can be created to step through the gates,
// or the gate set can be directly indexed as desired
// (the XamineGates object looks a lot like a
// readonly STL vector of gate description objects.
//
// Formal Parameters:
//     UInt_t nSpectrum:
//            The spectrum for which
//            to retrieve the gate.
// Returns:
//    CXamineGates* - gate object ptr to dynamically allocated gate list.
//
  return new CXamineGates(nSpectrum+1);

}


/*!
   addFit : adds a fit to the Xamine bindings.  We keep track of
   these fits in m_fitlineBindings.  This is a vector of lists.
   The index of each vector element is the Xamine 'display slot' fitlines
   are bound to. Each element is a list of pairs.  Each pair is the fitline
   id and fitline name.
   \param fit : CSpectrumFit&
     Reference to the fit to add.
*/
void
CProductionXamineShMem::addFit(CSpectrumFit& fit)
{
  // get the fit name and spectrum name... both of which we'll need to
  //   ensure we can add/bind the fit.

  string fitName      = fit.fitName();
  string spectrumName = fit.getName();
  Int_t  xSpectrumId  = findDisplayBinding(spectrumName);
  if (xSpectrumId < 0) {
    // Display is not bound to Xamine.

    return;
  }
  // The display is bound... ensure that our fitlines binding array is large
  // enough.

  while (m_FitlineBindings.size() <=  xSpectrumId) {
    FitlineList empty;
    m_FitlineBindings.push_back(empty);
  }

  // Now we must:
  //  1. Allocate a fitline id.
  //  2. Enter the fit line in Xamine.
  //  3. Add the fitline name/id to our m_FitlineBindings

  int fitId = m_nextFitlineId++;
  Xamine_EnterFitline(xSpectrumId+1, fitId,
              const_cast<char*>(fitName.c_str()),
              fit.low(), fit.high(),
              const_cast<char*>(fit.makeTclFitScript().c_str()));
  pair <int, string> fitInfo(fitId, fitName);
  m_FitlineBindings[xSpectrumId].push_back(fitInfo);


}
/*!
  Remove a fit.  It is  a no-op to delete a fit that does not exist or is
  on an unbound spectrum. The rough cut of what we will do is
  - Locate the spectrum id of the binding.
  - Locate any fit that matches the name of the fit we are given
    in the fit bindings list assocated with that spectrum.
  - Ask Xamine to delete that fit (fits are like gates).
  - Remove this fit from our bindings list.
  \param fit : CSpectrumFit&
     referenced to the fit to remove.
*/
void
CProductionXamineShMem::deleteFit(CSpectrumFit& fit)
{
  string spectrumName =  fit.getName();
  string fitName      = fit.fitName();
  int    xSpectrumId  = findDisplayBinding(spectrumName);
  if (xSpectrumId >= 0 && ( xSpectrumId <  m_FitlineBindings.size())) {

    // xSpectrumId < 0 means spectrum not bound.
    // xSpectrumId >= size of the bindings vector means no fitlines on spectrum.

    FitlineList::iterator i = m_FitlineBindings[xSpectrumId].begin();
    FitlineList::iterator e = m_FitlineBindings[xSpectrumId].end();
    while (i != e) {
      if (fitName == i->second) {
    // found it.. delete this one and return... don't delete all
    // occurences as
    // a. there's only supposed to be one occurence.
    // b. Depending on the underlying representation of a FitlineList,
    //    deletion may invalidate i.

    Xamine_RemoveGate(xSpectrumId+1, i->first,
              fitline);
    m_FitlineBindings[xSpectrumId].erase(i);
    return;
      }
      i++;
    }
    // Falling through here means no matching fit lines...which is a no-op.

  }
}


/**
 * setOverflows
 *    Set overflow statistics for a spectrum.
 * @param slot - Which slot in Xamine.
 * @param x    - X overflow counts.
 * @param y    - Y overflow couts.
 */
void
CProductionXamineShMem::setOverflows(unsigned slot, unsigned x, unsigned y)
{
    Xamine_setOverflow(slot+1, 0, x);
    Xamine_setOverflow(slot+1, 1, y);
}
/**
 * setUnderflows
 *    Same as above but the underflow counters get set.
 */
void
CProductionXamineShMem::setUnderflows(unsigned slot, unsigned x, unsigned y)
{
    Xamine_setUnderflow(slot+1, 0, x);
    Xamine_setUnderflow(slot+1, 1, y);
}
/**
 * clearStatistics
 *    Clear the over/underflow statistics associated witha bound spectrum.
 */
void
CProductionXamineShMem::clearStatistics(unsigned slot)
{
    Xamine_clearStatistics(slot+1);
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    XamineSpectrum& operator[] ( UInt_t n )
//  Operation Type:
//     Selector
//
CXamineSpectrum&
CProductionXamineShMem::operator[](UInt_t n)
{
// Returns a copy of the n'th spectrum description.  The spectrum
// description is considered a constant object from Xamine's point
// of view.  This object can be modified, but such modification has
// no effect on Xamine's operation.  The object must be deleted
// when done.
//
// Formal Parameters:
//    UInt_t n
//        Slot of the spectrum who's description we want.
// Exceptions:
//    Throws a range error if n is too large.
//

  if(n >= XAMINE_MAXSPEC) {
    throw CRangeError(0, XAMINE_MAXSPEC-1, n,
              "CXamine::operator[] indexing spectrum definitions");
  }
  CXamineSpectrum spec(m_pMemory, n);
  return ((spec.is1d() == kfTRUE) ? (CXamineSpectrum&)*(new CXamine1D(m_pMemory, n)) :
                (CXamineSpectrum&)*(new CXamine2D(m_pMemory, n)));

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CXamineSpectrumIterator begin (  )
//  Operation Type:
//     selector.
//
CXamineSpectrumIterator
CProductionXamineShMem::begin()
{
// Returns an iterator representing the first used spectrum slot.
// Dereferencing this pointer like object produces a reference to
// the CXamineSpectrum object which describes the spectrum.
// Increments step the iterator to the next used slot, while
// until equal to or greater than end() which produces an iterator
// whose dererference is undefined but which compares to
// other iterators to indicate the end of a traversal.
//

  return CXamineSpectrumIterator((Xamine_shared*)m_pMemory);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CXamineSpectrumIterator end (  )
//  Operation Type:
//     selector.
//
CXamineSpectrumIterator
CProductionXamineShMem::end()
{
// Returns a spectrum iterator which indicates the end of
// the set of spectrum slots maintained by Xamine.

  CXamineSpectrumIterator last((Xamine_shared*)m_pMemory, XAMINE_MAXSPEC-1);
  last++;
  return last;

}

string CProductionXamineShMem::createTitle(CSpectrum &rSpectrum, UInt_t maxLength, CHistogrammer &rSorter)
{
    return rSpectrum.getName();
}

/*!
    Return the size of the spectrum title string.
*/
UInt_t
CProductionXamineShMem::getTitleSize() const
{
  return sizeof(spec_title);
}

/*!
   Set the title of a specific slot in Xamine memory.
   The title will be truncated to the size of the spec_title if
   necessary
*/
void
CProductionXamineShMem::setTitle(string name, UInt_t slot)
{
  memset((void*)m_pMemory->dsp_titles[slot], 0, getTitleSize());
  strncpy((char*)m_pMemory->dsp_titles[slot], name.c_str(), getTitleSize() -1);
}

void
CProductionXamineShMem::setInfo(CSpectrum &rSpectrum, std::string name)
{
    Int_t slot = findDisplayBinding(rSpectrum);
    if (slot >= 0) {
        setInfo(name, slot);
    } else {
        throw std::runtime_error("CXamine::setInfo() Cannot set info on unbound spectrum.");
    }
}

/*!
  Set the info string of a specific slot in xamine memory.
  the info string will be truncated to spec_title size if needed.
*/
void
CProductionXamineShMem::setInfo(string info, UInt_t slot)
{
  memset((void*)m_pMemory->dsp_info[slot], 0, getTitleSize());
  strncpy((char*)m_pMemory->dsp_info[slot], info.c_str(), getTitleSize() - 1);
}

DisplayBindings CProductionXamineShMem::getDisplayBindings() const
{
   return m_DisplayBindings;
}

/*!
    Find the bindings for a spectrum by name.
   \param name  : string
       Name of the spectrum
   \return
   \retval -1   - Spectrum has no binding.
   \retval >= 0 - The binding index (xamine slot).

*/
Int_t
CProductionXamineShMem::findDisplayBinding(string name)
{
  for (int i = 0; i < m_DisplayBindings.size(); i++) {
    if (name == m_DisplayBindings[i]) {
      return i;
    }
  }
  return -1;
}

Int_t
CProductionXamineShMem::findDisplayBinding(CSpectrum& rSpectrum)
{
    return findDisplayBinding(rSpectrum.getName());
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    Address_t DefineSpectrum ( CXamineSpectrum& rSpectrum )
//  Operation Type:
//     Allocator
//
Address_t
CProductionXamineShMem::defineSpectrum(CXamineSpectrum& rSpectrum)
{
    // Allocates a spectrum slot and storage for a
    // spectrum.
    //
    // Formal Parameters:
    //    CXamineSpectrum& rSpectrum:
    //        Reference to a description of the spectrum.
    //        The slot number and pointer are ignored on input
    //        and filled in to correctly reflect the defined spectrum
    //        if it  is defined.
    // Returns:
    //   Address_t pointer to the spectrum storage,  or kpNULL if
    //   the spectrum could not be defined.
    // NOTE:  If m_fManaged is not true, then the spectrum area will
    //        be turned into one managed by the Xamine memory manager.
    //        this will destroy existing definitions in the Xamine memory at
    //        this implementation.
    // NOTE:  rSpectrum is modified to reflect the spectrum slot number and
    //        location.
    //        Xamine Eout of memory or slots results in an ENOMEM CErrnoException.
    Address_t pData;
    UInt_t    nSpectrum;

    if(!m_fManaged) {
        Xamine_ManageMemory();
        m_fManaged = kfTRUE;
    }
    CXamine1D* p1d = rSpectrum.Oned();
    CXamine2D* p2d = rSpectrum.Twod();
    assert( (p1d != kpNULL) || (p2d != kpNULL));

    if(p1d) {			// 1d spectrum
        pData = Xamine_Allocate1d((Int_t*)&nSpectrum,
                                  p1d->getChannels(),
                                  (char*)(p1d->getTitle().c_str()),
                                  p1d->getWord());
        if(pData) {
            // Apply the mapping transformation if it exists
            CXamineMap1D Xmap = p1d->getXamineMap();
            if(Xmap.getLow() != Xmap.getHigh()) {
                Xamine_SetMap1d(nSpectrum, Xmap.getLow(), Xmap.getHigh(),
                                const_cast<char*>(Xmap.getUnits().c_str()));
            }
            else {
                Xamine_SetMap1d(nSpectrum, 0.0, 0.0, const_cast<char*>(""));
            }
            CXamine1D result(m_pMemory, nSpectrum-1);
            *p1d  = result;
        }
        else {			// Failure.
            errno = ENOMEM;
            throw CErrnoException("CXamine::DefineSpectrum - Defining 1d spectrum");
        }
    }
    else if (p2d) {		// 2d spectrum
        pData = Xamine_Allocate2d((Int_t*)&nSpectrum,
                                  p2d->getXchannels(),
                                  p2d->getYchannels(),
                                  (char*)(p2d->getTitle().c_str()),
                                  p2d->getType());
        if(pData) {			// Success

            // Apply the mapping transformation if it exists
            CXamineMap2D Xmap = p2d->getXamineMap();
            if(Xmap.getXLow() != Xmap.getXHigh()) {
                Xamine_SetMap2d(nSpectrum,
                                Xmap.getXLow(), Xmap.getXHigh(),
                                const_cast<char*>(Xmap.getXUnits().c_str()),
                                Xmap.getYLow(), Xmap.getYHigh(),
                                const_cast<char*>(Xmap.getYUnits().c_str()));
            }
            CXamine2D result(m_pMemory, nSpectrum-1);
            *p2d = result;
        }
        else {			// Failure
            errno = ENOMEM;
            throw CErrnoException("CXamime::DefineSpectrum - Defining 2d spectrum");
        }

    }
    return pData;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void FreeSpectrum ( UInt_t nSpectrum )
//  Operation Type:
//     Deallocator.
//
void
CProductionXamineShMem::freeSpectrum(UInt_t nSpectrum)
{
    // Frees a spectrum slot and the associated storage
// in the Xamine shared region.
//
// Formal Parameters:
//     UInt_t nSpectrum:
//       The number of the spectrum to free.

  CXamineSpectrum spec(m_pMemory, nSpectrum);
  Xamine_FreeMemory((caddr_t)spec.getStorage());	// Free memory and.
  Xamine_FreeSpectrum(nSpectrum+1);       // Definition slot.

}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//   Xamine_gatetype MapFromGate_t(GateType_t type)
// Operation Type:
//   Utility function
//
Xamine_gatetype
CProductionXamineShMem::MapFromGate_t(GateType_t type)
{
  switch(type) {
  case kgCut1d:
    return Xamine_cut1d;
  case kgContour2d:
    return Xamine_contour2d;
  case kgBand2d:
    return Xamine_band;
  default:
    throw CRangeError(kgCut1d, kgBand2d, type,
              "CXamine::MapFromGate_t - Converting gate type");
  }
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//   ThrowGateStatus(Int_t nStatus, const CDisplayGate& rGate,
//                   const std::string& doing)
// Operation Type:
//    Protected utility.
//
void
CProductionXamineShMem::ThrowGateStatus(Int_t nStatus, const CXamineGate& rGate,
             const std::string& doing)
{
  // Maps Xamine gate maniplation status into either the appropriate
  // exception or normal return.
  //
  // The following possible cases:
  //    nStatus >= 0:         success.
  //    nStatus in [-1..-6] - Throw CXamineGateException
  //    nStatus == CheckErrno       - Throw CErrnoException
  //

  if(nStatus == CheckErrno) {
    throw CErrnoException(doing);
  }
  if(nStatus < 0) {
    throw CXamineGateException(nStatus, rGate,
                   doing);
  }
  // Normal completion.

  assert(nStatus >= 0);		// In case status allocations change.

}
