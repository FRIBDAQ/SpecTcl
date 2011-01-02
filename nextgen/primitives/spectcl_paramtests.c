#include <check.h>
#include "spectcl_experiment.h"
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

/*
** Create a fresh database
*/
static char* dbName;
spectcl_experiment db;
static void setup(void)
{
  dbName = tmpnam(NULL);
  db    = spectcl_experiment_create(dbName);
}

static void teardown(void)
{
  spectcl_experiment_close(db);
  unlink(dbName);
}

/** As created a database has no parameter definitions.
 */

START_TEST(test_initiallyempty)
{
  parameter_list p = spectcl_parameter_list(db,
					  "*");
  fail_if(p == NULL);
  fail_unless(*p == NULL);

  spectcl_free_parameter_list(p);
}
END_TEST
/*
** On inserting a parameter we should be able to get it back out as the only item in the
** database
*/
START_TEST(test_insertion)
{
  parameter_list pList; 
  pParameterInfo pInfo;
  double         low = -1.0;
  double         hi  = 1.0;
  int status = spectcl_parameter_create(db,
					"test.parameter",
					"mm", &low, &hi);
  fail_unless(status == SPEXP_OK);

  pList = spectcl_parameter_list(db, "*");

  /* Basic checks that we got something and only one thing back. */
  
  fail_if(pList == NULL);
  fail_if(*pList== NULL);
  fail_unless(pList[1] == NULL);

  /* Checks on what we got back */

  pInfo = *pList;
  fail_unless(strcmp(pInfo->s_pName, "test.parameter") == 0);
  fail_if(pInfo->s_pUnits == NULL);
  fail_unless(strcmp(pInfo->s_pUnits, "mm") == 0);
  fail_unless(pInfo->s_haveLowLimit);
  fail_unless(pInfo->s_lowLimit == -1.0);
  fail_unless(pInfo->s_haveHiLimit);
  fail_unless(pInfo->s_hiLimit = 1.0);

  spectcl_free_parameter_list(pList);

}
END_TEST

/*
** Should be able to insert a parametr with NULL fields where appropriate and see them as 
** nulls when querying about them.
*/
START_TEST(test_insertnulls)
{
  parameter_list pList;
  pParameterInfo pInfo;

  spectcl_parameter_create(db, "test.parameter", NULL, NULL, NULL);

  pList = spectcl_parameter_list(db, "test.parameter");
  fail_if(pList == NULL);
  pInfo = *pList;
  fail_if(pInfo == NULL);

  fail_unless(pInfo->s_pUnits == NULL);
  fail_if(pInfo->s_haveLowLimit);
  fail_if(pInfo->s_haveHiLimit);

}
END_TEST

/**
 ** Attempting to insert a duplicate parameter should fail.
 */
START_TEST(test_dup_insertion)
{
  double low = -1.0;
  double hi  = 1.0;
  spectcl_parameter_create(db, 
			   "test.parameter",
			   "mm", &low, &hi);
  fail_unless(spectcl_parameter_create(db, "test.parameter", NULL, NULL, NULL) == SPEXP_EXISTS);
}
END_TEST


/*
** Attempting to alter a nonexistet parameter should be doomed to fail with SPEXP_NOSUCH.
*/
START_TEST(test_nosuch_alter)
{
  fail_unless(spectcl_parameter_alter(db, "test.parameter", NULL, NULL) == SPEXP_NOSUCH);
}
END_TEST


/**
 ** Having made a new parameter we should be able to alter it and see the alteration take effect
 */
START_TEST(test_good_alter)
{
  parameter_list pList;
  pParameterInfo pInfo;
  double         hi = 1.0;
  double         lo = -1.0;
  int            status;

  spectcl_parameter_create(db, "test.parameter", NULL, NULL, NULL);

  fail_unless(spectcl_parameter_alter(db, "test.parameter", &lo, &hi) == SPEXP_OK);

  pList = spectcl_parameter_list(db, "test.parameter");
  pInfo = *pList;
  fail_if(pInfo == NULL);
  fail_unless(pInfo->s_haveLowLimit);
  fail_unless(pInfo->s_haveHiLimit);
  fail_unless(pInfo->s_lowLimit == lo);
  fail_unless(pInfo->s_hiLimit  == hi);

  spectcl_free_parameter_list(pList);
}
END_TEST

/**
 ** attempting to alter a parameter by id that does not exist should also fail 
*/

START_TEST(test_nosuchid_alter)
{
  fail_unless(spectcl_parameter_alter_byid(db, 1, NULL, NULL) == SPEXP_NOSUCH);
}
END_TEST
/*
** Adding a parameter, getting its id and doing an alter by that id should work.
*/
START_TEST(test_goodid_alter)
{
  parameter_list pList;
  double         hi = 1.0;
  double         lo = -1.0;
  int status;
  int id;

  spectcl_parameter_create(db, "test.parameter", "arbitrary", &lo, &hi);
  pList  = spectcl_parameter_list(db, "test.parameter");
  id = pList[0]->s_id;
  spectcl_free_parameter_list(pList);
  
  status  = spectcl_parameter_alter_byid(db, id, NULL, NULL);
  pList   = spectcl_parameter_list(db, "test.parameter");

  fail_unless(status == SPEXP_OK);
  fail_if(pList[0]->s_haveLowLimit);
  fail_if(pList[0]->s_haveHiLimit);
  spectcl_free_parameter_list(pList);
}
END_TEST

/**
 ** Should be able to add multiple parameters, and list selectively via pattern
 */
START_TEST(test_multipleinsert_list)
{
  double low = -1.0;
  double hi  = -1.0;
  parameter_list pList;
  pParameterInfo pInfo;
  int status;

  spectcl_parameter_create(db, "a", "mm", &low, &hi);
  spectcl_parameter_create(db, "b", NULL, NULL, NULL);
  spectcl_parameter_create(db, "c", NULL, NULL, NULL);
  
  pList = spectcl_parameter_list(db, "a*");

  fail_unless(pList != NULL);
  fail_unless(pList[0] != NULL);
  fail_if(pList[1] != NULL);		/* one match only. */
  pInfo = pList[0];

  fail_unless(strcmp("a", pInfo->s_pName) == 0);

  spectcl_free_parameter_list(pList);


}
END_TEST

/*------------------------------------ final setup ---------------------------------------------*/
int main(void) 
{
  int  failures;
  Suite *s = suite_create("parameter");
  SRunner* sr = srunner_create(s);
  TCase* tc_experiment = tcase_create("parameter");

  tcase_add_test(tc_experiment, test_initiallyempty);
  tcase_add_test(tc_experiment, test_insertion);
  tcase_add_test(tc_experiment, test_insertnulls);
  tcase_add_test(tc_experiment, test_dup_insertion);

  tcase_add_test(tc_experiment, test_nosuch_alter);
  tcase_add_test(tc_experiment, test_good_alter);
  tcase_add_test(tc_experiment, test_nosuchid_alter);
  tcase_add_test(tc_experiment, test_goodid_alter);

  tcase_add_test(tc_experiment, test_multipleinsert_list);

  tcase_add_checked_fixture(tc_experiment, setup, teardown);

  suite_add_tcase(s, tc_experiment);



  /* Set up the test runner:  */

  srunner_set_fork_status(sr, CK_NOFORK);
  srunner_run_all(sr, CK_NORMAL);
  failures = srunner_ntests_failed(sr);

  srunner_free(sr);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

  return 0;
}
