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
#include <sqlite3.h>

#include "spectcl_experiment.h"
#include "spectcl_experimentInternal.h"


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define DEFAULT_ATTACH_POINT  WORKSPACE_DEFAULT_ATTACH_POINT

/*---------------------- Private functions -------- */

/*-------------------- public fucntions ----------*/
/**
 ** Create a new spectrum definition.
 ** Once a spectrum has been created it can be instantiated
 ** instantiation is the process of using the spectrum
 ** definition, some event data and so on to create the
 ** channels of the spectrum for that analysis case.
 **
 ** @param experiment  - Spectra can only be created on
 **                      workspaces that are attached
 **                      to experiments.  This is because
 **                      parameters are foreign keys
 **                      to parameter definitions in the
 **                      parameter definition table.
 **                      This parameter is the handle to
 **                      an experiment database.
 ** @param type       - Type of spectrum being created.
 ** @param name       - Name of the spectrum to create.
 **                     If a spectrum by this name already
 **                     exists, a new version of the 
 **                     definition will be created. 
 **                     versioning allows for evolution in
 **                     the definition of workspace
 **                     items with the ability to backtrack.
 ** @param ppParams   - Pointer to a null terminated array
 **                     of parameter definition pointer.
 ** @param attachPoint- If NULL the default attach point will
 **                     be used.  Otherwise this parameter
 **                     indicates where the workspace
 **                     is attached to the experiment
 ** @return int
 ** @retval > 0 - The 'id' of the spectrum definition.
 **               in this case, spectcl_experiment_errno
 **               will either be SPEXP_OK or SPEXP_NEWVERS
 ** @retval -1    creation failed.   In this case
 **               spexp_experimment_errno will be an
 **               error code:
 **
 ** ERRORS:
 **  - SPEXP_NOT_EXPDATABASE - The experiment parameter
 **                            is not an epxeriment database.
 **  - SPEXP_SQLFAIL         - Some SQL that is part of the
 **                            creation failed.
 **  - SPEXP_NOSUCH          - One of the parameters is not
 **                            valid.
 **  - SPEXP_UNATTACHED      - the attach point is not
 **                            a workspace.
 **  - SPEXP_BADPARAMS       - The spectrum parameter
 **                            definition is inconsistent
 **                            with the spectrum type.
 ** - SPEXP_INVTYPE          - Invalid type.
 **
 **/
                            
int spectrum_workspace_create_spectrum(spectcl_experiment experiment,
				       const char*  pType,
				       const char*  pName,
				       const spectrum_parameter** ppParams,
				       const char* attachPoint)
{
  const char* whereAttached = DEFAULT_ATTACH_POINT;
  int         status;

  /* Ensure experiment is an experiment handle */

  if (!isExperimentDatabase(experiment)) {
    spectcl_experiment_errno = SPEXP_NOT_EXPDATABASE;
    return -1;
  }

  /* Figure out the attach point and ensure we have a
  ** workspace attached there */

  if (attachPoint) {
    whereAttached = attachPoint;
  }
  status = spectcl_checkAttached(experiment, whereAttached, "workspace", SPEXP_UNATTACHED);
  if (status != SPEXP_OK) {
    spectcl_experiment_errno = status;
    return -1;
  }

  spectcl_experiment_errno = SPEXP_UNIMPLEMENTED;
  return -1;
}
