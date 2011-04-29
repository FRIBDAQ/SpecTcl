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

/**
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
 ** Free a set of raw parameter info records.
 ** @param pInfo - pointer to a null terminated array of records to 
 **                free
 */
static void freeRawParameterInfo(ppRawParameterInfo pInfo)
{
  if (pInfo) {
    while (*pInfo) {
      free(*pInfo);
      pInfo++;
    }
    free(pInfo);
  }
}
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
 ** @return void 
 */
static void
freeSpectrumDefinition(spectrum_definition* pDef)
{
  if (!pDef) {
    /* First free the spectrum parameter definitions too */

    spectrum_parameter** pParams = pDef->s_parameters;
    while (*pParams) {
      free((*pParams)->s_name);
      free(*pParams);
      pParams++;
    }
    free(pDef->s_name);
    free(pDef->s_type);
    free(pDef);
  }
}
/**
 ** Given the first row of a result set from a spectrum definition
 ** selection joined to the corresponding parameter definitions, and parameter
 ** definitions (from the experiment database).
 ** Fills in the spectrum definition struct for that spectrum.
 ** @param pDef - Pointer to storage allocated fro the top level spectrum_definition.
 **               no storage has been allocated for s_parameters yet.
 **               and we should not assume anything about the
 **               vale of s_parameters.
 ** @param stmt - Sqlite statement context.
 ** @return int
 ** @retval SQLITE_ROW - We finished by getting the next
 **                      spectrum in the query.
 ** @retval SQLITE_DONE - We finished by exhausting the result set.
 **
 ** @note - side effects: 
 **     - pDef is filled in.
 **     - stmt is stepped until either there are no more rows or
 **       until we are at the first definition of the next spectrum.
 **
 ** @note - Not sure at this point how to deal with out of memory conditions.
 ** @note Assumptions on column order (sd - spectrum_definitions,
 **       sp - spectrum_parameters, p - parameters):
 **      - sd.id
 **      - sd.name
 **      - sd.type_id
 **      - sd.version
 **      - sp.dimension
 **      - p.name
 */
static int
fillSpectrumDefinition(spectrum_definition* pDef,
		       sqlite3_stmt*        stmt)
{
  int status;
  int paramCount  = 0;
  spectrum_parameter* pParam;
  int                 paramId;

  /* Pull out information for the top level part of the def */
  
  pDef->s_id   = sqlite3_column_int(stmt, 0);              /* Spectrum id */
  pDef->s_name = copyString(sqlite3_column_text(stmt, 1)); /* Spectrum name. */
  pDef->s_type = copyString(sqlite3_column_text(stmt, 2)); /* Spectrum type */
  pDef->s_version = sqlite3_column_int(stmt, 3);	   /* object version.  */



  /*  Now the parameter stuff.    */

  pDef->s_parameters = malloc(sizeof(spectrum_parameter*));
  (pDef->s_parameters)[0]  = NULL; /* Empty list. */


  do {
    /*  Only can fill stuff in if the sp.id is not null -- indicating there
        is a join row.. This allows for pathalogical spectrum types that don't
        need parameters.
    */

    if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
      pDef->s_parameters = realloc(pDef->s_parameters, sizeof(spectrum_parameter*) + 2);
      pParam = malloc(sizeof(spectrum_parameter));
      (pDef->s_parameters)[paramCount] = pParam;

      pParam->s_dimension = sqlite3_column_int(stmt, 4);
      pParam->s_name      = copyString(sqlite3_column_text(stmt, 5));

      paramCount++;
      (pDef->s_parameters)[paramCount] = NULL;
    }

    /* Next row... if there are none then break the loop as well. */

    status = sqlite3_step(stmt);
    if (status != SQLITE_ROW) break;
  } while (sqlite3_column_int(stmt, 0) == pDef->s_id);  /* done when spectrum changes. */
  return status;
  
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

  const char* sqlFormat = "SELECT sd.id, sd.name, sd.type_id, sd.version, sp.dimension, p.name \
                                    FROM %s%sspectrum_definitions sd \
                                    LEFT JOIN %%s%%sspectrum_parameters sp \
                                    ON         sd.id = sp.spectrum_id  \
                                    INNER JOIN parameters p             \
                                    ON         p.id = sp.parameter_id   \
                                    WHERE      sd.name  = :spname      \
                                    ORDER BY   sd.id, sd.version DESC";
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
    fillSpectrumDefinition(result, stmt);

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

  result = malloc(sizeof(pRawparameter_info));
  if (!result) {
    return NULL;
  }
  *result = NULL;		/* Empty list. */

  while(*ppParams) {
    pParameter  = spectcl_parameter_list(db, (*ppParams)->s_name);
    if (!pParameter) {
      freeRawParameterInfo(result);

      return NULL;
    }
    aParameter = malloc(sizeof(rawparameter_info));
    if (!aParameter) {
      spectcl_free_parameter_list(pParameter);
      freeRawParameterInfo(result);
      return NULL;
    }
    aParameter->s_id        = pParameter[0]->s_id;
    aParameter->s_dimension = (*ppParams)->s_dimension; 
    spectcl_free_parameter_list(pParameter);
    result                  = realloc(result,(nResults+2)*sizeof(pRawparameter_info));
    if(!result) {
      return NULL;
    }
    result[nResults] = aParameter;
    nResults++;
    result[nResults] = NULL;

    ppParams++;
  }
  return result;

}


/**
 ** Create a spectrum. Because of the nature of the spectrum
 ** definition, this is not spectrum type dependent.
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
  const char*    specInsertSqlTemplate = "INSERT INTO %s%sspectrum_definitions \
                                 (name,type_id,version)                     \
                                 VALUES (:name, :type_id, :version)";
  const char*    paramInsertSqlTemplate = "INSERT INTO %s%sspectrum_parameters \
                                      (spectrum_id, parameter_id, dimension)                                   \
                                      VALUES (:specid, :paramid, :dim)";
  char*          pQualifiedSql = NULL;
  sqlite3_stmt*  pStatement = NULL;
  int            status;
  int            spectrumId= -1;

  ppRawParameterInfo pParameterInfo = getRawParameterInfo(db, ppParams);
  ppRawParameterInfo pPs            = pParameterInfo;

  if (!pParameterInfo) {
    return -1;
  }
  

  /* First create the spectrum definition and get its rowid: */
  /* Much of this work can be done outside the transaction   */

  pQualifiedSql = spectcl_qualifyStatement(specInsertSqlTemplate, atPoint);
  
  if (!pQualifiedSql) {
    return -1;
  }
  status = sqlite3_prepare_v2(db, pQualifiedSql, -1, &pStatement, NULL);
  if (status != SQLITE_OK) {	/* Hopefully errors don't change pStatement: */
    goto non_trans_error;
  }
  status = sqlite3_bind_text(pStatement, 1, pName, -1, SQLITE_STATIC);
  if(status != SQLITE_OK) goto non_trans_error;

  status = sqlite3_bind_text(pStatement, 2, pType, -1, SQLITE_STATIC);
  if (status != SQLITE_OK) goto non_trans_error;
  
  status = sqlite3_bind_int(pStatement, 3, version);
  if (status != SQLITE_OK) goto non_trans_error;

  
  do_non_select(db, "BEGIN TRANSACTION");
  {
    /* execute the insert and get the rowid: */
    
    status = sqlite3_step(pStatement);
    if (status != SQLITE_DONE) goto error;

    sqlite3_finalize(pStatement);
    pStatement = NULL;

    spectrumId = sqlite3_last_insert_rowid(db);
    if (spectrumId <= 0) goto error;
    free(pQualifiedSql);
    pQualifiedSql = NULL;

    /* Now we need to form the parameter insertion SQL statement
       and use it to insert all the parameters the spectrum needs 
    */
    
    pQualifiedSql = spectcl_qualifyStatement(paramInsertSqlTemplate, atPoint);
    if (!pQualifiedSql) goto error;

    status = sqlite3_prepare_v2(db, pQualifiedSql, -1, &pStatement, NULL);
    if (status != SQLITE_OK) goto error;

    /* Bind the values that are not parameter dependent */
    
    status = sqlite3_bind_int(pStatement, 1, spectrumId);
    if(status != SQLITE_OK) goto error;
 

    /* Loop over the insertions of all parametesr */

    while (*pPs) {
      pRawparameter_info  p = *pPs;

      status = sqlite3_bind_int(pStatement, 2, p->s_id);
      if(status != SQLITE_OK) goto error;

      status = sqlite3_bind_int(pStatement, 3, p->s_dimension);
      if(status != SQLITE_OK) goto error;

      status = sqlite3_step(pStatement);
      if (status != SQLITE_DONE) goto error;

      sqlite3_reset(pStatement); /* Now can rebind and keep going. */

      pPs++;
    }
    
    
  }
  do_non_select(db, "COMMIT TRANSACTION");
  sqlite3_finalize(pStatement);
  return spectrumId;


 error:
  /* TODO: Free pParameterInfo here and non_trans_error */
  freeRawParameterInfo(pParameterInfo);
  do_non_select(db, "ROLLBACK TRANSACTION");

 non_trans_error:
  if (pStatement) {
    sqlite3_finalize(pStatement);
  }
  free(pQualifiedSql);
  freeRawParameterInfo(pParameterInfo);
  return -1;
}

/**
 * Return the set of known spectra that match a specific
 * pattern
 * @param db      - Experiment database (must be verfied by caller)
 * @param pattern - glob pattern of spectra to match.
 * @param ap      - Attach point of the workspace to the experiment
 *                  (verified by caller).
 * @return char**
 * @retval - Null terminated set of strings of spectrum names that match
 *           the pattern (could be that [0] is null if there are no matches.
 */
static char**
getMatchingSpectra(sqlite3* db, const char* pattern, const char* ap)
{
  const char* sqlTemplate = 
    "SELECT DISTINCT name FROM %s%sspectrum_definitions \
                 WHERE name GLOB :pattern ORDER BY id ASC";
  char*         pSql;
  int           status;
  char*         item;
  char**        result;
  int           nMatches = 0;
  sqlite3_stmt* pStatement;

  /* set up the result. */

  result = malloc(sizeof(char*));
  *result = NULL;		/* Start with an empty list. */

  /* Prepare the statement and bind the glob pattern: */

  pSql = spectcl_qualifyStatement(sqlTemplate, ap);
  status = sqlite3_prepare_v2(db, pSql, -1, &pStatement, NULL);
  free(pSql);
  if (status == SQLITE_OK) {
    status = sqlite3_bind_text(pStatement, 1, pattern, -1, SQLITE_STATIC);
    if (status == SQLITE_OK) {
      while ((status = sqlite3_step(pStatement)) == SQLITE_ROW) {
	item = copyString(sqlite3_column_text(pStatement, 0));
	result = realloc(result, (nMatches+1)*sizeof(char*));
	result[nMatches] = item;
	nMatches++;
	result[nMatches] = NULL;
      }
    }
    sqlite3_finalize(pStatement);
  }


  return result;
}

/**
 * Return spectrum definitions givena query and the set of spectra
 * to query within.  The query is assumed to require
 * two attach point substitutions, and one binding for a spectrum
 * name.
 *
 * @param db             - Experiment database to do the query on.
 * @param pQueryTemplate - Pointer to the query template. 
 * @param pNames         - Spectrum names to successively bind to the query.
 * @param pAttach        - Attach point of the workspace.
 * @return spectrum_definition**
 * @retval definitions of any spectra matching the query.
 */
static spectrum_definition**
getSpectrumDefinitions(sqlite3* db, const char* pQueryTemplate, char* const* pNames,  const char* pAttach)
{
  char*          pSub1;
  char*          pSub2;
  sqlite3_stmt*  pStatement;
  int            status;
  spectrum_definition** result;
  int            defCount = 0;

  /* Make an empty result: */
  
  result = malloc(sizeof(spectrum_definition*));
  result[0] = NULL;

  /* Do the workspace substitutions */

  pSub1 = spectcl_qualifyStatement(pQueryTemplate, pAttach);
  pSub2 = spectcl_qualifyStatement(pSub1, pAttach);
  free(pSub1);

  /* prepare the statement for execution:  */

  status = sqlite3_prepare_v2(db, pSub2, -1, &pStatement, NULL);
  free(pSub2);			/* Make sure we don't leak this... */
  if(status == SQLITE_OK) {
    /* We have to bind each of the parameter names to the query and step it until
       it's done:
       TODO:  Out of memory handling:
    */
    while (*pNames) {
      status = sqlite3_bind_text(pStatement, 1, *pNames, 
				 -1, SQLITE_STATIC);       /* pNames is caller's problem */
      if (status == SQLITE_OK) {
	status = sqlite3_step(pStatement);
	while (status  == SQLITE_ROW) {
	  spectrum_definition* pDef;
	  pDef = malloc(sizeof(spectrum_definition));
	  status = fillSpectrumDefinition(pDef, pStatement); /* Will leave us at next spectrum. */
	  result = realloc(result, sizeof(spectrum_definition*)*(defCount+2));
	  result[defCount] = pDef;
	  defCount++;
	  result[defCount] = NULL; /* Maintain null termination. */
	}
      }
      /* Prep the statment for the next binding: */

      sqlite3_reset(pStatement);
      pNames++;
    }
  }

  sqlite3_finalize(pStatement);
  return result;
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


/**
 ** Get information about spectrum definitions.
 ** @param db          - Experiment database.
 ** @param pattern     - Glob pattern to match against spectrum name.
 **                      If NULL "*" is used.
 ** @param allVersions - If false, only the most recent version of the spectrum is 
 **                      fetched, otherwise, all spectrum versions are fetched.
 ** @param attachpoint - Point at which the workspace is attached to the experiment database.
 **                      If NULL, WORKSPACE_DEFAULT_ATTACH_POINT is assumed.
 ** @return spectrum_definition**
 ** @retval not null   - A pointer to a null terminated list of pointers to spectrum_definition
 **                      structs.  This s dynamically allocated and should be deleted via
 **                      a call to spectcl_workspace_free_spectrum_definitions.
 **                      if there are no matching spectra, it is perfectly fine for
 **                      the list to be empty.
 ** @retval NULL       - Some sort of failure occured.  The actual failure reason is in 
 **                      spectcl_experiment_errno and is probably one of the following:
 **                      - SPEXP_NOT_EXPDATABASE - db is not an experiment database handle.
 **                      - SPEXP_UNATTACHED - No workspace can be found attached to the specified
 **                        location.
 **                      - SPEXP_SQLFAIL - Some uncategorized SQL error.
 */
spectrum_definition**
spectcl_workspace_find_spectra(spectcl_experiment db,
			       const char*        pattern,
			       int                allVersions,
			       const char*        attachPoint)
{
  const char* whereAttached = DEFAULT_ATTACH_POINT;
  int         status;
  const char* pActualPattern = "*";
  char**      pMatchingNames;
  spectrum_definition**   result;

  /*  Two Sqlite templates are used, one for the case of wanting the most recent version only
   *  the other for wanting all versions:
   */

  const char* mostRecentTemplate =
    "SELECT sd.id, sd.name, sd.type_id, sd.version, sp.dimension, p.name \
                                    FROM %s%sspectrum_definitions sd \
                                    LEFT JOIN %%s%%sspectrum_parameters sp \
                                    ON         sd.id = sp.spectrum_id  \
                                    INNER JOIN parameters p             \
                                    ON         p.id = sp.parameter_id   \
                                    WHERE      sd.name  = :spname      \
                                    ORDER BY   sd.version DESC\
                                    LIMIT 1";
  const char* allTemplate =
     "SELECT sd.id, sd.name, sd.type_id, sd.version, sp.dimension, p.name \
                                    FROM %s%sspectrum_definitions sd \
                                    LEFT JOIN %%s%%sspectrum_parameters sp \
                                    ON         sd.id = sp.spectrum_id  \
                                    INNER JOIN parameters p             \
                                    ON         p.id = sp.parameter_id   \
                                    WHERE      sd.name  = :spname      \
                                    ORDER BY    sd.version DESC";
  const char* pSelectTemplate;
  char*       qSql1;
  char*       qSql2;
                     

  spectrum_definition** pDefs = NULL;


  /* Ensure experiment is an experiment handle */

  if (!isExperimentDatabase(db)) {
    spectcl_experiment_errno = SPEXP_NOT_EXPDATABASE;
    return NULL;
  }

  /* Figure out the attachment point and determine if there is a workspace
     attached to that point
  */
  if(attachPoint) {
    whereAttached = attachPoint;
  }
  status = spectcl_checkAttached(db, whereAttached, "workspace", SPEXP_UNATTACHED);
  if (status != SPEXP_OK) {
    spectcl_experiment_errno = status;
    return NULL;
  }  
  /* Figure out our name match pattern: */

  if(pattern) {
    pActualPattern = pattern;
  }
  /* Get the list of spectra that match our pattern.
   * What happens next depends on the state of the allVersions flag
   */
  pMatchingNames = getMatchingSpectra(db, pActualPattern, whereAttached); 

  /*  If there are no matches we are done...return an empty result */

  if (!pMatchingNames || (pMatchingNames[0] == NULL)) {
    free(pMatchingNames);	/* Nothing pointed at by pointers. */
    result = malloc(sizeof(spectrum_definition*));
    result[0] = NULL;
    return result;
    
  }
  /* We have to pick out the correct query and ask for the matching definitions
    given the names we have:
  */
  result = getSpectrumDefinitions(db, 
				  allVersions ? allTemplate : mostRecentTemplate,
				  pMatchingNames,
				  whereAttached);


  spectcl_experiment_errno = SPEXP_UNIMPLEMENTED;
  return result;
}
