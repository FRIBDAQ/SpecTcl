//  CXaminePushButton.cpp
// Encapsulates a push button description.

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

#include "XaminePushButton.h"                               

static const char* Copyright = 
"CXaminePushButton.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXaminePushButton

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void FormatMessageBlock ( ButtonDescription& rButton )
//  Operation Type:
//     Adaptor to Xamine C-API
//
void 
CXaminePushButton::FormatMessageBlock(ButtonDescription& rButton) const 
{
// Formats the Button Description by setting
// the type to Push and letting the base class to the
// rest
//
// Formal Parameters:
//     ButtonDescription& rButton:
//         Refers to the buffer description being filled in.
//

  CXamineButton::FormatMessageBlock(rButton);
  rButton.type = Push;
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//    CXaminePushButton*   PushButton()
//    CXamineToggleButton* ToggleButton()
//
// Operation Type:
//    Type Safe upcast
//
CXaminePushButton*
CXaminePushButton::PushButton()
{
  return (CXaminePushButton*)this;
}
CXamineToggleButton*
CXaminePushButton::ToggleButton()
{
  return (CXamineToggleButton*)kpNULL;
}
