//  CXamineConfirmPrompt.cpp
// Encapsulates a confirmation prompt specification.
// Confirmation prompts are dialogs with a yes/no choice.
// the prompt string is displayed.  If the user clicks the Yes
// button an Xamine event is declared.  Otherwise, it is as if
// the initial button was never pressed from the application's
// point of view.
//
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


#include "XamineConfirmPrompt.h"                               
#include "RangeError.h"

#include <string.h>
#include <assert.h>
#include <Xamine.h>

static const char* Copyright = 
"CXamineConfirmPrompt.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXamineConfirmPrompt

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void FormatPrompterBlock ( const ButtonDescriptio rButton )
//  Operation Type:
//     Adaptor to C-API.
//
void 
CXamineConfirmPrompt::FormatPrompterBlock(ButtonDescription& rButton) const 
{
// Formats the prompter part of a button Descrption
// block.  The prompt string is placed in the promtpstr
// field and the prompter type is set to 
// Confirmation.
//
// Formal Parameters:
//
//   const ButtonDescription& rButton:
//       Refers to the button description block being formatted.


  
  rButton.prompter = Confirmation;
  SetPromptString(rButton);

  
}
