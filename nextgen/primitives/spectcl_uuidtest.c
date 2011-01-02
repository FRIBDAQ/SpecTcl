#include <check.h>
#include "spectcl_experiment.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sqlite3.h>
#include <uuid/uuid.h>

static char* dbName;
spectcl_experiment db;

/*------------------------------------- fixture code ----------------------------------*/


static void setup()
{
  dbName = tmpnam(NULL);
  db    = spectcl_experiment_create(dbName);
}
static void teardown()
{
  spectcl_experiment_close(db);
  unlink(dbName);

}

/*---------------------------------- The tests -----------------------------------------*/

/**
 ** An experiment database must have a uuid.
 */
START_TEST(test_haveuuid)
{
  uuid_t* uuid = spectcl_experiment_uuid(db);
  fail_if(uuid == NULL);


  free(uuid);
}
END_TEST

/**
 ** An experiment's uuid should match itself.
 */
START_TEST(test_matchself)
{
  uuid_t* uuid = spectcl_experiment_uuid(db);
  fail_unless(spectcl_correct_experiment(db, uuid));

  free(uuid);
}
END_TEST

/**
 ** An experiment's uuid should not match one just generated.
 */
START_TEST(test_mismatch)
{
  uuid_t random;

  uuid_generate(random);
  fail_if(spectcl_correct_experiment(db, &random));

}
END_TEST
/*------------------------------------ final setup ---------------------------------------------*/
int main(void) 
{
  int  failures;
  Suite *s = suite_create("uuids");
  SRunner* sr = srunner_create(s);
  TCase* tc_experiment = tcase_create("uuids");


  tcase_add_test(tc_experiment, test_haveuuid);
  tcase_add_test(tc_experiment, test_matchself);
  tcase_add_test(tc_experiment, test_mismatch);



  tcase_add_checked_fixture(tc_experiment, setup, teardown);
  suite_add_tcase(s, tc_experiment);



  /* Set up the test runner:  */

  srunner_run_all(sr, CK_NORMAL);
  failures = srunner_ntests_failed(sr);

  srunner_free(sr);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

  return 0;
}
