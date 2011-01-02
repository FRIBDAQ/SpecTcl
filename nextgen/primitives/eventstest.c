#include <check.h>
#include "spectcl_experiment.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sqlite3.h>
#include <uuid/uuid.h>
#include <string.h>
#include <limits.h>

static char dbName[PATH_MAX];
static char expName[PATH_MAX];
spectcl_experiment db;

/*------------------------------------- fixture code ----------------------------------*/


static void setup()
{
  strcpy(dbName, tmpnam(NULL));
  db     = spectcl_experiment_create(dbName);
  strcpy(expName,tmpnam(NULL));
}
static void teardown()
{
  spectcl_experiment_close(db);
  unlink(dbName);
  unlink(expName);
}


/*---------------------------------- The tests -----------------------------------------*/

/*  The run must be an existing  run number in the database */

START_TEST(test_runexists)
{
  spectcl_events pHandle;

  pHandle = spectcl_events_create(db, 1, expName);
  fail_unless(pHandle == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOSUCH);

}
END_TEST

/*  The events data base cannot be an existing file */

START_TEST(test_eventsnooverwrite)
{
  spectcl_events pHandle;

  spectcl_run_create(db, 1, "This is a test run", NULL);
  pHandle = spectcl_events_create(db, 1, dbName);

  fail_unless(pHandle == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_EXISTS);
}
END_TEST

/*------------------------------------ final setup ---------------------------------------------*/
int main(void) 
{
  int  failures;
  Suite *s = suite_create("events_database");
  SRunner* sr = srunner_create(s);
  TCase* tc_experiment = tcase_create("events_database");



  tcase_add_checked_fixture(tc_experiment, setup, teardown);
  suite_add_tcase(s, tc_experiment);

  tcase_add_test(tc_experiment, test_runexists);
  tcase_add_test(tc_experiment, test_eventsnooverwrite);

  /* Set up the test runner:  */

  srunner_set_fork_status(sr, CK_NOFORK);

  srunner_run_all(sr, CK_NORMAL);
  failures = srunner_ntests_failed(sr);

  srunner_free(sr);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

  return 0;
}
