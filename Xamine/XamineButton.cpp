//  CXamineButton.cpp
// Base class which encapsulates a button description 
// passed to Xamine to define a client button.  
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

#include "XamineButton.h"                               
#include "XamineButtonPrompt.h"
#include "RangeError.h"
#include "XaminePushButton.h"
#include "XamineToggleButton.h"


#include <string.h>
#include <Xamine.h>
static const char* Copyright = 
"CXamineButton.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXamineButton

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void FormatMessageBlock ( ButtonDescription& rButton )
//  Operation Type:
//     Adapter - Xamine C API
//
void 
CXamineButton::FormatMessageBlock(ButtonDescription& rButton) const 
{
// Formats a ButtonDescription block.  Calls
//   m_pPrompter.FormatMessageBlock() to get the
// prompter part filled in and sets all other fields except
// type from our internal attributes.
//
// Formal Parameters:
//      ButtonDescription& rButton:
//           References the button description block.
//

  // Easy stuff first:

  rButton.button_code   = m_nReturnCode;
  rButton.sensitive     = m_fEnabled ? T : F;
  rButton.whenavailable = m_eWhenSensitive;


  // Now the label:

  if(m_sLabel.size() > sizeof(rButton.label) -1) {
    throw CRangeError(0, sizeof(rButton.label) -1, m_sLabel.size(),	       
		      "CXamineButton::FormatMessageBlock - copying label");
  }
  memset(rButton.label, 0, sizeof(rButton.label));
  strncpy(rButton.label, m_sLabel.c_str(), sizeof(rButton.label) - 1);

  // And finally the prompt information:

  m_pPrompter->FormatPrompterBlock(rButton);

}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    CXamineToggleButton* ToggleButton()
//    CXaminePushButton*   PushButton()
//
// Operation Type:
//   Type safe upcasts.
//
CXamineToggleButton*
CXamineButton::ToggleButton()
{
  return (CXamineToggleButton*)kpNULL;
}
CXaminePushButton*
CXamineButton::PushButton()
{
  return (CXaminePushButton*)kpNULL;
}
