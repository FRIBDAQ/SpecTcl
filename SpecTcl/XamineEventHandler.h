
 
// Class: CXamineEventHandler                     //ANSI C++
//
// Set as a Tcl File handler on the Xamine gates file descriptor.
// This class manages Xamine file events by getting an event
// and calling either the OnGate or OnButton member functions.
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved XamineEventHandler.h
//

#ifndef __XAMINEEVENTHANDLER_H  //Required for current class
#define __XAMINEEVENTHANDLER_H

#ifndef __TCLFILEHANDLER_H
#include <TCLFileHandler.h>
#endif                               

                               //Required for 1:1 association classes
#ifndef __HISTOGRAMMER_H
#include <Histogrammer.h>
#endif

#ifndef __XAMINEPLUS_H
#include <Xamineplus.h>
#endif

#ifndef __GATEFACTORY_H
#include "GateFactory.h"
#endif

#ifndef __POINT_H
#include <Point.h>
#endif

#ifndef _TK_H
#include <tk.h>
#define _TK_H
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __STL_LIST
#include <list>
#ifndef __STL_LIST
#define __STL_LIST
#endif
#endif


class CDisplayGate;
class CButtonEvent;
class CSpectrum;

// XamineEventHandler class declaration:
                                                               
class CXamineEventHandler
{
  // Base class for button handlers:
public:
  class CButtonHandler {
  public:
    virtual Bool_t operator()(CButtonEvent& event) = 0;
  };
  class CRestartHandler {
  public:
    virtual void  operator()() = 0;
  };
  typedef STD(list)<CButtonHandler*> ButtonHandlerList;
  typedef STD(list)<CRestartHandler*> RestartHandlerList;
private:                       
  CTCLInterpreter*   m_pInterp;
  CHistogrammer*     m_pHistogrammer; //1:1 association object data member      
  int                m_nFd;
  Tcl_TimerToken     m_Timer;	// Poll timer for read.
  ButtonHandlerList  m_buttonHandlers; // List of button handlers.
  RestartHandlerList m_restartHandlers;
public:

   // Constructors and other cannonical operations:

  CXamineEventHandler (CTCLInterpreter* pInterp,
		       CHistogrammer*   pHistogrammer) :
    m_pInterp(pInterp),
    m_pHistogrammer(pHistogrammer),
    m_nFd(pHistogrammer->getDisplayer()->GetEventFd())
  { 
    Set();		// Starts out enabled.
  } 
  virtual  ~ CXamineEventHandler ( )  // Destructor 
     { }  
  
   //Copy constructor 

  CXamineEventHandler (const CXamineEventHandler& rhs ) :
    m_pInterp(rhs.m_pInterp),
    m_pHistogrammer(rhs.m_pHistogrammer),
    m_nFd(rhs.m_nFd),
    m_Timer(rhs.m_Timer)
  { 
   


  }                                     

   // Operator= Assignment Operator 

  CXamineEventHandler& operator= 
  (const CXamineEventHandler& aCXamineEventHandler) {
    if(this != &aCXamineEventHandler) {
      setHistogrammer(aCXamineEventHandler.m_pHistogrammer);
    }
    return *this;
  }
 
   //Operator== Equality Operator 

  int operator== (const CXamineEventHandler& rhs) const {
    return ( (m_pInterp == rhs.m_pInterp)      &&
	     (m_pHistogrammer == rhs.m_pHistogrammer) &&
	     (m_nFd == rhs.m_nFd)                     &&
	     (m_Timer == rhs.m_Timer));
  }
	
// Selectors:

public:
                       
  CHistogrammer* getHistogrammer() const
  { 
    return m_pHistogrammer;
  }
                       
// Mutators:

protected:
       
  void setHistogrammer (CHistogrammer* am_pHistogrammer)
  { 
    Clear();			// Disable callback on current fid.
    m_pHistogrammer = m_pHistogrammer;
    m_nFd = (m_pHistogrammer->getDisplayer()->GetEventFd()); // Update file id.
    Set();			// Set callback on next fid.
  }

  // User hooks that observer various things can be added here.

public:
  void addButtonHandler(CButtonHandler& handler);
  void addRestartHandler(CRestartHandler& handler);

  // Overridable operations.
public:

 virtual   void operator() ()    ;
 virtual   void Set();
 virtual   void Clear();

protected:
 virtual   void OnGate (CDisplayGate& rXamineGate)    ;
 virtual   void OnButton (CButtonEvent& rButton)    ;
 virtual   UInt_t FindDisplayBinding (const STD(string)& rName);

 static void CallbackRelay(ClientData pObject);
  // Utilities:
private:

  void make2dSumgate(STD(string)            gateName,
		     CGateFactory::GateType componentGateType,
		     CSpectrum*             pSpectrum,
		     STD(vector)<CPoint>    rawPoints);
  STD(string) createComponentGateName(STD(string) baseName,
				      UInt_t      p1,
				      UInt_t      p2);
  STD(vector)<FPoint> scaleSumSpectrumPoints(CSpectrum*          pSpectrum,
					     UInt_t              firstAxis,
					     STD(vector)<CPoint> rawPoints);
		     
};

#endif
