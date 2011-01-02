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

/**
 ** The definitions in this file are considered internal to Spectcl
 ** and should not be invoked from outside the library.  The implementor
 ** reserves the right to change these interfaces at whim and will and without
 ** notice or pity.
 */


static char* getfirst(sqlite3* db, const char* table, 
		      const char* field, const char* matchfield, const char* key);
int   isExperimentDatabase(sqlite3* db);

char* copyString(const unsigned char* s);
char* getTextField(sqlite3_stmt* stmt, int field);
char* getOptionalTextField(sqlite3_stmt* stmt, int field);

#endif
