//  CXamineFilePrompt.cpp
// Encapsulates a file chooser prompt.  These prompts
// pop up a file chooser dialog when the button is pressed.
// the selected file is then passed back with the button event
// when it is declared.

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

#include "XamineFilePrompt.h"                               

static const char* Copyright = 
"CXamineFilePrompt.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXamineFilePrompt

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void FormatPrompterBlock ( ButtonDescription& rButton )
//  Operation Type:
//     Adapter to Xamine's C-API.
//
void 
CXamineFilePrompt::FormatPrompterBlock(ButtonDescription& rButton) const 
{
// Formats the prompter part of the ButtonDescription buffer.
// Sets prompter to Filename, sets the prompt string as well.
//
// Formal Parameters:
//    ButtonDescription& rButton:
//      Refers to the button description being formatted.

  rButton.prompter = Filename;
  SetPromptString(rButton);

}
