#include "XamineSharedMemory.h"

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
#include <assert.h>

using namespace std;

//
// External References:
//
extern "C" {
  static Xamine_shared* Xamine_memory;	   // Pointer to shared memory.
}
extern   int            Xamine_newgates;  // fd for events.


int CXamineSharedMemory::m_nextFitlineId(1); // Next fitline id assigned.

CXamineSharedMemory::CXamineSharedMemory()
    :
     m_pMemory(0),
     m_fManaged(kfFALSE),
     m_nBytes(0)
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
CXamineSharedMemory::CXamineSharedMemory(size_t nBytes) :
  m_fManaged(kfFALSE),
  m_nBytes(nBytes)
{
  if(!Xamine_CreateSharedMemory(m_nBytes,
                   (volatile Xamine_shared**)&m_pMemory)) {
    throw CErrnoException("Failed to create Xamine shared memory!!");
  }

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
CXamineSharedMemory::GetMemoryName()
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
CXamineSharedMemory::MapMemory(const std::string& rsName, UInt_t nBytes)
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


UInt_t CXamineSharedMemory::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
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

          Address_t pStorage           = DefineSpectrum(*pXSpectrum);
          nSpectrum                    = pXSpectrum->getSlot();

//          setInfo(createTitle(rSpectrum, getTitleSize(), rSorter),
//                  nSpectrum);
          rSpectrum.ReplaceStorage(pStorage, kfFALSE);
          while(m_DisplayBindings.size() <= nSpectrum) {
              m_DisplayBindings.push_back("");
              m_boundSpectra.push_back(0);
          }

          m_DisplayBindings[nSpectrum] = rSpectrum.getName();
          m_boundSpectra[nSpectrum]    = &rSpectrum;
          delete pXSpectrum;		// Destroy the XamineSpectrum.
      }
      catch (...) {		// In case of throw after CXamine2D created.
          delete pXSpectrum;
          throw;
      }
//      // We must locate all of the gates which are relevant to this spectrum
//      // and enter them as well:
//      //

//      vector<CGateContainer> DisplayGates = getAssociatedGates(rSpectrum.getName(), rSorter);

//      UInt_t Size = DisplayGates.size();
//      for(UInt_t i = 0; i < DisplayGates.size(); i++) {
//          CXamineGate* pXgate = GateToXamineGate(rSpectrum, DisplayGates[i]);
//          if(pXgate) addGate(*pXgate);
//          delete pXgate;
//      }
//      // same for the fitlines:
//      //

//      CFitDictionary& dict(CFitDictionary::getInstance());
//      CFitDictionary::iterator pf = dict.begin();

//      while (pf != dict.end()) {
//          CSpectrumFit* pFit = pf->second;
//          if (pFit->getName() == rSpectrum.getName()) {
//              addFit(*pFit);		// not very efficient, but doesn't need to be
//          }
//          pf++;
//      }

      return nSpectrum;
}

void CXamineSharedMemory::removeSpectrum(CSpectrum &rSpectrum)
{
    Int_t slot = FindDisplayBinding(rSpectrum);
    if (slot >=0) {
        removeSpectrum(slot, rSpectrum);
    }
}

void CXamineSharedMemory::removeSpectrum(UInt_t nSpec, CSpectrum &rSpectrum)
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

      pSpectrum->ReplaceStorage(new char[pSpectrum->StorageNeeded()],
                    kfTRUE);
      m_DisplayBindings[nSpec] = "";
      m_boundSpectra[nSpec]    = 0;
      FreeSpectrum(nSpec);

    }
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void EnterGate (CSpectrum& rSpectrum, CGateContainer& rGate )
//  Operation Type:
//     mutator
//
void
CXamineSharedMemory::addGate(CSpectrum &rSpectrum, CGateContainer &rGate)
{
    CXamineGate* pDisplayed = GateToXamineGate(rSpectrum, rGate);
    if(pDisplayed)
        addGate(*pDisplayed);
    delete pDisplayed;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void EnterGate (CDisplayGate& rGate )
//  Operation Type:
//     mutator
//
void CXamineSharedMemory::addGate(CXamineGate& rGate)
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

void
CXamineSharedMemory::removeGate(CSpectrum& rSpectrum, CGateContainer& rGate)
{
    // Removes a gate that is just about to be destroyed from
    // the appropriate set of Xamine bound spectra.
    //
    // Formal Paramters:
    //    CGateContainer& rGate:
    //       Reference to the container which holds the gate about to be
    //       destroyed.  Note that for most purposes, a gate container
    //       can be treated as if it was a pointer to a gate.
    //
    UInt_t nGateId = rGate.getNumber();
    GateType_t eType;
    if(rGate->Type() == "c" || rGate->Type() == "gc") {
      eType = kgContour2d;
    }
    else if(rGate->Type() == "b" || rGate->Type() == "gb") {
      eType = kgBand2d;
    }
    else if (rGate->Type() == "s" || rGate->Type() == "gs") {
      eType = kgCut1d;
    }
    else {
      return;			// Non -primitive gates won't be displayed.
    }

}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void RemoveGate ( UInt_t nSpectrum, UInt_t nId, GateType_t eType )
//  Operation Type:
//     Mutator
//
void
CXamineSharedMemory::removeGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType)
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
CXamineSharedMemory::GetGates(UInt_t nSpectrum)
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

////////////////////////////////////////////////////////////////////////
//
// :Function:
//    CXamineGate* GateToXamineGate(UInt_t nBindingId, CGateContainer& rGate)
//  Operation Type:
//    Protected Utility
//
/*!
  Takes a gate container and turns it into a gate suitable for
  entry in the Xamine display program.

  Formal Parameters:
  \param <TT> rSpectrum (CSpectrum& [in]): </TT>
  The spectrum that is associated with the gate
  \param <TT> rGate (CGateContainer& [in]): </TT>
  The container which holds the gate to convert.  Note that
  gate containers can be treated as if they were pointers to
  gates.

  \retval    CXamineGate*  kpNULL -- if the gate is not convertable,
  e.g. it is not a gate directly suported
  by Xamine.
  other  -- Pointer to the gate which was created.
  \note
  The gate is dynamically allocated and therefore must be deleted by
  the client.

*/
CXamineGate* CXamineSharedMemory::GateToXamineGate(CSpectrum& rSpectrum,
                                       CGateContainer& rGate)
{
    CXamineGate* pXGate;
    UInt_t nBindingId = FindDisplayBinding(rSpectrum.getName());
    assert(nBindingId != -1); // make sure the thing was found

    CSpectrum* pSpectrum = &rSpectrum;

    // Summary spectra don't have gates displayed.

    if(pSpectrum->getSpectrumType() == keSummary) { // Summary spectrum.
        return (CXamineGate*)(kpNULL);
    }

    if((rGate->Type() == std::string("s")) ||
            (rGate->Type() == std::string("gs"))) {	// Slice gate
        CDisplayCut* pCut = new CDisplayCut(nBindingId,
                                            rGate.getNumber(),
                                            rGate.getName());
        // There are two points, , (low,0), (high,0)

        CCut& rCut = (CCut&)(*rGate);

        switch(pSpectrum->getSpectrumType()) {
        case ke1D:
        case keG1D:
        {
            // Produce the nearest channel to the gate points.
            // then add them to the display gate.
            //
            int x1 = (int)(pSpectrum->ParameterToAxis(0, rCut.getLow()));
            int x2 = (int)(pSpectrum->ParameterToAxis(0, rCut.getHigh()));
            pCut->AddPoint(x1,
                           0);
            // The weirdness below is all about dealing with a special boundary
            // case when we try to get the right side of the cut to land
            // on the right side of the channel on which it's set.
            // ..all this in the presence of gates accepted on fractional parameters.
            //(consider a fine spectrum (e.g. 400-401 with 100 bins and a coarse
            // spectrum, of the same parameter (e.g. 0-1023 1024 bins)..with
            // the gate set on 400.5, 400.51 and you'll see the thing I'm trying
            // to deal with here.
            //
            pCut->AddPoint(x1 == x2 ? x2 : x2 - 1,
                           0);
            return pCut;
            break;
        }

        }
    }
    else if ((rGate->Type() == std::string("b")) ||
             (rGate->Type() == std::string("gb"))) { // Band gate.
        pXGate = new CDisplayBand(nBindingId,
                                  rGate.getNumber(),
                                  rGate.getName());
    }
    else if ((rGate->Type() == std::string("c")) ||
             (rGate->Type() == std::string("gc"))) { // Contour gate
        pXGate = new CDisplayContour(nBindingId,
                                     rGate.getNumber(),
                                     rGate.getName());
    }
    else {			// Other.
        return (CXamineGate*)kpNULL;
    }
    // Control falls through here if 2-d and we just need
    // to insert the points.  We know this is a point list gate:

    assert((rGate->Type() == "b") || (rGate->Type() == "c") ||
           (rGate->Type() == "gb") || (rGate->Type() == "gc"));

    // If the spectrum is not 2-d the gate can't be displayed:
    //

    if((pSpectrum->getSpectrumType() == ke2D)   ||
            (pSpectrum->getSpectrumType() == keG2D)  ||
            (pSpectrum->getSpectrumType() == ke2Dm)  ||
            (pSpectrum->getSpectrumType() == keG2DD)) {


        CPointListGate& rSpecTclGate = (CPointListGate&)rGate.operator*();
        vector<FPoint> pts = rSpecTclGate.getPoints();
        //    vector<UInt_t> Params;
        //    pSpectrum->GetParameterIds(Params);

        // If necessary flip the x/y coordinates of the gate.
        // note that gamma gates never need flipping.
        //

        //    if((rSpecTclGate.getxId() != Params[0]) &&
        //   ((rSpecTclGate.Type())[0] != 'g')) {

        if ((rSpecTclGate.Type()[0] != 'g') &&
                flip2dGatePoints(pSpectrum, rSpecTclGate.getxId())) {
            for(UInt_t i = 0; i < pts.size(); i++) {	// Flip pts to match spectrum.
                Float_t x = pts[i].X();
                Float_t y = pts[i].Y();
                pts[i] = FPoint(y,x);
            }
        }
        // The index of the X axis transform is easy.. it's 0, but the
        // y axis transform index depends on spectrum type sincd gammas
        // have all x transforms first then y and so on:
        //
        int nYIndex;
        if((pSpectrum->getSpectrumType() == ke2D)   ||
                (pSpectrum->getSpectrumType() == keG2DD) ||
                (pSpectrum->getSpectrumType() == ke2Dm)) {
            nYIndex = 1;
        }
        else {
            CGamma2DW* pGSpectrum = (CGamma2DW*)pSpectrum;
            nYIndex               = pGSpectrum->getnParams();
        }

        for(UInt_t i = 0; i < pts.size(); i++) {

            CPoint pt((int)pSpectrum->ParameterToAxis(0, pts[i].X()),
                      (int)pSpectrum->ParameterToAxis(nYIndex, pts[i].Y()));
            pXGate->AddPoint(pt);

        }
    } else {
        return (CXamineGate*)kpNULL;
    }

    return pXGate;
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
CXamineSharedMemory::addFit(CSpectrumFit& fit)
{
  // get the fit name and spectrum name... both of which we'll need to
  //   ensure we can add/bind the fit.

  string fitName      = fit.fitName();
  string spectrumName = fit.getName();
  Int_t  xSpectrumId  = FindDisplayBinding(spectrumName);
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
CXamineSharedMemory::deleteFit(CSpectrumFit& fit)
{
  string spectrumName =  fit.getName();
  string fitName      = fit.fitName();
  int    xSpectrumId  = FindDisplayBinding(spectrumName);
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
CXamineSharedMemory::setOverflows(unsigned slot, unsigned x, unsigned y)
{
    Xamine_setOverflow(slot+1, 0, x);
    Xamine_setOverflow(slot+1, 1, y);
}
/**
 * setUnderflows
 *    Same as above but the underflow counters get set.
 */
void
CXamineSharedMemory::setUnderflows(unsigned slot, unsigned x, unsigned y)
{
    Xamine_setUnderflow(slot+1, 0, x);
    Xamine_setUnderflow(slot+1, 1, y);
}
/**
 * clearStatistics
 *    Clear the over/underflow statistics associated witha bound spectrum.
 */
void
CXamineSharedMemory::clearStatistics(unsigned slot)
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
CXamineSharedMemory::operator[](UInt_t n)
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
CXamineSharedMemory::begin()
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
CXamineSharedMemory::end()
{
// Returns a spectrum iterator which indicates the end of
// the set of spectrum slots maintained by Xamine.

  CXamineSpectrumIterator last((Xamine_shared*)m_pMemory, XAMINE_MAXSPEC-1);
  last++;
  return last;

}

string CXamineSharedMemory::createTitle(CSpectrum &rSpectrum, UInt_t maxLength, CHistogrammer &rSorter)
{
    return rSpectrum.getName();
}

/*!
    Return the size of the spectrum title string.
*/
UInt_t
CXamineSharedMemory::getTitleSize() const
{
  return sizeof(spec_title);
}

/*!
   Set the title of a specific slot in Xamine memory.
   The title will be truncated to the size of the spec_title if
   necessary
*/
void
CXamineSharedMemory::setTitle(CSpectrum& rSpectrum, string name)
{
  Int_t slot = FindDisplayBinding(rSpectrum);
  if (slot >= 0) {
      setTitle(name, slot);
  } else {
      throw std::runtime_error("CXamine::setTitle() Cannot set title on unbound spectrum.");
  }
}

/*!
   Set the title of a specific slot in Xamine memory.
   The title will be truncated to the size of the spec_title if
   necessary
*/
void
CXamineSharedMemory::setTitle(string name, UInt_t slot)
{
  memset((void*)m_pMemory->dsp_titles[slot], 0, getTitleSize());
  strncpy((char*)m_pMemory->dsp_titles[slot], name.c_str(), getTitleSize() -1);
}

void
CXamineSharedMemory::setInfo(CSpectrum &rSpectrum, std::string name)
{
    Int_t slot = FindDisplayBinding(rSpectrum);
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
CXamineSharedMemory::setInfo(string info, UInt_t slot)
{
  memset((void*)m_pMemory->dsp_info[slot], 0, getTitleSize());
  strncpy((char*)m_pMemory->dsp_info[slot], info.c_str(), getTitleSize() - 1);
}

/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//    std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum)
// Operation Type:
//    Protected utility.
//
std::vector<CGateContainer>
CXamineSharedMemory::getAssociatedGates(const std::string& spectrumName, CHistogrammer &rSorter)
{
  // Returns a vector of gates which can be displayed on the spectrum.
  // Gates are considered displayable on a spectrum iff the gate parameter set
  // is the same as the spectrum parameter set. If the spectrum is a gamma
  // spectrum, then the gate is only displayed on one spectrum. Note that
  // displayable gates at present are one of the following types only:
  //   Contour (type = 'c')
  //   Band    (type = 'b')
  //   Cut     (type = 's')
  //   Sum2d   {type = 'm2'}
  //   GammaContour   (type = 'gc')
  //   GammaBand      (type = 'gb')
  //   GammaCut       (type = 'gs')
  // All other gates are not displayable.
  //

  std::vector<CGateContainer> vGates;
  CSpectrum *pSpec = rSorter.FindSpectrum(spectrumName);
  if(!pSpec) {
    throw CDictionaryException(CDictionaryException::knNoSuchKey,
                   "No such spectrum CXamine::GatesToDisplay",
                   spectrumName);
  }
  //
  // The mediator tells us whether the spectrum can display the gate:
  //
  CGateDictionaryIterator pGate = rSorter.GateBegin();
  while(pGate != rSorter.GateEnd()) {
    CGateMediator DisplayableGate(((*pGate).second), pSpec);
    if(DisplayableGate()) {
      vGates.push_back((*pGate).second);
    }
    pGate++;
  }

  return vGates;
}

SpectrumContainer CXamineSharedMemory::getBoundSpectra() const
{
    return m_boundSpectra;
}

DisplayBindings CXamineSharedMemory::getDisplayBindings() const
{
   return m_DisplayBindings;
}

//////////////////////////////////////////////////////////////////////
//
// Function:
//      CSpectrum*  DisplayBinding(UInt_t xid)
// Operation type:
//      Selector.
//
CSpectrum* CXamineSharedMemory::getSpectrum(UInt_t xid) {
  // Returns  a pointer to a spectrum which is bound on a particular
  // xid.
  // Formal Parameters:
  //    UInt_t xid:
  //       The xid.
  // Returns:
  //     POinter to the spectrum or kpNULL Iff:
  //     xid is out of range.
  //     xid does not map to a spectrum.
  //
  if(xid >= DisplayBindingsSize())
    return (CSpectrum*)kpNULL;

  return m_boundSpectra[xid];
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    DisplayBindingsIterator DisplayBindingsBegin()
//  Operation Type:
//     Selector
//
DisplayBindingsIterator CXamineSharedMemory::DisplayBindingsBegin() {
  // Returns a begining iterator to support iterating through the set of
  // display bindings.
  return m_DisplayBindings.begin();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    DisplayBindingsIterator DisplayBindingsEnd (  )
//  Operation Type:
//
//
DisplayBindingsIterator CXamineSharedMemory::DisplayBindingsEnd() {
  // Returns an iterator which can be used to determin
  // if the end of the display bindings set has been iterated through.
  return m_DisplayBindings.end();
}


/*!
 *
 */
bool CXamineSharedMemory::spectrumBound(CSpectrum &rSpectrum)
{
    return (FindDisplayBinding(rSpectrum) >= 0);
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
CXamineSharedMemory::FindDisplayBinding(string name)
{
  for (int i = 0; i < DisplayBindingsSize(); i++) {
    if (name == m_DisplayBindings[i]) {
      return i;
    }
  }
  return -1;
}

Int_t
CXamineSharedMemory::FindDisplayBinding(CSpectrum& rSpectrum)
{
    return FindDisplayBinding(rSpectrum.getName());
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    UInt_t DisplayBindingsSize (  )
//  Operation Type:
//
//
UInt_t CXamineSharedMemory::DisplayBindingsSize() const {
  // Returns the number of spectra bound to the display.
  return m_DisplayBindings.size();
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    Address_t DefineSpectrum ( CXamineSpectrum& rSpectrum )
//  Operation Type:
//     Allocator
//
Address_t
CXamineSharedMemory::DefineSpectrum(CXamineSpectrum& rSpectrum)
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
CXamineSharedMemory::FreeSpectrum(UInt_t nSpectrum)
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
CXamineSharedMemory::MapFromGate_t(GateType_t type)
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
CXamineSharedMemory::ThrowGateStatus(Int_t nStatus, const CXamineGate& rGate,
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

/**
 * flip2dGatePoints
 *   Determine if the gate point coordinates must be flipped.  This happens
 *   for e.g. a gate on p1, p2 displayed on a spectrum with axes p2, p1
 *
 *  There's an implicit assumption that the gate is displayable on this spectrum
 *  because all we do is see if the X parameter is a match for a spectrum x parameter
 *  and, if not, flip.
 *
 * @param pSpectrum - pointer to the target spectrum.
 * @param gXparam   - Id of the x parameter of the spectrum.
 *
 * @return bool - true if it's necessary to flip axes.
 *
 */
bool
CXamineSharedMemory::flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam)
{
  std::vector<UInt_t> params;
  pSpectrum->GetParameterIds(params);
  if (pSpectrum->getSpectrumType() == ke2Dm) {
    for (int i = 0; i < params.size(); i += 2) {
      if (gXparam == params[i]) return false;
    }
    return true;
  } else {
    return gXparam != params[0];
  }
}
