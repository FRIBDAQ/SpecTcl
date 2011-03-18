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

/*----------------------------------  test cases -------------------------------------------*/

/*
** When the workspace is created it should have spectrum_types created and stocked:
*/

START_TEST(test_schema)
{
  char* description;

  description = getfirst(ws, "spectrum_types", 
			 "description", "type", "1");
  fail_if(description == NULL);
  if(description) {
    fail_unless(strcmp(description, "1-D") == 0);
  }
  free(description);
}
END_TEST
/**
 * getDescription should fail with SPEXP_NOT_WORKSPACE if the ws parameter is not a workspace.
 */
START_TEST(test_descr_notws)
{
  char* description;

  description = spectcl_workspace_getDescription(db, "1");
  fail_unless(description == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOT_WORKSPACE);

}
END_TEST

/**
 ** getDescription should fail with SPEXP_NOSUCH if there is no such
 ** spectrum type.
 */
START_TEST(test_descr_nosuch)
{
  char* description;
  description = spectcl_workspace_getDescription(ws, "no such");
  fail_unless(description == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOSUCH);

}
END_TEST

/* Ought to be able to get the descriptionfor '1' as 1-D
 */
START_TEST(test_descr_ok)
{
  char* description;

  description = spectcl_workspace_getDescription(ws, "1");
  fail_if(description == NULL);
  if (description) {
    fail_unless(strcmp(description, "1-D") == 0);
  }
}
END_TEST

/**
 ** isValidType - NOT_WORKSPACE if ws parameter is invalid.
 */
START_TEST(test_valid_notws)
{
  fail_unless(spectcl_workspace_isValidType(db, "1") == SPEXP_NOT_WORKSPACE );

}
END_TEST
/**
 *  isValidtype - NOSUCH if no such type:
 */
START_TEST(test_valid_nosuch)
{
  fail_unless(spectcl_workspace_isValidType(ws, "no such") == SPEXP_NOSUCH);
}
END_TEST
/*
** isValidtype - SPEXP_OK if there is a type like this
*/
START_TEST(test_valid_ok)
{
  fail_unless(spectcl_workspace_isValidType(ws, "1") == SPEXP_OK);
}
END_TEST
/**
 ** List for not workspace reports that
 */
START_TEST(test_list_notws)
{
  fail_unless(spectcl_workspace_spectrumTypes(db) ==NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOT_WORKSPACE);
}
END_TEST

/**
 ** Get the list of types:
 */
START_TEST(test_list_ok)
{
  spectcl_spectrum_type** types;

  types = spectcl_workspace_spectrumTypes(ws);
  fail_if(types == NULL);
  /*
  ** First one shouild be "1", "1-D", second one NULL for now.
  */
  if (types) {
    spectcl_spectrum_type* type= *types;
    fail_unless(strcmp(type->s_type, "1") == 0);
    fail_unless(strcmp(type->s_description, "1-D") == 0);

    type = types[1];
    fail_unless(type == NULL);

    /* Free storage */

    spectcl_workspace_free_typelist(types);
  }
}
END_TEST

/*
** This should just not bomb
*/
START_TEST(test_free_null)
{
  spectcl_spectrum_type** types= NULL;
  spectcl_workspace_free_typelist(types);
}
END_TEST
/*
*  experiment_spectrumTypes need experiment parameter.
*/
START_TEST(test_espectypes_notexp)
{
  spectcl_spectrum_type** pList;
  pList = spectcl_experiment_spectrumTypes(ws, NULL);
  fail_unless(pList == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOT_EXPDATABASE);

}
END_TEST

/*
** experiment_spectrumTypes must have an attached workspace.
*/

START_TEST(test_espectypes_noattach)
{
 
  fail_unless(spectcl_experiment_spectrumTypes(db, NULL) == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_UNATTACHED);
}
END_TEST
/**
 ** Get the correct set of types on a ws attached in the default place.
 */
START_TEST(test_espectypes_okdefault)
{
  spectcl_spectrum_type** types;

  spectcl_workspace_attach(db, wsName, NULL);

  types = spectcl_experiment_spectrumTypes(db, NULL);
  fail_if(types == NULL);
  /*
  ** First one shouild be "1", "1-D", second one NULL for now.
  */
  if (types) {
    spectcl_spectrum_type* type= *types;
    fail_unless(strcmp(type->s_type, "1") == 0);
    fail_unless(strcmp(type->s_description, "1-D") == 0);

    type = types[1];
    fail_unless(type == NULL);

    /* Free storage */

    spectcl_workspace_free_typelist(types);
  }
}
END_TEST

/**
 ** Get correct set of types when ws attachment point is specified.
 */
START_TEST(test_espectypes_okspecified)
{
  spectcl_spectrum_type** types;

  spectcl_workspace_attach(db, wsName, "TEST");

  types = spectcl_experiment_spectrumTypes(db, "TEST");
  fail_if(types == NULL);
  /*
  ** First one shouild be "1", "1-D", second one NULL for now.
  */
  if (types) {
    spectcl_spectrum_type* type= *types;
    fail_unless(strcmp(type->s_type, "1") == 0);
    fail_unless(strcmp(type->s_description, "1-D") == 0);

    type = types[1];
    fail_unless(type == NULL);

    /* Free storage */

    spectcl_workspace_free_typelist(types);
  }
}
END_TEST
/**
 *  experiment_isValidType should give SPEXP_NOT_EXPDATABASE
 ** if handed a different handle
 */
START_TEST(test_evalid_notexp)
{
  fail_unless(spectcl_experiment_isValidType(ws,"1",  NULL) == SPEXP_NOT_EXPDATABASE);
}
END_TEST
/*
** Fail if there's no attached workspace
*/
START_TEST(test_evalid_unattached)
{
  fail_unless(spectcl_experiment_isValidType(db, "1", NULL) == SPEXP_UNATTACHED);
}

END_TEST
/*------------------------------------ final setup ---------------------------------------------*/
int main(void) 
{
  int  failures;

  Suite* s = suite_create("spectcl_workspace_spectypes");
  Suite* se = suite_create("spectcl_experiment_spectypes");

  SRunner* sr = srunner_create(s);
  TCase*   tc_types = tcase_create("spectypes");
  TCase*   tc_exptypes = tcase_create("exp_spectypes");

  srunner_add_suite(sr, se);

  tcase_add_checked_fixture(tc_types, setup, teardown);
  suite_add_tcase(s, tc_types);

  tcase_add_checked_fixture(tc_exptypes, setup, teardown);
  suite_add_tcase(se, tc_exptypes);

  tcase_add_test(tc_types, test_schema);

  tcase_add_test(tc_types, test_descr_notws);
  tcase_add_test(tc_types, test_descr_nosuch);
  tcase_add_test(tc_types, test_descr_ok);

  tcase_add_test(tc_types, test_valid_notws);
  tcase_add_test(tc_types,  test_valid_nosuch);
  tcase_add_test(tc_types,  test_valid_ok);

  tcase_add_test(tc_types, test_list_notws);
  tcase_add_test(tc_types, test_list_ok);

  tcase_add_test(tc_types, test_free_null);

  /** Tests for experiment with workspace attached */

  tcase_add_test(tc_exptypes, test_espectypes_notexp);
  tcase_add_test(tc_exptypes, test_espectypes_noattach);
  tcase_add_test(tc_exptypes, test_espectypes_okdefault); /* default attach point. */
  tcase_add_test(tc_exptypes, test_espectypes_okspecified); /* Specified attach point */

  tcase_add_test(tc_exptypes, test_evalid_notexp);
  tcase_add_test(tc_exptypes, test_evalid_unattached);

  srunner_set_fork_status(sr, CK_NOFORK); 


  srunner_run_all(sr, CK_NORMAL);
  failures = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
