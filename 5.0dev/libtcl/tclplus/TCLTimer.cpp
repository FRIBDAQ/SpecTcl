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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
//  CTCLTimer.cpp
// Encapsulates a TCL/TK timer object.  A timer object can be
// scheduled for execution some number of milliseconds later from
// the TK event loop.  Timers are scheduled on a one-shot basis,
// however the Set() function allows the timer to repropagate itself.
//
//  Typically this class will be used by overriding the operator()
//  virtual member function to perform the actions required of a
//  timer.  
//

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
#include "TCLTimer.h"                               

using namespace std;

// Functions for class CTCLTimer

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void CallbackRelay ( ClientData pObject )
//  Operation Type:
//     Callback Relay.
//
void 
CTCLTimer::CallbackRelay(ClientData pObject) 
{
// Relays the Tk Time C call to object context.
//
// Formal Parameters:
//    ClientData  pObject:
//        Really a pointer to a CTCLTimer derived object who's
//        operator() will be called by this member.


  CTCLTimer* pTimer = (CTCLTimer*) pObject;
  pTimer->m_fSet = kfFALSE;		// No longer set when here.
  (*pTimer)();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Set (  )
//  Operation Type:
//     Mutator
//
void 
CTCLTimer::Set() 
{
// Sets the object's timer to fire again.
// Note that if the timer is pending it will
// be cancelled before it is set.  The delay
// used will be that of m_nMsec
//

  if(!m_fSet) {
    m_tToken = Tcl_CreateTimerHandler(m_nMsec, 
				     CallbackRelay,
				     (ClientData)this);
    m_fSet = kfTRUE;
  }

  
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Clear (  )
//  Operation Type:
//     mutator
//
void 
CTCLTimer::Clear() 
{
// Cancels this pending timer object,
// however only if m_fSet is kfTRUE
// indicating that it is pending in the first place.
// If m_fSet is kfFALSE, this is a NO-OP.
//
  
  if(m_fSet) {
    Tcl_DeleteTimerHandler(m_tToken);
    m_fSet = kfFALSE;
    m_tToken = 0;
  }

}

