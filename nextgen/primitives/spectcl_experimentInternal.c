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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef FALSE
#define FALSE (1 == 0);
#endif
#ifndef TRUE
#define TRUE (0==0)
#endif

/**
 ** Static function to determine if a database is of a specific type.
 ** - require that there is a configuratin_values table.
 ** - require that table have an entry type which matches the input.
 ** - require that table have a uuid which can have any value.
 ** @param db    sqlite3 handle open on the database.
 ** @param type  desired database type string.
 ** @return int
 ** @retval TRUE - If this is the right database type.
 ** @retval FALSE - if this is not the right database type.
 */

static int
isDatabaseType(sqlite3* db, const char* typeString)
{
  char* uuid;
  char* type = getfirst(db, "configuration_values", "config_value", 
			"config_item", "type");
  /**
   ** check type of database
   */

  if (!type) {
    return FALSE;
  }
  if (strcmp(type, typeString) != 0) {
    free(type);
    return FALSE;
  }
  free(type);

  /** Ensure there's a uuid entry:
   */
  uuid = getfirst(db, "configuration_values", "config_value",
		  "config_item", "uuid");
  if (!uuid) {
    return FALSE;
  }
  free(uuid);
  return TRUE;
}

/* Static function to get a  char* to a value from a field that matches select
   parameterized by table, field and value
   @param db    - sqlite3 database.
   @param table - Name of the table to query.
   @param field - Name of the field we want from the table.
   @param  matchfield - Name of the field to match fromt he table.
   @param  key  - value we want the match field to have.
*/
char* 
getfirst(sqlite3* db, const char* table, 
		     const char* field, const char* matchfield, const char* key)
{
  unsigned char* result = NULL;
  const unsigned char* column;
  sqlite3_stmt* statement;
  char query[10000];
  int status;

  /* Since parameters can only be bound for string literals: */

  sprintf(query, "SELECT %s FROM %s WHERE %s = '%s'", field, table, matchfield, key);

  status = sqlite3_prepare_v2(db,
			      query,
			      -1, &statement, NULL);

  if (status != SQLITE_OK) {
    return NULL;
  }

  status = sqlite3_step(statement);
  if (status != SQLITE_ROW) 
  {
    sqlite3_finalize(statement);
    return NULL;
  }
  result = getTextField(statement, 0);
  
  sqlite3_finalize(statement);

  return result;
  
}




/**
 ** Determines if an sqlite3 database is in fact an experiment database.
 ** We are going to see if there's a configuration values table and ensure that
 ** it contains entries for the type == experiment and uuid which can be
 ** anything
 ** @param db - sqlite3 handle for the database.
 ** @return int
 ** @retval FALSE - not an experiment database.
 ** @retval TRUE  - Is an experiment database.
 */
int
isExperimentDatabase(sqlite3* db)
{
  return isDatabaseType(db, "experiment");
}

/** 
 ** Determins if an sqlite3 database is in fact an events database.
 ** Requrie that there be  configuration_values table and that it has
 ** a 'type' entry with the value "run-data"..and that there is a UUID which could have any
 ** value.
 **
 ** @param db - sqlite3 handle for the database.
 ** @return int
 ** @retval FALSE - not an experiment database.
 ** @retval TRUE  - Is an experiment database.
 */
int isEventsDatabase(sqlite3* db)
{
  return isDatabaseType(db, "run-data");
}


/**
 ** Copy a string to a dynamically allocated string and return that.
 ** @param s   - Input string
 ** @return char* - Resulting string.
 ** @retval NULL - memory allocation failed.
 */
char*
copyString(const unsigned char* s)
{
  char* pResult = malloc(strlen(s) + 1);
  if (pResult) {
    strcpy(pResult, s);
  }
  return pResult;
}

/**
 ** Get a text field from the database.   Unlike the sqlite3_column_text
 ** function, this returns text owned by malloc/free etc.
 ** @param sqlite3_stmt*   Statement that has been stepped.
 ** @param field           Index of field to fetch.
 ** @return char*
 ** @retval Pointer to result.
 */
char* getTextField(sqlite3_stmt* stmt, int field)
{
  const unsigned char* pSqliteResult;

  pSqliteResult = sqlite3_column_text(stmt, field);

  return copyString(pSqliteResult);
}
/**
** Get a text field that is allowed to be NULL.
** If null, NULL is returned, otherwise, a pointer to the text
** retrieved.
*  There is a tacit assumption that sufficient memory exists for the
** text field.
** @param sqlite3_stmt*   Statement that has been stepped.
** @param field           Index of field to fetch.
** @return char*
** @retval Pointer to result.
** @retval field is NULL (or ENOMEM).
*/
char* getOptionalTextField(sqlite3_stmt* stmt, int field)
{
  if (sqlite3_column_type(stmt, field) != SQLITE_NULL) {
    return getTextField(stmt, field);
  }
  else {
    return NULL;
  }
}

/**
 ** sqlite function to execute a statement that is not a select..and has no bindable parameters.
 ** @param db         - sqlite3 database handle.
 ** @param statement  - statement to execute.
 */
void
do_non_select(sqlite3* db, const char* statement)
{
  sqlite3_stmt* stmt;
  int           status;

  status = sqlite3_prepare_v2(db, statement, -1, &stmt, NULL);
  if (status != SQLITE_OK) {
    fprintf(stderr, "%s", sqlite3_errmsg(db));
  }
  status = sqlite3_step(stmt);
  if (status != SQLITE_DONE) {
    fprintf(stderr, "%s", sqlite3_errmsg(db));
  }
  sqlite3_finalize(stmt);
}

/**
 ** Insert an item into a configuration database table.
 ** @param db     - Sqlite3 database handle.
 ** @param which  - Name of configuration item.
 ** @param what   - Value of configuration item.
 ** @return int
 ** @retval SQLITE_OK  - ok completion.
 ** @retval other      - SQLITE error returned from one of the functions needed to do thisk.
 **
 */
int
insertConfig(sqlite3* db, const char* which, const char* what)
{
  const char* insertSql = "INSERT INTO configuration_values \
                                (config_item, config_value) \
                                VALUES (:name, :value)";
  int          status;
  sqlite3_stmt* insert;

  status = sqlite3_prepare_v2(db, insertSql, -1, &insert, NULL);
  if(status != SQLITE_OK) return status;

  status = sqlite3_bind_text(insert, 1, which, -1, SQLITE_STATIC); 
  if (status != SQLITE_OK) {
    sqlite3_finalize(insert);
    return status;
  }
  status = sqlite3_bind_text(insert, 2, what,  -1, SQLITE_STATIC);
  if (status != SQLITE_OK) {
    sqlite3_finalize(insert);
    return status;
  }
  status = sqlite3_step(insert);
  if (status != SQLITE_DONE) {
    sqlite3_finalize(insert);
    return status;
  }


  return sqlite3_finalize(insert);

}
/*
** Marshall data from a row of the run table into a dynamically allocated pRunInfo struct.
** @param stmt - Sqlite statement that has just been stepped.
** @return pRunInfo
** @retval Pointer to dynamically allocated pRunInfo filled in with the data from the row.
*/
pRunInfo
marshallRunInfo(sqlite3_stmt* stmt)
{
  pRunInfo pItem;		/* We'll fill this in. */
  
  pItem = calloc(1, sizeof(run_info)); /* Assumes that NULL = (void*) 0 */
  if (!pItem) return NULL;

  /** the following items are mandatory so we don't bother to check to see if they are null.
   */

  pItem->s_id         = sqlite3_column_int(stmt, 0); /* id - run number. */
  pItem->s_pTitle     =  getTextField(stmt, 1);
  pItem->s_pStartTime =  getTextField(stmt, 2);

  /* The remainder may be null so we need to check: */

  pItem->s_pEndTime = getOptionalTextField(stmt, 3);

  return pItem;
}
/*
 ** Return the unparsed UUID fromt a event database
 ** @param db  - sqlite3 handle database
 ** @return uuid_t*
 ** @retval NULL - Unable to get the UUID, spectcl_experiment_errno has the reason for that.
 ** @retval other- Pointer to dynamically allocated uuid_t that contains the parsed UUID.
 */
uuid_t*
getDBUUID(sqlite3* db)
{
  const char*           pSql = "SELECT config_value FROM configuration_values \
                                                    WHERE config_item = 'uuid'";
  sqlite3_stmt*         stmt;
  int                   status;
  uuid_t*               result = NULL;
  uuid_t                uuid;
  const unsigned char*  uuidText;

  status = sqlite3_prepare_v2(db,
			      pSql, -1, &stmt, NULL);
  if(status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return NULL;
  }
  
  status = sqlite3_step(stmt);
  if (status != SQLITE_ROW) {
    spectcl_experiment_errno = status;
    return NULL;
  }
  uuidText = sqlite3_column_text(stmt, 0);
  result = malloc(sizeof(uuid_t));
  uuid_parse((char*)uuidText, *result);

  sqlite3_finalize(stmt);

  return result;
}
