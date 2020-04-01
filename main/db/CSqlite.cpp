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
# @file   CSqlite.cpp
# @brief  Implement CSqlite class.
# @author <fox@nscl.msu.edu>
*/

#include "CSqlite.h"
#include "CSqliteException.h"
#include <sqlite3.h>

//  static members: The open flags:

const int CSqlite::nomutex(SQLITE_OPEN_NOMUTEX);
const int CSqlite::fullmutex(SQLITE_OPEN_FULLMUTEX);
const int CSqlite::sharedcache(SQLITE_OPEN_SHAREDCACHE);
const int CSqlite::privatecache(SQLITE_OPEN_PRIVATECACHE);
const int CSqlite::uri(SQLITE_OPEN_URI);
const int CSqlite::readonly(SQLITE_OPEN_READONLY);
const int CSqlite::readwrite(SQLITE_OPEN_READWRITE);
const int CSqlite::create(SQLITE_OPEN_CREATE);

/**
 * Simple constructor
 *   Construct a database object given a connection id.
 *
 *  @param database - identifies the database to open.
 */
CSqlite::CSqlite(const char* database)
{
    int status = sqlite3_open(database, &m_pConnection);
    checkStatus(status);
}
/**
 * Constructor with flags
 *  Construct a database object using the connection id and open flags.
 *
 *  @param database - defines which database to open.
 *  @param flags    - mask of open flags.
 */
CSqlite::CSqlite(const char* database, int flags)
{
    int status = sqlite3_open_v2(database, &m_pConnection, flags, 0);
    checkStatus(status);
}

/**
 * destructor
 */
CSqlite::~CSqlite()
{
    sqlite3_close(m_pConnection);
}

/**
 * version
 *    Return the sqlite version string of the sqlite database library.
 * @return const char*
 */
const char*
CSqlite::version()
{
    return sqlite3_libversion();
}
/**
 * versionNumber
 *  return the version number for the sqlite database library:
 *
 * @return int - version number.
 */
int
CSqlite::versionNumber()
{
    return sqlite3_libversion_number();
}

/**
 * tableExists
 *   @param table - Name of table to check for.
 *   @return bool - true if table exists, false otherwise.
 */
bool
CSqlite::tableExists(const char* tableName)
{
    const char* strStatement = "SELECT COUNT(*) AS n FROM sqlite_master WHERE type='table' and name=?";
    sqlite3_stmt* pStatement;
    int status = sqlite3_prepare(m_pConnection, strStatement, -1, &pStatement, 0);
    checkStatus(status);
    
    status = sqlite3_bind_text(pStatement, 1, tableName, -1, SQLITE_STATIC);
    checkStatus(status);
    
    status = sqlite3_step(pStatement);
    if (status != SQLITE_ROW) {
        throw CSqliteException(status);
    }
    int num = sqlite3_column_int(pStatement, 0);
    
    sqlite3_finalize(pStatement);
    return num > 0;
}
/*----------------------------------------------------------------------------
 * Utility methods:
 */

/**
 * checkStatus
 *   Check a status value and throw a CSqliteException if it's no SQLITE_OK.
 *
 * @param status - the status to check.
 */
void
CSqlite::checkStatus(int status)
{
    if (status != SQLITE_OK) {
        throw CSqliteException(status);
    }    
}