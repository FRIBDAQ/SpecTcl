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

#include "spectcl_experiment.h"
int spectcl_experiment_errno = 0;

static const char* errorMessages[] = {
  "OK - Correct completion",
  "CREATE_FAILED - Unable to create a database",
  "OPEN_FAILED - Unable to open database",
  "NOT_EXPDATABASE - Database is not an experiment database",
  "CLOSE_FAILED  - Failed to close a database",
  "EXISTS - Item already exists",
  "NOMEM - Unable to allocated dynamic memory",
  "SQLFAIL - An SQL query failed",
  "NOSUCH - Item could not be found",
  "NOT_EVENTSDATABASE - Database is not an events database",
  "WRONGEXPERIMENT  - Attempting to attach database from another experiment"
};

static const char* catchAll = "BADCODE - Bad error code passed to spectcl_experiment_error_msg";
static const char* unimp    = "UNIMP   - Function not yet implemented completely";

static const nMessages = sizeof(errorMessages)/sizeof(const char*);

/**
 ** Return the error messagse appropriate to the error code passed in.
 ** if the error code is illegal, a message to that effect is returned.
 ** @param reason - Error reason to fetch text for.
 ** @return const char* 
 ** @retval  an error message appropriate to reason.
 */
const char* 
spectcl_experiment_error_msg(int reason)
{
  if (reason < 0) return catchAll;
  if (reason == SPEXP_UNIMPLEMENTED) return unimp;
  if (reason >= nMessages) return catchAll;
  return errorMessages[reason];
}
