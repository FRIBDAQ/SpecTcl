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


#include "XamineEvent.h"                               
#include "DisplayGate.h"
#include "ButtonEvent.h"
#include <histotypes.h>
#include <Xamine.h>

static const char* Copyright = 
"CXamineEvent.cpp: Copyright 1999 NSCL, All rights reserved\n";

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
CDisplayGate* 
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
	                      new CDisplayGate(m_EventData.data.gate)
	                      : (CDisplayGate*)kpNULL); 
                  
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
