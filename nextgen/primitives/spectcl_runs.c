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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "spectcl_experimentInternal.h"
#include <unistd.h>

#ifndef TRUE
#define TRUE (0==0)
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif

/*-------------------------------------------------------------------------------------------
  static utility functinos.
  --------------------------------------------------------------------------------------------*/

/**
 ** Check to see if a run already exists
 ** @param db   -Experiment database.
 ** @param run  -Run number to check.
 ** @return int
 ** @retval TRUE if run already exists FALSE otherwise.
 **
 */
static int
haveRun(sqlite3* db, unsigned int run)
{
  const char*   pCheckSql = "SELECT COUNT(*) FROM runs WHERE id = :run_number";
  sqlite3_stmt* stmt;
  int           count;

  sqlite3_prepare_v2(db,
		     pCheckSql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, run);

  sqlite3_step(stmt);
  count = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);

  return (count != 0);
  
}
/**
 ** free a run_item
 ** @param p - Run itemt to free.
 */
static void
freeRunItem(pRunInfo p)
{
  /*  Free the string fields.. mandatory ones require no check */

  free(p->s_pTitle);
  free(p->s_pStartTime);
  
  /* Free the optional fields */

  if(p->s_pEndTime)  free(p->s_pEndTime);
  free(p);
}
/*------------------------------------------------------------------------------------
 Public entry points.
 ----------------------------------------------------------------------------------*/

/**
 ** List the set of runs that have been defined for this experiment.
 ** At this time it is not possible to selectively list the runs.  The output,
 ** however will be sorted by run number (run id).
 **
 ** @param db   - Experiment database.
 ** @return run_list
 ** @retval NULL   - Request failed, spectcl_experiment_errno contains the reason for the failure.
 ** @retval non-null - run_list as described in spectcl_experiment.h
 **           
 */
run_list spectcl_run_list(spectcl_experiment db)
{
  run_list         result;
  int              items = 0;
  pRunInfo         pARun;
  const char*      sql = "SELECT * FROM runs ORDER BY id ASC";

  sqlite3_stmt*    stmt;
 

  /*
  ** Query the data:
  */
  int status = sqlite3_prepare_v2(db, 
				  sql,
				  -1, &stmt, NULL);
  if (status != SQLITE_OK) {
    spectcl_experiment_errno = SPEXP_SQLFAIL;
    return NULL;
  }

  /** 
   ** First build an empty run list..
   */
  result  = malloc(sizeof(pRunInfo));
  if(result) {
    *result = NULL;
  }
  else {
    spectcl_experiment_errno = SPEXP_NOMEM;
    return NULL;

  }
  /** Fetch the data an item at a time:
   */
  while((status=sqlite3_step(stmt)) == SQLITE_ROW) {
    pARun =  marshallRunInfo(stmt);
    if (!pARun) {
      spectcl_free_run_list(result);
      spectcl_experiment_errno = SPEXP_NOMEM;
      sqlite3_finalize(stmt);
      return NULL;
    }
    result[items] = pARun;
    items++;
    result = realloc(result, sizeof(pRunInfo)*(items+1));
    if (!result) {
      spectcl_experiment_errno = SPEXP_NOMEM;
      sqlite3_finalize(stmt);
      return NULL;
    }
    result[items] = NULL;
		    
  }
  sqlite3_finalize(stmt);	/*  done with stmt one way or another */

  /*  Be sure that the end was due to being done: */

  if (status != SQLITE_DONE) {
    spectcl_free_run_list(result);
    spectcl_experiment_errno = SPEXP_SQLFAIL;

    return NULL;
  }

  return result;
}
/**
 ** Frees data returned by spectcl_run_list
 ** @param pRunList  - a run_list pointer returned by spectcl_run_list.
 **/
void
spectcl_free_run_list(run_list p)
{
  run_list pCopy = p;
  if (!p) return;		/* Nothing to free.. */

  while (*pCopy) {
    freeRunItem(*pCopy);
    pCopy++;
  }
  free(p);
 
}
/**
 ** Add a new run to the runs table.
 ** @param db    - Experiment database.
 ** @param run   - New run number (should be unique)
 ** @param title - Run Title.
 ** @param start - Run start time if NULL ctime(time()) is used.
 ** @return int
 ** @retval SPEXP_OK      - Run added correctly.
 ** @retval SPEXP_EXISTS  - The run already exists and cannot be overwritten.
 ** @retval SPEXP_NOT_EXPDATABASE - db is not an experiment database handle.
 ** @retval SPEXP_SQLFAIL - SQL operation of some sort failed.
 */
int 
spectcl_run_create(spectcl_experiment db, 
		   unsigned int run,
		   const char* title,
		   const time_t* start)
{
  const char*  pInsertSql= "INSERT INTO runs ('id', 'title', 'start_time') \
                                             VALUES(:run_num, :title, :start_time)";
  sqlite3_stmt* stmt;
  time_t        theTime;
  char*         pTime;
  int           status;

  /* In order to give the time test the best chance to work, get now immediately if not supplied */

  if (start) {
    theTime = *start;
  }
  else {
    theTime = time(&theTime);
  }
  pTime = copyString(ctime(&theTime));
  pTime[strlen(pTime) -1] = 0;	/* ctime puts a trailing\n */


  /* Enforce a restriction on run duplication.  */

  if (haveRun(db,run)) {
    free(pTime);
    return SPEXP_EXISTS;
  }

  /*  Build the insertion statement: */
  
  status = sqlite3_prepare_v2(db, 
			      pInsertSql, -1, &stmt, NULL);
  if (status != SQLITE_OK) {
    free(pTime);
    spectcl_experiment_errno = status;
    return SPEXP_SQLFAIL;
  }
  
  status = sqlite3_bind_int(stmt, 1,  run);
  if(status != SQLITE_OK) {
    free(pTime);
    spectcl_experiment_errno =status;
    return SPEXP_SQLFAIL;
  }

  status = sqlite3_bind_text(stmt, 2, title, -1, SQLITE_STATIC);
  if(status != SQLITE_OK) {
    free(pTime);
    spectcl_experiment_errno =status;
    return SPEXP_SQLFAIL;
  }


  status = sqlite3_bind_text(stmt, 3, pTime, -1, SQLITE_TRANSIENT);
  free(pTime);
  if(status != SQLITE_OK) {
    spectcl_experiment_errno =status;
    return SPEXP_SQLFAIL;
  }
  status = sqlite3_step(stmt);
  if(status != SQLITE_DONE) {
    spectcl_experiment_errno =status;
    return SPEXP_SQLFAIL;
  }
 
  sqlite3_finalize(stmt);

  return SPEXP_OK;
}
/**
 ** Set the end time for a run.  This is normally done by the data acquisition system
 ** at the time the run is actually ended.
 ** @param db   - Experiment database.
 ** @param run  - Number of the run to modify.
 ** @param t    - end run time....if null now is used.
 ** @return int
 ** @retval SPEXP_OK - everything worked.
 ** @retval SPEXP_NOSUCH - Run does not exist.
 ** @retval SPEXP_SQLFAIL - SQL failure of some sort.. describedin spectcl_experiment_errno.
 **
 */
int
spectcl_run_end(spectcl_experiment db, unsigned int run, const time_t* endTime)
{
  const char*    sql = "UPDATE runs SET end_time=:end_time WHERE id=:run_number";
  sqlite3_stmt*  stmt;
  int            status;
  char         Time[1000];
  time_t       now  = time(NULL);

  if (!haveRun(db, run)) {
    return SPEXP_NOSUCH;
  }

  status = sqlite3_prepare_v2(db,
			      sql, -1, &stmt, NULL);
  if(status != SQLITE_OK) {
    return SPEXP_SQLFAIL;
  }
  strcpy(Time, ctime(endTime ? endTime : &now));
  Time[strlen(Time)-1] = 0;	/* strip off ctime's \n */

  status = sqlite3_bind_text(stmt, 1, Time, -1, SQLITE_TRANSIENT);
  if(status != SQLITE_OK) {
    return SPEXP_SQLFAIL;
  }

  status = sqlite3_bind_int(stmt, 2, run);
  if(status != SQLITE_OK) {
    return SPEXP_SQLFAIL;
  }

  status = sqlite3_step(stmt);
  if(status != SQLITE_DONE) {
    return SPEXP_SQLFAIL;
  }
  sqlite3_finalize(stmt);

  return SPEXP_OK;

}
/**
 ** Free information associated with run
 ** @param pInfo  - pRunInfo dynamically allocated that will be deleted.
 */
void
spectcl_free_run_info(pRunInfo pInfo)
{
  freeRunItem(pInfo);
}
