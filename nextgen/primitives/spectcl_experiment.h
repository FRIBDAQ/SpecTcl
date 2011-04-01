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



#ifndef __SPECTCL_EXPERIMENT_H
#define __SPECTCL_EXPERIMENT_H

#ifndef __UUID_H
#include <uuid/uuid.h>
#ifndef __UUID_H
#define __UUID_H
#endif
#endif

#ifndef _STDINT_H
#include <stdint.h>
#ifndef _STDINT_H
#define _STDINT_H
#endif
#endif

/**
 **  This file contains inteface functions to the SpecTcl nextgen experiment
 ** database.   See planning documentation for information about this database
 ** and its schema
 */

/**
 ** Constants:
 */
#define SCHEMA_VERSION "1.0"

/*  errno values */

#define SPEXP_OK              0
#define SPEXP_CREATE_FAILED   1	/* database creation failed. */
#define SPEXP_OPEN_FAILED     2 /* database open failed */
#define SPEXP_NOT_EXPDATABASE 3 /* Open but not an experiment database */
#define SPEXP_CLOSE_FAILED    4 /* Close failed.      */
#define SPEXP_EXISTS          5 /* Attempting create of existing object. */
#define SPEXP_NOMEM           6 /* memory allocation failed. */
#define SPEXP_SQLFAIL         7 /* SQL operation failed */
#define SPEXP_NOSUCH          8 /* No such object */
#define SPEXP_NOT_EVENTSDATABASE 9
#define SPEXP_WRONGEXPERIMENT  10
#define SPEXP_UNATTACHED       11
#define SPEXP_NOT_WORKSPACE    12
#define SPEXP_NEWVERS          13
#define SPEXP_INVTYPE          14  /* Invalid type of somwething */
#define SPEXP_BADPARAMS        15
#define SPEXP_UNIMPLEMENTED   100 /* for testing */

/*
** Database default attach points 
*/
#define EVENTS_DEFAULT_ATTACH_POINT    "EVENTS"
#define WORKSPACE_DEFAULT_ATTACH_POINT "WORKSPACE"

/**
 * Data type definitinos.
 */


#ifndef bool 
#define bool int
#endif

typedef void *spectcl_experiment; /* Handle to a spectcl experiment database. */
typedef void *spectcl_events;	  /* Handle to events database  */
typedef void *spectcl_workspace;  /* handle to a spectcl workspace database. */


/* What you get for a parameter definition */

typedef struct _parameter_info {
  unsigned int s_id;
  char*  s_pName;
  char*  s_pUnits;
  bool         s_haveLowLimit;
  double       s_lowLimit;
  bool         s_haveHiLimit;
  double       s_hiLimit;
} parameter_info;

typedef parameter_info*  pParameterInfo;
typedef parameter_info** parameter_list;


/* What you get for a run definition */

typedef struct _run_info {
  unsigned int    s_id;
  char*           s_pTitle;
  char*           s_pStartTime;
  char*           s_pEndTime;

} run_info, *pRunInfo;

typedef pRunInfo*  run_list;

typedef struct _ParameterData_ {
  uint32_t s_trigger;
  int      s_parameter;
  double   s_value;
} ParameterData, *pParameterData;

typedef enum _spectcl_storageType {
  st_static,
  st_dynamic,
} spectcl_StorageType;

typedef struct _AugmentResult_ {
  size_t              s_numParameters; 
  spectcl_StorageType s_destructMechanism;
  pParameterData      s_pData;
} AugmentResult, *pAugmentResult;

/* Informatino gotten for a spectrum type: */

typedef struct _spectcl_spectrum_type {
  char* s_type;
  char* s_description;
} spectcl_spectrum_type;

typedef  pAugmentResult   (AugmentCallback)(size_t, pParameterData, void*);

typedef struct _spectrum_parameter {
   char*       s_name;
   int         s_dimension;
} spectrum_parameter;

typedef struct _spectrum_definition  {
  int    s_id;
  char*  s_name;
  int    s_version;
  char*  s_timestamp;
  spectrum_parameter** s_parameters;
} spectrum_definition;

/**
 ** Function definitions; and globals:
 */
#ifdef __cplusplus
extern "C" {
#endif

  extern int spectcl_experiment_errno;

  /* functions that operate at the level of an entire database: */

  spectcl_experiment spectcl_experiment_create(const char* new_database);
  spectcl_experiment spectcl_experiment_open(const char*   database);
  int                spectcl_experiment_close(spectcl_experiment handle);
  /** TODO:  uncomment and implement */
  /*  spectcl_experiment spectcl_experiment_copy(const char*   old_database, const char* new_database); */

  /* Functions that operate on the parameter table */

  int spectcl_parameter_create(spectcl_experiment experiment,
			       const char* name, const char* units,
			       const double* low,  const double* hi);
  
  parameter_list spectcl_parameter_list(spectcl_experiment experiment,
					const char* pattern);
  int spectcl_parameter_alter(spectcl_experiment experiment,
			      const char* name,
			      const double* low, const double* hi);
  int spectcl_parameter_alter_byid(spectcl_experiment experiment,
				  int id, 
				  const double* low, const double* hi);
  void spectcl_free_parameter_list(parameter_list list);


  /* Functions that operate on the Run databse:   */

  int spectcl_run_create(spectcl_experiment experiment, unsigned int run_number,
			 const char* title,
			 const time_t* startTime);
  int spectcl_run_end(spectcl_experiment experiment, unsigned run_number,
		      const time_t* endTime);
  run_list spectcl_run_list(spectcl_experiment experiment);
  void     spectcl_free_run_list(run_list list);
  void     spectcl_free_run_info(pRunInfo  pInfo); 
  
  /* Functions that operate on UUIDS */

  bool spectcl_correct_experiment(spectcl_experiment experiment, uuid_t* uuid);
  uuid_t* spectcl_experiment_uuid(spectcl_experiment experiment);

  /*  Error handling */

  const char* spectcl_experiment_error_msg(int reason);

  /* Manipulation of  events databases.  */

  spectcl_events spectcl_events_create(spectcl_experiment pExpHandle, int run , const char* path);
  spectcl_events spectcl_events_open(const char* path);
  int            spectcl_events_close(spectcl_events pExpHandle);

  /* Associating events with an open experiment database:  */

  int spectcl_events_attach(spectcl_experiment pExpHandle, const char* path, const char* name);
  int spectcl_events_detach(spectcl_experiment pExpHandle, const char* name);

  /* Loading data into the events table; */


  int spectcl_events_load(spectcl_events pEvents, size_t nParameters,  pParameterData pData);
  int spectcl_events_augment(spectcl_events pEvents, AugmentCallback* pCallback, 
			     void* pClientData);

  /* Inquiries: */

  int      spectcl_events_run(int* run, spectcl_events pHandle);
  pRunInfo spectcl_experiment_eventsrun(spectcl_experiment pHandle, const char* attachPoint);
  uuid_t*   spectcl_events_uuid(spectcl_events pHandle);

  /* Workspace functions */
  /** Generic functions on the workspace as a whole: */

  int               spectcl_workspace_create(spectcl_experiment pHandle, const char* path);
  spectcl_workspace spectcl_workspace_open(const char* path);
  int               spectcl_workspace_attach(spectcl_experiment pHandle,
					     const char*        path, const char* attachPoint);
  int               spectcl_workspace_close(spectcl_workspace   pHandle);
  int               spectcl_workspace_detach(spectcl_experiment pHandle, const char* attachPoint);

  /** Functions that operate on the configuration items table: */

  char*       spectcl_workspace_version(spectcl_workspace ws);
  uuid_t*     spectcl_workspace_uuid(spectcl_workspace ws);
  
  /** Functions used to validate the handle against various conditions */;

  int spectcl_workspace_isWorkspace(spectcl_workspace ws);
  int spectcl_workspace_isCorrectExperiment(spectcl_experiment expdb,
					    spectcl_workspace ws);

  /** Functions used to query the spectrum types table. */

  void spectcl_workspace_free_typelist(spectcl_spectrum_type** typeList);
  spectcl_spectrum_type** spectcl_workspace_spectrumTypes(spectcl_workspace ws);
  int                    spectcl_workspace_isValidType(spectcl_workspace ws,
						       const char*       type);
  char*                  spectcl_workspace_getDescription(spectcl_workspace ws,
							  const char*     type);

  /** The same functions are needed when the workspace is attached to an experiment: */

  spectcl_spectrum_type** spectcl_experiment_spectrumTypes(spectcl_experiment exp,
							    const char* attachPoint);
  int                     spectcl_experiment_isValidType(spectcl_experiment exp,
							 const char*        type,
							 const char* attachPoint);
  char*                   spectcl_experiment_getDescription(spectcl_experiment exp,
							    const char*        type,
							    const char* attachPoint);

  /** Functions used to create spectra and get their properties.  */

  int spectcl_workspace__create_spectrum(spectcl_experiment exp,
					 const char* pType,
					 const char* pName,
					 const spectrum_parameter** pParams,
					 const char* attachPoint);
  spectrum_parameter** spectcl_workspace_parameters(spectcl_workspace ws, 
						    int id);
  void spectcl_workspace_free_spectrum_parameters(spectrum_parameter** p);
  void spectcl_workspace_free_spectrum_definitions(spectrum_definition** p);
  spectrum_definition** spectcl_workspace_find_spectra(spectcl_workspace ws,
						       const char* pattern,
						       int         allVersions);
  spectrum_definition*  spectcl_workspace_spectrum_properties(spectcl_workspace ws, int id);
  
  


#ifdef __cplusplus
}
#endif


#endif

