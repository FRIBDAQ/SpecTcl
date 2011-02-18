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
#include "TclEvtRunInfo.h"


#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <string>
#include <iostream>
/**
 ** Use the base class constuctor to registrer the command.
 ** Our operator() will be called when the command is invoked.
 ** @param interp - encapsulated Tcl_Interp* 
 */
CTclEvtRunInfo::CTclEvtRunInfo(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "evtruninfo", true)
{}
/**
 ** The base class can handle the destruction duties:
 */
CTclEvtRunInfo::~CTclEvtRunInfo()
{
}

/**
 ** Gains control when the evtruninfo command is invoked.  Marshall the parameters into
 ** a call to spectcl_experiment_eventsrun().
 ** @param interp   - Encapsulated interpreter that is running the command.
 ** @param objv     - Vector of encapsulated Tcl_Obj*'s that make up the command words.
 ** @return int
 ** @retval TCL_OK  - Command completed correctly.  The result is a four element list containing:
 **                   in order the:
 **                    - Run number,
 **                    - Run title.
 **                    - Start of run date/time.
 **                    - End of run date/time. 
 **/
int
CTclEvtRunInfo::operator()(CTCLInterpreter& interp,
			   std::vector<CTCLObject>& objv)
{

  try {
    spectcl_experiment pHandle = getDatabaseHandle(interp, objv, 1, 
						   " ");
    const char* pMountPoint = NULL;
    std::string alternatePoint;
    try {

      alternatePoint = getParameter<std::string>(interp, objv,2);
      pMountPoint = alternatePoint.c_str();
    } catch(...) {
    }

    pRunInfo pInfo = spectcl_experiment_eventsrun(pHandle, pMountPoint);
    if (!pInfo) {
      throw std::string(spectcl_experiment_error_msg(spectcl_experiment_errno));
    }
    int runNumber      = pInfo->s_id;
    std::string title  = pInfo->s_pTitle;
    std::string start  = pInfo->s_pStartTime ? pInfo->s_pStartTime : "";
    std::string end    = pInfo->s_pEndTime   ? pInfo->s_pEndTime   : "";

    spectcl_free_run_info(pInfo);

    CTCLObject result;
    result.Bind(interp);
    result +=     runNumber;
    result +=     title;
    result +=     start;
    result +=     end;
    interp.setResult(result);
  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  return TCL_OK;
}
