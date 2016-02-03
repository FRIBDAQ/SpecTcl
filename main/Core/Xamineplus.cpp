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
#include "XamineShMemDisplayImpl.h"

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
#include "XamineSharedMemory.h"
#include "XamineGateFactory.h"

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

#include "Point.h"
extern "C" {
#include <Xamine.h>
#include <clientgates.h>

}

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

//
// External References:
//
//extern "C" {
//  Xamine_shared* Xamine_memory;	   // Pointer to shared memory.
//}
extern   int            Xamine_newgates;  // fd for events.


//int CXamine::m_nextFitlineId(1); // Next fitline id assigned.

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


//// Functions for class CXamine
////////////////////////////////////////////////////////////////////////////
////
//// Function:
////   CXamine()
//// Operation Type:
////   Default constructor... binds to an already existing
////   instance of Xamine.  By now the memory must be mapped else undefined
////   results occur.
////
//CXamine::CXamine() :
//    m_pMemory(),
//    m_nBytes(0)
//{
//    try {
//        m_pMemory = new CXamineSharedMemory(m_nBytes);
//    } catch (CErrnoException& exc) {
//        cerr << "Caught errno exception: " << exc.ReasonText()
//             << " while: " << exc.WasDoing() << endl;
//        exit(errno);
//    }
//}

 /////////////////////////////////////////////////////////////////////////
//
// Function:
//    CXamine::CXamine(UInt_t nBytes)
// Operation Type:
//    Parameterized Constructor.
//
CXamine::CXamine(UInt_t nBytes) :
//    m_pMemory(0),
//    m_nBytes(nBytes)
    m_pImpl(new CXamineShMemDisplayImpl(nBytes))
{
}

CXamine::CXamine (const CXamine& aCXamine ) :
   m_pImpl()
{
    auto nBytes = aCXamine.getSharedMemory()->getSize();
    m_pImpl.reset(new CXamineShMemDisplayImpl(nBytes));
}

CXamine* CXamine::clone() const { return new CXamine(*this); }


CXamine::~CXamine() {
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t isAlive (  )
//  Operation Type:
//     Selector
//
bool
CXamine::isAlive() 
{
// Returns kfTRUE if Xamine is still alive.

  return (Xamine_Alive() ? true : false);

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
////////////////////////////////////////////////////////////
//
void CXamine::restart()
{
  Xamine_Closepipes();
//  m_pMemory->detach();
//  m_pMemory->attach();
  m_pImpl->detach();
  m_pImpl->attach();
  start();
}


const CXamineSharedMemory* CXamine::getSharedMemory() const
{
    return m_pImpl->getSharedMemory();
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
//    CXamineGateFactory factory(m_pMemory);
//    CXamineGate* pDisplayed = factory.fromSpecTclGate(rSpectrum, rGate);
//    if (pDisplayed)
//        m_pMemory->addGate(*pDisplayed);
//    delete pDisplayed;
    m_pImpl->addGate(rSpectrum, rGate);
}

void
CXamine::removeGate(CSpectrum& rSpectrum, CGateContainer& rGate)
{
//    // Removes a gate that is just about to be destroyed from
//    // the appropriate set of Xamine bound spectra.
//    //
//    // Formal Paramters:
//    //    CGateContainer& rGate:
//    //       Reference to the container which holds the gate about to be
//    //       destroyed.  Note that for most purposes, a gate container
//    //       can be treated as if it was a pointer to a gate.
//    //
//    UInt_t nGateId = rGate.getNumber();
//    GateType_t eType;
//    if(rGate->Type() == "c" || rGate->Type() == "gc") {
//      eType = kgContour2d;
//    }
//    else if(rGate->Type() == "b" || rGate->Type() == "gb") {
//      eType = kgBand2d;
//    }
//    else if (rGate->Type() == "s" || rGate->Type() == "gs") {
//      eType = kgCut1d;
//    }
//    else {
//      return;			// Non -primitive gates won't be displayed.
//    }
    m_pImpl->removeGate(rSpectrum, rGate);
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

/*!
    Return the size of the spectrum title string.
*/
UInt_t
CXamine::getTitleSize() const
{
    return m_pImpl->getTitleSize();
}

/*!
   Set the title of a specific slot in Xamine memory.
   The title will be truncated to the size of the spec_title if
   necessary
*/
void
CXamine::setTitle(CSpectrum& rSpectrum, string name)
{
    m_pImpl->setTitle(rSpectrum, name);
}



void
CXamine::setInfo(CSpectrum &rSpectrum, std::string name)
{
    m_pImpl->setInfo(rSpectrum, name);
}

/*!
    Binds a spectrum to the display.
    The first free spectrum will be allocated.
   If the spectrum is already bound, then we short circuit and
   just return the current binding number:
   This is not an error, just a user mistake we can handle.


    \param sName : const std::string
             Name of the spectrum to bind

   \throw   CDictionaryException - if spectrum of given name does not exist.
   \throw  CErrnoException      - may be thrown by routines we call.


  */
void CXamine::addSpectrum(CSpectrum &rSpectrum, CHistogrammer &rSorter)
{
    m_pImpl->addSpectrum(rSpectrum, rSorter);
}

void CXamine::removeSpectrum(CSpectrum &rSpectrum)
{
    m_pImpl->removeSpectrum(rSpectrum);
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
    m_pImpl->addFit(fit);
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
    m_pImpl->deleteFit(fit);
}

/**
 * updateStatistics
 *    Update the Xamine statistics for each bound spectrum.
 */
void
CXamine::updateStatistics()
{
    m_pImpl->updateStatistics();
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
    return m_pImpl->getAssociatedGates(spectrumName, rSorter);
}

SpectrumContainer CXamine::getBoundSpectra() const
{
    return m_pImpl->getBoundSpectra();
}


/*!
 *
 */
bool CXamine::spectrumBound(CSpectrum* pSpectrum)
{
    return m_pImpl->spectrumBound(pSpectrum);
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

