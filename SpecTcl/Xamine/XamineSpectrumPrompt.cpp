//  CXamineSpectrumPrompt.cpp
// Encapsulates a description of a spectrum chooser prompt
// Spectrum chooser prompts pop up a dialog with a list of
// spectra from which one may be chosen.  The id of the chosen
// spectrum is passec back to the client whent he button event is
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

#include "XamineSpectrumPrompt.h"                               
#include <assert.h>
#include <histotypes.h>

static const char* Copyright = 
"CXamineSpectrumPrompt.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXamineSpectrumPrompt

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void FormatPrompterBlock ( ButtonDescription& rButton )
//  Operation Type:
//     Adapter - to Xamine C-API
//
void 
CXamineSpectrumPrompt::FormatPrompterBlock(ButtonDescription& rButton) const 
{
// Formats the prompt region of a ButtonDescription as
// follows:
//    prompter    <- Spectrum
//    spectype   <- Determined by m_eSpectrumType
//    promptstr   <- From m_sPromptString
//
// Formal Parameters:
//     ButtonDescription& rButton:
//        Refers to the button description being formatted.

  rButton.prompter = Spectrum;
  switch(m_eSpectrumType) {
  case keAny:
    rButton.spectype = Any;
    break;
  case ke1d:
    rButton.spectype = Oned;
    break;
  case ke2d:
    rButton.spectype = Twod;
    break;
  case keCompatible:
    rButton.spectype = Compatible;
    break;
  default:
    assert(kfFALSE);
  }
  SetPromptString(rButton);

}
