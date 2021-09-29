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

#ifndef TCLRUNFILEHANDLER_H  //Required for current class
#define TCLRUNFILEHANDLER_H
                               //Required for base classes
#include <TCLIdleProcess.h>
#include "RunControl.h"
#include <histotypes.h>
                               
class CRunControl;		// Forward Class reference.
class CTCLInterpreter;		// Forward Class reference.
                                                               
class CTCLRunFileHandler  : public CTCLIdleProcess       
{
  
  CRunControl* m_pRun;
  UInt_t       m_nBufferSize;
  CTCLInterpreter* m_pInterp;

public:
  CTCLRunFileHandler (CTCLInterpreter* pInterp, CRunControl& rRun);
  ~ CTCLRunFileHandler ( ) { };       //Destructor
	
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



