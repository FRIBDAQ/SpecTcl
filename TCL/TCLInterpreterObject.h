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

//  CTCLInterpreterObject.h:
//
//    This file defines the CTCLInterpreterObject class.
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

#ifndef __TCLINTERPRETEROBJECT_H  //Required for current class
#define __TCLINTERPRETEROBJECT_H

#ifndef __TCL_H
#include <tcl.h>
#define __TCL_H
#endif                               

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __CRT_ASSERT_H
#include <assert.h>
#define __CRT_ASSERT_H
#endif

class CTCLInterpreter;		// Forward reference to interpreter.
                                                               
class CTCLInterpreterObject      
{
  
  CTCLInterpreter* m_pInterpreter;
  
public:
			//Default constructor

  CTCLInterpreterObject () : m_pInterpreter(0) 
  { }
  virtual  ~CTCLInterpreterObject ( ) { }       //Destructor
 


  CTCLInterpreterObject (CTCLInterpreter* pInterp ) :
    m_pInterpreter(pInterp)
  { }        
	
			//Copy constructor

  CTCLInterpreterObject (const CTCLInterpreterObject& aCTCLInterpreterObject ) 
  { 
    m_pInterpreter = aCTCLInterpreterObject.m_pInterpreter;
  }                                     

			//Operator= Assignment Operator

  CTCLInterpreterObject& operator= 
                        (const CTCLInterpreterObject& aCTCLInterpreterObject)
  { 
    if (this == &aCTCLInterpreterObject) return *this;          
    m_pInterpreter = aCTCLInterpreterObject.m_pInterpreter;
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLInterpreterObject& aCTCLInterpreterObject) const
  { 
    return (m_pInterpreter == aCTCLInterpreterObject.m_pInterpreter);

  }                             
                       
                       //Get accessor function for 1:1 association
  CTCLInterpreter* getInterpreter() const
  {
    return m_pInterpreter;
  }


  CTCLInterpreter* Bind (CTCLInterpreter& rBinding){
    return Bind(&rBinding);
  }
  CTCLInterpreter* Bind (CTCLInterpreter* pBinding);
  //
  // protected utility:
  //
protected:
  CTCLInterpreter* AssertIfNotBound() {
    CTCLInterpreter* pInterp = getInterpreter();
    assert(pInterp != kpNULL);
    return pInterp;
  }

};

#endif
