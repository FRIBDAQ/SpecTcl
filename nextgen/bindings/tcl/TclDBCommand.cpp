/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

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
#include "TclDBCommand.h"
#include "handleManager.h"
#include <TCLInterpreter.h>
#include <sqlite3.h>
#include <spectcl_experimentInternal.h>
#include <tcl.h>
#include <stdlib.h>

using namespace std;

CHandleManager* CTclDBCommand::m_pHandles(0);

/*---------------------- Canonicals --------------------------------*/

CTclDBCommand::CTclDBCommand(CTCLInterpreter& interp, std::string command, bool doRegister) :
  CTCLObjectProcessor(interp, "::spectcl::" + command, doRegister)
{
  string cmd = "namespace eval ::spectcl {namespace export ";
  cmd       += command;
  cmd       += "}";
  interp.Eval(cmd);
}
CTclDBCommand::~CTclDBCommand() {}


/*---------------------- Utility functions --------------------------------*/
/**
 ** Factors out common code that gets a database handle from a command
 ** @param interp - interpret running the command.
 ** @param objv   - Vector of encapsulated Tcl_Obj*'s that make up the comamnd
 ** @param which  - Which parameter is supposed to have the database handle.
 ** @param subcommand - subcommand this is being done for.. used to format error messages.
 ** @return spectcl_experiment
 ** @retval       - Handle fetched from the command and handle manager.
 ** @throws string - If there are any errors.
 */
spectcl_experiment
CTclDBCommand::getDatabaseHandle(CTCLInterpreter& interp,
				 std::vector<CTCLObject>& objv,
				 int which,
				 string subcommand) throw(std::string)
{
  // Common error message formattting:

  string message = subcommand;

  // There must be sufficient command line parameters:
  // If so get the string-ized handle.

  string handle = getParameter<string>(interp, objv, which);


  // Conver the string-ized handle to a spectcl_experiment using the handle manager:

  CHandleManager*    pManager = getHandleManager();
  spectcl_experiment pHandle  = pManager->find(handle);

  if (!pHandle) {
    message += " - invalid database handle ";
    message += handle;
    throw message;
  }

  return pHandle;

}
/**
 ** Implement the experiment handle manager as a singleton.
 ** @return CHandleManager*
 ** @retval m_pHandles  - if necessary creating the handle manager.
 */
CHandleManager*
CTclDBCommand::getHandleManager()
{
  if (!m_pHandles) {
    m_pHandles = new CHandleManager("spectcl");
  }
  return m_pHandles;
}
/**
 ** Take a file path an return the normalized version.
 ** @param path - input path.
 ** @return string
 ** @retval normalized path.
 ** @throw string - error message if normalization failed.
 **
 */
string
CTclDBCommand::normalizePath(string path) throw(string)
{
  Tcl_DString tempBuffer;
  Tcl_Interp* pInterp  = getInterpreter()->getInterpreter();
  char* result = Tcl_TranslateFileName(pInterp, path.c_str(),
				       &tempBuffer);
  if (!result) {
    string msg = "Invalid filename path: ";
    msg       +=  path;
    throw msg;
  }
  else {
    string sresult(result);
    Tcl_DStringFree(&tempBuffer);
    return sresult;
  }
}
/**
 ** Validate an experiment and events handle to esure that
 ** - the come from right type of databases.
 ** - they are actually related (events database made for this experiment).
 ** @param pExp   - Handle on experiment database.
 ** @param pEvts  - Handle on events database.
 ** @throws std::string 
 **               - error message string describing what's wrong with the handles,
 **                 if an error is detected.
 */
void
CTclDBCommand::validateExpEvtHandles(spectcl_experiment pExp, spectcl_events pEvts)
{
    if (!isExperimentDatabase(reinterpret_cast<sqlite3*>(pExp))) {
      throw std::string("::spectcl::loadevents - First database handle is not an experiment handle");
    }
    if(!isEventsDatabase(reinterpret_cast<sqlite3*>(pEvts))) {
      throw std::string("::spectcl::loadevents - Second database handle is not an events database");
    }
    // That we can get UUID's and that they match:

    uuid_t* pEvtUUID = spectcl_events_uuid(pEvts);
    if (!pEvtUUID) {
      throw std::string(spectcl_experiment_error_msg(spectcl_experiment_errno));
    }
    if (!spectcl_correct_experiment(pExp, pEvtUUID)) {
      free(pEvtUUID);
      throw std::string("::spectcl::loadevents - Events database from wrong experiment");
    }
    free(pEvtUUID);		// Smokey says only you can prevent memory leaks in C/C++.
  
}
/**
 ** If the database handle passed in is not an experiment handle, throw 
 ** an exception indicating the problem.
 ** @param expHandle - The alleged experiment datbase handle.
 ** @throw std::string on failure.
 */
void
CTclDBCommand::throwIfNotExpHandle(spectcl_experiment expHandle)
{
    uuid_t* uuid = spectcl_experiment_uuid(expHandle);
    if (!uuid) {
      throw std::string(spectcl_experiment_error_msg(spectcl_experiment_errno));
    } 
    else {
      free(uuid);
    }
}
