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

//  MaskEqualGate.h:
//
//    This file defines the CMaskEqualGate class.
//
//
//  Author:
//    Timothy Hoagland
//    NSCL / WIENER
//    s04.thoagland@wittenberg.edu
//
//  Adapted from the CTrueGate class written by:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __MASKEQUALGATE_H  //Required for current class
#define __MASKEQUALGATE_H
                               //Required for base classes
#ifndef __GATE_H
#include "Gate.h"
#endif                               
  
#ifndef __MASKGATES_H
#include "MaskGates.h"
#endif


#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif
                             
class CMaskEqualGate : public CMaskGate  
{
  
public:

  CMaskEqualGate ( UInt_t am_nId, UInt_t  am_lCompare ) : // Constructor
    CMaskGate(am_lCompare),
    m_nId (am_nId)  
    { }        

  ~ CMaskEqualGate ( ) { }       //!< Destructor

	
  //! Copy constructor

  CMaskEqualGate (const CMaskEqualGate& aCMaskEqualGate )   : CMaskGate (aCMaskEqualGate) 
  { 
    m_cCompare = aCMaskEqualGate.m_cCompare;
    m_nId      = aCMaskEqualGate.m_nId;               
  }                                     

  //! Operator= Assignment Operator

  CMaskEqualGate& operator= (const CMaskEqualGate& aCMaskEqualGate) 
  { 
    if (this == &aCMaskEqualGate) return *this;          
    CMaskGate::operator= (aCMaskEqualGate);
    m_cCompare = aCMaskEqualGate.m_cCompare;
    m_nId      = aCMaskEqualGate.m_nId;
    
    return *this;                                                                                                 
  }          

 
 long getCompare() const
  {
    return m_cCompare;
  }                           

  UInt_t getId() const
  {
    return m_nId;
  }
                            
private:
  int operator == (const CMaskEqualGate& aCMaskEqualGate) const;
  UInt_t m_nId;

public:            
  virtual   Bool_t operator() (CEvent& rEvent)  ;
  virtual   CGate* clone ()  ;
  virtual   std::string Type ()  const;
  virtual   Bool_t inGate(CEvent& rEvent, const STD(vector)<UInt_t>& Params);
  virtual   Bool_t inGate(CEvent& rEvent);


};

#endif
