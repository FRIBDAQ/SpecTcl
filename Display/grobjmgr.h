/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   grobjmgr.h:
**     Header file for clients of the graphical object management subsystem.
**     The subsystem builds heavily and completely on the classes and objects
**     which were defined in dispgrobj.h
** Author:
**     Ron FOx
**     NSCL
**     Michigan State University
**     East Lansing, MI 48824-1321
** SCCS History:
**     @(#)grobjmgr.h	8.1 6/23/95 
*/
#ifndef _GROBJMGR_H_INSTALLED
#define _GROBJMGR_H_INSTALLED
#include <stdio.h>
#include "dispgrob.h"

#ifndef True
#define True (1 == 1)
#endif
#ifndef False
#define False (1 != 1)
#endif


grobj_database *Xamine_GetObjectDatabase(); /*  Get graphical object db ptr. */
grobj_database *Xamine_SetObjectDatabase(grobj_database *newdbo);
grobj_generic  *Xamine_EnterGrobj(grobj_generic *entry);
int             Xamine_DeleteGrobj(int spec, int id);
void            Xamine_GetObjectTypeAndId(grobj_type *type, int *id,
                                          grobj_generic *entry);

int             Xamine_WriteGraphicalObjects(FILE *stream);
int             Xamine_ReadGraphicalObjects(FILE *stream);
int             Xamine_GetSpectrumObjectCount(int specid);
int             Xamine_GetSpectrumObjects(int specid,
                                          grobj_generic **objects,
                                          int        maxobjects,
                                          int        reset_scan);

int             Xamine_GetNextObjectId();

/*
**  The following work with the gates database rather than the
**  graphical object database. 
*/
grobj_generic *Xamine_EnterGate(grobj_generic *entry);
int Xamine_DeleteGate(int spec, int id);

inline void Xamine_GetGateTypeAndId(grobj_type *type, int *id, 
				   grobj_generic *entry) {
         Xamine_GetObjectTypeAndId(type, id, entry);     }

int Xamine_WriteGates(FILE *stream);
int Xamine_ReadGates(FILE *stream);
int Xamine_GetSpectrumGateCount(int specid);
int Xamine_GetSpectrumGates(int specid,
			    grobj_generic **objects,
			    int maxobjects,
			    int reset_scan);
int Xamine_GetNextGateId();
grobj_database *Xamine_GetGateDatabase();
#endif
