//  CXamineToggleButton.cpp
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

#include "XamineToggleButton.h"                               

static const char* Copyright = 
"CXamineToggleButton.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXamineToggleButton

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void FormatMessageBlock ( ButtonDescription& rButton )
//  Operation Type:
//     Adaptor - Xamime C-API
//
void 
CXamineToggleButton::FormatMessageBlock(ButtonDescription& rButton) const 
{
// Sets the button type and toggle state
// then delegates the rest to the parent class.
//
// Formal Paramters:
//    ButtonDescription& rButton:
//       References the button description being built.
//

  CXamineButton::FormatMessageBlock(rButton);
  rButton.type  = Toggle;
  rButton.state = m_fInitialState ? T : F;

}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    CXaminePushButton*   PushButton()
//    CXamineToggleButton* ToggleButton()
//
// Operation Type:
//   Type safe upcasts
//
CXaminePushButton*
CXamineToggleButton::PushButton()
{
  return (CXaminePushButton*)kpNULL;
}
CXamineToggleButton*
CXamineToggleButton::ToggleButton()
{
  return this;
}
