//  CTCLRunFileHandler.h:
//
//    This file defines the CTCLRunFileHandler class.
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

#ifndef __TCLRUNFILEHANDLER_H  //Required for current class
#define __TCLRUNFILEHANDLER_H
                               //Required for base classes
#ifndef __TCLFILEHANDLER_H
#include <TCLIdleProcess.h>
#endif  

#ifndef __RUNCONTROL_H
#include "RunControl.h"
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif                             
                               
class CRunControl;		// Forward Class reference.
class CTCLInterpreter;		// Forward Class reference.
                                                               
class CTCLRunFileHandler  : public CTCLIdleProcess       
{
  
  CRunControl* m_pRun;
  UInt_t       m_nBufferSize;

public:
  CTCLRunFileHandler (CTCLInterpreter* pInterp, CRunControl& rRun) :
    CTCLIdleProcess(pInterp),
    m_pRun(&rRun),
    m_nBufferSize(0)
  { }        
   ~ CTCLRunFileHandler ( ) { }       //Destructor
	
			//Copy constructor [illegal]
private:
  CTCLRunFileHandler (const CTCLRunFileHandler& aCTCLRunFileHandler );
public:

			//Operator= Assignment Operator [illegal]
private:
  CTCLRunFileHandler& operator= (const CTCLRunFileHandler& aCTCLRunFileHa);
public:
			//Operator== Equality Operator

  int operator== (const CTCLRunFileHandler& aCTCLRunFileHandler)
  { 
    return (
	    (CTCLIdleProcess::operator== (aCTCLRunFileHandler)) &&
	    (m_pRun == aCTCLRunFileHandler.m_pRun) &&
	    (m_nBufferSize == aCTCLRunFileHandler.m_nBufferSize)
	    );
  }                             
  //
  // Selectors:

public:
  const CRunControl* getRun() const
  {
    return m_pRun;
  }
  UInt_t getBufferSize() const
  {
    return m_nBufferSize;
  }
  //
  // Mutators:

protected:
  void setRun (CRunControl* am_pRun)
  { 
    m_pRun = am_pRun;
  }
public:
  void setBufferSize(UInt_t nBuffersize)
  {
    m_nBufferSize = nBuffersize;
  }
  // Public interface:

public:
  virtual   void operator() ();
 
};

#endif



