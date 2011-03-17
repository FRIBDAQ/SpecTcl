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

#define DEFAULT_ATTACH_POINT "WORKSPACE"

/*------------------------ Private functions ----------------------------------*/

/**
 ** Get the data from a type row;  The non-exception case is:
 ** - allocate a spectcl_spectrum_type struct.
 ** - get the values of the two fields and put them in the struct.
 ** - return the struct.
 **
 ** @param stmt  - the sqlit3_stmt* that represents the statement at the step
 **                being retrieved.
 ** @return spectcl_spectrum_type* 
 ** @retval NULL -- Memory allocation failure.
 ** @retval non_NULL - Pointer to the allocated spectcl_spectrum_type filled in
 **                    with our data.
 **
 */
static spectcl_spectrum_type*
getTypeRow(sqlite3_stmt* stmt)
{
  spectcl_spectrum_type* result = (spectcl_spectrum_type*)malloc(sizeof(spectcl_spectrum_type));

  /* Type code */


  if (!(  result->s_type = getTextField(stmt, 0))) {
    free(result);
    return NULL;
  }

  /* description field */

  if(!(result->s_description= getTextField(stmt, 1))) {
    free(result->s_type);
    free(result);
    return NULL;
  }

  return result;

}

/*------------------------- public functions ---------------------------------*/

/**
 ** Get a description of a spectrum type given the type name.
 ** @param ws    - Work space handle.
 ** @param type  - Abbreviated type.
 ** @return char*
 ** @retval NULL - Could not get the type. The error is put in spectcl_experiment_errno
 ** @retval other- Pointer to dynamically allocated storage containing the description
 **                of the spectrum type.
 **
 ** ERRORS:
 **   - SPEXP_NOT_WORKSPACE  - The ws parameters is not a workspace.
 **   - SPEXP_NOSUCH         - there is no matchto the specified 'type' parameter.
 */
char*
spectcl_workspace_getDescription(spectcl_workspace ws, const char* type)
{
  char* result;
  if (! spectcl_workspace_isWorkspace(ws)) {
    spectcl_experiment_errno = SPEXP_NOT_WORKSPACE;
    return NULL;
  }

  result = getfirst(ws, "spectrum_types",
		    "description", "type", type);
  if (!result) {
    spectcl_experiment_errno = SPEXP_NOSUCH;
    return NULL;
  }
  else {
    return result;
  }

}
/**
 ** Determine if a spectrum type is valid.
 ** @param ws   - workspace.
 ** @param tyep - Spectrum type to check.
 ** @return int
 ** @retval  SPEXP_OK     - normal completion.type exists.
 ** @retval  SPEXP_NOT_WORKSPACE - ws was not a workspace
 ** @retval  SPEXP_NOSUCH        - No such type.
 */
int
spectcl_workspace_isValidType(spectcl_workspace ws, const char* type)
{
  char* descrip = spectcl_workspace_getDescription(ws, type);
  free(descrip);
  return descrip ? SPEXP_OK : spectcl_experiment_errno;

} 
/**
 ** Free the storage associated with a spectcl_spectrum_type** returned by
 ** spectcl_workstation_spectrumTypes() call.
 ** @param pList   - pointer to the data to be freed.
 */
void
spectcl_workspace_free_typelist(spectcl_spectrum_type** pList)
{
  spectcl_spectrum_type** p = pList;
  if(!p) return;		/* nothing to free. */
  while(*p) {			

    /* free each entry and its chars. */

    free((*p)->s_type);
    free((*p)->s_description);
    free(*p);

    /* On to the next one until we hit the null terminator. */

    p++;

  }
  free(pList);			/* Free the whole list. */
}

/**
 ** Retrieve a list of spectrum types and their descriptions.
 ** @param ws   - Workspace.
 ** @return spectcl_spectrum_type**
 ** @retval  NULL -some problem described in spectcl_experiment_errno occured.
 ** @retval  not-null - A pointer to a null terminated array of spectcl_spectrum pointers.
 **                     each spectcl_spectrum element contains the fields:
 **                         s_type        - char* the spectrum type (encoded into spectrum definitions).
 **                         s_description - char* the Human readable spectrum type description.
 ** ERRORS (spectcl_experiment_errno)
 **  - SPEXP_NOT_WORKSPACE   - ws is not a workspace.
 **  - SPEXP_NOMEM           - Dynamic memory allocation failure.
 **  - SPEXP_SQLFAIL         - Some call to sqlite failed.
 */
spectcl_spectrum_type**
spectcl_workspace_spectrumTypes(spectcl_workspace ws)
{
  sqlite3_stmt*           stmt;
  const char*             sql    = "SELECT type,description FROM spectrum_types"; 
  spectcl_spectrum_type** result = NULL;
  spectcl_spectrum_type*  entry;
  int                     status;
  size_t                  nItems = 0; /* number of types gotten so far. */

  if(!spectcl_workspace_isWorkspace(ws)) {
    spectcl_experiment_errno = SPEXP_NOT_WORKSPACE;
    return NULL;
  }

  /** Create the statement */

  status = sqlite3_prepare_v2(ws, sql, -1, &stmt, NULL);
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = SPEXP_SQLFAIL;
    return NULL;
  }

  while (status = sqlite3_step(stmt)) {
    if (status != SQLITE_ROW) {
      break;
    }
    entry  = getTypeRow(stmt);	/* get one entry of the type row. */
    if (!entry) {
      spectcl_workspace_free_typelist(result);
      spectcl_experiment_errno = SPEXP_NOMEM;
      return NULL;
    }
    nItems++;
    result = (spectcl_spectrum_type**)realloc(result,  nItems+1);
    if (!result) {
      free(entry);
      spectcl_workspace_free_typelist(result);
      spectcl_experiment_errno = SPEXP_NOMEM;
      return NULL;
    }
    result[nItems -1] = entry;	
    result[nItems]    = NULL;	/* null termination always. */
  }
  /* Finalize the statement and then analyze the result */

  sqlite3_finalize(stmt);

  if (status != SQLITE_DONE) {
    spectcl_workspace_free_typelist(result);

    spectcl_experiment_errno = SPEXP_SQLFAIL;
    return NULL;
  }

  return result;


}
/**
 ** Returns spectrum type list from a workspace that is attached to an experiment.
 ** @param exp          - Experiment database handle.
 ** @param attachPoint  - Attach point NULL means attached to DEFAULT_ATTACH_POINT
 ** @return same as spectcl_workspace_spectrumTypes
 **
 **  Errors are:
 **   - SPEXP_NOT_EXPDATABASE - exp is not an experiment database.
 **   - SPEXP_NOMEM           - Out of memewory allocating storage.
 **   - SPEXP_SQLFAIL         - Some SQL failed.
 **   - SPEXP_UNATTACHED      - There is no attached workspace.
 */

spectcl_spectrum_type** spectcl_experiment_spectrumTypes(spectcl_experiment exp,
							  const char* attachPoint)
{
  spectcl_experiment_errno = SPEXP_UNIMPLEMENTED;
  return NULL;
}
/*
 * Stubs:
 */

int                     spectcl_experiment_isValidType(spectcl_experiment exp,
						       const char* attachPoint)
{
  return SPEXP_UNIMPLEMENTED;
}
char*                   spectcl_expermient_getDescription(spectcl_experiment exp,
							    const char* attachPoint)
{
  spectcl_experiment_errno =  SPEXP_UNIMPLEMENTED;
  return NULL;
}
