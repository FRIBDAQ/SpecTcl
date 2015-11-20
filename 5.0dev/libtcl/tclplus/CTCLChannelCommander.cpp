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
#include "CTCLChannelCommander.h"
#include "TCLInterpreter.h"
#include "TCLObject.h"
#include "TCLVariable.h"


using namespace std;

//////////////////////////////////////////////////////////

/*!
   The channel commanders use a 2 phase construction scheme
   so that we can hang on to a commander and put it in and out
   of the event loop as we please.
   
   Construction does nothing but save data.  To enable
   event procesing/command dispatch on the channel, you must
   call start.  Event/command processing can be disabled
   by invoking stop.

   \param interp   - Pointer to the CTCLInterpreter object
                     to which complete commands will be dispatched.
   \param channel  - Tcl_Channel on which commands will be 
                     accepted for submission to *interp.
*/
CTCLChannelCommander::CTCLChannelCommander(CTCLInterpreter* interp,
					  Tcl_Channel      channel) :
  m_pInterp(interp),
  m_channel(channel)

{
  // strings get constructed as empty which is fine for us.
}

/*!
   Destruction will stop command processing.
   - The interpreter will, of course remain alive.
   - The channel will not be closed.  That's up to the framing
     software.
*/
CTCLChannelCommander::~CTCLChannelCommander()
{
  stop();
}

//////////////////////////////////////////////////////////////////////////////

/*!
   Start accumulating commands to be processed.
   This involves setting a channel handler for input on the channel
   pointed to the inputRelay function that establishes object context and
   calls the virtual function onInput.
   Note that since channel handlers are like highlander (there can be only one),
   there's no harm in calling this function if the handler is already established,
   so there's no attempt to catch this as an error.
*/
void
CTCLChannelCommander::start()
{

  Tcl_CreateChannelHandler(m_channel, TCL_READABLE | TCL_EXCEPTION,
			   inputRelay, this);

}

/*!
   Stop accumulating commands by disabling the channel handler.
   Any data in m_command is discarded, so that if start is called later,
   there's no stale partial command.  m_command is cleared here, rather than
   in start so that multiple calls to start are harmless.
*/
void
CTCLChannelCommander::stop()
{
  if (m_channel){ 
    Tcl_DeleteChannelHandler(m_channel, inputRelay, this);
    m_channel = 0;
  }
}

/*!
  \return Tcl_Channel
  \retval The channel on which commands are processed.
*/
Tcl_Channel
CTCLChannelCommander::getChannel() const
{
  return m_channel;
}
/**
 * stopped
 *   Returns true if the channel has stopped (due to eof).
 */
bool
CTCLChannelCommander::stopped() const
{
  return m_channel == 0;
}
////////////////////////////////////////////////////////////////////////////


/*!
   This static function establishes object context and calls
   onInput if there was an input event or onInputException if there
   was an exception event.

   \param  pData - Really a pointer to a CTCLChannelHandler.
   \param  mask  - Mask of the events that actually fired.  In theory,
                   more than one event can be dispatched to us.
		   Since exceptions may result in channel closures,
                   reads are handled first.
*/
void
CTCLChannelCommander::inputRelay(ClientData pData, int mask)
{
  CTCLChannelCommander* pObject = reinterpret_cast<CTCLChannelCommander*>(pData);

  if(mask & TCL_READABLE) {
    pObject->onInput();
  }

  if (mask & TCL_EXCEPTION) {
    pObject->onInputException();
  }
}

//////////////////////////////////////////////////////////////////////////

/*!
   Input handler.  The default action is to 
   read a line of input with Tcl_GetsObj. The resulting object is converted
   to its string representation and appended to m_command.  
   The m_command is then sent through Tcl_CommandComplete and, if the command is
   complete, onCommand is invoked to handle it.
   Prompting is done as follows:
   - If the object acquired does not complete a line, then prompt2 is called.
   - If the object acquired completes the line, then on the return from
     onCommand, prompt1() is called.

   \note Tcl_GetsObj will return a -1 if no input is available
         This can happen for three reasons which are all dealt with:
        - Channel is in nonblocking mode and there's no data (normal return).
        - Channel has an eof condition (onEndFile called).
	- Channel has some error (onInputException called).



*/
void
CTCLChannelCommander::onInput()
{
  CTCLObject theLine;
  Tcl_Obj*   theObject = theLine.getObject();

  if (Tcl_GetsObj(m_channel, theObject) != -1) {
    // Data:

    string fragment = (string)(theLine);
    fragment += "\n";		// Newlines get filtered out by getsobj.
    m_command += fragment;

    if (Tcl_CommandComplete(m_command.c_str())) {
      onCommand();
      m_command = "";	       	// blank string for next command
      prompt1();		// Prompt for next command.
						       
    }
    else {
      prompt2();
    }
  }
  else {
    // Blocking mode?

    if (Tcl_InputBlocked(m_channel)) {
      return;
    }
    // EOF?

    if (Tcl_Eof(m_channel)) {
      onEndFile();
      return;
    }

    // Some error happend:

    onInputException();
    return;
  }
}

/*!
   This is called when there's a problem on the input.
   default action is to stop accepting input events.
   The channel remains open, it's up to the client software to determine
   when, and if, the channel should be closed.
*/
void
CTCLChannelCommander::onInputException()
{
  stop();
}
/*!
  Called when an end of file was seen on the input.
  This calls the stop function by default, leaving it up to the
  client software to decidew what to do about the channel (which is still open).
*/
void
CTCLChannelCommander::onEndFile()
{
  stop();
}

/*!
   Called when a command has been received. Default behavior:
   - submits the command to the interpreter for execution.
   - calls returnResult so that it can decide what to do with the
     interpreter result.
   Interpreter errors are just absorbed in the sense that we just let the
   result speak for itself,and Tcl's own -abort the proc on error- behavior
   deal with it all.
*/
void
CTCLChannelCommander::onCommand()
{

  try {
    m_pInterp->Eval(m_command);
  }
  catch(...) {
  }
  returnResult();
}
/*!
  Return the result of a command to well... somewhere.  By default
  this does nothing.  In other contexts, you could expect it to 
  write the result string somewhere (e.g. for sockets back to the client,
  for stdin, to stdout or stderr.
*/
void
CTCLChannelCommander::returnResult()
{
}

/*!
   Performs the begin of command prompt.
   Default action is to fetcht he prompt1 string and
   pass it to sendPrompt which has to decide what to do with it:
*/
void
CTCLChannelCommander::prompt1()
{

  string prompt = prompt1String();
  sendPrompt(prompt);

}
/*!
  Pefrorms the prompt for the middle of a command e.g.
\verbatim

prompt1>  proc a {b} {
prompt2>    ...

\endverbatim



*/
void
CTCLChannelCommander::prompt2()
{
  string prompt = prompt2String();
  sendPrompt(prompt);
}

/*!
  Peform a prompt, given a string.  The default is to do nothing leaving it to the
  derived classes to determine how prompting is emitted.
  Note that if different behavior is required for prompt1 and prompt2, those
  members can be directly overridden.
*/
void
CTCLChannelCommander::sendPrompt(string p)
{
}



///////////////////////////////////////////////////////////////////////////////

/*
** Return a string that represents the prompt for the beginning of a new command.
** This is either the default prompt string (currently hard coded to
** "% ") or the results of executing the script tcl_prompt1
** If this sounds like the scheme used by tclsh.. well that's intentional.
*/
string
CTCLChannelCommander::prompt1String()
{
  return getPromptString("tcl_prompt1", "% ");
}
/*
** Returns the string that should be used to prompt for command continuation/
** completion.  This is either the string "%_ " or the value returned by
** executing the script tcl_prompt2 if defined.  This is only slightly different
** than tclsh in that that that program has an empty string as prompt2's string.
*/
string
CTCLChannelCommander::prompt2String()
{
  return getPromptString("tcl_prompt2", "%_ ");
}

/*
** Determine a prompt string.  The prompt string can come from one of two places
** If a prompt variable exists, its value is a script that is run and the result
** (if the script is successful) is the prompt string.  Otherwise a default
** value is used:
**
** Parameters:
**   scriptVariable - Name of the variable that contains the command that should
**                    return a prompt.
**   default        - Value of the prompt string if scriptVariable cannot be used
**                    to deliver unto us a prompt string.
*/

string
CTCLChannelCommander::getPromptString(const char* scriptVariable,
				    const char* defaultValue)
{
  CTCLVariable promptVar(m_pInterp, scriptVariable, false);

  const char* pPrompt = promptVar.Get(TCL_GLOBAL_ONLY);
  if (!pPrompt) {
    return defaultValue;		// No such variable.
  }

  try {
    m_pInterp->Eval(pPrompt);
  }
  catch (...) {
    return defaultValue;		// Script had an error.
  }
  return m_pInterp->GetResultString(); // Script result.

}
