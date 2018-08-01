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
//    This file defines the CMaskNotGate class.
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

#ifndef MASKNOTGATE_H  //Required for current class
#define MASKNOTGATE_H
                               //Required for base classes
#include "Gate.h"
#include "MaskGates.h"
#include <histotypes.h>
                             
class CMaskNotGate : public CMaskGate  
{
  
public:

  CMaskNotGate ( UInt_t am_nId, UInt_t  am_lCompare ) : // Constructor
    CMaskGate(am_lCompare),
    m_nId (am_nId)  
    { }        

  ~ CMaskNotGate ( ) { }       //!< Destructor

	
  //! Copy constructor

  CMaskNotGate (const CMaskNotGate& aCMaskNotGate )   : CMaskGate (aCMaskNotGate) 
  { 
    m_cCompare = aCMaskNotGate.m_cCompare;
    m_nId      = aCMaskNotGate.m_nId;               
  }                                     

  //! Operator= Assignment Operator

  CMaskNotGate& operator= (const CMaskNotGate& aCMaskNotGate) 
  { 
    if (this == &aCMaskNotGate) return *this;          
    CMaskGate::operator= (aCMaskNotGate);
    m_cCompare = aCMaskNotGate.m_cCompare;
    m_nId      = aCMaskNotGate.m_nId;
    
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
  int operator == (const CMaskNotGate& aCMaskNotGate) const;
  UInt_t m_nId;


public:            
  virtual   Bool_t operator() (CEvent& rEvent)  ;
  virtual   CGate* clone ()  ;
  virtual   std::string Type ()  const;
  virtual   Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params);
  virtual   Bool_t inGate(CEvent& rEvent);


};

#endif
