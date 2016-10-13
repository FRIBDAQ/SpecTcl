/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 2011, All rights reserved";
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
#include <config.h>
#include "XamineButtonPrompt.h"
#include "RangeError.h"
#include <string.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


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
