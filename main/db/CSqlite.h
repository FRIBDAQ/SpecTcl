/**

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

##
# @file   CSqlite.h
# @brief  Class that represents a connection.
# @author <fox@nscl.msu.edu>
*/

#ifndef __CSQLITE_H
#define __CSQLITE_H
#include <sqlite3.h>

/**
 * @class CSqlite
 *    This class represents a conneection to an sqlite3 database.
 *    Construction connects and destruction disconnects.
 *    The handle of the database can be gotten
 *    A few other utility functions are provided as well.
 *
 *   To actually do queries, you must instantiate a CSqliteStatement object
 *   using this as the connection parameter.  That statement when properly
 *   bound can be executed and the result set iterated on.
 */
class CSqlite {
private:
    sqlite3*  m_pConnection;

    // public data connection flags:
public:    
    static const int nomutex;
    static const int fullmutex;
    static const int sharedcache;
    static const int privatecache;
    static const int uri;
    static const int readonly;
    static const int readwrite;
    static const int create;
    
    // canonicals (these do the real work).
public:
    CSqlite(const char* database);
    CSqlite(const char* database, int flags);
    virtual ~CSqlite();
    
    // Copy semantics are not supplied as they would require reference
    // counting on the connection and I'm too lazy to do that now.
    
private:
    CSqlite(const CSqlite& rhs);
    CSqlite& operator=(const CSqlite& rhs);
    
    
    // Selectors
    
public:
    sqlite3* connection() {return m_pConnection; }
    
    // operations on the database.  Note that since I'm lazy these will be
    // added as needed except for version which is there for testing.
    
    static const char* version();
    static int         versionNumber();

    // Shared utilities.
public:
    bool tableExists(const char* tableName);
    static void checkStatus(int status);
    
};

#endif