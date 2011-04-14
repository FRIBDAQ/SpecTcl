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

#include <config.h>
#include <sqlite3.h>

#include "spectcl_experiment.h"
#include "spectcl_experimentInternal.h"


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define DEFAULT_ATTACH_POINT  WORKSPACE_DEFAULT_ATTACH_POINT

/*--------------------- Local data structures ------*/

typedef int (*validate_parameters)(const spectrum_parameter**);

typedef struct _spectrum_driver_entry {
  const char*         s_type;
  validate_parameters s_pParameterValidator;
} spectrum_driver_entry, *pspectrum_driver_entry;


typedef struct _rawparameter_info {
  int     s_id;			/* Id of parameter in definition table. */
  int     s_dimension;		/* Parameter dimension number. */
} rawparameter_info, *pRawparameter_info, **ppRawParameterInfo;


/*--------------------- 1D spectrum driver functions --------------*/
/** This section should probably be spun off into another module  */

/*
** Validate 1d parameters.  We require that:
** - There's only one parameter in the list.
** - The parameter that is present is for dimension 1
** @note The caller is assumed to already have determined that the parameter
**       name(s) actually exist.
** @param ppParams  - List of parameter definition pointers (null terminated).
** @return int
** @retval TRUE  - The parameters pass validation.
** @retval FALSE - The parameters fail validation
*/
static int 
validate1dParameters(const spectrum_parameter** ppParams)
{
  int count = 0;
  while (*ppParams) {
    if ((*ppParams)->s_dimension != 1) return FALSE; /* Bad dimension number. */
    ppParams++;
    count++;
  }
  if (count != 1) return FALSE;
  return TRUE;
}

/*
** Local data - consiting of the spectrum driver switch table.
*/
spectrum_driver_entry spectrumTypeSwitchTable[] = 
{
  {"1", validate1dParameters}
};
static const int spectrumTypeCount = 
  sizeof(spectrumTypeSwitchTable)/sizeof(spectrum_driver_entry);


/*---------------------- Private functions -------- */
/**
 ** Determine if all spectrum parameters exist for
 ** a spectrum.
 ** @param db  - Experiment database that has the
 **              parameter definitions.
 ** @param ppParams - Parameter definitions to check for.
 ** @return int
 ** @retval TRUE - All parameters are defined.
 ** @retval FALSE - At least one parameters is not defined.
 */
static int
allParametersExist(spectcl_experiment db, 
		   const spectrum_parameter** ppParams)
{
  parameter_list list;
  parameter_info* pInfo;
  while (*ppParams) {
    list = spectcl_parameter_list(db, (*ppParams)->s_name);
    if (!list) {		/* Unforeseen problem. */
      return FALSE;
    }
    pInfo = list[0];
    spectcl_free_parameter_list(list);
    if(!pInfo) {		/* No matching spectrum. */
      return FALSE;
    }
    ppParams++;
  }
  return TRUE;
}
/**
 * Validate that the parameters in a spectrum definition are
 * valid for that spectrum type.  This is done by locating
 * the appropriate spectrum driver and calling its s_pParameterValidator
 * entry.
 * The spectrum type is already supposed to have been validated, but as an
 * added safety net, not finding the driver makes the list automatically 
 * invalid.
 * @param pType    - Type string for the spectrum.
 * @param ppParams - Parameter definitions.
 * @return int
 * @retval TRUE    - Valid parameter list for spectrum type.
 * @retval FALSE   - Invalid parameter list for spectrum type.
 */
static int
validateParameters(const char* pType, const spectrum_parameter** ppParams)
{
  int                             i;
  pspectrum_driver_entry driver = NULL;
  /* locate the spectrum driver. */

  for(i =0; i < spectrumTypeCount; i++) {
    if (strcmp(pType, spectrumTypeSwitchTable[i].s_type) == 0) {
      driver = &(spectrumTypeSwitchTable[i]);
    }
  }
  if(!driver) return FALSE;

  return  (*(driver->s_pParameterValidator))(ppParams);

}
/**
 ** Release storage associated with a spectrum definition.
 ** @param pDef - Pointer to the dymamically allocated spectrum storage.
 ** @retun void 
 */
static void
freeSpectrumDefinition(spectrum_definition* pDef)
{
}
/**
 ** Locate the most recent version of a spectrum, or determine that there are
 ** no versions of the specified spectrum.
 ** @param db    - Sqlite handle to the experiment data base.
 ** @param pName - Name of the spectrum.
 ** @param atPoint - Where the spectrum database is attached to the
 **                  experiment database (used to create the table names).
 ** @return spectrum_definition*
 ** @retval NULL - pName has never been defined.
 ** @retval non-null -Spectrum definition for the most recent spectrum 
 **                   definition with that name.
 */
static spectrum_definition*
findLastDefinition(sqlite3* db, const char* pName, const char* atPoint)
{
  /* There's a bit of dirt here  the %%s.spectrum_parameters  after
     passing through the first run of qualify statement will turn into
     %s.spectrum_parameters for the next round of substitution. 
  */

  const char* sqlFormat = "SELECT * FROM %s.spectrum_definitions sd \
                                    INNER JOIN %%s.spectrum_parameters sp \
                                    ON         sd.id = sp.spectrum_id  \
                                    WHERE      sd.name  = :spname      \
                                    AND        sd.version = sp.version \
                                    ORDER BY   sd.version DESC";
  char* sql1;		/* used to build the statments.  */
  char* sql;
  sqlite3_stmt* stmt;
  int status;
  spectrum_definition* result = NULL;

  /* Build the full statment into sql */

  sql1 = spectcl_qualifyStatement(sqlFormat, atPoint);
  if (!sql1) {
    return NULL;
  }
  sql = spectcl_qualifyStatement(sql1, atPoint);
  free(sql1);
  if (!sql) {
    return NULL;
  }
  
  /* parse and bind:  */

  status = sqlite3_prepare_v2(db,
			      sql, -1, &stmt, NULL);
  if (status != SQLITE_OK) {
    return NULL;		/* Complete failure. */
  }
  status = sqlite3_bind_text(stmt, 1, pName, -1, SQLITE_STATIC);
  if(status != SQLITE_OK) {
    sqlite3_finalize(stmt);
  }

  /* First step will either be done (no matching statement)
     or will provide the spectrum definition and the first parameter definition.
  */
  status = sqlite3_step(stmt);
  if (status == SQLITE_ROW) {
    result = malloc(sizeof(spectrum_definition));
    if (!result) goto done;

  } 
 done:
  sqlite3_finalize(stmt);
  return result;
} 
/**
 ** Allocate a spectrum version number for a given spectrum name.
 ** @param db      - Experiment database
 ** @param pName   - Spectrum name for which we need a version.
 ** @param atPoint - Where the workspace is attached to the experiment.
 ** @return int 
 ** @retval The version number to use for a spectrum with this name.
 ** @note if no spectrum named pName exists, then
 **       1 is returned, if one does exist, one larger than the highest
 **       version number is returned.
 */
static int
allocateVersion(sqlite3* db, const char*pName, const char* atPoint)
{
  spectrum_definition* pLastOne;
  int                  version;

  pLastOne = findLastDefinition(db, pName, atPoint);
  if(!pLastOne) return 1;

  version = pLastOne->s_version + 1;

  freeSpectrumDefinition(pLastOne);

  return version;

}
/**
   Fill in a rawparameter_info struct with information about the
   parameters in the requested spectrum.
   @param db       - Experiment database handle (checked by caller).
   @param ppParams - Null terminated arrayw of spectrum parameter descriptors.
   @return pRawparameter_info
   @retval NULL - if failure.
   @retval pointer to a null terminated array of pointers to rawoarameter_info structs.
*/ 
static ppRawParameterInfo
getRawParameterInfo(sqlite3* db, const spectrum_parameter** ppParams)
{
  parameter_list      pParameter;
  ppRawParameterInfo  result = NULL;
  pRawparameter_info  aParameter;
  int                 nResults = 0;

  while(*ppParams) {
    pParameter  = spectcl_parameter_list(db, (*ppParams)->s_name);
    if (!pParameter) {
      /* TODO: Free result */

      return NULL;
    }
    aParameter = malloc(sizeof(rawparameter_info));
    if (!aParameter) {
      /* TODO: Free result */
      
      return NULL;
    }
    /* TODO: Finish this!!! */

    ppParams++;
  }
  return result;

}


/**
 ** Create a spectrum using the specific spectrum type driver.
 ** Note that at this point everything has been validated and
 ** I think spectrum creation is not spectrum type dependent.
 ** I must:
 **  - Look up the parameter ids for each parameter.
 **  - Add the spectrum definition to the spectrum_definitions table.
 **  - Get the id of the spectrum I've added.
 **  - Add the parameters to the spectrum_parameters table linked to the
 **    spectrum, and version.
 ** All of this is done in a transaction so that:
 ** - Nobody can stomp on my gettingthe rowid.
 ** - I can rollback in case of failure or commit atomically in case of success.
 **
 ** 
 ** @param db   - Experiment database.
 ** @param pName - Name of the new spectrum.
 ** @param pType - Spectrum type.
 ** @param ppParams - Parmeter definitions.
 ** @param version  - Version number for the spectrum.
 ** @param atpoint  - Where the workspace is attached to the experiment database.
 ** @return int
 ** @retval primary key of the created spectrum in the spectrum_definitions table.
 ** @retval -1      - Failed creation.
 */
int
createSpectrum(sqlite3* db, const char* pName, const char* pType,
	       const spectrum_parameter** ppParams, int version, const char* atPoint)
{
  ppRawParameterInfo pParameterInfo = getRawParameterInfo(db, ppParams);

  if (!pParameterInfo) {
    return -1;
  }
  
  do_non_select(db, "BEGIN TRANSACTION");

  do_non_select(db, "COMMIT TRANSACTION");
  return 1;
}

/*--------------------- public functions ----------*/
/**
 ** Create a new spectrum definition.
 ** Once a spectrum has been created it can be instantiated
 ** instantiation is the process of using the spectrum
 ** definition, some event data and so on to create the
 ** channels of the spectrum for that analysis case.
 **
 ** @param experiment  - Spectra can only be created on
 **                      workspaces that are attached
 **                      to experiments.  This is because
 **                      parameters are foreign keys
 **                      to parameter definitions in the
 **                      parameter definition table.
 **                      This parameter is the handle to
 **                      an experiment database.
 ** @param type       - Type of spectrum being created.
 ** @param name       - Name of the spectrum to create.
 **                     If a spectrum by this name already
 **                     exists, a new version of the 
 **                     definition will be created. 
 **                     versioning allows for evolution in
 **                     the definition of workspace
 **                     items with the ability to backtrack.
 ** @param ppParams   - Pointer to a null terminated array
 **                     of parameter definition pointer.
 ** @param attachPoint- If NULL the default attach point will
 **                     be used.  Otherwise this parameter
 **                     indicates where the workspace
 **                     is attached to the experiment
 ** @return int
 ** @retval > 0 - The 'id' of the spectrum definition.
 **               in this case, spectcl_experiment_errno
 **               will either be SPEXP_OK or SPEXP_NEWVERS
 ** @retval -1    creation failed.   In this case
 **               spexp_experimment_errno will be an
 **               error code:
 **
 ** ERRORS:
 **  - SPEXP_NOT_EXPDATABASE - The experiment parameter
 **                            is not an epxeriment database.
 **  - SPEXP_SQLFAIL         - Some SQL that is part of the
 **                            creation failed.
 **  - SPEXP_NOSUCH          - One of the parameters is not
 **                            valid.
 **  - SPEXP_UNATTACHED      - the attach point is not
 **                            a workspace.
 **  - SPEXP_BADPARAMS       - The spectrum parameter
 **                            definition is inconsistent
 **                            with the spectrum type.
 ** - SPEXP_INVTYPE          - Invalid type.
 **
 **/
                            
int spectcl_workspace_create_spectrum(spectcl_experiment experiment,
				       const char*  pType,
				       const char*  pName,
				       const spectrum_parameter** ppParams,
				       const char* attachPoint)
{
  const char* whereAttached = DEFAULT_ATTACH_POINT;
  int         status;
  int         version;
  int         spectrumId;

  /* Ensure experiment is an experiment handle */

  if (!isExperimentDatabase(experiment)) {
    spectcl_experiment_errno = SPEXP_NOT_EXPDATABASE;
    return -1;
  }

  /* Figure out the attach point and ensure we have a
  ** workspace attached there */

  if (attachPoint) {
    whereAttached = attachPoint;
  }
  status = spectcl_checkAttached(experiment, whereAttached, "workspace", SPEXP_UNATTACHED);
  if (status != SPEXP_OK) {
    spectcl_experiment_errno = status;
    return -1;
  }
  /*  Ensure the spectrum type is valid */

  if (spectcl_experiment_isValidType(experiment,
				      pType,
				      attachPoint) != SPEXP_OK) {
    spectcl_experiment_errno = SPEXP_INVTYPE;
    return -1;
  }
  if(!allParametersExist(experiment,  ppParams)) {
    spectcl_experiment_errno = SPEXP_NOSUCH;
    return -1;
  }

  if (!validateParameters(pType, ppParams)) {
    spectcl_experiment_errno = SPEXP_BADPARAMS;
    return -1;
  }

  version    = allocateVersion(experiment, pName, whereAttached);
  spectrumId = createSpectrum(experiment, pName, pType, ppParams, version, whereAttached);
  if (spectrumId < 0) {
    spectcl_experiment_errno = SPEXP_SQLFAIL;
    return -1;
  }

  spectcl_experiment_errno = (version == 1) ? SPEXP_OK : SPEXP_NEWVERS;
  return spectrumId;

}


