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
#include <string.h>
#include <stdio.h>

#define DEFAULT_ATTACH_POINT WORKSPACE_DEFAULT_ATTACH_POINT

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

/**
 ** Common code to get a description string for a
 ** spectrum type given that the workspace may or may not
 ** be attached somewhere.
 ** @param db - sqlite3 database handle that could be
 **             either for an experiment or a workspace.
 ** @param type - Abbreviated type string.
 ** @param pAttach - If not null, the workspace database
 **                  tables must be prefixed by this (.).
 ** @return char* 
 ** @retval NULL  - There is no matching spectrum type.
 ** @retval other - Pointer to the spectrum type 
 **                 long description string.  This is
 **                 dynamically allocated and must
 **                 eventually be free(3)'d.
 */
static char*
typeDescription(sqlite3* db, const char* type, const char* pAttach)
{
  const char* pSeparator="";
  const char* pAttachment="";
  int         status;
  char*       result;
  char*       tableFormat = "%s%sspectrum_types";
  size_t      tableLength;
  char*       table;
  
  
  table = spectcl_qualifyStatement(tableFormat, pAttach);
  result = getfirst(db,table, "description", "type", type);
  free(table);
  
  if (!result) {
    spectcl_experiment_errno = SPEXP_NOSUCH;
  }
  return result;

}
/**
 ** Common code to get spectrum types.  This can operate on a 'root' database
 ** or on a workspace attached to an attach point depending on the parameters:
 **
 ** @param db            - An sqlite3 datbase handle.
 ** @param attachPoint   - If not null, this is the poitn at which the workspace is
 **                        attached relative to the database root.
 ** @return spectcl_spectrum_type**
 ** @retval non-null - Successful completion.
 ** @retval NULL - Unsuccessful completion with an error in the 
 **                spectcl_experiment_errno
 **                global variable.
 */
static spectcl_spectrum_type**
spectrumTypes(sqlite3* db, const char* attachPoint)
{
  const char* pSeparator="";
  const char* pAttachment="";
  int         status;
  const char*   statementFormat = "SELECT type,description FROM %s%sspectrum_types";
  char*         sqlStatement;
  sqlite3_stmt* stmt;
  spectcl_spectrum_type** result = NULL;
  spectcl_spectrum_type*  entry;
  size_t                  nItems = 0; /* number of types gotten so far. */
  size_t                  statementSize;


  if(attachPoint) {
    pAttachment = attachPoint;
    status = spectcl_checkAttached(db, pAttachment, "workspace", SPEXP_UNATTACHED);
    if (status != SPEXP_OK) {
      spectcl_experiment_errno = status;
      return NULL;
    }   
  }

 
  sqlStatement = spectcl_qualifyStatement(statementFormat,
					  attachPoint);


  /* figure out the statement, allocate memory for it, prepare it and free the memory. */

  if(!sqlStatement) {
    spectcl_experiment_errno = SPEXP_NOMEM;
    return NULL;
  }

  status = sqlite3_prepare_v2(db, sqlStatement, -1, &stmt, NULL);
  free(sqlStatement);
  if (status != SQLITE_OK) {
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

  return typeDescription(ws, type, NULL);


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



  if(!spectcl_workspace_isWorkspace(ws)) {
    spectcl_experiment_errno = SPEXP_NOT_WORKSPACE;
    return NULL;
  }
  return spectrumTypes(ws, NULL);


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
  const char* pAttach = DEFAULT_ATTACH_POINT;
  

  if(!isExperimentDatabase(exp)) {
    spectcl_experiment_errno = SPEXP_NOT_EXPDATABASE;
    return NULL;
  }
  /* Figure out the attachment point and if there is a workspace
  ** attached there.
  */

  if (attachPoint) {
    pAttach = attachPoint;
  }
  return spectrumTypes(exp, pAttach);

}
/**
 ** Determines if a spectrum type is valid for a workspace attached
 ** to an experiment database
 ** @param exp         - experiment database.
 ** @param type        - type to validate.
 ** @param attachPoint - If not null this is the attachment point
 **                      If null,DEFAULT_ATTACH_POINT is used instead.
 ** @return int
 ** @retval SPEXP_OK           - Valid type.
 ** @retval SPEXP_NOSUCH       - Invalid type.
 ** @retval SPEXP_NOT_EXPDATABASE -  exp isn not an experiment database handle.
 ** @retval SPEXP_UNATTACHED      -  There is not a workspace attached at the specified point.
 */ 
int spectcl_experiment_isValidType(spectcl_experiment exp, 
				   const char* type,
				   const char* attachPoint)
{
  const char* pAttach = WORKSPACE_DEFAULT_ATTACH_POINT;
  const char* pDescrip;
  int status;

  if (!isExperimentDatabase(exp)) {
    return SPEXP_NOT_EXPDATABASE;
  }
  if (attachPoint) {
    pAttach = attachPoint;
  }
  status = spectcl_checkAttached(exp,
				 pAttach, "workspace",
				 SPEXP_UNATTACHED);
  if (status != SPEXP_OK) {
    return status;
  }

  pDescrip = typeDescription(exp, type, pAttach);
  free(pDescrip);
  return pDescrip ? SPEXP_OK : SPEXP_NOSUCH;

}

/**
 * Retrieve the spectrum type description given a spectrum
 * type code.
 * @param exp   - Experiment database.
 * @param type  - Short spectrum type.
 * @param attachPoint - the point at which the workspace
 *                      is attached to the experiment
 *                      database.
 * @return char*
 * @retval NULL - Failure with the reason for failure
 *                stored in spectcl_experiment_errno.
 * @retval non_null - Pointer to a dynamically allocated
 *                    text string that contains the
 *                    spectrum type description.
 * 
 * Errors:
 *
 ** @retval SPEXP_OK           - Valid type.
 ** @retval SPEXP_NOSUCH       - Invalid type.
 ** @retval SPEXP_NOT_EXPDATABASE -  exp isn not an experiment database handle.
 ** @retval SPEXP_UNATTACHED      -  There is not a workspace attached at the specified point.
 */
char*  spectcl_expermient_getDescription(spectcl_experiment exp,
					 const char*        type,
					 const char* attachPoint)
{
  spectcl_experiment_errno =  SPEXP_UNIMPLEMENTED;
  return NULL;
}

