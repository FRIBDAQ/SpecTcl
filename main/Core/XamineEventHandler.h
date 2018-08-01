
 
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

#ifndef XAMINEEVENTHANDLER_H  //Required for current class
#define XAMINEEVENTHANDLER_H

#include <TCLFileHandler.h>
#include <Histogrammer.h>
#include <Xamineplus.h>
#include "GateFactory.h"
#include <Point.h>
#include <tk.h>
#include <string>
#include <vector>
#include <list>


class CDisplayGate;
class CHistogrammer;
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
  typedef std::list<CButtonHandler*> ButtonHandlerList;
  typedef std::list<CRestartHandler*> RestartHandlerList;
private:                       
  CHistogrammer*     m_pSorter;
  CXamine*           m_pDisplay; //1:1 association object data member
  int                m_nFd;
  Tcl_TimerToken     m_Timer;	// Poll timer for read.
  ButtonHandlerList  m_buttonHandlers; // List of button handlers.
  RestartHandlerList m_restartHandlers;
public:

   // Constructors and other cannonical operations:

  CXamineEventHandler (CHistogrammer* pHistogrammer,
                       CXamine*   pDisplay);
  virtual  ~ CXamineEventHandler ( )  // Destructor 
     { }  
  
   //Copy constructor 
  CXamineEventHandler (const CXamineEventHandler& rhs );

   // Operator= Assignment Operator 
  CXamineEventHandler& operator= 
  (const CXamineEventHandler& aCXamineEventHandler);
 
   //Operator== Equality Operator 
  int operator== (const CXamineEventHandler& rhs) const;

// Selectors:

public:
                       
  CXamine* getDisplay() const
  { 
    return m_pDisplay;
  }
                       
// Mutators:

protected:
       
  void setDisplay (CXamine* am_pDisplay)
  { 
    Clear();			// Disable callback on current fid.
    m_pDisplay = am_pDisplay;
    m_nFd = (m_pDisplay->GetEventFd()); // Update file id.
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
 virtual   void OnGate (CXamineGate& rXamineGate)    ;
 virtual   void OnButton (CButtonEvent& rButton)    ;
 virtual   UInt_t FindDisplayBinding (const std::string& rName);

 static void CallbackRelay(ClientData pObject);
  // Utilities:
private:

  void make2dSumgate(std::string            gateName,
		     CGateFactory::GateType componentGateType,
		     CSpectrum*             pSpectrum,
		     std::vector<CPoint>    rawPoints);
  std::string createComponentGateName(std::string baseName,
				      UInt_t      p1,
				      UInt_t      p2);
  std::vector<FPoint> scaleSumSpectrumPoints(CSpectrum*          pSpectrum,
					     UInt_t              firstAxis,
					     std::vector<CPoint> rawPoints);
		     
};

#endif
