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

#define DEFAULT_ATTACH_POINT  "WORKSPACE"

/*---------------------- private funtions -------------------------*/

/**
 ** Create and populate the configuration table:
 ** @param db     - sqlite3* open on the database.
 ** @param uuid   - uuid_t* that is the UUID with which to populate the
 **                 uuid element.
 ** @return int
 ** @retval 0   - Everything worked.
 ** @retval -1  - Failed on some insert.
 **
 **/
static int
create_config_values(sqlite3* db, uuid_t* uuid)
{
  char  uuidText[100];
  int   status;

  uuid_unparse(*uuid, uuidText);


  do_non_select(db,
		"CREATE TABLE configuration_values ( \
                                id           INTEGER PRIMARY KEY,	\
                                config_item  VARCHAR(256),		\
                                config_value VARCHAR(256))");

  status = insertConfig(db, "version", SCHEMA_VERSION);
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return -1;
  }
  status = insertConfig(db, "type", "workspace");
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return -1;
  }
  status = insertConfig(db, "uuid", uuidText);
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return -1;
  }

  return 0;
  
}

/**
 ** Create and populate the spectrum types database:
 ** @param ws  - The workspace database handle
 ** @return int
 ** @retval 0   - Everything worked.
 ** @retval -1  - Something failed.
 **
 */
static
int createTypesTable(sqlite3* ws)
{
  /* Create the table: */

  do_non_select(ws,
		"CREATE TABLE spectrum_types (       \
                       id     INTEGER PRIMARY KEY,   \
                       type   VARCHAR(10),           \
                       description VARCHAR(256))");
  
  /* Insert known spectrum types: */

  do_non_select(ws,
		"INSERT INTO spectrum_types (type,description)  \
                             VALUES ('1', '1-D')");
}

/**
 ** Create tables for spectrum definitions and
 ** parameters in a spectrum.
 ** @param ws   - Workspace sqlite3 connection id.
 **
 */
void
createSpectrumDefTables(sqlite3* ws)
{
  do_non_select(ws,
		"CREATE TABLE spectrum_definitions ( \
                    id INTEGER PRIMARY KEY,         \
                    name VARCHAR(256),              \
                    type_id INTEGER,                 \
                    version  INTEGER)");
  do_non_select(ws,
		"CREATE TABLE spectrum_parameters ( \
                 id  INTEGER PRIMARY KEY,           \
                 spectrum_id INTEGER,               \
                 parameter_id INTEGER,              \
                 dimension    INTEGER,              \
                 version      INTEGER)");

}
/**
 ** Determine if an sqlite3 handle is open on a workspace database.
 ** this requires that we be able to get the 'type' config_item from
 ** the configuration_values table and that it have the value 'workspace'.
 ** @param ws   - Handle open on the database to check.
 ** @return int
 ** @retval TRUE - Database is a workspace
 ** @retval FALSE - Database is not a workspace.
 */
static int
isWorkspace(sqlite3* ws)
{
  char* result  = getfirst(ws, "configuration_values", "config_value",
			   "config_item", "type");
  if (result) {
    int match = strcmp("workspace", result);
    free(result);
    return (match == 0);

  }
  else {
    return FALSE;
  }
}

/* --------------------- public entries ------------------------------*/
/**
 ** Create a workspace database.
 ** Workspace databases hold analysis artifacts...definitions and instances.
 ** These include and are not limited to:
 ** - spectra
 ** - gates
 ** - instances of all of the above.
 **
 ** The workspace is bound to an experiment database file in order to be sure
 ** definitions across databases are consistent.  This binding is done by
 ** writing the uuid of the expermiment into the 
 ** @param pHandle - handle of an experiment database to which the workspace will
 **                  be bound.
 ** @param path    - Filesystem path of the database to create.
 ** @return int
 ** @retval SPEXP_OK              - Database properly created.
 ** @retval SPEXP_NOT_EXPDATABASE - pHandle is not an experiment data base handle.
 ** @retval SPEXP_CREATE_FAILED   - Unable to create the database file.
 ** @retval SPEXP_SQLFAIL         -  Failed to create the initial schema.
 */
int spectcl_workspace_create(spectcl_experiment pHandle, const char* path)
{
  sqlite3* db = (sqlite3*)pHandle;
  sqlite3* ws;
  int      status;

  if (!isExperimentDatabase(db)) {
    return SPEXP_NOT_EXPDATABASE;
  }
  
  /* Now attempt to create the database: */
  
  status = sqlite3_open_v2(path, &ws,
			   SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
			   NULL);
  if (status != SQLITE_OK) {
    return SPEXP_CREATE_FAILED;
  }
  uuid_t* uuid = spectcl_experiment_uuid(db);
  status = create_config_values(ws, uuid);
  free(uuid);

  createTypesTable(ws);
  createSpectrumDefTables(ws);

  spectcl_workspace_close(ws);

  if (status) {
    return SPEXP_SQLFAIL;
  }
  
  return SPEXP_OK;
}
/**
 ** Open an existing workspace.
 ** @param path  - Path in the filesystem to the workspace.
 ** @return spectcl_workspace
 ** @retval not NULL -handle to use in other calls that need a spectcl_workspace.
 ** @retval NULL     -Open failed.  The spectcl_experiment_errno variable
 **                   holds the reason for the failure:
 **      - SPEXP_OPEN_FAILED    - databased does not exist or is not writeable.
 **      - SPEXP_NOT_WORKSPACE  - The file is not a workspace.
 **      - SPEXP_SQLFAIL        - Some SQL failued when probing the correctness of the workspace.
 */
spectcl_workspace
spectcl_workspace_open(const char* path)
{
  sqlite3*   db;
  int        status;

  /* The file must be have read/write access */

  if(access(path, R_OK | W_OK)) {
    spectcl_experiment_errno = SPEXP_OPEN_FAILED;
    return NULL;
  }
  /* Open the database and ensure tha it is a workspace: */

  status = sqlite3_open_v2(path,
			   &db, SQLITE_OPEN_READWRITE, NULL);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = SPEXP_OPEN_FAILED; /* sqlite couldn't open so ... */
    return NULL;
    
  }
  if(!isWorkspace(db)) {
    sqlite3_close(db);
    spectcl_experiment_errno = SPEXP_NOT_WORKSPACE;
    return NULL;
  }
  
  return  db;
}
/**
 ** Attach a workspace to an experiment.  To do this we require that:
 ** - The database being attached is a workspace.
 ** - The database being attached to is an experiment.
 ** - The workspace be of the same experiment.
 ** @param pHandle    - Experiment database to which the attachment will occur.
 ** @param path       - Filesystem path to the workspace.
 ** @param attachPoint- The name of the attached database.  If NULL a default
 **                     attach point is used (WORKSPACE).
 ** @return int
 ** @retval SPEXP_OK             - Attach succeeded.
 ** @retval SPEXP_OPEN_FAILED    - The workspace databasd could not be opened.
 ** @retval SPEXP_NOT_EXPDATABASE - pHandle is not open on an experiment.
 ** @retval SPEXP_NOT_WORKSPACE  - path does not represent a workspace.
 ** @retval SPEXP_WRONGEXPERIMENT- The workspace experiment does not match the
 **                                experiment open on pHandle.
 ** @retval SPEXP_SQLFAIL         - Some sql operation failed.
 */
int
spectcl_workspace_attach(spectcl_experiment pHandle, const char* path, const char* attachPoint)
{
  spectcl_workspace  ws;
  uuid_t*            expUuid;
  uuid_t*            wsUuid;
  int                status;

  /* ensure pHandle is an experiment database handle */

  if(!isExperimentDatabase(pHandle)) {
    return SPEXP_NOT_EXPDATABASE;
  }
  /* The path must be path that can pass muster with spectcl_workspace_open(). */

  ws = spectcl_workspace_open(path);

  if (!ws) {
    return spectcl_experiment_errno;
  }
  else {
    /* Check that the workspace UUID matches that of the experiment */
    
    if (!spectcl_uuidCheck(pHandle, ws)) {
      spectcl_workspace_close(ws);
      return SPEXP_WRONGEXPERIMENT;
    }

    spectcl_workspace_close(ws);		/* TODO: When close is working use that instead. */

  }
  /* We've made all the validity checks... attach the database: */

  return spectcl_attach(pHandle, path, attachPoint, DEFAULT_ATTACH_POINT);


}
/**
 ** Close an open workspace.
 ** @param pHandle   - Handle open on a workspace.
 ** @return int
 ** @retval SPEXP_OK            - Correct completion.
 ** @retval SPEXP_NOT_WORKSPACE - Handle not open on a workspace.
 */
int
spectcl_workspace_close(spectcl_workspace pHandle)
{
  int status;


  if (!isWorkspace(pHandle)) {
    return SPEXP_NOT_WORKSPACE;
  }
  status = sqlite3_close(pHandle);

  if (status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    status = SPEXP_SQLFAIL;
    
  }
  else {
    status = SPEXP_OK;
  }

  return status;
}
/**
 ** Detach a workspace from an experiment.
 ** @param pHandle     - Experiment database handle from which the detach will be done.
 ** @param attachPoint - Attach point from which the detach is done, if NULL,
 **                      the detach is from the default attach point ('WORKSPACE').
 ** @return int
 ** @retval SPEXP_OK             - Everything worked.
 ** @retval SPEXP_NOT_EXPDATABASE - pHandle is not an experiment database.
 ** @retval SPEXP_NOT_WORKSPACE  - Something is attached there but it's not a workspace.
 ** @retval SPEXP_UNATTACHED     - the sql to do the detach failed.
 ** @retval SPEXP_NOMEM          - out of memory for some operation.
 */
int
spectcl_workspace_detach(spectcl_experiment pHandle, const char* attachPoint)
{
  const char*   whereAttached = DEFAULT_ATTACH_POINT;
  char          table[1001];	/* Out */
  char*         attachedDbType;
  const char*   pQuery = "DETACH DATABASE :dbname";
  sqlite3_stmt* statement;
  int           status;

  /* Require pHandle to be an epxeriment database */

  if (!isExperimentDatabase(pHandle)) {
    return SPEXP_NOT_EXPDATABASE;
  }

  /* Figure out the attached point and whether a workspace is attached there. */


  if (attachPoint) {
    whereAttached = attachPoint;
  }
  status = spectcl_checkAttached(pHandle, whereAttached, "workspace", SPEXP_NOT_WORKSPACE);
  if (status != SPEXP_OK) {
    return status;
  }


  /* Do the detach */

  return spectcl_detach(pHandle, whereAttached);

}
/**
 ** Return the schema version of a workspace.
 ** @param ws   - Workspace handle from spectcl_workspace_open.
 ** @return char* 
 ** @retval NULL - Failed in some way, error reason in spectcl_experiment_errno.
 ** @retval other - Pointer to a dynamically allocated version string.
 ** 
 ** Errors:
 **  - SPEXP_NOT_WORKSPACE   - ws is not a workspace.
 **  - SPEXP_SQLFAIL         - could not retrieve the version string in some way.
 */
char*
spectcl_workspace_version(spectcl_workspace ws)
{
  char* pResult;

  if (!isWorkspace(ws) ) {
    spectcl_experiment_errno = SPEXP_NOT_WORKSPACE;
    return NULL;
  }

  pResult = getfirst(ws, "configuration_values", "config_value", "config_item", "version");
  if (!pResult) {
    spectcl_experiment_errno = SPEXP_SQLFAIL;
  }
  return pResult;

}
/**
 ** Return the uuid of the workspace.
 **  @param ws  - Workspace handle.
 **  @return uuid_t* (dynamically allocated).
 **  @retval NULL     - error of some sort.  see below.
 **  @retval not-null - Pointer to the parsed UUID of the workspace.
 **                     must be free'd to avoid memory leaks. 
 **
 **  Errors (in spectcl_experiment_errno if return is NULL):
 **     SPEXP_NOT_WORKSPACE - ws is not a workspace.
 **     SPEXP_SQLFAIL       - Could not get the UUID from the database though it purports to be
 **                           a workspace.
 */
uuid_t*
spectcl_workspace_uuid(spectcl_workspace ws)
{
  uuid_t* uuid;

  if (!isWorkspace(ws)) {
    spectcl_experiment_errno = SPEXP_NOT_WORKSPACE;
    return NULL;
  }
  
  uuid = getDBUUID(ws);
  if (!uuid) {
    spectcl_experiment_errno = SPEXP_SQLFAIL;
  }
  return uuid;
}
/**
 ** Determines if a database handle is a workspace handle.
 ** @param ws  - Alleged workspace handle.
 ** @return int
 ** @retval TRUE  - Handle is a workspace.
 ** @retval FALSE - Handle is not a workspace.
 */
int
spectcl_workspace_isWorkspace(spectcl_workspace ws)
{
  return isWorkspace(ws) ? TRUE : FALSE;
}
/**
 ** If both parameters are the proper type of database, returns
 ** whether or not the workspace belongs to the experiment.
 ** @param expdb   - Experiment database.
 ** @param ws      - Worksapce database.
 ** @return int
 ** @retval TRUE  expdb is an experiment, and ws is a workspace for that exeriment.
 ** @retval FALSE (not any of the above).
 */
int
spectcl_workspace_isCorrectExperiment(spectcl_experiment expdb, spectcl_workspace ws)
{
  if (!isExperimentDatabase(expdb)) return FALSE;
  if (!isWorkspace(ws))     return FALSE;
  return spectcl_uuidCheck(expdb, ws);
}
