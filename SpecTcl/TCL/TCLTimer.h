//  CTCLTimer.h:
//
//    This file defines the CTCLTimer class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __TCLTIMER_H  //Required for current class
#define __TCLTIMER_H
                               //Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif    

#ifndef __TCL_TCL_H
#include <tcl.h>
#define __TCL_TCL_H 
#endif
        
#ifndef __TCL_TK_H
#include <tk.h>
#define _TCL_TK_H
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif
                  
                               
class CTCLTimer  : public CTCLInterpreterObject        
{
  Tk_TimerToken m_tToken;  // Timer token returned from Tk_CreateTimerHandler.
  UInt_t m_nMsec;		// No. Milleseconds delay on timer.
  Bool_t m_fSet;		// kfTRUE if timer is  pending (m_tToken valid).
  
public:
			//Default constructor

  CTCLTimer () :  
    CTCLInterpreterObject(),
    m_tToken(0),  
    m_nMsec(0),  
    m_fSet(0)   
  { } 
  CTCLTimer(CTCLInterpreter* pInterp, UInt_t nMsec = 0) :
    CTCLInterpreterObject(pInterp),
    m_tToken(0),
    m_nMsec(nMsec),
    m_fSet(0)
  {}
  virtual ~CTCLTimer ( ) { 
    Clear();
  }       
  //Destructor

			//Copy constructor [ not allowed ]
private:
  CTCLTimer (const CTCLTimer& aCTCLTimer );

			//Operator= Assignment Operator [ not allowed ]

  CTCLTimer& operator= (const CTCLTimer& aCTCLTimer);

			//Operator== Equality Operator [ makes no sense ]
  int operator== (const CTCLTimer& aCTCLTimer);

  // Selectors:

public:

  Tk_TimerToken getToken() const
  {
    return m_tToken;
  }
  UInt_t getMsec() const
  {
    return m_nMsec;
  }
  Bool_t IsSet() const
  {
    return m_fSet;
  }
  // Mutators:

protected:                   

  void setToken (Tk_TimerToken am_tToken)
  { 
    m_tToken = am_tToken;
  }
  void setMsec (UInt_t am_nMsec)
  { 
    m_nMsec = am_nMsec;
  }
  void setSet (Bool_t am_fSet)
  { 
    m_fSet = am_fSet;
  }
  // Operations and overridables:

public:
  virtual   void operator() ()   = 0;
  static  void CallbackRelay (ClientData pObject)  ;
  void Set ()  ;
  void Set(UInt_t nms) {
    m_nMsec = nms;
    Set();
  }
  void Clear ()  ;
};

#endif




