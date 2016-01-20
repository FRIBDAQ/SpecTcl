/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 2005, All rights reserved";
//  CXamine.cpp
// Encapsulates the Xamine application programming interface.
// Note that the Xamine interfaces are at present captured in a
// non-object oriented way. 
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//
#include <config.h>
#include "Xamineplus.h"                               
#include "XamineGates.h"
#include "ErrnoException.h"
#include "RangeError.h"
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
#include <XamineGate.h>

#include <Display.h>
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
#include "Spectrum.h"

#include <Xamine1D.h>
#include <Xamine2D.h>

#include "Point.h"
extern "C" {
#include <Xamine.h>
#include <clientgates.h>

}

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

//
// External References:
//
extern "C" {
  Xamine_shared* Xamine_memory;	   // Pointer to shared memory.
}
extern   int            Xamine_newgates;  // fd for events. 


int CXamine::m_nextFitlineId(1); // Next fitline id assigned.

//
// Static declarations:
//
 static CXamineNoPrompt none;



 // Functions for CXamineCreator
 ////////////////////////////////////////////////////////////////////////

 CXamineCreator::CXamineCreator(UInt_t nDisplayBytes)
     :  m_nDisplayBytes(nDisplayBytes)
 {}

 CXamine* CXamineCreator::create()
 {
     return new CXamine(m_nDisplayBytes);
 }


// Functions for class CXamine
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   CXamine()
// Operation Type:
//   Default constructor... binds to an already existing 
//   instance of Xamine.  By now the memory must be mapped else undefined
//   results occur.
//
CXamine::CXamine() :
  m_pDisplay(0),
  m_fManaged(kfFALSE),
  m_nBytes(0)
{
  m_pDisplay = Xamine_memory;
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    CXamine::CXamine(UInt_t nBytes)
// Operation Type:
//    Parameterized Constructor.
//
CXamine::CXamine(UInt_t nBytes) :
  m_fManaged(kfFALSE),
  m_nBytes(nBytes)
{
  if(!Xamine_CreateSharedMemory(m_nBytes, 
			       (volatile Xamine_shared**)&m_pDisplay)) {
    perror("Failed to create Xamine shared memory!!");
    exit(errno);
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
CXamine::GetMemoryName() 
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
CXamine::MapMemory(const std::string& rsName, UInt_t nBytes) 
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
		       (volatile Xamine_shared**)&m_pDisplay)) {
    perror("Failed to map Xamine shared memory!");
    exit(errno);
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t isAlive (  )
//  Operation Type:
//     Selector
//
Bool_t 
CXamine::isAlive() 
{
// Returns kfTRUE if Xamine is still alive.

  return (Xamine_Alive() ? kfTRUE : kfFALSE);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Start (  )
//  Operation Type:
//     Initialization
//
void 
CXamine::start()
{
// Starts the autonomous display subsystem.

  if(!Xamine_Start()) {
    perror("Xamine failed to start!! ");
    exit(errno);
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Stop (  )
//  Operation Type:
//     Finalization
//
void 
CXamine::stop()
{
// Stops the autonomous display subsystem.

  if (!Xamine_Stop()) {
    perror("Xamine failed to stop!!");
    exit(errno);
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
CXamine::addGate(CSpectrum &rSpectrum, CGateContainer &rGate)
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
void CXamine::addGate(CXamineGate& rGate)
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
CXamine::removeGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType)
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
//    void EnterPeakMarker ( UInt_t nSpectrum, UInt_t nId, const std::string& rsName, Float_t fCentroid, Float_t fWidth )
//  Operation Type:
//     Mutator
//
void 
CXamine::EnterPeakMarker(UInt_t nSpectrum, 
			 UInt_t nId, 
			 const std::string& rsName, 
			 Float_t fCentroid, Float_t fWidth) 
{
// Enters a peak marker graphical object
// on a spectrum display.  A peak marker is
// a graphical object which displays peak
// width and a peak center graphically
//  Peak markers are graphical objects, rather than
// gates, however they can be entered by the 
// client program.  At present, peak markers can
// only be removed by the user interface.
//
// Formal Parameters:
//      UInt_t nSpectrum:
//          Number of the spectrum on which the marker is set.
//      UInt_t nId:
//           Id of the peak marker (must be unique amongst all
//           the peak markers in the spectrum.
//       const std::string& rsName:
//            Name of the peak marker.
//        Float_t  fCentroid:
//            Center channel of the marker.
//        Float_t fWidth:
//            Width of the marker.
//

  int nStatus = Xamine_EnterPeakMarker(nSpectrum+1, nId,
				       (char*)(rsName.c_str()), fCentroid, fWidth);
  if(nStatus < 0) {
    msg_object msg;
    msg.spectrum = nSpectrum;
    msg.id       = nId;
    msg.type     = Xamine_cut1d; // A bit dishonest.. kludge for now.
    msg.hasname  = kfTRUE;
    memset(msg.name, 0, sizeof(grobj_name));
    strncpy(msg.name, rsName.c_str(), sizeof(grobj_name)-1);
    msg.npts        = 2;
    msg.points[0].x = (int)fCentroid;
    msg.points[1].x = (int)fWidth;
    ThrowGateStatus(nStatus, CXamineGate(msg),
		    "CXamine::EnterPeakMarker - Entering the marker");
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamineGates GetGates ( UInt_t nSpectrum )
//  Operation Type:
//     Selector
//
CXamineGates*
CXamine::GetGates(UInt_t nSpectrum) 
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
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t GetEventFd (  )
//  Operation Type:
//     Selector.
//
UInt_t 
CXamine::GetEventFd() 
{
// Returns the file descriptor on which
// Xamine events are read.  This allows
// the file descriptor to be put inside a select
// set such as that run by e.g. the X-11 server
// or the TCL/TK interpreter.
//

  return (UInt_t)Xamine_newgates;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t PollEvent ( Int_t nTimeout, CXamineEvent& rEvent )
//  Operation Type:
//     I/O
//
Bool_t 
CXamine::PollEvent(Int_t nTimeout, CXamineEvent& rEvent) 
{
// Polls for an event on the Xamine event
// pipeline.  Returns kfTRUE if an event
// is received within the appropriate timeout.
// returns kfFALSE if not.
//
// Formal Parameters:
//     Int_t nTimeout:
//        Number of seconds to wait fo the event.
//        0 is an immediate return, and negative
//        specifies a indefinite wait until an event is presented.
//    CXamineEvent& rEvent:
//         The event read is stored here.
//  Returns:
//      kfTRUE   - if an event was read
//      kfFALSE  - if no event was read.
//

  msg_XamineEvent Event;
  int nStatus = Xamine_PollForEvent(nTimeout, &Event);

  // Three possible return cases:
  // nStatus >  0  - an event was read.
  // nStatus == 0  - Read timed out.
  // nStatus <  0  - errno has the reason for an error.

  if(nStatus > 0) {		// Data arrived.:
    CXamineEvent event(Event);
    rEvent = event;
    return kfTRUE;
  }
  if(nStatus == 0)		// Timeout:
    return kfFALSE;
  if(nStatus < 0) {		// Errno exception
    if (errno != EPIPE) {	// Restart Xamine
      throw CErrnoException("CXamine::PollEvent - Poll failed");
    } else {
      return kfFALSE;
    }
  }
  assert(kfFALSE);		// Should never land here.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Address_t DefineSpectrum ( CXamineSpectrum& rSpectrum )
//  Operation Type:
//     Allocator
//
Address_t 
CXamine::DefineSpectrum(CXamineSpectrum& rSpectrum)
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
      CXamine1D result(m_pDisplay, nSpectrum-1);
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
      CXamine2D result(m_pDisplay, nSpectrum-1);
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
CXamine::FreeSpectrum(UInt_t nSpectrum) 
{
// Frees a spectrum slot and the associated storage 
// in the Xamine shared region.
// 
// Formal Parameters:
//     UInt_t nSpectrum:
//       The number of the spectrum to free.

  CXamineSpectrum spec(m_pDisplay, nSpectrum);
  Xamine_FreeMemory((caddr_t)spec.getStorage());	// Free memory and.
  Xamine_FreeSpectrum(nSpectrum+1);       // Definition slot. 

}
/**
 * setOverflows
 *    Set overflow statistics for a spectrum.
 * @param slot - Which slot in Xamine.
 * @param x    - X overflow counts.
 * @param y    - Y overflow couts.
 */
void
CXamine::setOverflows(unsigned slot, unsigned x, unsigned y)
{
    Xamine_setOverflow(slot+1, 0, x);
    Xamine_setOverflow(slot+1, 1, y);
}
/**
 * setUnderflows
 *    Same as above but the underflow counters get set.
 */
void
CXamine::setUnderflows(unsigned slot, unsigned x, unsigned y)
{
    Xamine_setUnderflow(slot+1, 0, x);
    Xamine_setUnderflow(slot+1, 1, y);
}
/**
 * clearStatistics
 *    Clear the over/underflow statistics associated witha bound spectrum.
 */
void
CXamine::clearStatistics(unsigned slot)
{
    Xamine_clearStatistics(slot+1);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void DefineButtonBox ( UInt_t nColumns=8, UInt_t NRows=3 )
//  Operation Type:
//     Creator.
//
void 
CXamine::DefineButtonBox(UInt_t nColumns, UInt_t nRows) 
{
// Defines an Xamine managed button box
// to allow the client to integrate parts of it's
// GUI into the Xamine GUI.  Buttons defined
// in this box can produce prompters for entities
// which are part of the Xamine display.
//  
// Formal Parameters:
//    UInt_t nColumns:
//            Number of button columns.
//    UInt_t nRows:
//            Number of button rows.
//

 int nStatus =  Xamine_DefineButtonBox(nColumns, nRows);
 if(nStatus == CheckErrno) {
   throw CErrnoException("CXamine::DefineButtonBox - Defining the box");
 }
 if(nStatus < 0) {
   throw CXamineButtonException(nStatus,
				CXamineButton(0,
					      std::string("Button box"),
					      kfFALSE, Anytime, 
					      none),
					      "CXamine::DefineButtonBox - Defining the box");
 }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void DefineButton ( UInt_t nColumn, UInt_t nRow, 
//                        const CXamineButton& rButton )
//  Operation Type:
//     Creational
//
void 
CXamine::DefineButton(UInt_t nColumn, 
		      UInt_t nRow, const CXamineButton& rButton) 
{
// Defines a button in the button box.  
// Button box coordinates have the origin
// at the upper left corner. columns increase
// to the right and rows going down.
//
// Formal Parameters:
//      UInt_t   nColumn:
//         Column coordinate of the button.
//      UInt_t nRow:
//         Row coordinate of the button.
//      const CXamineButton& rButton:
//          Refers to the specification of the button to create.
//

  ButtonDescription ButtonBlock;
  rButton.FormatMessageBlock(ButtonBlock);

  int nStatus = Xamine_DefineButton(nRow, nColumn, &ButtonBlock);

  ThrowButtonStatus(nStatus, rButton, 
		    "CXamine::DefineButton - Defining the button");


}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void ModifyButton ( UInt_t nColumn, UInt_t nRow, const CXamineButton rButton )
//  Operation Type:
//     Mutator
//
void 
CXamine::ModifyButton(UInt_t nColumn, UInt_t nRow, 
		      const CXamineButton& rButton) 
{
// Modifies the existence of a defined button.
//
// Formal Parameters:
//    UInt_t nColumn
//       Column coordinates of the button to change.
//    UInt_t nRow:
//        Row coordinates of the button to change.
//   const CXamineButton& rButton:
//        Refers to the new definintion of the button.

  ButtonDescription ButtonBlock;
  rButton.FormatMessageBlock(ButtonBlock);

  int nStatus = Xamine_ModifyButton(nRow, nColumn, &ButtonBlock);

  ThrowButtonStatus(nStatus, rButton,
		    "CXamine::ModifyButton - Modifying the button");

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void EnableButton ( UInt_t nColumn, UInt_t nRow )
//  Operation Type:
//     Mutator
//
void 
CXamine::EnableButton(UInt_t nColumn, UInt_t nRow) 
{
// Enables a button.  When enabled, a button
// will react to stimulus (being pressed).
//
// Formal Parameters:
//    UInt_t nColumn
//    UInt_t nRow:
//         Column and row coordinates of the
//         button to enable.
//

  int nStatus = Xamine_EnableButton(nRow, nColumn);
  ThrowButtonStatus(nStatus,
		    CXamineButton(-1,
				  "-- Enabling button --",
				  kfFALSE,
				  Anytime,
				  none),
		    "CXamine::EnableButton - Enabling a button");

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void DisableButton ( UInt_t  nColumn, UInt_t nRow )
//  Operation Type:
//     Mutator
//
void 
CXamine::DisableButton(UInt_t  nColumn, UInt_t nRow) 
{
// Disables a button.  When disabled,
// the button is ghosted, and insenstive
// to presses.
//
// Formal Parameters:
//      UInt_t nColumn
//      UInt_t nRow:
//         Column and row coordinates of the
//         button to disable.

  int nStatus = Xamine_DisableButton(nRow, nColumn);
  ThrowButtonStatus(nStatus,
		    CXamineButton(-1,
				  "-- Disabling --",
				  kfFALSE,
				  Anytime,
				  none),
		    "CXamine::DisableButton - Disabling the button");


}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void DeleteButton ( UInt_t nColumn, UInt_t nRow )
//  Operation Type:
//     Detroyer
//
void 
CXamine::DeleteButton(UInt_t nColumn, UInt_t nRow) 
{
// Removes a button from the Xamine
// GUI Button box extension
// 
// Formal Parameters:
//     UInt_t nColumn
//     UInt_t nRow:
//         Button box coordinates of the button to remove.
//

  int nStatus = Xamine_DeleteButton(nRow, nColumn);
  ThrowButtonStatus(nStatus,
		   CXamineButton(-1,
				 "-- Deleting button --",
				 kfFALSE,
				 Anytime,
				 none),
		   "CXamine::DeleteButton - Removing button from box");

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void DeleteButtonBox (  )
//  Operation Type:
//     Destroyer
//
void 
CXamine::DeleteButtonBox() 
{
// Removes the Xamine GUI button box from
// the display permanently.  
//

  Xamine_DeleteButtonBox();	// Don't care about errors here.

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamineButton* InquireButton ( UInt_t nColumn, UInt_t nRow )
//  Operation Type:
//     Selector
//
CXamineButton*
CXamine::InquireButton(UInt_t nColumn, UInt_t nRow) 
{
// Returns the button description of the selected
// button in the button box.  The XamineButton
// object returned is dynamically allocated so the
// caller must at some time delete it to avoid memory
// leaks.
//
// Formal Parameters:
//    UInt_t nColumn
//    UInt_t nRow:
//        Coordinates of the button being asked about.
//
// Returns:
//    CXamineButton* a dynamically allocated 
//    button description. kpNULL if there is no button
//    defined in that slot of the box.
//

  msg_InquireButtonAck Data;

  int nStatus = Xamine_InquireButton(nRow, nColumn, &Data);
  ThrowButtonStatus(nStatus,
		    CXamineButton(-1,
				  "-- Inquiring about button --",
				  kfFALSE,
				  Anytime,
				  none),
		    "CXamine::InquireButton - Getting button info");

  //
  // Construct the CXamineButton from the description:
  //
  CXamineButton* pButton;
  CXamineButtonPrompt* pPrompt;
  if(Data.valid) {
    ButtonDescription* bd = &(Data.button);
    switch(bd->prompter) {
    case NoPrompt:
      pPrompt = new CXamineNoPrompt();
      break;
    case Confirmation:
      pPrompt = new CXamineConfirmPrompt(std::string(bd->promptstr));
      break;
    case Text:
      pPrompt = new CXamineTextPrompt(std::string(bd->promptstr));
      break;
    case Points:
      pPrompt = new CXaminePointsPrompt(std::string(bd->promptstr),
					bd->minpts,
					bd->maxpts);
      break;
    case Spectrum:
      pPrompt = new CXamineSpectrumPrompt(std::string(bd->promptstr),
					 MaptoSpec_t(bd->spectype));
      break;
    case Filename:
      pPrompt = new CXamineFilePrompt(std::string(bd->promptstr));
      break;
    default:
      assert(kfFALSE);
    }
    switch(bd->type) {
    case Push:
    case Toggle:
    default:
      assert(kfFALSE);
    }
  }
  else {
    return (CXamineButton*)kpNULL;
  }

  delete pPrompt;
  return pButton;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    XamineSpectrum& operator[] ( UInt_t n )
//  Operation Type:
//     Selector
//
CXamineSpectrum& 
CXamine::operator[](UInt_t n) 
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
  CXamineSpectrum spec(m_pDisplay, n);
  return ((spec.is1d() == kfTRUE) ? (CXamineSpectrum&)*(new CXamine1D(m_pDisplay, n)) :
		        (CXamineSpectrum&)*(new CXamine2D(m_pDisplay, n)));

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamineSpectrumIterator begin (  )
//  Operation Type:
//     selector.
//
CXamineSpectrumIterator 
CXamine::begin() 
{
// Returns an iterator representing the first used spectrum slot.
// Dereferencing this pointer like object produces a reference to
// the CXamineSpectrum object which describes the spectrum.
// Increments step the iterator to the next used slot, while 
// until equal to or greater than end() which produces an iterator
// whose dererference is undefined but which compares to 
// other iterators to indicate the end of a traversal.
//

  return CXamineSpectrumIterator((Xamine_shared*)m_pDisplay);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamineSpectrumIterator end (  )
//  Operation Type:
//     selector.
//
CXamineSpectrumIterator 
CXamine::end() 
{
// Returns a spectrum iterator which indicates the end of 
// the set of spectrum slots maintained by Xamine.

  CXamineSpectrumIterator last((Xamine_shared*)m_pDisplay, XAMINE_MAXSPEC-1);
  last++;
  return last;

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
CXamine::ThrowGateStatus(Int_t nStatus, const CXamineGate& rGate,
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
////////////////////////////////////////////////////////////////////////
//
// Function:
//   void ThrowButtonStatus(Int_t nStatus, const CXamineButton& rButton,
// 			    const std::string& doing)
// Operation type:
//   Utility
void 
CXamine::ThrowButtonStatus(Int_t nStatus, const CXamineButton& rButton,
			   const std::string& doing)
{
  if(nStatus == CheckErrno) {
    throw CErrnoException(doing);
  }
  if(nStatus < 0) {
    throw CXamineButtonException(nStatus, rButton, doing);
  }
}
///////////////////////////////////////////////////////////////////////
//
// Function:
//     DialogSpectrumType_t MaptoSpec_t(ButtonDialogSpectrumType t)
// Operation Type:
//    Protected utility.
//
DialogSpectrumType_t
CXamine::MaptoSpec_t(ButtonDialogSpectrumType t)
{
  switch(t) {
  case Any:
    return keAny;
  case Oned:
    return ke1d;
  case Twod:
    return ke2d;
  case Compatible:
    return keCompatible;
  default:
    assert(kfFALSE);
  }
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   Xamine_gatetype MapFromGate_t(GateType_t type)
// Operation Type:
//   Utility function
//
Xamine_gatetype
CXamine::MapFromGate_t(GateType_t type)
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
////////////////////////////////////////////////////////////
//
void CXamine::restart()
{
  Xamine_Closepipes();
  Xamine_DetachSharedMemory();
  assert(Xamine_CreateSharedMemory(m_nBytes, (volatile Xamine_shared**)&m_pDisplay));  
  start();
  m_fManaged = kfFALSE;		// Memory not yet managed.
}
/*!
    Return the size of the spectrum title string.
*/
UInt_t
CXamine::getTitleSize() const
{
  return sizeof(spec_title);
}
/*!
   Set the title of a specific slot in Xamine memory.
   The title will be truncated to the size of the spec_title if
   necessary
*/
void 
CXamine::setTitle(string name, UInt_t slot)
{
  memset((void*)m_pDisplay->dsp_titles[slot], 0, getTitleSize());
  strncpy((char*)m_pDisplay->dsp_titles[slot], name.c_str(), getTitleSize() -1);
}
/*!
  Set the info string of a specific slot in xamine memory.
  the info string will be truncated to spec_title size if needed.
*/
void
CXamine::setInfo(string info, UInt_t slot)
{
  memset((void*)m_pDisplay->dsp_info[slot], 0, getTitleSize());
  strncpy((char*)m_pDisplay->dsp_info[slot], info.c_str(), getTitleSize() - 1);
}

/*!
    Binds a spectrum to the display.
    The first free spectrum will be allocated.
   If the spectrum is already bound, then we short circuit and
   just return the current binding number:
   This is not an error, just a user mistake we can handle.


    \param sName : const std::string
             Name of the spectrum to bind.

    \return   UInt_t  - Actual spectrum number chosen.


   \throw   CDictionaryException - if spectrum of given name does not exist.
   \throw  CErrnoException      - may be thrown by routines we call.


  */
UInt_t CXamine::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter) {

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

        setInfo(createTitle(rSpectrum, getTitleSize(), rSorter),
                nSpectrum);
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
    // We must locate all of the gates which are relevant to this spectrum
    // and enter them as well:
    //

    vector<CGateContainer> DisplayGates = getAssociatedGates(rSpectrum.getName(), rSorter);

    UInt_t Size = DisplayGates.size();
    for(UInt_t i = 0; i < DisplayGates.size(); i++) {
        CXamineGate* pXgate = GateToXamineGate(rSpectrum, DisplayGates[i]);
        if(pXgate) addGate(*pXgate);
        delete pXgate;
    }
    // same for the fitlines:
    //

    CFitDictionary& dict(CFitDictionary::getInstance());
    CFitDictionary::iterator pf = dict.begin();

    while (pf != dict.end()) {
        CSpectrumFit* pFit = pf->second;
        if (pFit->getName() == rSpectrum.getName()) {
            addFit(*pFit);		// not very efficient, but doesn't need to be
        }
        pf++;
    }

    return nSpectrum;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void UnBindFromDisplay ( UInt_t nSpec )
//  Operation Type:
//     mutator
//
void CXamine::removeSpectrum(UInt_t nSpec, CSpectrum& rSpectrum) {
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
CXamine::addFit(CSpectrumFit& fit)
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
CXamine::deleteFit(CSpectrumFit& fit)
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
 * updateStatistics
 *    Update the Xamine statistics for each bound spectrum.
 */
void
CXamine::updateStatistics()
{
    for (int i =0; i < m_boundSpectra.size(); i++) {
        CSpectrum* pSpec = m_boundSpectra[i];
        if (pSpec) {
            std::vector<unsigned> stats = pSpec->getUnderflows();
            setUnderflows(i, stats[0], (stats.size() == 2 ? stats[1] : 0));
            stats = pSpec->getOverflows();
            setOverflows(i, stats[0], (stats.size() == 2 ? stats[1] : 0));
        }
    }
}

////////////////////////////////////////////////////////////////////////
//
// :Function:
//    CDisplayGate* GateToXamineGate(UInt_t nBindingId, CGateContainer& rGate)
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
CXamineGate* CXamine::GateToXamineGate(CSpectrum& rSpectrum,
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

/////////////////////////////////////////////////////////////////////////////
//
//  Function:
//    std::vector<CGateContainer> GatesToDisplay(const std::string& rSpectrum)
// Operation Type:
//    Protected utility.
//
std::vector<CGateContainer>
CXamine::getAssociatedGates(const std::string& spectrumName, CHistogrammer &rSorter)
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

DisplayBindings CXamine::getDisplayBindings() const
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
CSpectrum* CXamine::getSpectrum(UInt_t xid) {
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
DisplayBindingsIterator CXamine::DisplayBindingsBegin() {
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
DisplayBindingsIterator CXamine::DisplayBindingsEnd() {
  // Returns an iterator which can be used to determin
  // if the end of the display bindings set has been iterated through.
  return m_DisplayBindings.end();
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
CXamine::FindDisplayBinding(string name)
{
  for (int i = 0; i < DisplayBindingsSize(); i++) {
    if (name == m_DisplayBindings[i]) {
      return i;
    }
  }
  return -1;
}

Int_t
CXamine::FindDisplayBinding(CSpectrum& rSpectrum)
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
UInt_t CXamine::DisplayBindingsSize() const {
  // Returns the number of spectra bound to the display.
  return m_DisplayBindings.size();
}


//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Create a spectrum displayer title from the information in the
//   spectrum definition.   As needed items will be dropped from the
//   definition to ensure that this will all fit in the limited
//   number of characters avaialable to a spectrum title.
//
string
CXamine::createTitle(CSpectrum& rSpectrum, UInt_t maxLength, CHistogrammer& rSorter)
{
  CSpectrum::SpectrumDefinition def = rSpectrum.GetDefinition();
  string name = def .sName;
  ostringstream typestream;
  typestream << def.eType;
  string type = typestream.str();

  // Create the axis vector:

  vector<string> axes;
  for (int i = 0; i < rSpectrum.Dimensionality(); i++) {
    ostringstream axisstream;
    axisstream << rSpectrum.GetLow(i) << ", " << rSpectrum.GetHigh(i)
           << " : " << rSpectrum.Dimension(i);
    axes.push_back(axisstream.str());
  }
  // gate name:

  const CGateContainer& gate(*(rSpectrum.getGate()));
  string gateName;
  if (&gate != pDefaultGate) {
    gateName = gate.getName();
  } else {
    gateName = "";
  }
  //  Get the parameter names

  vector<UInt_t> ids = def.vParameters;
  vector<UInt_t> yids= def.vyParameters;
  vector<string> parameters;;
  vector<string> yparameters;



  for (int i =0; i < ids.size(); i++) {
    CParameter* pParam = rSorter.FindParameter(ids[i]);
    if (pParam) {
      parameters.push_back(pParam->getName());
    } else {
      parameters.push_back(string("--deleted--"));
    }
  }
  for (int i = 0; i < yids.size(); i++) {
    CParameter* pParam = rSorter.FindParameter(yids[i]);
    if (pParam) {
      yparameters.push_back(pParam->getName());
    }
    else {
      yparameters.push_back(string("--deleted--"));
    }
  }


  // Ok now the following variables are set up for the first try:
  //  name       - Name of the spectrum
  //  type       - String type of the spectrum
  //  axes       - Vector of axis definitions.
  //  gateName       - name of gateName on spectrum.
  //  parameters - vector of parameter names.

  string trialTitle = createTrialTitle(type, axes, parameters, yparameters, gateName);
  if (trialTitle.size() < maxLength) return trialTitle;

  // Didn't fit.one by one drop the parameters..replacing the most recently
  // dropped parameter by "..."

  while (yparameters.size()) {
    yparameters[yparameters.size()-1] = "...";
    trialTitle = createTrialTitle(type, axes, parameters, yparameters, gateName);
    if (trialTitle.size() < maxLength) return trialTitle;
    vector<string>::iterator i = yparameters.end();
    i--;
    yparameters.erase(i);
  }
  while (parameters.size()) {
    parameters[parameters.size()-1] = "..."; // Probably smaller than it was.
    trialTitle = createTrialTitle(type, axes, parameters, yparameters, gateName);
    if (trialTitle.size() < maxLength) return trialTitle;
    vector<string>::iterator i = parameters.end();
    i--;
    parameters.erase(i);	// Kill off last parameter.
  }

  // Still didn't fit... and there are no more parameters left to drop.
  // now we drop the axis definition...

  axes.clear();
  trialTitle = createTrialTitle(type , axes, parameters, yparameters, gateName);
  if (trialTitle.size() < maxLength) return trialTitle;

  // Now compute if we can delete the tail of the spectrum name
  // to fit... For this try we drop at most 1/2 of the name.

  if ((trialTitle.size() - (name.size()/2 + 3)) < maxLength) {
    while(trialTitle.size() > maxLength) {
      name = name.assign(name.c_str(), name.size()-4) + string("...");
      trialTitle = createTrialTitle(type, axes, parameters, yparameters, gateName);
    }
    return trialTitle;
  }


  // nope...drop the gateName and delete the tail of the spectrum name so it fits.
  //

  name.assign(name.c_str(), maxLength - 3) + string("...");

  return name;
}

///////////////////////////////////////////////////////////////////////////
//
//  Creates a trial spectrum title.  This unconditionally
//  glues the elements of a title together to form a title.
//  The string created is of the form:
//   title : type [low hi chans] x [low hi chans]  Gated on  gatename : {parameters...}
//  However:
//    - If there are no axes (size of vector is 0), the axes are omitted.
//    - If there are no characters in the gate name, the gate is omitted.
//    - If the parameters vector is size 0 it is omitted too.
//   The idea is for createTitle to use this to iteratively try to fit
//   title elements into the number of characters accepted by a displayer.
// Parameters:
//     name  : string
//          Spectrum name
//     type  : string
//          type of the spectrum.
//     axes  : vector<string>
//          Axis names.
//     parameters : vector <string>
//          Names of parameters.
//     yparameter : vector<string>
//         vector of y axis parameters (gamma 2d deluxe).
//     gate : string
//          Name of gate on spectrum.
//
// Returns:
//    A string that describes the spectrum in standard from from these
//    elements.
//
string
CXamine::createTrialTitle(string type, vector<string>      axes,
                vector<string>      parameters,
                vector<string>      yparameters,
                string gate)
{
  string result;
  result += type;

  // If there are axes, put them in:

  if (axes.size() > 0) {
    string separator = " ";
    for (int i =0; i < axes.size(); i++) {
      result += separator;
      result += "[";
      result += axes[i];
      result += "]";

      separator = " X ";
    }
  }
  // If there's a nonempty gate string add that information:

  if (gate != string("")) {
    result += " Gated on : ";
    result += gate;
  }
  // If there are parameters comma separate them in curlies.

  if(parameters.size() > 0) {
    string separator = "";
    result += " {";
    for (int i = 0; i < parameters.size(); i++) {
      result += separator;
      result += parameters[i];
      separator = ", ";
    }
    result += "}";
  }
  if (yparameters.size() > 0) {
    string separator = "";
    result += " {";
    for (int i = 0; i < yparameters.size(); i++) {
      result += separator;
      result += yparameters[i];
      separator = ", ";
    }
    result += "}";
  }

  return result;
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
CXamine::flip2dGatePoints(CSpectrum* pSpectrum, UInt_t gXparam)
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
