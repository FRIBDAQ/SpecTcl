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
spectcl_experiment db;


/*------------------------------------- fixture code ----------------------------------*/


static void setup()
{
  strcpy(dbName, tmpnam(NULL));
  db     = spectcl_experiment_create(dbName);
  strcpy(expName,tmpnam(NULL));

  spectcl_run_create(db,1, "this is a test run", NULL); /* Run we'll hang events onn. */
 
}
static void teardown()
{
  spectcl_experiment_close(db);
  unlink(dbName);
  unlink(expName);
}


/*---------------------------------- The tests -----------------------------------------*/

/**
 ** Error checking and basic creation.
 */

/*  The run must be an existing  run number in the database */

START_TEST(test_runexists)
{
  spectcl_events pHandle;

  pHandle = spectcl_events_create(db, 2, expName);
  fail_unless(pHandle == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOSUCH);

}
END_TEST

/*  The events data base cannot be an existing file */

START_TEST(test_eventsnooverwrite)
{
  spectcl_events pHandle;

  pHandle = spectcl_events_create(db, 1, dbName);

  fail_unless(pHandle == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_EXISTS);
}
END_TEST

/*   The events databasefile  must exist after creation */
START_TEST(test_dbexists)
{
  spectcl_events pHandle;
  int            status;

  pHandle  = spectcl_events_create(db, 1, expName);
  
  fail_if(pHandle == NULL);
  status = access(dbName, F_OK);
  fail_unless(status == 0);

}
END_TEST

/*  Test can't create database (bad filename or not writeable) */

START_TEST(test_badcreate)
{
  spectcl_events pHandle;
  pHandle = spectcl_events_create(db, 1, "/this/path/doesnot/exist.db");
  
  fail_unless(pHandle == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_CREATE_FAILED);

}
END_TEST

/** Schema tests
 **
 */

/* Config table should exist and:
** type = run-data
** run  = Run number we passed in.
** uuid = experiment's uuid.
*/
START_TEST(test_config)
{
  char*          pRun;
  char*          pType;
  char*          pUuid;
  uuid_t         uuid;
  spectcl_events pHandle;
  pHandle = spectcl_events_create(db, 1, expName);

  pRun = getfirst(pHandle, "configuration_values", "config_value", "config_item", "run");
  pType= getfirst(pHandle, "configuration_values", "config_value", "config_item", "type");
  pUuid= getfirst(pHandle, "configuration_values", "config_value", "config_item", "uuid");

  /* first of all the retrievs should succed: */

  fail_unless(pRun != NULL);
  fail_unless(pType!= NULL);
  fail_unless(pUuid!= NULL);

  if (!(pRun && pType &&  pUuid)) return;

  /* And the values should be as expected */

  fail_unless(strcmp(pRun, "1") == 0);
  fail_unless(strcmp(pType, "run-data") == 0);

  uuid_parse(pUuid, uuid);
  fail_unless(spectcl_correct_experiment(pHandle, &uuid));

  
  free(pRun);
  free(pType);
  free(pUuid);

}
END_TEST

/* There should be an empty events table.
 */
START_TEST(test_events)
{
  spectcl_events pHandle = spectcl_events_create(db, 1, expName);
  const char*    sql     = "SELECT COUNT(*) FROM events";
  sqlite3_stmt*  query;
  int            status;

  status = sqlite3_prepare_v2(pHandle, sql, -1, &query, NULL);
  fail_unless(status == SQLITE_OK);
  fail_if(sqlite3_step(query) != SQLITE_ROW);
  fail_unless(sqlite3_column_int(query, 0) == 0);
  sqlite3_finalize(query);
  
}
END_TEST

/*----------------------- tests for spectcl_events_open ---------*/

static void opensetup()
{
  spectcl_events     eventsdb;
  setup();			/* Make an experiment database and events database name. */
  eventsdb = spectcl_events_create(db, 1, expName);
  sqlite3_close(eventsdb);

}
static void openteardown()
{
  teardown();
}

/**
 ** Opening a non-existent database file should fail in a predicatable manner.
 */
START_TEST(test_nosuch)
{
  spectcl_events edb = spectcl_events_open("/no/such/file/as/this.db");
  fail_unless(edb == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_OPEN_FAILED);
}
END_TEST
/**
 ** Opening a database that is not an events database shouild also fail ina predictable
 ** manner
 */
START_TEST(test_notevents)
{
  spectcl_events edb = spectcl_events_open(dbName); /* experiment db not events */
  fail_unless(edb == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOT_EVENTSDATABASE);

}
END_TEST
/*
** Should be able to open the database we created in the fixture.
*/
START_TEST(test_openok)
{
  spectcl_events edb  = spectcl_events_open(expName);
  fail_if(edb == NULL);
  sqlite3_close(edb);
}
END_TEST
/*---------------------------------- attach tests -------------------------------------------*/

/**
 ** no such database file should fail in a predictable way.
 */
START_TEST(test_attach_nofile)
{
  int status = spectcl_events_attach(db, "/no/such/database.db", NULL);
  fail_unless(status == SPEXP_OPEN_FAILED);
}
END_TEST
/**
 ** If the database is not an experiment database that should also fail predictably.
 */
START_TEST(test_attach_notevents)
{
  fail_unless(spectcl_events_attach(db, dbName, NULL) == SPEXP_NOT_EVENTSDATABASE);
}
END_TEST
/**
 ** If the database was created on the wrong experiment that's an error too.
 */
START_TEST(test_attach_wrongexp)
{
  char* wrongExp           = tmpnam(NULL);
  spectcl_experiment wrong = spectcl_experiment_create(wrongExp);

  int status               = spectcl_events_attach(wrong, expName, NULL);
  
  fail_unless(status == SPEXP_WRONGEXPERIMENT);
  spectcl_experiment_close(wrong);
 
			
  unlink(wrongExp);
}
END_TEST

/**
** Attaching as the default database 
** - should succeed.
** - should allow us to fetch the type from EVENTS.configuration_values"
**   and it should be "run-data".
*/
START_TEST(test_attach_default)
{
  int   status;
  char* pType;

  status = spectcl_events_attach(db, expName, NULL);
  fail_unless(status == SPEXP_OK);
  
  pType = getfirst(db, "EVENTS.configuration_values",
		   "config_value", "config_item", "type");
  fail_if(pType == NULL);
  if (pType) fail_unless(strcmp(pType, "run-data") == 0);

  
}
END_TEST
/**
 ** Should be able to specify the attachment 'point'
 */
START_TEST(test_attach_specified)
{
  int   status;
  char* pType;

  status = spectcl_events_attach(db, expName, "WonkyName");
  fail_unless(status == SPEXP_OK);
  
  pType = getfirst(db, "WonkyName.configuration_values",
		   "config_value", "config_item", "type");
  fail_if(pType == NULL);
  if (pType) fail_unless(strcmp(pType, "run-data") == 0);}
END_TEST

/**
 ** Should not be able to detach unless attached <default database attach point>
 */
START_TEST(test_detach_none1)
{
  fail_unless(spectcl_events_detach(db, NULL) == SPEXP_NOT_EVENTSDATABASE);
}
END_TEST
/*
** Same as above with attach poin given but wrong.
*/
START_TEST(test_detach_none2)
{
  spectcl_events_attach(db, expName, NULL);
  fail_unless(spectcl_events_detach(db, "NoSuchDatabaseHere") == SPEXP_NOT_EVENTSDATABASE);
}
END_TEST
/**
 ** A default attach should default detach:
 */
START_TEST(test_detach_ok1)
{
  char* pType;

  spectcl_events_attach(db, expName, NULL);
  fail_unless(spectcl_events_detach(db, NULL) == SPEXP_OK);

  /* Once this is successful, I should not be able to get EVENTS.configuration_values data */

  pType = getfirst(db, "EVENTS.configuration_values", "config_value", "config_item", "type");
  fail_if(pType != NULL);

}
END_TEST
/**
 ** Should also be able to detach using a specified name
 */
START_TEST(test_detach_ok2)
{
  char* pType;

  spectcl_events_attach(db, expName, "Squirrel");
  fail_unless(spectcl_events_detach(db, "Squirrel") == SPEXP_OK);

  /* Once this is successful, I should not be able to get EVENTS.configuration_values data */

  pType = getfirst(db, "Squirrel.configuration_values", "config_value", "config_item", "type");
  fail_if(pType != NULL);
}
END_TEST

/*----------------------- Tests for the load functions -------------------------------------*/

static spectcl_events pEvents;
static void loadsetup()
{
  opensetup();
  pEvents = spectcl_events_open(expName);
  
}

static void loadteardown()
{
  spectcl_events_close(pEvents);
  openteardown();
}
/*
** Can't insert unless the database is an events database
*/
START_TEST(test_load_notexpdb)
{
  fail_unless(spectcl_events_load(db, 0, NULL) == SPEXP_NOT_EVENTSDATABASE);
}
END_TEST
/*
** It's always ok to add 0 parameters
*/
START_TEST(test_load_zerook)
{
  fail_if(spectcl_events_load(pEvents, 0, NULL) != SPEXP_OK);
}
END_TEST
/*  
 * Should be able to load an event and get it back out again.
 */

int getEvent(pParameterData p, spectcl_events db, int trigger, int param)
{	
  /* Utility to get a trigger/param/value triplet */

  int           status;
  sqlite3_stmt* statement;
  const char*   sql = "SELECT trigger, param_id, value \
                              FROM events WHERE trigger = :trigger \
                                          AND   param_id= :param";
  sqlite3_prepare_v2(db,
		     sql,
		     -1, &statement, NULL);
  sqlite3_bind_int(statement, 1, trigger);
  sqlite3_bind_int(statement, 2, param);
  status = sqlite3_step(statement);
  if (status == SQLITE_ROW) {
    p->s_trigger = sqlite3_column_int(statement, 0);
    p->s_parameter= sqlite3_column_int(statement, 1);
    p->s_value    = sqlite3_column_double(statement, 2);
    return 0;
  }
  else {
    return -1;			/* no data. */
  }
}

START_TEST(test_load_one)
{/*
 ** Data for a single trigger three parameters 1,5,7 value -1.0 0.0 1.0
 */
  ParameterData data[] = {
    {1, 1, -1.0},
    {1, 5, 0.0},
    {1, 7, 1.0}
  };
  ParameterData item;
  int status;

  status = spectcl_events_load(pEvents, 3, data);
  fail_unless(status == SPEXP_OK);
  status = getEvent(&item, pEvents, 1,1); /* parameter 1. */
  fail_if(status);
  fail_unless(item.s_trigger == 1);
  fail_unless(item.s_parameter == 1);
  fail_unless(item.s_value  == -1.0);

  status =  getEvent(&item, pEvents, 1, 5); /* paramater 5 */
  fail_if(status);
  fail_unless(item.s_trigger == 1);
  fail_unless(item.s_parameter == 5);
  fail_unless(item.s_value  == 0.0);

  status = getEvent(&item, pEvents,1, 7); /* parameter 7 */
  fail_unless(item.s_trigger == 1);
  fail_unless(item.s_parameter == 7);
  fail_unless(item.s_value  == 1.0);

}
END_TEST
/**
 ** Load several triggers
 */
START_TEST(test_load_multiple)
{
  ParameterData data[] = {
    {1, 1, -1.0},		/* first event */
    {1, 5, 0.0},
    {1, 7, 1.0},

    {2, 2, 1.0}, {2,6,5.0},	/* second event. */
  };
  ParameterData item;
  int status;
  int i;

  status = spectcl_events_load(pEvents, 5, data);
  fail_unless(status == SPEXP_OK);

  for (i =0; i < 5; i++) {
    status = getEvent(&item, pEvents, data[i].s_trigger, data[i].s_parameter);
    fail_unless(item.s_trigger == data[i].s_trigger );
    fail_unless(item.s_parameter == data[i].s_parameter);
    fail_unless(item.s_value  == data[i].s_value);
  }
}
END_TEST
/*----------------------- Tests of the event augmentation api ---------------------------------*/
static void augsetup()
{
  /* The parameter data provides the three cases:
  ** first event, middle event and last event
  */
  ParameterData data[] = {
    {1, 1, -1.0},		/* first event */
    {1, 5, 0.0},
    {1, 7, 1.0},

    {2, 1, 1.0}, {2,6,5.0},	/* second event. */

    {3, 1, 1.0}, {3, 7, 3.1416}	/* 3'd event */
  };

  loadsetup();

  // Add some events to the events file: 
  
  spectcl_events_load(pEvents, 7, data);

}
static void augteardown()
{
  loadteardown();
}
/* Bad database should fail predictably */

START_TEST(test_augment_baddb) 
{
  fail_unless(spectcl_events_augment(db, NULL, NULL) == SPEXP_NOT_EVENTSDATABASE);
}
END_TEST
/*  The number of times we are called back should equal the trigger count. (3) */

static  int cbcount = 0;
static AugmentResult empty = {
  0, st_static, NULL
};

static pAugmentResult
countCB(size_t nParameters, pParameterData pData, void* cbData)
{
  cbcount++;
  return &empty;
}

START_TEST(test_augment_count)
{
  int status;
  cbcount = 0;
  status = spectcl_events_augment(pEvents, countCB, NULL);
  fail_unless(status == SPEXP_OK);
  fail_unless(cbcount == 3);	/* Number of events in the test data. */
}
END_TEST

/*
 * Full test of augment, each event has parameter 1, we'll multiply it's value
 * by our parameter and set it as parameter 10 which is not yet used.
 */

static ParameterData pseudo = {
  0, 10, 0.0			/* The parameter number is pre-set just add trigger/value. */
};
static AugmentResult one = {
  1, st_static, &pseudo
};

static pAugmentResult newMult(size_t nParams, pParameterData pData, void* cbData)
{
  int i;
  double multiplier = *(double*)cbData; /* safest way to pass the double is by pointer */

  for (i=0; i < nParams; i++) {
    if (pData[i].s_parameter == 1) {
      pseudo.s_trigger = pData[i].s_trigger;
      pseudo.s_value   = multiplier * pData[i].s_value;
      return &one;
    }
  }
}

START_TEST(test_augment_newparams)
{
  int           status;
  ParameterData item;
  double        mult = 3.0;
  spectcl_events_augment(pEvents, newMult, &mult);

  /* Assume we know what the events look like in the code below (which we do). */

  /** first triggers hould have -3.0 for parameter 10. */

  status = getEvent(&item, pEvents, 1, 10);
  fail_if(status);
  fail_unless(item.s_value == -3.0);

  /* Second triggers should have 3.0 */

  status = getEvent(&item, pEvents, 2, 10);
  fail_if(status);
  fail_unless(item.s_value == 3.0);

  /* Third trigger should just have 3*3.1416 */

  status = getEvent(&item, pEvents, 3, 10);
  fail_if(status);
  fail_unless(item.s_value == 3*3.1416);

}
END_TEST
/*------------------------------------ final setup ---------------------------------------------*/
int main(void) 
{
  int  failures;
  Suite *s = suite_create("events_create");
  Suite* s1= suite_create("events_schema");
  Suite* sOpen = suite_create("events_open");
  Suite* sAttach= suite_create("events_attach");
  Suite* sLoad  = suite_create("events_load");
  Suite* sAugment=suite_create("events_augment");

  SRunner* sr = srunner_create(s);
 
  TCase* tc_experiment = tcase_create("events_database");
  TCase* tc_schema     = tcase_create("events_schema");
  TCase* tc_open       = tcase_create("events_open");
  TCase* tc_attach     = tcase_create("events_attach");
  TCase* tc_load       = tcase_create("events_load");
  TCase* tc_augment    = tcase_create("evnts_augment");

  srunner_add_suite(sr, sAugment);
  srunner_add_suite(sr, s1);
  srunner_add_suite(sr, sOpen);
  srunner_add_suite(sr, sAttach);
  srunner_add_suite(sr, sLoad);

  tcase_add_checked_fixture(tc_experiment, setup, teardown);
  tcase_add_checked_fixture(tc_schema, setup, teardown);
  tcase_add_checked_fixture(tc_open,   opensetup, openteardown);
  tcase_add_checked_fixture(tc_attach, opensetup, openteardown);
  tcase_add_checked_fixture(tc_load, loadsetup, loadteardown);
  tcase_add_checked_fixture(tc_augment, augsetup, augteardown);


  suite_add_tcase(s,     tc_experiment);
  suite_add_tcase(s1,    tc_schema);
  suite_add_tcase(sOpen, tc_open);
  suite_add_tcase(sAttach, tc_attach);
  suite_add_tcase(sLoad, tc_load);
  suite_add_tcase(sAugment, tc_augment);

  tcase_add_test(tc_experiment, test_runexists);
  tcase_add_test(tc_experiment, test_eventsnooverwrite);
  tcase_add_test(tc_experiment, test_dbexists);
  tcase_add_test(tc_experiment, test_badcreate);

  tcase_add_test(tc_schema, test_config);
  tcase_add_test(tc_schema, test_events);

  tcase_add_test(tc_open, test_nosuch);
  tcase_add_test(tc_open, test_notevents);
  tcase_add_test(tc_open, test_openok);

  tcase_add_test(tc_attach, test_attach_nofile);
  tcase_add_test(tc_attach, test_attach_notevents);
  tcase_add_test(tc_attach, test_attach_wrongexp);
  tcase_add_test(tc_attach, test_attach_default);
  tcase_add_test(tc_attach, test_attach_specified);
  tcase_add_test(tc_attach, test_detach_none1);
  tcase_add_test(tc_attach, test_detach_none2);
  tcase_add_test(tc_attach, test_detach_ok1);
  tcase_add_test(tc_attach, test_detach_ok2);

  tcase_add_test(tc_load,   test_load_notexpdb);
  tcase_add_test(tc_load,   test_load_zerook);
  tcase_add_test(tc_load,   test_load_one);
  tcase_add_test(tc_load,   test_load_multiple);

  tcase_add_test(tc_augment, test_augment_baddb);
  tcase_add_test(tc_augment, test_augment_count);
  tcase_add_test(tc_augment, test_augment_newparams);

  /* Set up the test runner:  */

  srunner_set_fork_status(sr, CK_NOFORK);

  srunner_run_all(sr, CK_NORMAL);
  failures = srunner_ntests_failed(sr);

  srunner_free(sr);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

  return 0;
}
