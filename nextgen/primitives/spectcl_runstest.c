#include <check.h>
#include "spectcl_experiment.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sqlite3.h>


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

/*
** A new data base should have no runs
*/

START_TEST(test_initiallyempty)
{
  run_list pList;
  pList = spectcl_run_list(db);
  fail_if(pList == NULL);
  fail_unless(pList[0] == NULL);

}

END_TEST
/*
** Inserting a run should allow me to fetch it out of the database.
*/
START_TEST(test_addrun)
{
  run_list pList;
  pRunInfo pInfo;
  const char* pTitle = "This is a test run";
  time_t      now;
  int         status;
  char        t[1000];

  time(&now);

  status =   spectcl_run_create(db, 1, pTitle, NULL);
  fail_unless(status == SPEXP_OK);

  pList = spectcl_run_list(db);
  fail_if(pList == NULL);
  fail_if(pList[0] == NULL);

  pInfo = pList[0];
  fail_unless(pInfo->s_id == 1);
  fail_unless(strcmp(pInfo->s_pTitle, pTitle) == 0);

  strcpy(t, ctime(&now));

  t[strlen(t)-1] = 0;
  fail_unless(strcmp(pInfo->s_pStartTime, t) == 0);

  /* These times were not yet known: */

  fail_unless(pInfo->s_pEndTime == NULL);

  spectcl_free_run_list(pList);

}
END_TEST


/**
 ** Should not be possible to add a duplicate run.
 */
START_TEST(test_adddup) 
{

  const char* pTitle = "This is a test run";
  int         status;


  status =   spectcl_run_create(db, 1, pTitle, NULL);
  fail_unless(status == SPEXP_OK);

  status = spectcl_run_create(db, 1, pTitle, NULL);
  fail_if(status == SPEXP_OK);
  fail_unless(status == SPEXP_EXISTS);
}
END_TEST

/**
 ** Should be able to add several runs
 */
START_TEST(test_addMultiple)
{
  const char* pTitle = "This is a test run";
  int         status;
  run_list pList;
  pRunInfo pInfo;
  int      irun =0;

  for(irun = 0; irun < 10; irun++) {
    status = spectcl_run_create(db, irun, pTitle, NULL);
    fail_unless(status == SPEXP_OK);	       
  }
  /** Should be able to fish them all out -- sorted by runnumber */

  pList = spectcl_run_list(db);
  for(irun =0; irun < 10; irun++) {
    fail_if(pList[irun] == NULL);
    pInfo = pList[irun];
    fail_unless(pInfo->s_id == irun);
    fail_unless(strcmp(pInfo->s_pTitle, pTitle)== 0);
  }

  spectcl_free_run_list(pList);

}
END_TEST

/**
 ** Should not be able to end a run that does not exist:
 */
START_TEST(test_endnonexistent)
{
  int status = spectcl_run_end(db, 1, NULL);
  fail_unless(status == SPEXP_NOSUCH);
}
END_TEST
/*
** Ending a run that exists should give SPEXP_OK
*/
START_TEST(test_end)
{
  int status;
  spectcl_run_create(db, 1, "This is a test", NULL);

  status = spectcl_run_end(db, 1, NULL);
  fail_unless(status == SPEXP_OK);
}
END_TEST
/*
** Ending a run that exists should modify the date/time
*/
START_TEST(test_end_tok)
{
  int status;
  time_t t;
  run_list pList;
  pRunInfo pInfo;
  char Time[1000];
  time(&t);

  spectcl_run_create(db, 1, "This is a atest", NULL);

  spectcl_run_end(db, 1, &t);	/* end run with a known time. */

  /* Assume the run we made is the only one: */
  
  pList = spectcl_run_list(db);
  fail_if(pList==NULL );
  fail_if(*pList == NULL);

  pInfo = *pList;
  fail_if(pInfo->s_pEndTime == NULL);
  strcpy(Time, ctime(&t));
  Time[strlen(Time) - 1] = 0;	/* strip of \n */

  fail_unless(strcmp(pInfo->s_pEndTime, Time) == 0);

}
END_TEST

/*------------------------------------ final setup ---------------------------------------------*/
int main(void) 
{
  int  failures;
  Suite *s = suite_create("runs");
  SRunner* sr = srunner_create(s);
  TCase* tc_experiment = tcase_create("runs");


  tcase_add_test(tc_experiment, test_initiallyempty);
  tcase_add_test(tc_experiment, test_addrun);
  tcase_add_test(tc_experiment, test_adddup);
  tcase_add_test(tc_experiment, test_addMultiple);

  tcase_add_test(tc_experiment, test_endnonexistent);
  tcase_add_test(tc_experiment, test_end);
  tcase_add_test(tc_experiment, test_end_tok);



  tcase_add_checked_fixture(tc_experiment, setup, teardown);
  suite_add_tcase(s, tc_experiment);



  /* Set up the test runner:  */

  /*  srunner_set_fork_status(sr, CK_NOFORK); */
  srunner_run_all(sr, CK_NORMAL);
  failures = srunner_ntests_failed(sr);

  srunner_free(sr);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

  return 0;
}
