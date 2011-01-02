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
#include "spectcl_experimentInternal.h"
#include <uuid/uuid.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifndef FALSE
#define FALSE (1 == 0);
#endif
#ifndef TRUE
#define TRUE (0==0)
#endif

/**
 ** Implementation of the C API for the spectcl experiment database API.
 ** See spectcl_experiment_tcl.c for Tcl bindings.
 ** See spectcl_experiment.cpp   for C++ bindings.
 */

/**
 ** sqlite function to execute a statement that is not a select:
 */
static void
do_non_select(sqlite3* db, const char* statement)
{
  sqlite3_stmt* stmt;

  sqlite3_prepare_v2(db, statement, -1, &stmt, NULL);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

/**
 ** Create/populate the configuration_values table:
 */
static void
create_config_values(sqlite3* db)
{
  uuid_t        uuid;
  char          uuid_string[40]; /* 36 + null terminator according to docs. */
  sqlite3_stmt* stmt;
  do_non_select(db,
		"CREATE TABLE configuration_values \
                                 (id integer primary key,   \
                                  config_item varchar(256),	\
                                  config_value varchar(256))");

  /* Insert version */
  
  sqlite3_prepare_v2(db,
		     "INSERT INTO configuration_values \
                            (config_item, config_value) \
                            VALUES ( :what, :value)",
		     -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, "version", -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, SCHEMA_VERSION, -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);

  /* Insert type:   */

  sqlite3_bind_text(stmt, 1, "type", -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, "experiment", -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);

  /*  Insert uuid text  */

  uuid_generate(uuid);
  uuid_unparse(uuid, uuid_string);
  sqlite3_bind_text(stmt, 1, "uuid", -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, uuid_string, -1, SQLITE_STATIC);
  sqlite3_step(stmt);

  sqlite3_finalize(stmt);

  

}
/**
 ** Create an empty parameters table:
 */
static void
create_parameters(sqlite3* db)
{
  do_non_select(db,"CREATE TABLE parameters    \
                         (id integer primary key,  \
                          name varchar(512) NOT NULL,       \
                          units varchar(512),      \
                          lowest REAL,              \
                          highest REAL)");

}
/**
 ** Create an empty runs table:
 */
static void
create_runs(sqlite3* db)
{
  do_non_select(db, "CREATE TABLE runs                     \
                    (id integer primary key,               \
                     title varchar(80)      NOT NULL,      \
                     start_time varchar(80) NOT NULL,      \
                     end_time   varchar(80),               \
                     database   varchar(512))");
}
/**
 ** Create the initial schema:
 */
static void
create_schema(sqlite3* db)
{
  create_config_values(db);
  create_parameters(db);
  create_runs(db);
}



/*-----------------------------------------------------------------
**  Public interfaces:
**-----------------------------------------------------------------*/
/**
 ** Create an experiment database.
 **  @param new_database - Path to to the database file to create.
 **  @return spectcl_experiment
 **  @retval NULL     - unable to create the experiment.
 **  @retval non-null - handle to use in further calls to this api.
 */
spectcl_experiment
spectcl_experiment_create(const char* new_database)
{
  sqlite3* dataBase;
  int status = sqlite3_open_v2(new_database, &dataBase,
			       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
			       NULL);
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = SPEXP_CREATE_FAILED;
    return NULL;
  }
  create_schema(dataBase);

  spectcl_experiment_errno = SPEXP_OK;
  return (spectcl_experiment)(dataBase);
}

/**
 ** Open an existing experiment database file.
 ** to succeed:
 ** - sqlite must be able to open the file as a database.
 ** - there must be a configuration_values table.
 ** - The type entry in that table must be "experiment"
 */
spectcl_experiment
spectcl_experiment_open(const char* database)
{
  sqlite3* db;
  int status = sqlite3_open_v2(database, &db,
				SQLITE_OPEN_READWRITE, NULL);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = SPEXP_OPEN_FAILED;
    return NULL;
  }
  if (!isExperimentDatabase(db)) {
    sqlite3_close(db);
    spectcl_experiment_errno = SPEXP_NOT_EXPDATABASE;
    return NULL;
  }
  return (spectcl_experiment)(db);
}
/**
 ** Close an experiment database.
 ** @param db  - spectcl_experiment handle to the database.
 ** @return int
 ** @retval 0     - Success
 ** @retval other - Failure code in the spectcl_experiment.h
 **                 this will also be stuffed inthe spectcl_experiment_errno variable.
 */
int
spectcl_experiment_close(spectcl_experiment db)
{
  int status = sqlite3_close((sqlite3*) db);
  return status == SQLITE_OK ? 0 : SPEXP_CLOSE_FAILED;
}
