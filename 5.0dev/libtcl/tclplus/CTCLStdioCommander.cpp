/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include <config.h>
#include "CTCLStdioCommander.h"
#include "TCLInterpreter.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////


/*!
   Construction requires 
   - Base class construction with the stdin channel
   - Initialization of the m_stdOut channel member data:

   These can be determined using Tcl_GetStdChannel so we don't
   need the caller's help to get all this going:

   \param pInterp - Pointer to the interpreter object to which commands
                   will be submitted.
*/
CTCLStdioCommander::CTCLStdioCommander(CTCLInterpreter* pInterp) :
  CTCLChannelCommander(pInterp, Tcl_GetStdChannel(TCL_STDIN)),
  m_stdOut(Tcl_GetStdChannel(TCL_STDOUT))
{
}
/*!
  Destruction is a placeholder to ensure base class destruction is done..
  that's where the action is.

*/

CTCLStdioCommander::~CTCLStdioCommander()
{
}

/////////////////////////////////////////////////////////////////////////////

/*!
   This member is expected to send a prompt string wherever prompts go.
   in our case that's easy. We send it to the stdout channel.
   \param prompt - the string to write.
*/
void
CTCLStdioCommander::sendPrompt(string prompt)
{
  Tcl_WriteChars(m_stdOut, prompt.c_str(), -1);
  Tcl_Flush(m_stdOut);
}
/*!
  This member is expected to fetch the result string from the interpreter
  and do whatever is required of the result.  In our case, we 
  pass it to sendPrompt which writes it to stdout.
*/
void
CTCLStdioCommander::returnResult()
{
  string result = m_pInterp->GetResultString();
  result += "\n";
  sendPrompt(result);
}
