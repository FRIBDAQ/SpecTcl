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
#include <check.h>
#include "spectcl_experiment.h"
#include <sqlite3.h>
#include "spectcl_experimentInternal.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <uuid/uuid.h>
#include <string.h>
#include <limits.h>



static char dbName[PATH_MAX];
static char wsName[PATH_MAX];
static char wsName1[PATH_MAX];


spectcl_experiment db;
spectcl_workspace  ws;

/*----------------------------------- fixture code ----------------------------------------*/

static void setup()
{
  strcpy(dbName, tmpnam(NULL));
  db     = spectcl_experiment_create(dbName);
  strcpy(wsName, tmpnam(NULL));
  spectcl_workspace_create(db, wsName);
  ws = spectcl_workspace_open(wsName);
}

static void teardown()
{
  spectcl_workspace_close(ws);
  spectcl_experiment_close(db);
  unlink(dbName);
  unlink(wsName);
}
/*----------------- test cases ---------------*/

/**
 ** ensure the correct set of schema were made
 ** for spectrum definitions when a workspace is created.
 ** Specifically we should be able to make a query from
 ** spectrum_definitions and
 ** spectrum_parameters
 ** (with zero rows coming back).
 **
 ** 
 */
START_TEST(test_schema)
{
  sqlite3_stmt* stmt;
  const char* query1 = "SELECT * FROM spectrum_definitions";
  const char* query2 = "SELECT * FROM spectrum_parameters";
  int   status;

  status = sqlite3_prepare_v2(ws, query1,
			      -1, &stmt, NULL);
  fail_unless(status == SQLITE_OK);
  status = sqlite3_step(stmt);
  fail_unless(status == SQLITE_DONE);
  sqlite3_finalize(stmt);
  
  status = sqlite3_prepare_v2(ws, query2,
			      -1, &stmt, NULL);
  fail_unless(status == SQLITE_OK);
  status = sqlite3_step(stmt);
  fail_unless(status == SQLITE_DONE);
  sqlite3_finalize(stmt);
}
END_TEST

/*------------------- Final setup  -----------*/
int main(void) 
{
  int  failures;

  Suite* s = suite_create("spectcl_workspace_spectra");
  SRunner* sr = srunner_create(s);
  TCase*   tc_spectra = tcase_create("spectra");

  tcase_add_checked_fixture(tc_spectra, setup, teardown);
  suite_add_tcase(s, tc_spectra);


  tcase_add_test(tc_spectra, test_schema);

  srunner_set_fork_status(sr, CK_NOFORK);


  srunner_run_all(sr, CK_NORMAL);
  failures = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
