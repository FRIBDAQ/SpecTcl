//  CXamineTextPrompt.cpp
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

#include "XamineTextPrompt.h"                               

static const char* Copyright = 
"CXamineTextPrompt.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXamineTextPrompt

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void FormatPrompterBlock ( ButtonDescription& rButton )
//  Operation Type:
//     Adaptor to Xamine C-API
//
void 
CXamineTextPrompt::FormatPrompterBlock(ButtonDescription& rButton) const 
{
// Formats the ButtonDescription by setting
// Prompter to Text and the promptstr to the value
// of the string.

  rButton.prompter = Text;
  SetPromptString(rButton);

}
