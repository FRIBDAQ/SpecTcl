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

static const char* Copyright = "(C) Copyright Michigan State University 2011, All rights reserved";
//  CXamineEvent.cpp
// Encapsulates the sorts of events which can come
// from the Xamine display program.
// Events can either be Button action
// events or Gate events.
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include <config.h>
#include "XamineEvent.h"                               
#include "XamineGate.h"
#include "ButtonEvent.h"
#include <histotypes.h>
#include <Xamine.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Functions for class CXamineEvent

//////////////////////////////////////////////////////////////////////////
//
// Function:
//   operator==(const XamineEvent& rRhs)
// Operation Type:
//    Equality comparison.
//
int
CXamineEvent::operator==(const CXamineEvent& rRhs)
{
  return (memcmp(&m_EventData, &(rRhs.m_EventData), 
		 sizeof(m_EventData)) == 0);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CDisplayGate* GateCast (  )
//  Operation Type:
//     Typesafe Cast.
//
CXamineGate* 
CXamineEvent::GateCast() 
{
//    If the XamineEvent m_EventData member is a 
// gate event, a CDisplayGate object is dynamically
// instantiated from the data in m_EventData and
// a pointer to it is returned to the caller.  It is up to
// the caller to delete that object when done.
//
//    If the m_EventData member does not represent
// a gate event, then a NULL pointer is returned.
//    This member is most often used in the following
//  template of code:
// 
//       if(CDisplayGate* pGate = Event.Gate()) {
//                        // process gate input
//       }
//       if(CButtonEvent* pButton = Event.Button()) {
//                       // Process button
//       }
// Exceptions:  

  return ((m_EventData.event == Gate) ?  
	                      new CXamineGate(m_EventData.data.gate)
	                      : (CXamineGate*)kpNULL); 
                  
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CButtonEvent* ButtonCast (  )
//  Operation Type:
//     Type-safe cast.
//
CButtonEvent* 
CXamineEvent::ButtonCast() 
{
//    If m_EventData represents a Button event, then
// a CButtonEvent object is new'ed into existence from
// the data in m_EventData.  A pointer to this object is
// returned.  It is up to the caller to eventually delete the
// object when done.
//
//    If m_EventData does not represent a button event, then
//  a null pointer (kpNULL) is returned.  See the comments for
//  the Gate() member function for typicall usage of this type safe
//  'cast'.

  return ((m_EventData.event == Button_Press) ? 
	                            new CButtonEvent(m_EventData.data.button)  
	                         :  (CButtonEvent*) kpNULL);
}
