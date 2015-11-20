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
#include <TCLCompatibilityProcessor.h>
#include <TCLProcessor.h>
#include <TCLObject.h>
#include <TCLInterpreter.h>
#include <TCLResult.h>

#include <tcl.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
/*!
    Construct the object.  The assumption is that the CTCL processor
    is constructed 'enough' that we can pull out the command name,
    and the interpreter from it.
    \param actualCommand  : CTCLProcessor
        Object that implements an old style argc/argv command.

     \note the CTCLProcessor required a two-step construct/register,
           we enforce that by not allowing the base class constructor to register us.
*/
CTCLCompatibilityProcessor::CTCLCompatibilityProcessor(CTCLProcessor& actualCommand) :
  CTCLObjectProcessor(*(actualCommand.getInterpreter()), 
		      actualCommand.getCommandName(), false),
  m_ActualCommand(actualCommand)
{
}
/////////////////////////////////////////////////////////////////////////////
/*!
   Destruction is all taken care of by the base class destructor.
*/
CTCLCompatibilityProcessor::~CTCLCompatibilityProcessor()
{
}
/////////////////////////////////////////////////////////////////////////////
/*!
    Called to execute the command.  Here's where the serious adaptor
    work comes in to play.  We need to turn the objects passed in to us
    into an argc/argv pair, and create a CTCLResult to give the
    command we're adapting for.

    \param interp : CTCLInterpreter& 
       Interpreter that's running this command
    \param objv   : vector<CTCLOBject>& 
       Reference to the vector of command line objects.
      
    \return int
    \retval from m_ActualCommand->operator()
*/
int
CTCLCompatibilityProcessor::operator()(CTCLInterpreter&    interp,
				       vector<CTCLObject>& objv)
{
  CTCLResult result(&interp);

  // marshall the argc/argv:

  int    argc = objv.size();
  char** argv = new char*[argc];
  for(int i = 0; i < argc; i++) {
    argv[i] = Tcl_GetString(objv[i].getObject()); // using string forces const.
  }
  // Encapsulate the call in a try/catch so we can delete the argv pointers:
  
  int status;
  try {
    m_ActualCommand.preCommand();
    status = m_ActualCommand(interp, result, argc, argv);
    m_ActualCommand.postCommand();
    result.commit();
  }
  catch (...) {
    m_ActualCommand.postCommand();
    delete [] argv;
    throw;
  }

  // Clean up the dynamic storage and return the result:

  delete []argv;
  return status;
}
////////////////////////////////////////////////////////////////////////
/*!
    Adapt to actual commands command deletion handler.
*/
void
CTCLCompatibilityProcessor::onUnregister()
{
  m_ActualCommand.preDelete();
  m_ActualCommand.OnDelete();
  m_ActualCommand.postDelete();
}
