/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**    grobjmgr.cc:
**      This file contains a set of convenience routines for accessing and
**      manipulating Xamine's graphical object database.
**        Xamine initially only supports a single database, however these
**      routines provide support for multiple databases.  Client modules
**      should include grobjmgr.h to get function prototypes.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/


/*
** Include files:
*/
#include <config.h>
#include <stdio.h>
#include <errno.h>
#include "dispgrob.h"
#include "grobjmgr.h"


/*
** Static local storage:
*/

grobj_database Xamine_DefaultObjectDatabase;
grobj_database Xamine_DefaultGateDatabase;

/*
** Exported storage.  The pointer is exported for the benefit of
** grobjfile.y and grobjfile.l
*/

grobj_database *grobjdb = &Xamine_DefaultObjectDatabase;


/*
** Functional Description:
**   Xamine_GetObjectDatabase:
**     This function returns a pointer to the graphical object database currently
**     in use.
*/
grobj_database *Xamine_GetObjectDatabase()
{
  return grobjdb;
}
/*
** Functional Description:
**    Xamine_SetObjectDatabase:
**      This function sets a new graphical object database as the current
**      database.
** Formal Parameters:
**   grobj_database *newdb:
**     Pointer to the new database.
** Returns:
**    Pointer to the prior database.
*/
grobj_database *Xamine_SetObjectDatabase(grobj_database *newdb)
{
  grobj_database *old;

  old = grobjdb;
  grobjdb = newdb;
  return old;
}

/*
** Functional Description:
**   Xamine_EnterGrobj:
**      This function enters a graphical object in the database.
**      Some consistency checks are performed:
**     1) We make sure the entry can be entered (error is ENOSPC
**        if can't.
** Formal Parameters:
**    grobj_genereic *entry:
**       Database entry to insert.
**    Returns:
**       Object entered  - (same as entry but with ID set).
**       False - Failure.
*/
grobj_generic  *Xamine_EnterGrobj(grobj_generic *entry)
{
  grobj_generic *o;

  /* Now we can add the entry: */

  if((o = grobjdb->enter(entry)) == (grobj_generic *)NULL) {
    errno = ENOSPC;
    return NULL;
  }
  return o;
  
}

/*
** Functional Description:
**  Xamine_DeleteGrobj:
**     Attempts to delete a graphical object.
**  Formal Parameters:
**     int spectrum:
**       Spectrum number to delete from.
**     int id:
**        ID of object to delete.
** Returns:
**   True   - Success.
**   False  - If failed.  The description of the error will be in errno.
*/
int Xamine_DeleteGrobj(int spectrum, int id)
{
  /* Locate the corresponsing entry */

  grobj_generic *entry = grobjdb->find_first(spectrum);
  while(entry != NULL) {
    if(entry->getid() == id) {	/* Delete the located entry. */
      return grobjdb->remove(entry);
    }
    entry = grobjdb->find_next();	/* Locate the next entry. */
  }
  /* If control passes here, the entry did not exist, so return the failure */

  errno = ENOENT;
  return False;

}

/*
** Functional Description:
**   Xamine_GetObjectTypeAndId:
**      This function gets the object type and identifier associated with
**      a graphical object.
** Formal Parameters:
**      grobj_type *type:
**      int        *id:
**         BUffers to hold the returned object type and Id.
**      grobj_generic *entry:
**         Pointer to the object we're asking about.
*/
void Xamine_GetObjectTypeAndId(grobj_type *type, int *id, grobj_generic *entry)
{
  *type = entry->type();
  *id   = entry->getid();
}

/*
** Functional Description:
**   Xamine_WriteGraphicalObjects:
**     Writes the current set of graphical objects to file.
** Formal Parameter:
**    FILE *stream:
**      File stream pointer open on the output file to write.
** Returns:
**   # bytes written on success, EOF on faiure.
*/
int Xamine_WriteGraphicalObjects(FILE *stream)
{
  return grobjdb->write(stream);
}
/*
** Functional Description:
**    Xamine_ReadGraphicalObjects:
**       Reads the current set of graphical objects to file.
** Formal Parameter:
**    FILE *stream:
**      Pointer to the file stream. open on the file to read graphical objects
**      from.
** Returns:
**   Number of bytes read if success or EOF if failure.
*/
int Xamine_ReadGraphicalObjects(FILE *stream)
{
  grobj_database temp_db;
  int stat;

  stat = temp_db.read(stream);	/* First try to read to temporary storage. */
  if(stat == EOF) return stat;	/* If it fails leave real db untouched.    */

  /* Now that the read was successful, we need to clear the current database
  ** and copy the temporary one over.
  */
  grobj_generic *entry;
  grobjdb->clear();
  
  entry = temp_db.find_first();
  while(entry != NULL) {	/* This loop copies the objects from temp. */
    grobjdb->enter(entry);
    entry = temp_db.find_next();
  }

  /* Return the final status. */

  return stat;
}

/*
** Functional Description:
**   Xamine_GetSpectrumObjectCount:
**     Gets the number of objects in a spectrum.
** Formal Parameters:
**     int specid:
**       Id of the spectrum.
** Returns:
**   Count of number of objects in a spectrum.
*/
int Xamine_GetSpectrumObjectCount(int id)
{
  int count = 0;
  grobj_generic *entry =   grobjdb->find_first(id);
  while(entry != NULL) {
    count++;
    entry = grobjdb->find_next();
  }
  return count;
}

/*
** Functional Description:
**   Xamine_GetSpectrumObjects:
**     Retrieves a list of object from the object manager.
** Formal Parameters:
**     int specid:
**       Number of the spectrum to search.
**     grobj_generic *entries:
**       List of pointers to the entries located.
**     int maxobjects:
**       Size of entries list.
**     int reset_scan:
**       If nonzero then the scan is reset with a find_first. Otherwise
**       The call is assumed to be a continuatino and find_next is the
**       first search call.
** Returns:
**   Number of entries actually loaded into entries.
*/
int Xamine_GetSpectrumObjects(int specid, grobj_generic **objects,
			      int maxobjects, int reset_scan)
{
  int count = 0;
  if(maxobjects <=0) return 0;

  grobj_generic *entry;

  /* Set up for the while loop: */

  if(reset_scan) {
    entry = grobjdb->find_first(specid);
  }
  else {
    entry = grobjdb->find_next();
  }
  while(  (count < maxobjects) && ( entry != NULL)) {
    *objects++ = entry;
    count++;
    if(count < maxobjects)
      entry = grobjdb->find_next();
  }
  return count;
}

/*
** Functional Description:
**    Xamine_GetNextObjectId:
**      Returns the ID of the next object allocated.
*/
int Xamine_GetNextObjectId()
{
  return grobjdb->nextid();
}

/*
** Functional Description:
**   Xamine_EnterGate:
***    Enters a gate in the gates database.
** Formal Parameters:
**   grobj_generic *gate:
**     The gate to enter.
** Returns:
**    The object that's actually created.
*/
grobj_generic *Xamine_EnterGate(grobj_generic *gate)
{
  grobj_database *old;
  grobj_generic  *o;

  old = Xamine_SetObjectDatabase(&Xamine_DefaultGateDatabase);
  if((o = grobjdb->enteras(gate)) == (grobj_generic *)NULL) {
    errno = ENOSPC;
  }


  Xamine_SetObjectDatabase(old);
  return o;
}

/*
** Functional Description:
**   Xamine_DeleteGate:
**     This function deletes a gate from the gates data base.
**     we just flip databases and call DeleteGrobj.
** Formal Parameters:
**   int spec:
**      Spectrum from which we delete the gate.
**   int id:
**      Id of the gate to delete.
** Returns:
**   The result of the Xamine_DeleteGrobj call.
*/
int Xamine_DeleteGate(int spec, int id)
{
  int status;
  grobj_database *old;

  old = Xamine_SetObjectDatabase(&Xamine_DefaultGateDatabase);

  status = Xamine_DeleteGrobj(spec, id);
  Xamine_SetObjectDatabase(old);

  return status;
}

/*
** Functional Description:
**   Xamine_WriteGates:
**     This function writes the gates database.
** Formal Parameter:
**     FILE *stream:
**        File pointer for stream open on the file to write.
** Returns:
**    Result of Xamine_WriteGraphicalObjects
*/
int Xamine_WriteGates(FILE *stream)
{
  grobj_database *old;
  int status;

  old = Xamine_SetObjectDatabase(&Xamine_DefaultGateDatabase);
  status = Xamine_WriteGraphicalObjects(stream);

  Xamine_SetObjectDatabase(old);
  return status;
}

/*
** Functional description:
**    Xamine_ReadGates:
**       This function reads the gates database.
**    Formal Parameter:
**      FILE *stream:
**        File pointer for stream open on the file to read.
** Returns:
**   Result of Xamine_ReadGraphicalObjects.
*/
int Xamine_ReadGates(FILE *stream)
{
  grobj_database *old;
  int status;

  old    = Xamine_SetObjectDatabase(&Xamine_DefaultGateDatabase);
  status = Xamine_ReadGraphicalObjects(stream);

  Xamine_SetObjectDatabase(old);
  return status;
}

/*
** Functional Description:
**   Xamine_GetSpectrumGateCount:
**     Returns the number of gates that are currently defined on a spectrum.
** Formal Parameters:
**   int specid:
**      Number of the spectrum that we're looking at.
** Returns:
**   number of gates defined on the spectrum.
*/
int Xamine_GetSpectrumGateCount(int specid) 
{
  grobj_database* old;
  int count;

  old   = Xamine_SetObjectDatabase(&Xamine_DefaultGateDatabase);
  count = Xamine_GetSpectrumObjectCount(specid);
  Xamine_SetObjectDatabase(old);

  return count;
}

/*
** Functional Description:
**   Xamine_GetSpectrumGates:
**     This function gets the set of gates that are defined on a spectrum.
** Formal Parameters:
**   int specid:
**     Spectrum number to count gates.
**   grobj_generic **objects:
**     Points to a buffer to hold pointers to all the gate objects.
**   int maxobjects:
**     Number of objects we can hold in the list.
**   int reset_scan:
**     Nonzero to reset the gates scan to zero (in case multiple calls are
**     needed).
** Returns:
**   Number of gate pointers actually loaded into objects.
*/
int
Xamine_GetSpectrumGates(int specid, grobj_generic** objects, int maxobjects,
			int reset_scan)
{
  int count;
  grobj_database* old;

  old   = Xamine_SetObjectDatabase(&Xamine_DefaultGateDatabase);
  count = Xamine_GetSpectrumObjects(specid, objects, maxobjects, reset_scan);
  Xamine_SetObjectDatabase(old);

  return count;
}

/*
** Functional Description:
**   Xamine_GetNextGateId:
**     Returns a unique gate number.
*/
int Xamine_GetNextGateId()
{
  return Xamine_DefaultGateDatabase.newid();
}

/*
** Functional Description:
**   Xamine_GetGateDatabase:
**     Retrieves a pointer to the gate database.
*/
grobj_database* Xamine_GetGateDatabase()
{
  return &Xamine_DefaultGateDatabase;
}
