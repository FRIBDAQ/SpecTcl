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
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "spectcl_experimentInternal.h"

#ifndef TRUE
#define TRUE (0==0)
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif

/*-----------------------------------------------------------------------
  Static utility functions.
  ------------------------------------------------------------------------*/


/**
 ** bind the lowest/higest values of a parameter into a statement.
 ** the placeholders are assumed to be contiguous with lowest first.
 ** @param stmt    - Pointer to sqlite3 statment to bind.
 ** @param lownum  - Number of the placeholder for lowest.
 ** @param lo      - Pointer to lo value.
 ** @param hi      - Pointer to hi value.
 ** @return SPEXP_OK - if ok
 ** @return other  if not.
 */
static int
bindHiLo(sqlite3_stmt* stmt, unsigned lownum, const double* lo, const double* hi)
{
  int status;

  if (lo) {
    status = sqlite3_bind_double(stmt, lownum, *lo);
      if (status != SQLITE_OK) {
	sqlite3_finalize(stmt);
	spectcl_experiment_errno = status;
	return SPEXP_SQLFAIL;
      }
  }
  if (hi) {
    status = sqlite3_bind_double(stmt, lownum+1, *hi);
    if (status != SQLITE_OK) {
      sqlite3_finalize(stmt);
      spectcl_experiment_errno = status;
      return SPEXP_SQLFAIL;
    }
  }
  return SPEXP_OK;

}

/**
 ** Build a parameter_info struct from a step in a query of the
 ** parameters table
 ** @param stmt   - Pointer to the sqlite statement being stepped.
 ** @return pParameterInfo
 ** @retval NULL - Unable to allocate/fill the data.
 ** @retval other - Dynamically allocated parameter info block filled in.
 */
static pParameterInfo 
getParameterInfo(sqlite3_stmt* stmt)
{
  int            status;
  double         aDouble;
  const unsigned char* pAString;
  pParameterInfo pResult = calloc(1, sizeof(parameter_info)); /* assumes NULL == 0. */

  /** Start fishing stuff out of the colums of the result item.
   ** we'll use sqlite3_column_type to ferret out nulls....where they are allowed.
   */

  pResult->s_id = sqlite3_column_int(stmt, 0); /* id */

  pResult->s_pName = getTextField(stmt, 1);

  status = sqlite3_column_type(stmt, 2);  /* units can be null  */
  pResult->s_pUnits = getOptionalTextField(stmt, 2);
 
  status = sqlite3_column_type(stmt, 3); /* lowlimit can be null... */
  if (status != SQLITE_NULL) {
    pResult->s_haveLowLimit = TRUE;
    pResult->s_lowLimit     = sqlite3_column_double(stmt, 3);
  }
  else {
    pResult->s_haveLowLimit = FALSE;
  }
  
  status = sqlite3_column_type(stmt, 4); /* hilimit can be null */
  if (status != SQLITE_NULL) {
    pResult->s_haveHiLimit = TRUE;
    pResult->s_hiLimit     = sqlite3_column_double(stmt, 4);
  }
  else {
    pResult->s_haveHiLimit = FALSE;
  }

  return pResult;

}
/**
 ** Executes a table content modifying statment.
 ** @param db       - sqlite3 database handle.
 ** @param stmt    - Sqlite statment all bound
 ** @return int
 ** @retval >= 0 Number of rows modified.
 ** @retval < 0  Error detected.
 ** @note - the statement will be finalized too.
 */
static int 
modifySql(sqlite3* db, sqlite3_stmt* stmt)
{
  int status;
  int modifiedRows;

  status = sqlite3_step(stmt);
  if (status != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    spectcl_experiment_errno = status;
    return -1;
  }
  modifiedRows = sqlite3_changes(db);
  sqlite3_finalize(stmt);

  return modifiedRows;
}

/*-------------------------------------------------------------------------
     Public entries.
     ---------------------------------------------------------------------*/
/**
 ** This file contains code that manipulates and queries parameters
 ** It is part of the spectcl experiment database library.
 */

/**
 ** Returns a list of the parameters that match some specific pattern.
 ** the pattern can contain any GLOB wild card characters.
 ** Note that the use of GLOB is sqlite specific.
 **
 ** @param experiment   - A database handle for an experiment.
 ** @param pattern      - Pattern to match.
 ** @return spetcl_parameter_list*
 ** @retval NULL - Failure of some sort.
 ** @retval NOT NULL - Pointer to a list of pointers each of which is a
 **                    pParameterInfo.  The list is terminated by a NULL
 **                    pointer.  It is possible, if there are no matches but
 **                    no error that the first pointer will be NULL.
 */
parameter_list
spectcl_parameter_list(spectcl_experiment experiment,
		       const char*        pattern)
{
  pParameterInfo  pInfo;
  parameter_list  result  = NULL;
  unsigned int    matches = 1;
  char*           query   = "SELECT * FROM parameters WHERE name GLOB :pattern";
  sqlite3_stmt*   stmt;
  int status;

  /* Ensure this is an experiment database:  */

  if(!isExperimentDatabase((sqlite3*)experiment)) {
    spectcl_experiment_errno = SPEXP_NOT_EXPDATABASE;
    return NULL;
  }

  /* Create the query and bind the parameters */

  status = sqlite3_prepare_v2(experiment,
			      query, -1,
			      &stmt, NULL);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = SPEXP_SQLFAIL;
    return NULL;
  }

  status = sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_STATIC);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = SPEXP_SQLFAIL;
    sqlite3_finalize(stmt);
    return NULL;
  }

  /* Initialize an empty result */

  result = realloc(result, matches*sizeof(pParameterInfo));
  *result= NULL;

  /* marshall the result */

  while ((status = sqlite3_step(stmt)) == SQLITE_ROW) {
    pInfo = getParameterInfo(stmt);
    if (!pInfo) {
      spectcl_free_parameter_list(result);
      spectcl_experiment_errno = SPEXP_NOMEM;
      return NULL;      
    }

    result[matches-1] = pInfo;
    matches++;
    result = realloc(result, matches*sizeof(pParameterInfo));
    if (!result) {
      spectcl_free_parameter_list(result);
      spectcl_experiment_errno = SPEXP_NOMEM;
      return NULL;
    }

    result[matches-1] = NULL;
  }
  if (status != SQLITE_DONE) {
    spectcl_free_parameter_list(result);
    spectcl_experiment_errno = SPEXP_SQLFAIL;
    sqlite3_finalize(stmt);
    return NULL;
  }

  sqlite3_finalize(stmt);
  return result;

  
}
/**
 ** Frees a parameter list that was created and returned by spectcl_parameter_list.
 ** the list is a dynamically allocated array of of pointers to dynamic storage.
 ** The array of pointers is null terminated.
 **
 ** @param pList - The parameter list to be freed.
 */
void
spectcl_free_parameter_list(parameter_list pList)
{
  parameter_list p = pList;

  while (*p) {
    pParameterInfo pp = *p;
    free(pp->s_pName);
    if (pp->s_pUnits) free(pp->s_pUnits);
    free(pp);
    p++;
  }
  free(pList);
}


/**
 ** Creates a parameter.  This is done by inserting it in the 
 ** parameters table.  Note that it is allowed for the units and limits to be null
 ** @param db     - Experiment database.
 ** @param name   - Name of the new parameter, It is an error for this parameter to exist.
 **                 Use spectcl_parameter_alter to change an existing parameter.
 ** @param units  - Pointer to the units string.  If this is NULL, the units field will also be null.
 ** @param low    - Pointer to a double containing the low limit on parameter values.   If the pointer
 **                 is NULL, null will be inserted.
 ** @param hi     - Pointer to a double containing the hi limit on parameter values.  if the
 **                 pointer is null the record field will also be null.
 ** @return int
 ** @retval SPEXP_OK   - Parameter successfully created.
 ** @retval SPEXP_EXISTS - Parameter already exists.
 ** @retval SPEXP_NOT_EXPDATABASE - This is not an experiment database.
 ** @retval SPEXP_SQLFAIL - The insertion operation failed for some sqlite reson.
 **                            the failed sqlite status is in spectcl_experiment_errno.
 */
int
spectcl_parameter_create(spectcl_experiment db,
			 const char* name, const char* units, const double* low, const double* hi)
{
  parameter_list pList = spectcl_parameter_list(db, name);
  const char*    insert= "INSERT INTO parameters (name, units, lowest, highest) \
                                 VALUES (:name, :units, :lowest, :highest)";
  sqlite3_stmt*  stmt;
  int            status;

  if (pList && (*pList != NULL)) {
    
    spectcl_free_parameter_list(pList);
    return SPEXP_EXISTS;
  }
  /* Prepare, bind and execute the insertion.  */

  status = sqlite3_prepare_v2(db,
			      insert,
			      -1, &stmt, NULL);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }

  status = sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    sqlite3_finalize(stmt);
    return SPEXP_SQLFAIL;
  }

  if(units) {
    status = sqlite3_bind_text(stmt, 2, units, -1, SQLITE_STATIC);
    if (status != SQLITE_OK) {
      spectcl_experiment_errno = status;
      sqlite3_finalize(stmt);
      return SPEXP_SQLFAIL;
    }
  }
  status = bindHiLo(stmt, 3, low, hi);
  if (status != SPEXP_OK) {
    return status;
  }

  return modifySql(db,stmt) < 0 ? SPEXP_SQLFAIL : SPEXP_OK;

}
/**
 ** Alter the characteristics of an existing parameter.
 ** @param db     - Experiment database.
 ** @param name   - Name of the parameter to alter.
 ** @param low    - New value for low limit 
 ** @param hi     - New value for hi limit
 **
 ** @note Both low and hi will be modified. If a pointer is NULL, then the corresponding
 **       value will be altered to be NULL.
 ** @return int
 ** @retval SPEXP_OK     - Completed ok.
 ** @retval SPEXP_NOSUCH - No such parameter (no changes made). 
 ** @retval SPEXP_SQLFAIL - The sql failed for a reason that is storedin spectcl_experiment_errno.
 **                         the reason is an SQLITE3 status code.
 ** @retval SPEXP_EXISTS - Something really bad happened and there is more than one parameter
 **                        with the selected name!!!
 */
int
spectcl_parameter_alter(spectcl_experiment db, const char* name, const double* low, const double* hi)
{
  sqlite3_stmt* stmt;
  const char*   query = "UPDATE parameters SET lowest=:lowest, highest=:highest WHERE name=:name";
  int           status;
  int           modifiedRows;

  /* Prep the update statement and bind the parameters:  */

  status = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  status = bindHiLo(stmt, 1, low, hi);
  if (status != SPEXP_OK) {
    return status;
  }


  status = sqlite3_bind_text(stmt, 3, name, -1, SQLITE_STATIC);
  if (status != SQLITE_OK) {
    sqlite3_finalize(stmt);
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }

  /* Execute the statement, and figure out how many rows were modified.  If that's 0 
  ** this is an error... should be exactly 1..else that's a really bad error!!!
  */

  modifiedRows = modifySql(db, stmt);

  if (modifiedRows < 0) {
    return SPEXP_SQLFAIL;
  }

  if (modifiedRows == 1) {
    return SPEXP_OK;
  }
  if (modifiedRows == 0) {
    return SPEXP_NOSUCH;
  }

  return SPEXP_EXISTS;
  
}
/**
 ** Alter the hi/lo value of a parameter given it's id.
 ** @param db    - Experiment database.
 ** @param id    - Parameter id (primary key of parameter)
 ** @param lo    - Low limit
 ** @param hi    - Hi limit.
 ** @return int
 ** @retval SPEXP_OK     - Completed ok.
 ** @retval SPEXP_NOSUCH - No such parameter (no changes made). 
 ** @retval SPEXP_SQLFAIL - The sql failed for a reason that is storedin spectcl_experiment_errno.
 **                         the reason is an SQLITE3 status code.
 ** @retval SPEXP_EXISTS - Something really bad happened and there is more than one parameter
 **                        with the selected name!!!
 */ 
int
spectcl_parameter_alter_byid(spectcl_experiment db, int id, const double* lo, const double* hi)
{
  sqlite3_stmt* stmt;
  const char*   query = "UPDATE parameters SET lowest=:lowset, highest=:highets WHERE id = :id";
  int           status;
  int           modifiedRows;

  status = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  status = bindHiLo(stmt, 1, lo, hi);
  if (status != SPEXP_OK) {
    return status;
  }
  status = sqlite3_bind_int(stmt, 3,  id);
  if (status != SQLITE_OK) {
    sqlite3_finalize(stmt);
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }

  modifiedRows = modifySql(db, stmt);

  if (modifiedRows == 1) {
    return SPEXP_OK;
  }
  if (modifiedRows == 0) {
    return SPEXP_NOSUCH;
  }
  if (modifiedRows < 0) {
    return SPEXP_SQLFAIL;
  }
  /* Must be > 1.. very bad... */

  return SPEXP_EXISTS;
}
