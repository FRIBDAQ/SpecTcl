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
extern  int            Xamine_newgates;  // fd for events. 


//
// Static declarations:
//
 static CXamineNoPrompt none;


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
CXamine::Start() 
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
CXamine::Stop() 
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
//    void EnterGate (DisplayGate& rGate )
//  Operation Type:
//     mutator
//
void 
CXamine::EnterGate(CDisplayGate& rGate) 
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
CXamine::RemoveGate(UInt_t nSpectrum, UInt_t nId, GateType_t eType) 
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
 ThrowGateStatus(nStatus, CDisplayGate(msg),
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
    ThrowGateStatus(nStatus, CDisplayGate(msg), 
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
	Xamine_SetMap1d(nSpectrum, 0.0, 0.0, 
			const_cast<char*>(""));
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
CXamine::ThrowGateStatus(Int_t nStatus, const CDisplayGate& rGate,
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
void CXamine::Restart()
{
  Xamine_Closepipes();
  Xamine_DetachSharedMemory();
  assert(Xamine_CreateSharedMemory(m_nBytes, (volatile Xamine_shared**)&m_pDisplay));  
  Start();
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
