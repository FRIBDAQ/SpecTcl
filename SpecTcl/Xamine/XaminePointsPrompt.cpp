//  CXaminePointsPrompt.cpp
// Encapsulates the description of a points prompter.
// Points prompters pop up a points selection dialog box
// which allows users to accept a series of  points.
// The points along with a string which can be typed in by
// the user is passed back when the button event is finally
// declared.
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

#include "XaminePointsPrompt.h"                               

static const char* Copyright = 
"CXaminePointsPrompt.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXaminePointsPrompt

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void FormatPrompterBlock ( ButtonDescription& rButton )
//  Operation Type:
//     
//
void 
CXaminePointsPrompt::FormatPrompterBlock(ButtonDescription& rButton) const 
{
// Formats the ButtonDescription buffer as follows:
// 
//  prompter  <- Points
//  minpts      <- m_nMinPoints
//  maxpts     <- m_nMaxPoints
//  promptstr <- m_sPromptString.
//
// Formal Parameters:
//     ButtonDescription& rButton:
//         Refers to the button description being
//         formatted.
// Exceptions:  

  rButton.prompter   = Points;
  rButton.minpts     = m_nMinPoints;
  rButton.maxpts     = m_nMaxPoints;
  SetPromptString(rButton);

}
