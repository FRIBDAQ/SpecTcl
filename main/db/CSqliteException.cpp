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
# @file   CSqliteException.cpp
# @brief  Implementation of CSqliteException.cpp
# @author <fox@nscl.msu.edu>
*/
#include "CSqliteException.h"
#include <sqlite3.h>

// Since sqlite3_errstr seems not to actually actually exist in my version of
// sqlite3; These are mercielessly stolen from sqlite3.h

const char* CSqliteException::m_errorMessages[] = {
   "Successful result",
   "SQL error or missing database",
   "Internal logic error in SQLite",
   "Access permission denied",
   "Callback routine requested an abort",
   "The database file is locked",
   "A table in the database is locked",
   "A malloc() failed",
   "Attempt to write a readonly database",
   "Operation terminated by sqlite3_interrupt()",
   "Some kind of disk I/O error occurred",
   "The database disk image is malformed",
   "Unknown opcode in sqlite3_file_control()",
   "Insertion failed because database is full",
   "Unable to open the database file",
   "Database lock protocol error",
   "Database is empty",
   "The database schema changed",
   "String or BLOB exceeds size limit",
   "Abort due to constraint violation",
   "Data type mismatch",
   "Library used incorrectly",
   "Uses OS features not supported on host",
   "Authorization denied",
   "Auxiliary database format error",
   "2nd parameter to sqlite3_bind out of range",
   "File opened that is not a database file"

};


/**
 * normal construtor
 *
 * @param error - the sqlite3 error code that is encapsulated by this exception
 *                At this time it's scout's honor that this is a valid sqlite3
 *                error.
 */
CSqliteException::CSqliteException(int error) noexcept :
    exception(),
    m_errorCode(error)
{}

/**
 * copy constructor.
 *
 * @param rhs  - The exception that we are constructed from.
 */
CSqliteException::CSqliteException(const CSqliteException& rhs) noexcept :
    exception(rhs),
    m_errorCode(rhs.m_errorCode)
{}


/**
 * destructor
 * No-op for now
 */

CSqliteException::~CSqliteException() noexcept {}

/**
 * operator=
 *    @param rhs - object being assigned to *this
 *    @return *this
 */
CSqliteException&
CSqliteException::operator=(const CSqliteException& rhs) noexcept
{
    m_errorCode = rhs.m_errorCode;   // NO need to protect from &rhs == this
    return *this;
}

/**
 * what
 *  @return const char* - The error message string corresponding to the error.
 */
const char*
CSqliteException::what() const noexcept
{
    if (m_errorCode < sizeof(m_errorMessages)/sizeof(const char*)) {
        return m_errorMessages[m_errorCode];
    } else {
        return "Invalid error code";
    }
}

