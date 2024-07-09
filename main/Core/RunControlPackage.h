/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


//  CRunControlPackage.h:
//
//    This file defines the CRunControlPackage class.
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

#ifndef RUNCONTROLPACKAGE_H  //Required for current class
#define CRUNCONTROLPACKAGE_H
                               //Required for base classes
#include "TCLCommandPackage.h"


class CTCLInterpreter;
class CStartRun;
class CStopRun;  
class CTCLVariable;
class CPackagedCommandAlias;
                             
class CRunControlPackage  : public CTCLCommandPackage        
{
  CStartRun*     m_pStartRun;
  CPackagedCommandAlias*   m_pStartAlias;
  CStopRun*      m_pStopRun;
  CPackagedCommandAlias*   m_pStopAlias;

  CTCLVariable*  m_pRunState;
  
public:
                        // Parameterized Constructor.

  CRunControlPackage(CTCLInterpreter* pInterp);
  virtual ~ CRunControlPackage ( ); // Destructor


			//Copy constructor

  CRunControlPackage (const CRunControlPackage& aCRunControlPackage )   : 
    CTCLCommandPackage (aCRunControlPackage) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CRunControlPackage& operator= (const CRunControlPackage& aCRunControlPackage)
  { 
    if (this == &aCRunControlPackage) return *this;          
    CTCLCommandPackage::operator= (aCRunControlPackage);
  
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CRunControlPackage& aCRunControlPackage)
  { 
    return ((CTCLCommandPackage::operator== (aCRunControlPackage)));
  }                             

  // Operations on the class:

public:
    void InitializeRunState ()  ;
  TCLPLUS::Bool_t StartRun ()  ;
  TCLPLUS::Bool_t StopRun ()  ;
  TCLPLUS::Bool_t isRunning ()  ;

};

#endif
