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
/**
 ** Common code to attache a database to a base database.
 ** @param db   - Sqlite handle to the base database.
 ** @param path - Filesystem path to the database to attach.
 ** @param point- If not null, the database is attached here.
 ** @param defaultPoint - If point is NULL, the database is attached here.
 ** @return int
 ** @retval SPEXP_OK  - Attach ok.
 ** @retval SPEXP_SQLFAIL - something went wrong... The details are in
 **                         spectcl_experiment_errno
 */
int
spectcl_attach(sqlite3* db, const char* otherDatabase, const char* point, const char* defaultPoint)
{
  char          formattedAttach[100];
  const char*   attach =  "ATTACH DATABASE :path AS %s";
  sqlite3_stmt* statement;
  int           status;



  sprintf(formattedAttach, attach, point ? point : defaultPoint);
  status = sqlite3_prepare_v2(db, 
			      formattedAttach,
			      -1, &statement, NULL
			      );
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  status = sqlite3_bind_text(statement, 1, otherDatabase, -1, SQLITE_STATIC);
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
 ** Detaches a database at the specified detach point.
 ** @param db    - sqlite3 handle to the database.
 ** @param pName - Attach point for the database.
 ** @return int
 ** @retval SPEXP_OK - Success.
 ** @retval SPEXP_SQLFAIL - error prepping or executing the detach statement.
 */
int
spectcl_detach(sqlite3* db, const char* pName)
{
  const char*   pQuery = "DETACH DATABASE :dbname";
  sqlite3_stmt* statement;
  int           status;

  status = sqlite3_prepare_v2(db, 
			      pQuery,  -1, &statement, NULL);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  status = sqlite3_bind_text(statement, 1, pName, -1, SQLITE_STATIC);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = status;
    sqlite3_finalize(statement);
    return SPEXP_SQLFAIL;
  }
  status = sqlite3_step(statement);
  if (status != SQLITE_DONE) {
    sqlite3_finalize(statement);
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  sqlite3_finalize(statement);
 

  return SPEXP_OK;



}
/**
 ** Checks that an attach point has the right type of database on it by 
 ** looking at the configuration_values 'type' entry.
 ** @param db           - The base database handle to check.
 ** @param pAttachName  - The Attachment point
 ** @param type         - Desired database type (e.g. 'events').
 ** @param incorrectStatus - Desired return if the query worked but the database was wrong.
 ** @return int
 ** @retval SPEXP_OK    - All the queries worked and the database was the right type.
 ** @retval SPEXP_UNATTACHED - got nothing back from the query..so not attached.
 ** @retval incorrectStatus  - If the type returned did not match the desired type.
 */
int 
spectcl_checkAttached(sqlite3* db, const char* pAttachname, const char* type, int incorrectStatus)
{
  char*  pType;
  char*  tableName;
  size_t tableNameLen;
  int    status;

  tableNameLen = strlen(pAttachname) + strlen(".configuration_values ");
  tableName    = malloc(tableNameLen);
  if(!tableName) return SPEXP_NOMEM;

  strcpy(tableName, pAttachname);
  strcat(tableName, ".configuration_values");
  pType = getfirst(db, tableName, "config_value", "config_item", "type");
  free(tableName);
  if (!pType) return SPEXP_UNATTACHED;

  status = strcmp(pType, type) == 0 ? SPEXP_OK : incorrectStatus;

  free(pType);

  return status;

}
/**
 ** Determines if the UUID's associated with a pair of databases match.
 ** @param db1   - First database.
 ** @param db2   - Second database.
 ** @return int
 ** @retval TRUE   - UUID could be retrieved and matched.
 ** @retval FALSE  - Either one or more UUID's could not be retrieved,
 **                  or they both could be but did not match.
 */
int
spectcl_uuidCheck(sqlite3* db1, sqlite3* db2)
{
  uuid_t*   uuid1;
  uuid_t*   uuid2;
  int       retval;

  retval = 0;			/* Assume false. */

  uuid1 = getDBUUID(db1);
  uuid2 = getDBUUID(db2);
  if (uuid1 && uuid2) {
    retval = uuid_compare(*uuid1, *uuid2) == 0;
  }
  free(uuid1);
  free(uuid2);

  return retval;
}
/**
 **common code to take a statement that may need to
 ** have a single point qualified by a workspace.
 ** The workspace must be passed in in the form:
 ** xxxxxx%s%sxxxxx
 ** Where %s%s is where, if necessary the
 ** AttachPoint. string will be substituted.
 **
 ** @param format - The format string described above
 **                 for the thing to be qualified.
 ** @param attachPoint - The attach point or null if there
 **                      is none.
 ** @return char*
 ** @return NULL - Dynamic storage allocation failed.
 ** @return other - Dynamically allocated result string.
 */
char* 
spectcl_qualifyStatement(const char* format, const char* pAttach)
{
  const char* pSeparator="";
  const char* pAttachment="";
  size_t      tableLength;
  char*       table;

  if (pAttach) {
    pSeparator = ".";
    pAttachment = pAttach;
  }
  tableLength = strlen(format) + 
    strlen(pAttachment) + strlen(pSeparator) + 2;
  table = malloc(tableLength);
  sprintf(table, format, pAttachment, pSeparator);

  return table;
}
/**
 ** Given a null termianted list of pointer to dynamically allocated simple pointers:
 * - free the storage pointed by each pointer.
 * - free the entire list.
 * @param pList   - Pointer to the list to free.
 */
void
spectcl_freePtrList(void** p)
{
  void** pList = p;
  while (*p) {
    free(*p);
  }
  free(pList);
}