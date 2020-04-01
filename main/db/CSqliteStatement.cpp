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
# @file   CSqliteStatement.cpp
# @brief  Implement CSqliteStatement class.
# @author <fox@nscl.msu.edu>
*/

#include "CSqliteStatement.h"
#include "CSqlite.h"
#include "CSqliteException.h"
#include <sqlite3.h>
#include <sched.h>
#include <stdexcept>


// Static constant definitions:

// Sqlite native data types:

const int CSqliteStatement::integer(SQLITE_INTEGER);
const int CSqliteStatement::floating(SQLITE_FLOAT);
const int CSqliteStatement::text(SQLITE_TEXT);
const int CSqliteStatement::blob(SQLITE_BLOB);
const int CSqliteStatement::null(SQLITE_NULL);



/**
 * constructor:
 *    Create a new prepared statement object from the sql and
 *    a database connection.
 *
 * @param db -  The database connection
 * @param sql - The sql
 */
CSqliteStatement::CSqliteStatement(CSqlite& db, const char* sql) :
    m_connection(db),
    m_cursorState(CSqliteStatement::interior),
    m_retriable(false)
{
    int status = sqlite3_prepare_v2(
        m_connection.connection(), sql, -1, &m_statement, 0
    );
    CSqlite::checkStatus(status);
    
    
}

/**
 * destructor:
 *    Finalize the statement.
 */
CSqliteStatement::~CSqliteStatement()
{
    sqlite3_finalize(m_statement);
}


/**
 * database
 *   Return the database to the caller
 * @return CSqlite& - reference to our database.
 */
CSqlite&
CSqliteStatement::database()
{
    return m_connection;    
}
/**
 * statement
 *   Return the current statement handle.
 *
 *   @return sqlite3_stmt*
 */
sqlite3_stmt*
CSqliteStatement::statement()
{
    return m_statement;
}

/**
 * bindIndex
 *    Return the binding index of a named parameter.
 *
 * @param name - Name of the parameter to lookup.
 * @return int - Integer note that 0 is returned if there's no such parameter.
 *
*/
int
CSqliteStatement::bindIndex(const char* name)
{
    return sqlite3_bind_parameter_index(m_statement, name);
}

/**
 * bind (blob)
 *   Binds a blob to a parameter in the statement.
 *
 *   @param paramNo - the number of the parameter to bind.
 *   @param pBlob   - Pointer to the blob data.
 *   @param nBytes  - Number of bytes of data from pBlob to bind.
 *   @param destructor -Describves how to dispose of the data (see
 *                     sqlite3_bind_blob for more information)
 *                
 */
void
CSqliteStatement::bind(
    int paramNo, const void* pBlob, int nBytes, ObjectDestructor destructor
)
{
    CSqlite::checkStatus(
        sqlite3_bind_blob(m_statement, paramNo, pBlob, nBytes, destructor)
    );
}
/**
 * bind(double)
 *
 *  Binds a double to a parameter in the statement.
 *
 *  @param paramNo - the parameter number.
 *  @param data    - The double to bind.
 */
void
CSqliteStatement::bind(int paramNo, double data)
{
    CSqlite::checkStatus(
        sqlite3_bind_double(m_statement, paramNo, data)
    );
}
/**
 * bind (int)
 *
 *   Binds an integer to a statement parameter.
 *
 *   @param paramNo - the parameter to bind to.
 *   @param data    - The integer to bind.
 */
void
CSqliteStatement::bind(int paramNo, int data)
{
    CSqlite::checkStatus(
        sqlite3_bind_int(m_statement, paramNo, data)
    );
}

/**
 * bind (int64_t)
 *
 *  Binds a 64 bit signed integer to a parameter in the statement
 *
 *  @param paramNo the parameter nunmber.
 *  @param data    the int64_t to bind.
 */
void
CSqliteStatement::bind(int paramNo, int64_t value)
{
    CSqlite::checkStatus(
        sqlite3_bind_int64(m_statement, paramNo, value)
    );
}
/**
 * bind (null)
 *  Binds an explicit null to a parameter.
 *
 *  @param paramNo - the number of the parameter to bind.
 */
void
CSqliteStatement::bind(int paramNo)
{
    CSqlite::checkStatus(
        sqlite3_bind_null(m_statement, paramNo)
    );
}
/**
 * bind (text)
 *
 * Binds utf-8 strings to a parameter.  The size must fit in an int.
 * whatever that is on the platform.
 *
 * @param paramNo - the parameter number to bind.
 * @param data    - Pointer to the string to bind.
 * @param size    - if >=0 the number of bytes of the string to bind. -1 means
 *                  bind the whole string.
 * @param destructor - Specifies how to clean up the string.
 */
void
CSqliteStatement::bind(
        int paramNo, const char* pValue, int nBytes,
        ObjectDestructor destructor
)
{
    CSqlite::checkStatus(
        sqlite3_bind_text(m_statement, paramNo, pValue, nBytes, destructor)
    );
}
/**
 * bind (zero filled blob)
 *
 *    Binds a blob with a specific number of bytes filled with zeroes to
 *    a parameter.
 * @param paramNo - Number of the parameter.
 * @param size    - Number of bytes in the blob.
 */
void
CSqliteStatement::bind(int paramNo, uint64_t nBytes)
{
    CSqlite::checkStatus(
        sqlite3_bind_zeroblob(m_statement, paramNo, static_cast<int>(nBytes))
    );
}

/**
 * clearBindings
 *    Clears all current bindings on a statement.
 */
void CSqliteStatement::clearBindings()
{
    CSqlite::checkStatus(
        sqlite3_clear_bindings(m_statement)
    );
}

/**
 * operator++
 *   Step the statement.  The return value is handled in this way:
 *   -  SQLITE_DONE - sets the state to end
 *   -  SQLITE_ROW  - sets the state to interior.
 *   -  SQLITE_BUSY - Throw an Sqlite Exception (database locked unless)
 *                    m_retriable is true in which case we retry the step
 *                    until something o ther than SQLITE_BUSY is returned.
 *   -  Any other - an SqliteException is thrown with the status.
 * @return *this
 */
CSqliteStatement&
CSqliteStatement::operator++()
{
   
    if (m_cursorState != end) {
        
        bool again = true;
        while(again) {
            int status = sqlite3_step(m_statement);
            switch (status) {
                case SQLITE_DONE:
                    m_cursorState = end;
                    again = false;
                    break;
                case SQLITE_ROW:
                    m_cursorState = interior;
                    again = false;
                    break;
                case SQLITE_BUSY:
                    // Explicitly marked statements can be retried on locked db.
                    
                    again = m_retriable;
                    if (!again) {
                        CSqlite::checkStatus(status);
                    } else {
                        sched_yield();
                    }
                    break;                    // TODO
                default:
                    CSqlite::checkStatus(status);
                    break;
            }
        }
    } else {
        throw std::logic_error("Attempted to step an sqlite statement past end");
    }
    return *this;
}
/**
 * atEnd
 *   Return true if there are no more records to iterate over:
 */
bool
CSqliteStatement::atEnd()
{
    return m_cursorState == end;
}
/**
 * reset
 *    Resets the iteration:
 *    - Invokes sqlite3_reset().
 *    - Sets m_cursorState -> interior.
 */
void
CSqliteStatement::reset()
{
    CSqlite::checkStatus(
        sqlite3_reset(m_statement)
    );
    sqlite3_reset(m_statement);
    m_cursorState = interior;
}


/**
 * bytes
 *    Return the number of bytes of storage requruired by a colum value.
 *    See the sqlite3 docs for sqlite3_column_bytes() for more information
 *    as type coercion can make this return counter-intuitive results.
 *    ...depending on what's intuitive ot you.
 *
 *  @param col - the column number to query.
 *  @return int size of column value.
 */
int
CSqliteStatement::bytes(int col)
{
    return sqlite3_column_bytes(m_statement, col);
}
/**
 * bytes16
 *   Same as bytes above but the storage requirement for strings is given in
 *   UTF-16. This is at least double the size of ASCII.
 *  @param col - the column number to query.
 *  @return int size of column value.
 */
int
CSqliteStatement::bytes16(int col)
{
    return sqlite3_column_bytes16(m_statement, col);
}
/**
 * getBlob
 *   Get data that are a blob from a column:
 *  @param col - Column to fetch.
 *  @return const void* - Pointer to the blob storage. Note that bytes
 *                        can be used to get the size of the blob.
 *.
 */
const void*
CSqliteStatement::getBlob(int col)
{
    return sqlite3_column_blob(m_statement, col);
}

/**
 * getDouble
 *    Get a double from a column.
 *  @param col
 *  @return double.
*/
double
CSqliteStatement::getDouble(int col)
{
    return sqlite3_column_double(m_statement, col);
}

/**
 * getInt
 *    Return an integer field from a result set.
 *  @param col  - Number of column to return.
 *  @return integer value fromt he column.
 */
int
CSqliteStatement::getInt(int col)
{
    return sqlite3_column_int(m_statement, col);
}
/**
 * getInt64
 *    Return an 64 bit integer field.
 *  @param col - number of the field.
 *  @return int64_t value in that field.
 */
int64_t
CSqliteStatement::getInt64(int col)
{
    return sqlite3_column_int64(m_statement, col);
}
/**
 * getText
 *    Return text from a column.
 *
 *    @param col -the column number.
 *    @return const unsigned char* - pointer to the text.
 */
const unsigned char*
CSqliteStatement::getText(int col)
{
    return sqlite3_column_text(m_statement, col);
}

/**
 * columnType
 *    Returns a column type afinity code
 *  @param col - Column Number.
 *  @return int -column type.
 */
int
CSqliteStatement::columnType(int col)
{
    return sqlite3_column_type(m_statement, col);
}

/**
 * sql
 *   Return the sql text of a prepared statement.
 * @return const char*
 */
const char*
CSqliteStatement::sql()
{
    return sqlite3_sql(m_statement);
}

/**
 * lastInsertId
 *     Returns the rowid of the most recent insert.
 * @return int
 */
int
CSqliteStatement::lastInsertId()
{
    return sqlite3_last_insert_rowid(m_connection.connection());
}
/**
 * execute
 *   Static method to execute an SQL statement that does not require
 *   parameters be bound and also does not return a result set.
 *   Typical examples of such statements are:
 *   -   DDL
 *   -   BEGIN/END transaction..
 *   -   DELETE
 *   -   DROP
 *   -  Well you get the idea.
 *
 * @param db         - Reference to an CSqlite object that is a database connection.
 * @param sql        - The SQL statement to execute.
 */
void
CSqliteStatement::execute(CSqlite& db, const char* statement)
{
    sqlite3*      pDb = db.connection();
    sqlite3_stmt* pStatement;
    
    int status = sqlite3_prepare_v2(pDb, statement, -1, &pStatement, 0);
    CSqlite::checkStatus(status);
    
    status = sqlite3_step(pStatement);
    if (status != SQLITE_DONE) {
        throw CSqliteException(status);
    }
    
    status = sqlite3_finalize(pStatement);
    CSqlite::checkStatus(status);

}

