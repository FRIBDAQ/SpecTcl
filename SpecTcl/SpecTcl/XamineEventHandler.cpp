
// Class: CXamineEventHandler
// Set as a Tcl File handler on the Xamine gates file descriptor.
// This class manages Xamine file events by getting an event
// and calling either the OnGate or OnButton member functions.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "XamineEventHandler.h"    				
#include "GateFactory.h"
#include "GatePackage.h"	// need to assign gate ids.

#include <Exception.h>
#include <DictionaryException.h>
#include <Point.h>
#include <ButtonEvent.h>
#include <DisplayGate.h>
#include <Spectrum.h>
#include <XamineEvent.h>

#include <vector>
#include <string>
#include <assert.h>
#include <iostream.h>

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved XamineEventHandler.cpp \n";

static inline UInt_t scale(UInt_t value, Int_t nshift)
{
  if(nshift > 0) {
    return (value >> nshift);
  }
  else {
    return (value << (-nshift));
  }
}


// Functions for class CXamineEventHandler

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()(int mask)
//  Operation Type: 
//     Evaluation operator.
void CXamineEventHandler::operator()(int mask)  
{
  // Called when Xamine's gate fd is readable.
  // the histogrammer is used to locate the
  // xamine object and poll the event.
  // The event is decoded into either a button
  // or a gate event.  The appropriate Onxxx member
  // is called.
  //
  // Formal Parameters:
  //   int mask:
  //      Mask indicating what sort of Fd event
  //     triggered this handler.
  //

  CXamine* pDisplay = m_pHistogrammer->getDisplayer();

  // Poll the event from the displayer:

  CXamineEvent Event;
  if(pDisplay->PollEvent(0, Event)) {
    
    // Determine what kind of beast we got:

    CDisplayGate* pGate   = Event.GateCast();
    CButtonEvent* pButton = Event.ButtonCast();
    
    // Not allowed to be both:

    assert(!(pGate && pButton));

    if(pGate) {			// Process gates.
      OnGate(*pGate);
      delete pGate;		// Delete dynamically created gate.
    }
    if(pButton) {		// Process buttons:
      OnButton(*pButton);
      delete pButton;		// Delete dynamically created button.
    }
  }
  else {      // If the poll fails, and Xamine is dead, we need to restart it:
    if(!pDisplay->isAlive()) {
      cerr << "Xamine just died....";
      cerr << "\n Unbinding spectra...";
      SpectrumDictionaryIterator p = m_pHistogrammer->SpectrumBegin();
      
      for(; p != m_pHistogrammer->SpectrumEnd(); p++) {
	try {
	  CSpectrum *pSpec = (*p).second;
	  UInt_t Xid = FindDisplayBinding(pSpec->getName());
	  m_pHistogrammer->UnBindFromDisplay(Xid);
	}
	catch(...) { } // Some spectra will not be bound.
  }


      cerr << "\n Restarting Xamine...";
      pDisplay->Restart();
      
      // Re-associate ourselves with the input channel:

      cerr << "\n Reconnecting with Xamine gate inputs..";

      m_nFd = m_pHistogrammer->getDisplayer()->GetEventFd();
      Tcl_Close(m_pInterp->getInterpreter(), m_SocketChannel);
      m_SocketChannel = Tcl_MakeTcpClientChannel((ClientData)m_nFd);
      Set(TK_READABLE);
      cerr << "\n";
    }
    
  }
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     OnGate(CDisplayGate& rXamineGate)
//  Operation Type: 
//     Overridable action
void CXamineEventHandler::OnGate(CDisplayGate& rXamineGate)  
{
  // Decodes an Xamine gate and enters it
  // (if possible) into the SpecTcl gate dictionary
  // this entry will also trigger an entry back into
  //  Xamine's data base of gates as needed.
  //  Note:
  //     Duplicate gate names will overwrite
  //     existing gates.
  //
  // Formal Parameters:
  //     CDisplayGate& rXamineGate:
  //           Refers to the gate created by Xamine.
  // 

  CGateFactory Factory(m_pHistogrammer);; // We'll use this to create the 
				          // Gate itself.

  // Before invoking the factory creation method, we must 
  // pull the points out of the Xamine gate into a local array and
  // map the Xamine gate type into a SpecTcl Gate type..

  UInt_t nSpec       = rXamineGate.getSpectrum() - 1; // Numbering is Xamine's.
  string strSpecName = (m_pHistogrammer->getDisplayBindings())[nSpec];
  string strGateName = rXamineGate.getName();
  CSpectrum* pSpec   = m_pHistogrammer->FindSpectrum(strSpecName);
  if(!pSpec) {
    cerr << "Spectrum in Xamine not defined in SpecTcl, ignoring gate\n";
    return;
  }

  SpectrumType_t spType = pSpec->getSpectrumType();

  vector<CPoint>          GatePoints;
  CGateFactory::GateType  gType;

  GatePoints.insert(GatePoints.begin(),
		    rXamineGate.begin(),
		    rXamineGate.end());

  switch(rXamineGate.getGateType()) {
  case kgCut1d:
    if(spType == ke1D) gType = CGateFactory::cut;
    else if(spType == keG1D) gType = CGateFactory::gammacut;
    else {
      cerr << "Unknown gate type received from Xamine... better update SpecTcl"
	   << endl;
      return;
    }
    break;
  case kgContour2d:
    if(spType == ke2D) gType = CGateFactory::contour;
    else if(spType == keG2D) gType = CGateFactory::gammacontour;
    else {
      cerr << "Unknown gate type received from Xamine... better update SpecTcl"
	   << endl;
      return;
    }
    break;
  case kgBand2d:
    if(spType == ke2D) gType = CGateFactory::band;
    else if(spType == keG2D) gType = CGateFactory::gammaband;
    else {
      cerr << "Unknown gate type received from Xamine... better update SpecTcl"
	   << endl;
      return;
    }
    break;
  default:
    cerr << "Unknown gate type received from Xamine... better update SpecTcl"
         << endl;
    return;
  }

  // We can only accept appropriate gates on simple 1d and 2d spectra:
  //

  switch(spType) {
  case ke1D:                      // 1-d spectrum must be a cut..
    if(gType != CGateFactory::cut) {
      cerr << "Only cuts can be accepted on 1-d spectra\n";
      return;
    }
    break;
  case keG1D:                     // 1-d gamma spectrum must be a gamma cut...
    if((gType != CGateFactory::gammacut) &&
       (gType != CGateFactory::cut)) {
      cerr << "Only gammacuts or cuts can be accepted on 1-d gamma spectra\n";
      return;
    }
    break;
  case ke2D:                     // 2-d spectrum must be a band or contour..
    if( (gType != CGateFactory::band) && 
	(gType != CGateFactory::contour)) {
      cerr << "Only bands or contours can be accepted "
	   << "on 2-d spectra\n";
      return;
    }
    break;
  case keG2D:       // 2-d gamma spectrum must be a gammaband or gammacontour..
    if((gType != CGateFactory::band) && 
       (gType != CGateFactory::contour) &&
       (gType != CGateFactory::gammaband) &&
       (gType != CGateFactory::gammacontour)) {
      cerr << "Only [gamma]bands or [gamma]contours can be accepted "
	   << "on 2-d gamma spectra\n";
      return;
    }
    break;
  default:
    cerr << "Gates must only be accepted on simple 1-d or 2-d spectra\n";
    return;
  }
  // The points are in spectrum coordinates, so we need to reverse the
  // scale factor which is applied to each parameter.  How this is done
  // depends on the gate type a bit too.
  //  Note that doing things the way we do makes stuff work even for
  //  1-d cut points for which the y axis is irrelevant.
  //
  Int_t xscale = 0;
  Int_t yscale = 0;
  if(pSpec->Dimensionality() > 0) xscale = -pSpec->getScale(0);	// Inverse 
  if(pSpec->Dimensionality() > 1) yscale = -pSpec->getScale(1);	// scale..
  vector<CPoint>::iterator p = GatePoints.begin();
  for(; p != GatePoints.end(); p++) {
    *p  = CPoint(scale((*p).X(), xscale),
		 scale((*p).Y(), yscale));
  }
  // Need to create a vector of parameter names for the gate too:
  //
  vector<UInt_t> pIds;
  vector<string> Parameters;

  pSpec->GetParameterIds(pIds);
  vector<UInt_t>::iterator pid;
  CGate* pSpecTclGate;
  vector<string> Names;  // vector to hold spectrum name which is passed to
                         // GateFactory on gamma gates

  switch(spType) {
  case ke1D:
  case ke2D:
    for(pid = pIds.begin(); pid != pIds.end(); pid++) {
      CParameter* pParam = m_pHistogrammer->FindParameter(*pid);
      if(!pParam) {
	cerr << "Spectrum parameter " << *pid << "has been deleted!!\n";
	return;
      }
      Parameters.push_back(pParam->getName());    
    }
    // Use the gate factory creation mechanism to produce a dynamically
    // allocated SpecTcl gate:
    pSpecTclGate = Factory.CreateGate(gType, Parameters, GatePoints);
    break;
  case keG1D:
  case keG2D:
    Names.push_back(strSpecName);
    pSpecTclGate = Factory.CreateGate(gType, GatePoints, Names);
    break;
  default:
    cerr << "Spectrum type cannot accept a gate!!\n";
    return;
  }
  
  
  // Now that we have a gate, we must enter it into the histogrammer.
  //
  
  try {
    if(m_pHistogrammer->FindGate(strGateName)) { // Replace existing gate.
      cerr << "Replacing exisiting gate: " << strGateName << endl;
      m_pHistogrammer->ReplaceGate(strGateName, *pSpecTclGate);
    } 
    else {			// Add new gate.
      m_pHistogrammer->AddGate(strGateName,CGatePackage::AssignId(), 
			       *pSpecTclGate);
    }
  }
  catch (CException& rExcept) {
    cerr << "Could not enter gate into SpecTcl: \n";
    cerr << rExcept.ReasonText();
    cerr << "Gate not entered" << endl;
  }
  delete pSpecTclGate;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     OnButton(CXamineButton& rButton)
//  Operation Type: 
//     Overridable Action
void CXamineEventHandler::OnButton(CButtonEvent& rButton)  
{
  // Processes an Xamine Button event.  
  // At present this is a no-op....later we'll provide
  // mechanisms for attaching TCL scripts to button
  // definitions in Xamine.  This is needed in addition to TK
  // buttons as Xamine buttons can accept points on Xamine
  // spectra etc, as well as return information about the
  //  currently selected spectrum.
  //
  // Formal Parameters:
  //      CXamineButton& rButtonEvent:
  //            Refers to the Xamine button event
  //            which was just received.
  //
  
  // Currently a no-op.
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//   UInt_t FindDisplayBinding(const string& nId)
// Operation Type:
//   Protected Utility:
//
UInt_t
CXamineEventHandler::FindDisplayBinding(const std::string& rName)
{
  // Locate a display binding associated with a spectrum name.
  // 
  // Formal Paramters:
  //     const std:string& rName:
  //          Name of the spectrum whose binding we'll locate.
  // Returns:
  //     The Xid (Display id number) of the binding.
  // Throws:
  //    CDictionary Exception if the spectrum is not bound.
  // 
  
  CSpectrum *pSpec = m_pHistogrammer->FindSpectrum(rName);
  if(!pSpec) {			// the spectrum must exist in fact..
    throw CDictionaryException(CDictionaryException::knNoSuchId,
			       "Looking up spectrum from name",
			       rName);
  }

  for(UInt_t i = 0; i < m_pHistogrammer->DisplayBindingsSize(); i++) {
    CSpectrum* pBoundSpec = m_pHistogrammer->DisplayBinding(i);
    if(pBoundSpec) {
      if(rName == pBoundSpec->getName()) 
	return i;
    }
  }
  throw CDictionaryException(CDictionaryException::knNoSuchKey,
			     "Spectrum with this name is not bound",
			     rName);
}
//
// Functional Description:
//   void Set(int mask)
//     Registers the CallbackRelay static member as a socket callback
//     handler as per the mask.
//
void
CXamineEventHandler::Set(int mask)
{
  Tcl_CreateChannelHandler(m_SocketChannel, mask,
			   CallbackRelay, this);
}
//
// Functional Description:
//   void Clear()
//     Disable callbacks.
//
void 
CXamineEventHandler::Clear()
{
  Tcl_DeleteChannelHandler(m_SocketChannel, CallbackRelay, this);
}
//
// Functional Description:
//    void CallbackRelay(ClientData pObject, int mask)
// Called as a channel handler as specified by Set().  
// pObject is assumed to be this in disguise.
// We get into object context and call operator().
//
void
CXamineEventHandler::CallbackRelay(ClientData pObject, int mask)
{
  CXamineEventHandler* pThis = (CXamineEventHandler*)pObject;

  pThis->operator()(mask);
}
