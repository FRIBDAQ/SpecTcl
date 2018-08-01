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


//  CXamineEvent.h:
//
//    This file defines the CXamineEvent class.
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

#ifndef XAMINEEVENT_H  //Required for current class
#define XAMINEEVENT_H

#include <Xamine.h>

// Forward definitions:

class CXamineGate;
class CButtonEvent;

//
                               
class CXamineEvent      
{
  msg_XamineEvent m_EventData;  // Raw event data from Xamine.
  
public:
  CXamineEvent() {}
  virtual ~ CXamineEvent ( ) { }       //Destructor
			//Constructor with arguments
				       
  CXamineEvent (  msg_XamineEvent am_EventData  ) :   
    m_EventData (am_EventData)  
  { }        
	
			//Copy constructor

  CXamineEvent (const CXamineEvent& aCXamineEvent ) 
  {   
    m_EventData = aCXamineEvent.m_EventData;
  }                                     

			//Operator= Assignment Operator

  CXamineEvent& operator= (const CXamineEvent& aCXamineEvent)
  { 
    if (this == &aCXamineEvent) return *this;          
    m_EventData = aCXamineEvent.m_EventData;
        
    return *this;
  }                                     

			//Operator== Equality Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  int operator== (const CXamineEvent& aCXamineEvent);
  int operator!=(const CXamineEvent& aCXamineEvent) {
    return !(operator==(aCXamineEvent));
  }

  // selectors:
public:
  const msg_XamineEvent& getm_EventData() const
  {
    return m_EventData;
  }
  // Mutators

protected:
  void setm_EventData (msg_XamineEvent am_EventData)
  { 
    m_EventData = am_EventData;
  }
public:                       
    CXamineGate* GateCast ()  ;
    CButtonEvent* ButtonCast ()  ;
 
};

#endif
