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



/* Static function to get a  char* to a value from a field that matches select
   parameterized by table, field and value
   @param db    - sqlite3 database.
   @param table - Name of the table to query.
   @param field - Name of the field we want from the table.
   @param  matchfield - Name of the field to match fromt he table.
   @param  key  - value we want the match field to have.
*/
static char* 
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
  if (status != SQLITE_ROW) {
    return NULL;
  }
  column = sqlite3_column_text(statement, 0);
  result = malloc(strlen(column) + 1);
  strcpy(result, column);
  
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
  char* uuid;
  char* type = getfirst(db, "configuration_values", "config_value", 
			"config_item", "type");
  /**
   ** check type of database
   */

  if (!type) {
    return FALSE;
  }
  if (strcmp(type, "experiment") != 0) {
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
