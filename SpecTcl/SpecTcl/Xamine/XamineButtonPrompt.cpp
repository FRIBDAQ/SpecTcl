//
// XamineButtonPrompt.cpp
//   Implementation of the XamineButtonPrompt abstract base class
//   Provides utility functions for that class.
//
// Author:
//   Ron FOx
//   NSCL
//   Michigan State Unversity
//   East Lansing, MI 48824-1321
//
//
#include "XamineButtonPrompt.h"
#include "RangeError.h"

static const char* Copyright = 
"XamineButtonPrompt.cpp  (c) Copyright 1999 NSCL, All rights reserved";

//////////////////////////////////////////////////////////////////////////
//
// Function:
//     void SetPromptString(ButtonDescription& rButton) const
// Operation type:
//    Protected utility.
//
void
CXamineButtonPrompt::SetPromptString(ButtonDescription& rButton) const
{
  std::string rs = getPromptString();
  if(rs.size() > (sizeof(rButton.promptstr) - 1)) {
    throw CRangeError(0, sizeof(rButton.promptstr) - 1, rs.size(),
	   "CXamineButtonPrompt::SetPromptString() - Copying prompt string.");
  }
  memset(rButton.promptstr, 0, sizeof(rButton.promptstr));
  strncpy(rButton.promptstr, rs.c_str(), sizeof(rButton.promptstr) - 1);

  
}
