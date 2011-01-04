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
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*--------------------- private utilities.   ----------------------------*/


/**
 ** Create the configuration_values table, and stock it accordingly.
 ** @param pHandle - experiment handle.
 ** @param pEvts   - Sqlite handel to newly created events database.
 ** @param run     - Number of the run represented by this database.
 ** @return int
 ** @retval 0 - everything worked.
 ** @retval -1- Failure of some sort.
 */
static int
createConfiguration(sqlite3* pHandle, sqlite3* pEvents, int run)
{
  const char* createSql = "CREATE TABLE configuration_values ( \
                                id           INTEGER PRIMARY KEY,\
                                config_item  VARCHAR(256),       \
                                config_value VARCHAR(256))";
  const char* insertSql = "INSERT INTO configuration_values \
                                (config_item, config_value) \
                                VALUES (:name, :value)";
  int          status;
  sqlite3_stmt* insert;
  char         runText[100];
  uuid_t*      pUuid;
  char         uuidText[100];

  do_non_select(pEvents, createSql);

  /* Prep arguments: */

  sprintf(runText, "%d", run);
  pUuid = spectcl_experiment_uuid(pHandle);
  uuid_unparse(*pUuid, uuidText);
  free(pUuid);


  status = insertConfig(pEvents, "version", SCHEMA_VERSION);
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return -1;
  }
  status = insertConfig(pEvents, "type", "run-data");
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return -1;
  }
  status = insertConfig(pEvents, "run",   runText);
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return -1;
  }
  status = insertConfig(pEvents, "uuid", uuidText);
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return -1;
  }


  
  return 0;
}

/**
 ** Actually create the database once everything is good. 
 ** The database schema is also created and stocked where appropriate.
 ** @param pHandle - Experiment database handle (we use this to get our uuid).
 ** @param run     - Run number we are linked to.
 ** @param path    - Path to the database file to be created.
 ** @param run     - Run number this database is bound to.
 ** @return spetcl_events - See spectcl_events_create for return values and errors.
 */
static spectcl_events 
createDatabase(spectcl_experiment pHandle, const char* path, int run)
{
  sqlite3* pEvents;
  int      status;
  
  status = sqlite3_open_v2(path,
			   &pEvents, SQLITE_OPEN_READWRITE |SQLITE_OPEN_CREATE,
			   NULL);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = SPEXP_CREATE_FAILED;
    return NULL;
  }
  /* Create/stock the configuration table */

  if (createConfiguration(pHandle, pEvents, run)) {
    spectcl_experiment_errno = SPEXP_SQLFAIL;
    sqlite3_close(pEvents);
    return NULL;
  }
  do_non_select(pEvents,
		"CREATE TABLE events ( \
                    id       INTEGER PRIMARY KEY,   \
                    trigger  INTEGER,               \
                    param_id INTEGER,               \
                    value    REAL)" );

  /* Index the trigger and param_id  to improve query performance */

  do_non_select(pEvents,
		"CREATE INDEX event_trigger ON events (trigger)");
  do_non_select(pEvents,
		"CREATE INDEX parameter     ON events (param_id)");

  return pEvents;
}

/*---------------------- public entries --------------------------------*/
/**
 ** Create a new events database associated with a run in an experiment.
 ** @param pExpHandle - Handle to the experiment database.
 ** @param run        - Number of the run we are going to be.
 ** @param path       - Path to the new database.
 ** @return spectcl_events
 ** @return NULL - Some problem occured, reason in spectcl_experiment_errno.
 ** @return other - Handle to the experiment database.
 ** 
 ** Possible errors include:
 **    SPEXP_OK   - Success
 *     SPEXP_NOSUCH - Run number does not exist in pExpHandle
 **    SPEXP_EXISTS - The database file already exists.
 **    SPEXP_NOT_EXPDATABASE - pExpHandle is not open on an experiment database.
 **    SPEXP_CREATE_FAILED  - Could not create database file (e.g. permissions).
 **    SPEXP_SQLFAIL- Some sql needed to create/stock the database failed.
 */
spectcl_events spectcl_events_create(spectcl_experiment pExpHandle, int run, 
				     const char* path)
{
  char*       pRun;
  char        runString[1000];
  int         status;
 
  /* See if the run number exists:  */

  sprintf(runString, "%d", run);
  pRun = getfirst(pExpHandle, "runs", "id", "id", runString);
  if (!pRun) {
    spectcl_experiment_errno = SPEXP_NOSUCH;
    return NULL;
  }
  free(pRun);


  /* Ensure the events database file does not yet exist */

  status  = access(path, F_OK);
  if (status == 0) {
    spectcl_experiment_errno = SPEXP_EXISTS;
    return NULL;
  }

  /* Create and stock the events database  */

  return createDatabase(pExpHandle, path, run);
}
/**
 ** Open an events database independent of an experiment.  This is used to
 ** e.g. load events into the database; while the source of data may need
 ** access to e.g. the parameter definitions, the
 ** actual loader does not..especially if we envision datasourece |loader as an architecture
 ** for loading the database.
 ** @param path - Path to the database.
 ** @return spetcl_events
 ** @retval NULL -failure and the error code is set in spectcl_experiment_errno.
 ** @retval other- Handle to the events database requested.
 **
 ** Possible errors include:
 ** - SPEXP_OPEN_FAILED - sqlite3_open failed...e.g. no such file.
 ** - SPEXP_NOT_EVENTSDATABASE - database is not an events database.
 ** - 
 */
spectcl_experiment
spectcl_events_open(const char* path)
{
  sqlite3* db;
  char*    pType;
  int      status;

  status = sqlite3_open_v2(path, &db, SQLITE_OPEN_READWRITE, NULL);
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = SPEXP_OPEN_FAILED;
    return NULL;
  }
  
  if (!isEventsDatabase(db)) {
    sqlite3_close(db);
    spectcl_experiment_errno = SPEXP_NOT_EVENTSDATABASE;
    return NULL;
  }
  
  return db;			/* success! */
}
/**
 ** Close an events database.
 ** @param db   - handle open on the events database.
 ** @return int
 ** @retval SPEXP_OK -success
 ** @retval OTHER -failure.
 ** @note if SPEXP_SQLFAIL that indicates an error from an sqlite3 call and the sqlite status
 **       is available in spectcl_experiment_errno
 */
int
spectcl_events_close(spectcl_events db)
{
  int status = sqlite3_close(db);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  return SPEXP_OK;
}
/**
 ** Attach an events database to an experiment database.  This is generally done
 ** to allow the production of spectrum instances from the event data contained
 ** in the events database and would require a workspace be attached as well.
 ** - The database file must be a valid events database.
 ** - The experiment handle must be a handle to an experiment database.
 ** - The SQL to attach the database must succeed.
 ** @param pExpHandle - handle open on an experiment database.
 ** @param path       - Path to the events database.
 ** @param name       - Pointer to the name under which the attachment will occur.
 **                     this defaults to "EVENTS" if the pointer is null.
 ** @return int
 ** @retval SPEXP_OK  - Success.
 ** @retval SPEXP_SQLFAIL - the attachment sql failed.  In this case, the sql status is in
 **                         spectcl_experiment_errno.l
 ** @retval SPEXP_OPEN_FAILED        - Unable to open the event database to determine its type.
 ** @retval SPEXP_NOT_EVENTSDATABASE - path is not an events database. 
 ** @retval SPEXP_WRONGEXPERIMENT    - events database uuid does not match that of the experiment.
 */
int
spectcl_events_attach(spectcl_experiment pExpHandle, const char* path, const char* name)
{
  const char*    pDefaultAttachName = "EVENTS";
  spectcl_events pEvents;
  sqlite3_stmt*  statement;
  char*          myUuidString;
  uuid_t         myuuid;
  const char*    attach = "ATTACH DATABASE :path AS %s";
  char           formattedAttach[100];
  int            status;

  /* The best way to check the validity of the database is to just open it: */

  pEvents = spectcl_events_open(path);
  if (!pEvents) {
    return spectcl_experiment_errno;
  }
  /* Ensure this event database matches the experiment we are open on. */

  myUuidString = getfirst(pEvents, "configuration_values", "config_value", "config_item", "uuid");
  uuid_parse(myUuidString, myuuid);
  free(myUuidString);
  spectcl_events_close(pEvents); /* don't need the handle for the attach, so close here */
  if (!spectcl_correct_experiment(pExpHandle, &myuuid)) {
    return SPEXP_WRONGEXPERIMENT;
  }
  
  /* Try to do the attach.  Note that it's not legal (I think) to parameterize the database
  ** name.. hence the sprintf.
  */
  sprintf(formattedAttach, attach, name ? name : pDefaultAttachName);
  status = sqlite3_prepare_v2(pExpHandle, 
			      formattedAttach,
			      -1, &statement, NULL
			      );
  if (status != SQLITE_OK) {
    fprintf(stderr, "%s", sqlite3_errmsg(pExpHandle));
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  status = sqlite3_bind_text(statement, 1, path, -1, SQLITE_STATIC);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    sqlite3_finalize(statement);
    return SPEXP_SQLFAIL;
  }
  status = sqlite3_step(statement);
  sqlite3_finalize(statement);
  if(status != SQLITE_DONE) {
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  

  return SPEXP_OK;
}
/**
 ** Detach an event database from the associated experiment database.
 ** - There must be a database attached on the specified name.
 ** - The database attached there must be an experiment database.
 ** @param pExperiment  - Experiment database handle.
 ** @param name         - Attach point of the database.  If NULL,
 **                       "EVENTS" is used.
 ** @return int
 ** @retval SPEXP_OK  - Success.
 ** @retval SPEXP_NOT_EVENTSDATABASE - there is no events database at that point.
 ** @retval SPEXP_SQLFAIL - The detach sql failed.
 ** @retval SPEXP_NOMEM   - memory allocation failed.
 */
int
spectcl_events_detach(spectcl_experiment pExperiment, const char* name)
{
  char* pType;
  const char*   pName        = "EVENTS";
  const char*   pQuery = "DETACH DATABASE :dbname";
  char*         tableName;
  size_t        tableNameLen;
  sqlite3_stmt* statement;
  int           status;

  /* Ensure there's a database attached there */

  if (name != NULL) {
    pName = name;
  }
  tableNameLen = strlen(pName) + strlen(".configuration_values ");
  tableName    = malloc(tableNameLen);
  if(!tableName) return SPEXP_NOMEM;

  strcpy(tableName, pName);
  strcat(tableName, ".configuration_values");
  pType = getfirst(pExperiment, tableName, "config_value", "config_item", "type");
  free(tableName);
  if (!pType) return SPEXP_NOT_EVENTSDATABASE;

  
  status = sqlite3_prepare_v2(pExperiment, 
			      pQuery,  -1, &statement, NULL);
  if (status != SQLITE_OK) {
    fprintf(stderr, "%s\n", sqlite3_errmsg(pExperiment));
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  status = sqlite3_bind_text(statement, 1, pName, -1, SQLITE_STATIC);
  if (status != SQLITE_OK) {
    fprintf(stderr, "%s\n", sqlite3_errmsg(pExperiment));
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  status = sqlite3_step(statement);
  if (status != SQLITE_DONE) {
    fprintf(stderr, "%s\n", sqlite3_errmsg(pExperiment));
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
 

  return SPEXP_OK;
}
 
/**
 ** Load data into the events table.  The load is done within a transaction so its an all or nothing
 ** sort of thing.  Doing the load within a  transaction is also empirically faster in sqlite.
 ** @param pEvents     -   Handle open on the events database.
 ** @param nParameters -   Number of parameters to insert in the events table.
 ** @param pData       -   data to insert.  This is an array of nParameters elements of ParameterData
 **                        structs.  The data in each struct is inserted in the database.  The fields
 **                        of this struct are as follows:
 **                        - s_trigger - trigger number.  All parameters that share a trigger number
 **                                      are considered to belong to the same event.
 **                        - s_parameter - Id of the parameter to set.  This should be a parameter id
 **                                      from the experiment database that has the same uuid as 
 **                                      this database.
 **                        - s_value   - A double that is the value of the parameter for that trigger.
 ** @return int
 ** @retval SPEXP_OK  - Success.
 ** @retval SPEXP_NOT_EVENTSDATABASE - pEvents is not an events database.
 ** @retval SPEXP_SQLFAIL - Some SQL Failed...status is in the spectcl_experiment_errno variable.
 **
 ** @note If there is a failure in the insertion an time after insertions have begun,
 **       the transaction is rolled back and no events get inserted.
 */
int
spectcl_events_load(spectcl_events pEvents, size_t nParameters,  const pParameterData* pData)
{
  const char*   insertSql;
  sqlite3_stmt* insert;
  int           status;

  // Ensure this is an events database:

  if(!isEventsDatabase(pEvents)) return SPEXP_NOT_EVENTSDATABASE;

  /* It's always ok to hand nParameters -- no point in starting transactions etc. */

  if (nParameters == 0) return SPEXP_OK;

  return SPEXP_UNIMPLEMENTED;
}
