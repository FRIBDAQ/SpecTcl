//  CTKRunControl.h:
//
//    This file defines the CTKRunControl class.
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

#ifndef __TKRUNCONTROL_H  //Required for current class
#define __TKRUNCONTROL_H
                               //Required for base classes
#ifndef __RUNCONTROL_H
#include "RunControl.h"
#endif    

#ifndef __TCLRUNFILEHANDLER_H
#include "TCLRunFileHandler.h"
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

class CAnalyzer;
class CFile;
class CTCLInterpreter;
                               
class CTKRunControl  : public CRunControl        
{
  // Attributes:
private:
  CTCLInterpreter*   m_pInterp;
  CTCLRunFileHandler m_FileHandler;
  std::string        m_sEndScript;
  
  // Static attributes:

public:

static UInt_t m_nDefaultBufferSize;
  
public:
			//Constructor with arguments
  CTKRunControl (CTCLInterpreter* pInterp,
		 CAnalyzer&       rAnalyzer,
		 CFile&           rEventSource,
		 UInt_t     nBufferSize=CTKRunControl::m_nDefaultBufferSize) :
    CRunControl(rAnalyzer, rEventSource),
    m_pInterp(pInterp),
    m_FileHandler(pInterp, *this),
    m_sEndScript("stop")
  {
    m_FileHandler.setBufferSize(nBufferSize);
  }
       
			//Copy constructor [illegal]
private:
  CTKRunControl (const CTKRunControl& aCTKRunControl );
public:

			//Operator= Assignment Operator [illegal]
private:
  CTKRunControl& operator= (const CTKRunControl& aCTKRunControl);
public:


			//Operator== Equality Operator

  int operator== (const CTKRunControl& aCTKRunControl)
  { 
    return (
	    (CRunControl::operator== (aCTKRunControl)) &&
	    (m_FileHandler        == aCTKRunControl.m_FileHandler)
	    );
  }                             
  // Public Interface:

public:         
  void setBufferSize(UInt_t nBufferSize) {
    m_FileHandler.setBufferSize(nBufferSize);
  }
  virtual   void Start ()  ;  
  void Stop ()  ;
  virtual void OnEnd();
  virtual   void OnBuffer (UInt_t nBytes)  ;

  
  ~ CTKRunControl ( ) { }       //Destructor
};

#endif
