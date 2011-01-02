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

/** Unit tests for the
 ** spectcl_experiment.c module
 */

#include <stdlib.h>
#include <check.h>
#include <unistd.h>
#include <stdio.h>
#include "spectcl_experiment.h"
#include <sqlite3.h>




/* Static function to get a const char* to a value from a field that matches select
   parameterized by table, field and value
*/
static char* getfirst(sqlite3* db, const char* table, 
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
    fail(sqlite3_errmsg(db));
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


static char* tmpFile;

static void setup()
{
}
static void teardown()
{
  unlink(tmpFile);
}


/*--------------------------- database creation tests --------------------------------------*/

/*
** Creating an experiment database should create the database file and return a non-null handle:
*/
START_TEST(test_create)
{
  tmpFile = tmpnam(NULL);
  spectcl_experiment handle = spectcl_experiment_create(tmpFile);

  fail_if(handle == NULL);
  fail_unless(access(tmpFile, R_OK | W_OK) == 0);

  unlink(tmpFile);


}
END_TEST
/*
** Creating an experiment database should create the set of tables we need with the
** correct schema "configuration_values" should have been created and stuffed with some things:
*/
START_TEST(test_create_config_table)
{
  tmpFile = tmpnam(NULL);
  spectcl_experiment handle = spectcl_experiment_create(tmpFile);
  
  /* At this time we assume the handle is just an sqlite3 handle. */;

  sqlite3* db = (sqlite3*)(handle);  

  /* there should be a configuration_values table it must have entries for 
     version, uuid, and type. 
  */
  char* version =  getfirst(db, "configuration_values", "config_value", "config_item", "version");
  char* uuid    =  getfirst(db, "configuration_values", "config_value", "config_item", "uuid");
  char* type    =  getfirst(db, "configuration_values", "config_value", "config_item", "type");

  fail_if(version == NULL);
  fail_if(uuid    == NULL);
  fail_if(type    == NULL);

  /*  The type should be "experiment" */

  fail_if(strcmp(type, "experiment") != 0);

  free(version);
  free(uuid);
  free(type);

  unlink(tmpFile);
}
END_TEST

/*
** Creating the experiment database should also create an empty 'parameters' table.
*/
START_TEST(test_create_parameters_table)
{
  tmpFile = tmpnam(NULL);
  spectcl_experiment handle = spectcl_experiment_create(tmpFile);
  
  /* At this time we assume the handle is just an sqlite3 handle. */;

  sqlite3*      db = (sqlite3*)(handle);  
  sqlite3_stmt* stmt;
  int           rows;

  int status = sqlite3_prepare_v2(db, 
				 "SELECT COUNT(*) FROM parameters",
				 -1, &stmt, NULL);
  if (status != SQLITE_OK) {
    fail(sqlite3_errmsg(db));
  }

  status = sqlite3_step(stmt);
  if (status != SQLITE_ROW) {
    fail(sqlite3_errmsg(db));
  }

  /* there shouldn't be any rows in this table: */

  rows =  sqlite3_column_int(stmt, 0);
  fail_if(rows != 0);

  sqlite3_finalize(stmt);

  unlink(tmpFile);
  
}
END_TEST


/**
 ** Should also create an empty runs table
 */
START_TEST(test_create_runs_table)
{
  tmpFile = tmpnam(NULL);
  spectcl_experiment handle = spectcl_experiment_create(tmpFile);
  
  /* At this time we assume the handle is just an sqlite3 handle. */;

  sqlite3*      db = (sqlite3*)(handle);  
  sqlite3_stmt* stmt;
  int           rows;

  int status = sqlite3_prepare_v2(db, 
				 "SELECT COUNT(*) FROM runs",
				 -1, &stmt, NULL);
  if (status != SQLITE_OK) {
    fail(sqlite3_errmsg(db));
  }

  status = sqlite3_step(stmt);
  if (status != SQLITE_ROW) {
    fail(sqlite3_errmsg(db));
  }

  /* there shouldn't be any rows in this table: */

  rows =  sqlite3_column_int(stmt, 0);
  fail_if(rows != 0);

  sqlite3_finalize(stmt);

  unlink(tmpFile);
}
END_TEST

/*---------------------------------- Data base open tests: -----------------------*/

START_TEST(test_good_open) 
{
  char* uuid1;
  char* uuid2;
  uuid_t u1, u2;
  spectcl_experiment h1;
  spectcl_experiment h2;

  tmpFile = tmpnam(NULL);
  
  /* Create a good database; and then open it.  */
  
  h1 = spectcl_experiment_create(tmpFile);
  h2 = spectcl_experiment_open(tmpFile);

  /* should have a non-null h2 */

  fail_if(h2 == NULL);

  /* uuid's of the databases on the two handles should match */

  uuid1 = getfirst((sqlite3*)h1,
			       "configuration_values",
			       "config_value", "config_item", "uuid");
  fail_if(uuid1 == NULL);
  uuid2 = getfirst((sqlite3*)h2,
			       "configuration_values",
			       "config_value", "config_item", "uuid");
  fail_if(uuid2 == NULL);

  uuid_parse(uuid1, u1);
  uuid_parse(uuid2, u2);
  fail_if(uuid_compare(u1, u2) != 0);

  free(uuid1);
  free(uuid2);

  unlink(tmpFile);
  


}

END_TEST

/** If I just make a randomdatabase it should fail to open
 ** as an experiment database.
 */
START_TEST(test_bad_open)
{
  sqlite3*  h1;
  spectcl_experiment h2;
  tmpFile = tmpnam(NULL);

  sqlite3_open_v2(tmpFile,
		  &h1,
		  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

  h2 = spectcl_experiment_open(tmpFile);
  fail_unless(h2 == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOT_EXPDATABASE);

  unlink(tmpFile);

			   
}
END_TEST

/**
 ** creating a database...I should be able toclose it.
 */
START_TEST(test_good_close)
{
  spectcl_experiment h1;
  tmpFile = tmpnam(NULL);

  h1 = spectcl_experiment_create(tmpFile);
  fail_unless(spectcl_experiment_close(h1) == SPEXP_OK);
  unlink(tmpFile);
}
END_TEST

/**
 ** Closing a closed database handle ought to fail:
 */
START_TEST(test_bad_close)
{
  spectcl_experiment h1;
  tmpFile = tmpnam(NULL);

  h1 = spectcl_experiment_create(tmpFile);
  spectcl_experiment_close(h1);
  unlink(tmpFile);
  fail_if(spectcl_experiment_close(h1) == SPEXP_OK);
}
END_TEST

int main(void) 
{
  int  failures;
  Suite *s = suite_create("create");
  SRunner* sr = srunner_create(s);
  TCase* tc_experiment = tcase_create("Experiment");

  /* Set up the test cases: */


  tcase_add_test(tc_experiment, test_create); 
  tcase_add_test(tc_experiment, test_create_config_table);
  tcase_add_test(tc_experiment, test_create_parameters_table);
  tcase_add_test(tc_experiment, test_create_runs_table);
  
  tcase_add_test(tc_experiment, test_good_open);
  tcase_add_test(tc_experiment, test_bad_open);
  tcase_add_test(tc_experiment, test_good_close);
  tcase_add_test(tc_experiment, test_bad_close);
  

  tcase_add_checked_fixture(tc_experiment, setup, teardown);
  suite_add_tcase(s, tc_experiment);


  /* Set up the test runner:  */

  srunner_run_all(sr, CK_NORMAL);
  failures = srunner_ntests_failed(sr);

  srunner_free(sr);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

  return 0;
}
