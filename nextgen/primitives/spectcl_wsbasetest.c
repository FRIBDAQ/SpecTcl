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
static char expName[PATH_MAX];
static char wsName[PATH_MAX];
static char wsName1[PATH_MAX];

spectcl_experiment db;
spectcl_experiment db1;
spectcl_workspace  db_ws;
spectcl_workspace  db1_ws;

/*------------------------------------- fixture code ----------------------------------*/


static void setup()
{
  strcpy(dbName, tmpnam(NULL));
  db     = spectcl_experiment_create(dbName);
  strcpy(expName,tmpnam(NULL));
  strcpy(wsName, tmpnam(NULL));

 
}

static void setupws()
{
  setup();
  strcpy(wsName1, tmpnam(NULL));

  db1 = spectcl_experiment_create(expName);

  spectcl_workspace_create(db, wsName);
  db_ws = spectcl_workspace_open(wsName);

  spectcl_workspace_create(db1, wsName1);
  db1_ws = spectcl_workspace_open(wsName1);
  
  
}

static void teardown()
{
  spectcl_experiment_close(db);
  unlink(dbName);
  unlink(expName);
  unlink(wsName);
}

static void teardownws()
{
  spectcl_experiment_close(db1);
  spectcl_workspace_close(db_ws);
  spectcl_workspace_close(db1_ws);
  unlink(wsName1);

  teardown();

}

/*------------------------------------ The tests ----------------------------------------------*/

/**  Experiment database must be provided */

START_TEST(test_badexpbase)
{
  spectcl_events pEv;
  int            status;

  spectcl_run_create(db, 1, "this is a test run", NULL);
  pEv = spectcl_events_create(db, 1, expName);

  status = spectcl_workspace_create(pEv, wsName); /* wrong type of handle. */
  spectcl_events_close(pEv);

  fail_unless(status == SPEXP_NOT_EXPDATABASE);
}
END_TEST

/**
 ** A valid writable path must be used for the workspace.
 */
START_TEST(test_badpath)
{
  int status;
  status = spectcl_workspace_create(db, "/this/path/does/not/exist");
  fail_unless(status == SPEXP_CREATE_FAILED);

}
END_TEST

/** Check that we can create the database..and that the file exists after the create
 */
START_TEST(test_okcreate)
{
  int status = spectcl_workspace_create(db, wsName);
  fail_unless(status == SPEXP_OK);
  status     = access(wsName, R_OK | W_OK);
  fail_if(status != 0);
  
}
END_TEST

/** After a good creation, we should have a table named 'configuration_values' with a 
 ** config_item = 'type' and its config_value = 'workspace'
 ** config_item = 'uuid' and its config_value = the uuid of the experiment database.
 */
START_TEST(test_createschema)
{
  sqlite3* ws;
  const char* query = "SELECT config_value FROM configuration_values WHERE config_item = :item";
  int status = spectcl_workspace_create(db, wsName);

  status = sqlite3_open_v2(wsName, &ws, 
			   SQLITE_OPEN_READWRITE, NULL);
  char* type = getfirst(ws, "configuration_values", "config_value", "config_item", "type");
  char* uuid = getfirst(ws, "configuration_values", "config_value", "config_item", "uuid");
  char* expuuid = getfirst(db, "configuration_values", "config_value", "config_item", "uuid");

  fail_if(type == NULL);
  fail_if(uuid == NULL);

  if (type && uuid) {
    fail_unless(strcmp(type, "workspace") == 0);
    fail_unless(strcmp(uuid, expuuid) ==  0);
    free(type);
    free(uuid);
    free(expuuid);
  }
  
  spectcl_workspace_close(ws);


  

}
END_TEST
/**
 ** Opening a nonexistent workspace should result in a null workdspace
 */
START_TEST(test_opennosuch)
{
  spectcl_workspace ws = spectcl_workspace_open("/no/such/file/exists/here");
  fail_unless(ws == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_OPEN_FAILED);
}
END_TEST

/**
 ** Opening the experiment database as a workspace is bound to fail.
 */
START_TEST(test_notws)
{
  spectcl_workspace ws = spectcl_workspace_open(dbName);
  fail_unless(ws == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOT_WORKSPACE);
}
END_TEST

/**
 ** If I create a workspace I should then be able to open it and get a workspace
 ** handle that's not null.
 */
START_TEST(test_openok)
{
  spectcl_workspace  ws;
  spectcl_workspace_create(db, wsName);
  ws = spectcl_workspace_open(wsName); /* ought to work. */
  fail_if(ws == NULL);

  /* If not null we should close it but we've not tested close yet so just do an
     sqlite close */

  if (ws) {
    spectcl_workspace_close(ws);
  }
}
END_TEST


/**
 ** Attaching to something that is not an experiment should
 ** fail with SPEXP_NOT_EXPDATABASE
 */
START_TEST(test_notexp)
{
  spectcl_events pEv;
  int            status;

  spectcl_run_create(db, 1, "this is a test run", NULL);
  pEv = spectcl_events_create(db, 1, expName);

  status = spectcl_workspace_attach(pEv, wsName, NULL);
  spectcl_events_close(pEv);

  fail_unless(status == SPEXP_NOT_EXPDATABASE);
  

}
END_TEST
/**
 ** The attach needs a workspace:
 */
START_TEST(test_attachnotws)
{
  int status;

  status = spectcl_workspace_attach (db, wsName, NULL); /* Ws doesn't exist. */
  fail_unless(status == SPEXP_OPEN_FAILED);
}
END_TEST

/**
 ** If the experiment we attach to is not the right one we complain.
 */
START_TEST(test_wrongexp)
{
  spectcl_experiment exp;
  int                status;
  spectcl_workspace_create(db, wsName); /* Make the workspace on this experiment. */
  exp = spectcl_experiment_create(expName); /* Create another experiment */
  status = spectcl_workspace_attach(exp, wsName, NULL);
  spectcl_experiment_close(exp);

  fail_unless(status == SPEXP_WRONGEXPERIMENT);

  
}
END_TEST

/**
 ** Should be able to attach to the default location and read configuration
 ** table entries from WORKSPACE.configuration_values
 **
 */
START_TEST(test_attachdefault)
{
  int   status;
  char* value;
  /* Create the workspace: */
  
  spectcl_workspace_create(db, wsName);
  
  /* Attach */
 
  status = spectcl_workspace_attach(db, wsName, NULL);
  value = getfirst(db, "WORKSPACE.configuration_values",
		   "config_value", "config_item", "type");
  if (value) {
    fail_unless(strcmp(value, "workspace") == 0);
    free(value);
  }
  else {
    fail();
  }
  
  
}
END_TEST

/**
 ** Test attachment on a non default point.
 */
START_TEST(test_attachspecific)
{
  int   status;
  char* value;
  /* Create the workspace: */
  
  spectcl_workspace_create(db, wsName);
  
  /* Attach */
 
  status = spectcl_workspace_attach(db, wsName, "WS");
  value = getfirst(db, "WS.configuration_values",
		   "config_value", "config_item", "type");
  if (value) {
    fail_unless(strcmp(value, "workspace") == 0);
    free(value);
  }
  else {
    fail();
  }
}
END_TEST

/**
 ** When you call close you better hand me a workspace
 */
START_TEST(test_close_notws)
{
  int status = spectcl_workspace_close(db); /* This is not a workspace!! */
  fail_unless(status == SPEXP_NOT_WORKSPACE);
}
END_TEST


/*
** Calling a good close should give SPEXP_OK
*/
START_TEST(test_close_ok)
{
  int status;
  spectcl_workspace ws;
  spectcl_workspace_create(db, wsName);

  ws = spectcl_workspace_open(wsName);

  status = spectcl_workspace_close(ws);
  fail_unless(status == SPEXP_OK);

}
END_TEST

/**
 ** the 'base' database for a detach must be an experiment database.
 **/
START_TEST(test_detach_notexp)
{
  int status;
  spectcl_workspace ws;
  spectcl_workspace_create(db, wsName);
  ws = spectcl_workspace_open(wsName);

  status = spectcl_workspace_detach(ws, NULL);
  spectcl_workspace_close(ws);
  fail_unless(status == SPEXP_NOT_EXPDATABASE);


}
END_TEST

/**
 ** The attach point must be a workspace.
 */
START_TEST(test_detach_notws)
{

  int                status;
  spectcl_events     pEv;

  spectcl_run_create(db, 1, "this is a test run", NULL);
  pEv = spectcl_events_create(db, 1, expName);
  spectcl_events_close(pEv);
  spectcl_events_attach(db, expName, "WORKSPACE");

  status = spectcl_workspace_detach(db, NULL);

  fail_unless(status == SPEXP_NOT_WORKSPACE);

}
END_TEST

/**
 ** Should be able to detach a workspace attached on the default
 ** attach point
 */
START_TEST(test_detachdefault)
{

  int               status;

  spectcl_workspace_create(db, wsName);
  spectcl_workspace_attach(db, wsName, NULL);
  status = spectcl_workspace_detach(db, NULL);
  fail_unless(status == SPEXP_OK);
}
END_TEST


/**
 ** If we ask for the version of an experiment workspace,
 ** we should not get it and spectcl_experiment_errno = SPEXP_NOT_WORKSPACE
 */
START_TEST(test_vsn_notws)
{
  char* vsn = spectcl_workspace_version(db);
  fail_if(vsn != NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOT_WORKSPACE);
}
END_TEST
/**
 ** Workspace versions should be SCHEMA_VERSION
 */
START_TEST(test_vsn_ok)
{
  char* vsn;
  spectcl_workspace ws;

  spectcl_workspace_create(db, wsName);
  

  ws = spectcl_workspace_open(wsName);
  vsn = spectcl_workspace_version(ws);
  spectcl_workspace_close(ws);

  fail_if(vsn == NULL);
  fail_unless(strcmp(vsn, SCHEMA_VERSION) == 0);
  free(vsn);

}
END_TEST
/**
 ** uuid but not a workspace should give me NULL and 
 ** errno of SPEXP_NOT_WORKSPACE.
 */
START_TEST(test_uuid_notws)
{
  uuid_t* uuid = spectcl_workspace_uuid(db); /* not a worksapce. */
  fail_if(uuid != NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOT_WORKSPACE);
}
END_TEST
/**
 ** UUID should be the same as the one used to create the experiment.
 */
START_TEST(test_uuid_ok)
{
  uuid_t* wsUuid;
  uuid_t* expUuid;
  spectcl_workspace ws;

  spectcl_workspace_create(db, wsName);
  ws  = spectcl_workspace_open(wsName);

  wsUuid = spectcl_workspace_uuid(ws);
  expUuid= spectcl_experiment_uuid(db);
  spectcl_workspace_close(ws);

  fail_if(wsUuid == NULL);
  if (wsUuid) {
    fail_if(uuid_compare(*wsUuid, *expUuid));
  }
  free(wsUuid);
  free(expUuid);

  
}
END_TEST

/*
** Passing an experiment handle into isWorkspace should give FALSE.
*/
START_TEST(test_isws_no)
{
  fail_if(spectcl_workspace_isWorkspace(db));
}
END_TEST
/**
 ** Passing a workspace handle into isWorkspace should give TRUE
 */
START_TEST(test_isws_yes)
{
  spectcl_workspace ws;

  spectcl_workspace_create(db, wsName);
  ws = spectcl_workspace_open(wsName);
  fail_unless(spectcl_workspace_isWorkspace(ws));
  spectcl_workspace_close(ws);
}
END_TEST

/**
 * isCorrectExperiment will be false if the experiment is isn't one.
 */
START_TEST(test_iscorrectexp_notexp)
{
  fail_if(spectcl_workspace_isCorrectExperiment(db_ws, db_ws));
}
END_TEST
/*
  isCorrectExperiment sb. false if the workspace isn't wone.
 */
START_TEST(test_iscorrectexp_notws)
{
  fail_if(spectcl_workspace_isCorrectExperiment(db, db));
}
END_TEST

/*
  isCorrectExperiment sb false if the workspace doesn't match the experiment
*/
START_TEST(test_iscorrectexp_no)
{
  fail_if(spectcl_workspace_isCorrectExperiment(db, db1_ws));
  fail_if(spectcl_workspace_isCorrectExperiment(db1, db_ws));
}
END_TEST
/*
   isCorrectExperiment sb true if stuff matches:
*/
START_TEST(test_iscorrectexp_yes)
{
  fail_unless(spectcl_workspace_isCorrectExperiment(db, db_ws));
  fail_unless(spectcl_workspace_isCorrectExperiment(db1, db1_ws));
}
END_TEST

/*------------------------------------ final setup ---------------------------------------------*/
int main(void) 
{
  int  failures;

  Suite* s = suite_create("spectcl_workspace_base");

  SRunner* sr = srunner_create(s);

  TCase*   tc_base = tcase_create("base");
  TCase*   tc_basews = tcase_create("basews"); /* tests that need a bit more setup. */


  tcase_add_checked_fixture(tc_base, setup, teardown);
  tcase_add_checked_fixture(tc_basews, setupws, teardownws);

  suite_add_tcase(s, tc_base);

  tcase_add_test(tc_base, test_badexpbase);
  tcase_add_test(tc_base, test_badpath);
  tcase_add_test(tc_base, test_okcreate);
  tcase_add_test(tc_base, test_createschema);

  tcase_add_test(tc_base, test_opennosuch);
  tcase_add_test(tc_base, test_notws);
  tcase_add_test(tc_base, test_openok);

  tcase_add_test(tc_base, test_notexp);
  tcase_add_test(tc_base, test_attachnotws);
  tcase_add_test(tc_base, test_wrongexp);
  tcase_add_test(tc_base, test_attachdefault);
  tcase_add_test(tc_base, test_attachspecific);

  tcase_add_test(tc_base, test_close_notws);
  tcase_add_test(tc_base, test_close_ok);

  tcase_add_test(tc_base, test_detach_notexp);
  tcase_add_test(tc_base, test_detach_notws);
  tcase_add_test(tc_base, test_detachdefault);

  tcase_add_test(tc_base, test_vsn_notws);
  tcase_add_test(tc_base, test_vsn_ok);

  tcase_add_test(tc_base, test_uuid_notws);
  tcase_add_test(tc_base, test_uuid_ok);

  tcase_add_test(tc_base, test_isws_no);
  tcase_add_test(tc_base, test_isws_yes);

  tcase_add_test(tc_basews, test_iscorrectexp_notexp);
  tcase_add_test(tc_basews, test_iscorrectexp_notws);
  tcase_add_test(tc_basews, test_iscorrectexp_no);
  tcase_add_test(tc_basews, test_iscorrectexp_yes);


  srunner_set_fork_status(sr, CK_NOFORK); /*  /* Uncomment for gdb */

  
  srunner_run_all(sr, CK_NORMAL);
  failures = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
