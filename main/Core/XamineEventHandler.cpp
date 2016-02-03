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

static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";

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
#include <config.h>
#include "XamineEventHandler.h"    				
#include "GateFactory.h"
#include "GatePackage.h"	// need to assign gate ids.

#include <Exception.h>
#include <DictionaryException.h>
#include <Point.h>
#include <ButtonEvent.h>
#include <XamineGate.h>
#include <Spectrum.h>
#include <CSpectrum2Dm.h>
#include <SpecTcl.h>
#include <XamineEvent.h>

#include <Gamma2DW.h>


#include <stdio.h>
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>
#include <stdexcept>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


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


CXamineEventHandler::CXamineEventHandler(CHistogrammer *pHistogrammer,
                                         CXamine *pDisplay) :
   m_pSorter(pHistogrammer),
   m_pDisplay(pDisplay),
   m_nFd(pDisplay->GetEventFd())
 {
   Set();		// Starts out enabled.
 }

CXamineEventHandler::CXamineEventHandler (const CXamineEventHandler& rhs ) :
  m_pSorter(rhs.m_pSorter),
  m_pDisplay(rhs.m_pDisplay),
  m_nFd(rhs.m_nFd),
  m_Timer(rhs.m_Timer)
{
}

CXamineEventHandler& CXamineEventHandler::operator=
(const CXamineEventHandler& aCXamineEventHandler) {
  if(this != &aCXamineEventHandler) {
      m_pSorter = aCXamineEventHandler.m_pSorter;
      setDisplay(aCXamineEventHandler.m_pDisplay);
  }
  return *this;
}

int CXamineEventHandler::operator== (const CXamineEventHandler& rhs) const {
  return ( (m_pSorter == rhs.m_pSorter)    &&
           (m_pDisplay == rhs.m_pDisplay)  &&
           (m_nFd == rhs.m_nFd)            &&
           (m_Timer == rhs.m_Timer));
}

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()(int mask)
//  Operation Type: 
//     Evaluation operator.
void CXamineEventHandler::operator()()  
{
  // Called when Xamine's gate fd is readable.
  // the histogrammer is used to locate the
  // xamine object and poll the event.
  // The event is decoded into either a button
  // or a gate event.  The appropriate Onxxx member
  // is called.
  //
  //

  // Poll the event from the displayer:

  CXamineEvent Event;
  int pollstat = m_pDisplay->PollEvent(0, Event);
  if(pollstat > 0) {		// Successful poll => something read.
    
    // Determine what kind of beast we got:

    CXamineGate* pGate   = Event.GateCast();
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
    if(!m_pDisplay->isAlive()) {
      cerr << "Xamine just died....";
      cerr << "\n Unbinding spectra...";
//      SpectrumDictionaryIterator p = m_pSorter->SpectrumBegin();

        SpectrumContainer spectra = m_pDisplay->getBoundSpectra();
        for (size_t slot = 0; slot<spectra.size(); ++slot) {
            CSpectrum* pSpectrum = spectra.at(slot);
            if (pSpectrum) {
                m_pDisplay->removeSpectrum(*pSpectrum);
            }
        }
//      for(; p != m_pSorter->SpectrumEnd(); p++) {
//          try {
//              CSpectrum *pSpec = (*p).second;
//              UInt_t Xid = FindDisplayBinding(pSpec->getName());
//              m_pDisplay->removeSpectrum(Xid, *pSpec);
//          }
//          catch(...) { } // Some spectra will not be bound.
//      }

      cerr << "\n Restarting Xamine...";
      m_pDisplay->restart();

      // Re-associate ourselves with the input channel:

      cerr << "\n Reconnecting with Xamine gate inputs..";
      m_nFd = m_pDisplay->GetEventFd();
      cerr << "\n";

      // Now that we're all back together we need to let the
      // restart handlers observer this event (e.g. to re-establish
      // the button box.
      //

      RestartHandlerList::iterator i = m_restartHandlers.begin();
      while (i != m_restartHandlers.end()) {
          CRestartHandler* pHandler = *i;
          (*pHandler)();
          i++;
      }
    }
  }

  Set();			// Re-schedule self.
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     OnGate(CDisplayGate& rXamineGate)
//  Operation Type: 
//     Overridable action
void CXamineEventHandler::OnGate(CXamineGate &rXamineGate)
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


  CGateFactory Factory(m_pSorter); // We'll use this to create the
				          // Gate itself.

  // Before invoking the factory creation method, we must 
  // pull the points out of the Xamine gate into a local array and
  // map the Xamine gate type into a SpecTcl Gate type..

  UInt_t nSpec         = rXamineGate.getSpectrum() - 1; // Numbering is Xamine's.
  string strGateName   = rXamineGate.getName();
  auto spectra = m_pDisplay->getBoundSpectra();
  if (nSpec >= spectra.size()) {
      cerr << "Spectrum in Xamine not defined in SpecTcl. ignoring gate\n";
      return;
  }

  CSpectrum* pSpec = spectra[nSpec];
  if(!pSpec) {
    cerr << "Spectrum in Xamine not defined in SpecTcl, ignoring gate\n";
    return;
  }

  SpectrumType_t spType = pSpec->getSpectrumType();

  vector<CPoint>          GatePoints;
  vector<FPoint>          ScaledPoints;
  CGateFactory::GateType  gType;
  GatePoints.insert(GatePoints.begin(),	 // These are the unmapped gate points.
		    rXamineGate.begin(), // Later we need to map them to 
		    rXamineGate.end());	 //  Parameter coordinates.

  switch(rXamineGate.getGateType()) {

  case kgCut1d:
    if((spType == ke1D))	// Ordinary 1d spectrum -> ordinary cut.
      gType = CGateFactory::cut;
    else if((spType == keG1D))	// Gamma spectrum -> Gamma cut.
      gType = CGateFactory::gammacut;
    else {
      cerr << "Cut gate received on a spectrum type that doesn't know about"
           << " cut gates.\n Consider updating SpecTcl??"
	   << endl;
      return;
    }
    break;

  case kgContour2d:
    if((spType == ke2D)) 
      gType = CGateFactory::contour;
    else if((spType == keG2D) || (spType == keG2DD))
      gType = CGateFactory::gammacontour;
    else if (spType == ke2Dm) {
      gType = CGateFactory::contour; // will become an or of contours.
    }
    else {
      cerr << "Contour gate received on a spectrum type that doesn't know"
           << " about contours.\n Consider updating SpecTcl?"
	   << endl;
      return;
    }
    break;
  case kgBand2d:
    if((spType == ke2D))	// Ordinary 2d -> Band.
      gType = CGateFactory::band;
    else if((spType == keG2D) || (spType == keG2DD))	// Gamma spectrum -> Gamma aband.
      gType = CGateFactory::gammaband;
    else if (spType == ke2Dm) {
      gType = CGateFactory::band; // will become an or of bands.
    }
    else {
      cerr << "Band received on a spectrum that type that doesn't know how"
      << " to handle it.\n Consider updating Spectcl."
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
  case  ke2Dm:
    make2dSumgate(strGateName, gType, pSpec,
		  GatePoints);
    return;
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
  case keG2DD:
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
  // Points are in spectrum coordinates (axis coordinates). They must be 
  // transformed back into parameter coordinates.

  UInt_t yIndex = 1;	// For typcial 2d, this index into the

  switch(spType) {
  case ke1D:
  case keG1D:
    {
      // Only allowed 2 points, and the right point must be 
      // set so that it is on the right side of its channel.
      
      int low   = GatePoints[0].X();
      int high  = GatePoints[1].X();
      if (low > high) {
	int temp = low;
	low  = high;
	high = temp;
      }
      //  Note that high is offset by 1 channel to put it on the right side
      // of our channel:
      
      Float_t xlow = pSpec->AxisToParameter(0, low);
      Float_t xhigh= pSpec->AxisToParameter(0, high+1);

      
      ScaledPoints.push_back(FPoint(xlow, 0));
      ScaledPoints.push_back(FPoint(xhigh, 0));
      
    }
    
    break;
    // 
    //  Thist must be contiguous with the case for ke2D keG2dd!!!
    //
  case keG2D:			// For a gamma all x transforms are first.
    {
      CGamma2DW* pGSpectrum = (CGamma2DW*)pSpec;
      yIndex = pGSpectrum->getnParams();
    }
  case ke2D:
  case keG2DD:
    {
      Float_t x,y(0.0);


      
      vector<CPoint>::iterator p = GatePoints.begin();
      for(; p != GatePoints.end(); p++) {
	CPoint& rp(*p);
	x = pSpec->AxisToParameter(0, rp.X());
	if(pSpec->Dimensionality() > 1) 
	  y = pSpec->AxisToParameter(yIndex, rp.Y());
	ScaledPoints.push_back(FPoint(x,y));

      }
      break;
    }
    
  default:
    cerr << "Gates must only be accepted on simple 1-d or 2-d spectra\n";
    return;
  }

  // Need to create a vector of parameter names for the gate too:
  //
  vector<UInt_t> pIds;
  vector<string> Parameters;

  pSpec->GetParameterIds(pIds);
  vector<UInt_t>::iterator pid;
  CGate* pSpecTclGate(0);
  vector<string> Names;  // vector to hold spectrum name which is passed to
                         // GateFactory on gamma gates


  try {
    switch(spType) {
    case ke1D:
    case ke2D:
      for(pid = pIds.begin(); pid != pIds.end(); pid++) {
    CParameter* pParam = m_pSorter->FindParameter(*pid);
	if(!pParam) {
	  cerr << "Spectrum parameter " << *pid << "has been deleted!!\n";
	  return;
	}
	Parameters.push_back(pParam->getName());    
      }
      // Use the gate factory creation mechanism to produce a dynamically
      // allocated SpecTcl gate:
      pSpecTclGate = Factory.CreateGate(gType, Parameters, ScaledPoints);
      break;
    case keG1D:
    case keG2D:
    case keG2DD:
      for(pid = pIds.begin(); pid != pIds.end(); pid++) {
    CParameter* pParam = m_pSorter->FindParameter(*pid);
	if(!pParam) {
	  cerr << "Spectrum parameter " << *pid << "has been deleted!!\n";
	  return;
	}
      }
      pSpecTclGate = Factory.CreateGate(gType, ScaledPoints, pIds);
      break;
    default:
      cerr << "Spectrum type cannot accept a gate!!\n";
      return;
    }
    
  }
  catch (CException& e) {
    cerr << "Could not create a new gate" <<  e.ReasonText() << endl;
    return;
  }
  catch (std::string& msg) {
    cerr << "Exception caught while creating a new gate from Xamine\n";
    cerr << msg << endl;
    return;
  }
  catch (const char* msg ) {
    cerr << "An Exception was caught while creating a new gate from Xamine\n";
    cerr << msg << endl;
    return;
  }
  catch (...) {
    cerr << "An unexpected exception was caught while creating a new gate from Xamine\n";
    return;
  }
  // Now that we have a gate, we must enter it into the histogrammer.
  //
  
  try {
    if(m_pSorter->FindGate(strGateName)) { // Replace existing gate.
      cerr << "Replacing exisiting gate: " << strGateName << endl;
      m_pSorter->ReplaceGate(strGateName, *pSpecTclGate);
    } 
    else {			// Add new gate.
      m_pSorter->AddGate(strGateName,CGatePackage::AssignId(),
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
/*!
   Process button events.  This is done by traversing the
   set of established button handlers.  Each handler is called
   in turn until true is returned by the handler or 
   until the end of the list is reached.

   \pram rButton : CButtonEvent&
       Reference to the button event that triggered us.
*/

void CXamineEventHandler::OnButton(CButtonEvent& rButton)  
{
  ButtonHandlerList::iterator p = m_buttonHandlers.begin();
  while (p != m_buttonHandlers.end()) {
    CButtonHandler* pHandler = *p;
    if ((*pHandler)(rButton)) return;
    p++;
  }

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
  
  CSpectrum *pSpec = m_pSorter->FindSpectrum(rName);
  if(!pSpec) {			// the spectrum must exist in fact..
    throw CDictionaryException(CDictionaryException::knNoSuchId,
			       "Looking up spectrum from name",
			       rName);
  }

  SpectrumContainer spectra =m_pDisplay->getBoundSpectra();
  size_t nSpectra = spectra.size();
  for(size_t i = 0; i < nSpectra; i++) {
      CSpectrum* pBoundSpec = spectra[i];
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
//   void Set()
//     Registers the CallbackRelay static member as a socket callback
//     handler as per the mask.  There are problems now with cygwin's TCL
//     fd notification on sockets,so we'll use a timer <sigh>
//
void
CXamineEventHandler::Set()
{
  m_Timer = Tcl_CreateTimerHandler(500, CallbackRelay, (ClientData)this);

}
//
// Functional Description:
//   void Clear()
//     Disable callbacks.
//
void 
CXamineEventHandler::Clear()
{
  Tcl_DeleteTimerHandler(m_Timer);
}

/*!
   Add a button handler to the list of handlers that are
   given a chance to deal with button events:
   \param handler : CXamineEventHandler::CButtonHandler& 
      Reference to a concrete button handler object (derived from
      the abstract base class CXamineEventHandler::CButtonHandler).

*/
void 
CXamineEventHandler::addButtonHandler(CXamineEventHandler::CButtonHandler& handler)
{
  m_buttonHandlers.push_back(&handler);
}
/*!
   Add a restart handler to the restart observer list.
   This allows application specific operations to be done when
   Xamine is restarted as a result of a crash/inadvertent exit etc.
   \param handler : CRestartHandler&
      Function object derived from CRestartHandler that will be
      invoked on an Xamine restart, once Xamine is all back up and
      connected.
*/
void
CXamineEventHandler::addRestartHandler(CRestartHandler& handler)
{
  m_restartHandlers.push_back(&handler);
}
///////////////////////////////////////////////////////////////////////
////////////////////// Private utility functions //////////////////////
///////////////////////////////////////////////////////////////////////
//
// Functional Description:
//    void CallbackRelay(ClientData pObject, int mask)
// Called as a channel handler as specified by Set().  
// pObject is assumed to be this in disguise.
// We get into object context and call operator().
//
void
CXamineEventHandler::CallbackRelay(ClientData pObject)
{
  CXamineEventHandler* pThis = (CXamineEventHandler*)pObject;

  pThis->operator()();
}


// Create a gate on a sum spectrum.  The gate type will
// be either a contour or a band.  The gate will be made by
// creating individual gates for each parameter pair in the
// spectrum and then creating a gate that will or these together.
// so the gate is true whenever any pair of parameters falls in it.
//
// Parameters:
//     std::string gateName                        - Name of the final gate to create.
//     CGateFactory::GateType componentGateType    - Type of component gates.
//     CSpectrum*             pSpectrum            - Pointer to the spectrum.
//     std::vector<CPoint>    rawPoints            - Spectrum coordinates points.
//
void
CXamineEventHandler::make2dSumgate(string                 gatename,
				   CGateFactory::GateType componentGateType,
				   CSpectrum*             pSpectrum,
				   vector<CPoint>         rawPoints)
{
  vector<string> componentNames; // So that we can create the OR gate.
  SpecTcl& api(*SpecTcl::getInstance()); // so we can call API members.

  CSpectrum2Dm* pSumSpectrum(dynamic_cast<CSpectrum2Dm*>(pSpectrum));
  vector<UInt_t> parameterIds;
  pSumSpectrum->GetParameterIds(parameterIds);

  //  Now we're ready to get what we need to create the component gates.

  for (int i = 0; i < parameterIds.size(); i+=2) {
    UInt_t xId   = parameterIds[i];
    UInt_t yId   = parameterIds[i+1];
    vector<string>  gateParams;
    CParameter* pX = api.FindParameter(parameterIds[i]);
    CParameter* pY = api.FindParameter(parameterIds[i+1]);

    gateParams.push_back(pX->getName());
    gateParams.push_back(pY->getName());

    vector<FPoint> points = scaleSumSpectrumPoints(pSpectrum, 
						   i,
						   rawPoints);
    string         componentName = createComponentGateName(gatename,
							   xId, yId);
    CGate* pGate = api.CreateGate(componentGateType,
				  gateParams, points);

    api.AddGate(componentName, pGate);

    componentNames.push_back(componentName);
  }
  // Now create the or gate:

  CGate* orGate = api.CreateOrGate(componentNames);

  if (api.FindGate(gatename)) {
    api.ReplaceGate(gatename, *orGate);
  }
  else {
    api.AddGate(gatename, orGate);
  }
}
// Create a unique component name for a component of a 
// gate accepted on a sum spectrum.
// component names are of the form basename.p1.p2.serial
// where
//   basename - the final name of the or gate of which this is a component.
//   p1       - First parameter id of the gate.
//   p2       - Second paraemter id of the gate
//   serial   - Is an unsigned integer that uniquifies any conflicts.
// 
// Parameters:
//   std::string baseName     - Name of the gate being made.
//   UInt_t p1, p2            - Parameter ids of the component gate.
//
string 
CXamineEventHandler::createComponentGateName(string baseName,
					     UInt_t p1,
					     UInt_t p2)
{
  UInt_t serial=0;
  SpecTcl& api(*(SpecTcl::getInstance()));

  while (true) {
    char gateName[1000];	// hopefully this is large enough...
    snprintf(gateName, sizeof(gateName), "_%s.%d.%d.%03d",
	     baseName.c_str(), p1, p2, serial);
    string strName(gateName);
    if (!api.FindGate(strName)) {
      return strName;
    }
    serial++;
  }
}
//  Create a set of scaled points for a component gate of ta gate accepted on
//  as summed 2d spectrum.
// Parameters:
//     CSpectrum* pSpectrum           - pointer to the spectrum in which we're 
//                                      scaling gates.
//     UInt_t     firstAxis           - Number of the converter for the x point.
//                                      For the y point it's firstAxis+1.
//     std::vector<CPoint> rawPoints  - The list of points to convert.
// Returns:
//   std::vector<FPoint>     - The converted points.
//
vector<FPoint>
CXamineEventHandler::scaleSumSpectrumPoints(CSpectrum* pSpectrum,
					  UInt_t     firstAxis,
					  vector<CPoint> rawPoints)
{
  CSpectrum2Dm*      p = dynamic_cast<CSpectrum2Dm*>(pSpectrum);
  assert(p);
  CSpectrum2Dm::Axes a = p->getAxisMaps();
  vector<FPoint>     result;

  for (int i =0; i < rawPoints.size(); i++) {
    Float_t x = a[firstAxis].AxisToParameter(rawPoints[i].X());
    Float_t y = a[firstAxis+1].AxisToParameter(rawPoints[i].Y());
    
    result.push_back(FPoint(x,y));
  }
  return result;

}
