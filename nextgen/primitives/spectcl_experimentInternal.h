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

#ifndef __SPECTCL_EXPERIMENT_INTERNAL_H
#define __SPECTCL_EXPERIMENT_INTERNAL_H
#ifdef __cplusplus 
extern "C" {
#endif
/**
 ** The definitions in this file are considered internal to Spectcl
 ** and should not be invoked from outside the library.  The implementor
 ** reserves the right to change these interfaces at whim and will and without
 ** notice or pity.
 **
 ** NOTE: you must include <sqlite3.h> prior to including this. 
 */


char* getfirst(sqlite3* db, const char* table, 
		      const char* field, const char* matchfield, const char* key);
int   isExperimentDatabase(sqlite3* db);
int   isEventsDatabase(sqlite3* db);

char* copyString(const unsigned char* s);
char* getTextField(sqlite3_stmt* stmt, int field);
char* getOptionalTextField(sqlite3_stmt* stmt, int field);
void  do_non_select(sqlite3* db, const char* statement);
int   insertConfig(sqlite3* db, const char* which, const char* what);
pRunInfo marshallRunInfo(sqlite3_stmt* stmt);
uuid_t* getDBUUID(sqlite3* db);
int    spectcl_attach(sqlite3* db, const char* otherDatabase, const char* point, const char* defaultPoint);
int    spectcl_detach(sqlite3* db, const char* pName);
int    spectcl_checkAttached(sqlite3* db, const char* pAttachName, const char* type, int incorrectStatus);
int    spectcl_uuidCheck(sqlite3* db1, sqlite3* db2);
char*  spectcl_qualifyStatement(const char* format, const char* attachPoint);
void   spectcl_freePtrList(void** pList);
#ifdef __cplusplus
}
#endif
#endif
