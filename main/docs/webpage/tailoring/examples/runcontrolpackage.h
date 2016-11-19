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

#ifndef __RUNCONTROLPACKAGE_H  //Required for current class
#define __CRUNCONTROLPACKAGE_H
                               //Required for base classes
#ifndef __TCLCOMMANDPACKAGE_H
#include "TCLCommandPackage.h"
#endif                               


class CTCLInterpreter;
class CStartRun;
class CStopRun;  
class CTCLVariable;
                             
class CRunControlPackage  : public CTCLCommandPackage        
{
  CStartRun*     m_pStartRun;
  CStopRun*      m_pStopRun;
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
    Bool_t StartRun ()  ;
    Bool_t StopRun ()  ;
    Bool_t isRunning ()  ;

};

#endif
