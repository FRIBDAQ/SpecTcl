//  CXamineNoPrompt.cpp
// Encapsulates a 'noprompt' prompter.  Noprompt buttons
// Declare an event to the user without any prompt.  This
// class is used to improve processing uniformity.
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


#include "XamineNoPrompt.h"                               

static const char* Copyright = 
"CXamineNoPrompt.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXamineNoPrompt

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void FormatPrompterBlock ( ButtonDescription& rPrompt )
//  Operation Type:
//     Adaptor for Xamine C-API
//
void 
CXamineNoPrompt::FormatPrompterBlock(ButtonDescription& rPrompt) const 
{
// Sets the prompter type to NoPrompt indicating
// that the button does not require a prompter.
// 
// Formal parameters:
//     ButtonDescription* rButton:
//        Reference to the button description to be modified.
//

  rPrompt.prompter = NoPrompt;

}
