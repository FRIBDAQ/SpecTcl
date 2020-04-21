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
# @file   CSqliteStatement.h
# @brief  Wrap an sqlite statement object.
# @author <fox@nscl.msu.edu>
*/
#ifndef  __CSQLITESTATEMENT_H
#define  __CSQLITESTATEMENT_H

#include <stdint.h>


class CSqlite;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef struct sqlite3      sqlite3;

/**
 * @class CSqliteStatement
 *
 * This class allows clients to prepare bind and execute sql statements.
 * The result set is encpasulated via an iterator that is singly included in
 * the class as that's what sqlite3 demands (Single cursor per query).
 * The field getters are patterned after the sqlite3_column_* functions but
 * achieve their goals via overloads and hence the actual number of visible
 * methods (vs. implemented methods) is reduced.  Binding is done in a similar manner.
 *
 * All errors are reported via CSqliteException throws.
 */
class CSqliteStatement {
public:
    static const int integer;
    static const int floating;
    static const int text;
    static const int blob;
    static const int null;
    
    typedef void (*ObjectDestructor)(void*);
private:
    typedef enum _IteratorStatus {
        interior,
        end
    } IteratorStatus;
private:
    CSqlite&       m_connection;
    sqlite3_stmt*  m_statement;
    IteratorStatus m_cursorState;
    bool           m_retriable;
    
    // Canonicals
public:
    CSqliteStatement(CSqlite& connection, const char* statement);
    virtual ~CSqliteStatement();
    
private:
    CSqliteStatement(const CSqliteStatement&);
    CSqliteStatement& operator=(const CSqliteStatement&);

    // Getters
public:    
    CSqlite&  database(); 
    sqlite3_stmt* statement();
    
    // Interface methods
    
    // Sqlite statements that don't need result sets or binding (e.g. ddl):
    
    static void execute(CSqlite& connection, const char* statement);
    
    // Binding parameters in prepared statements:
    // binding sqlite3_value's are defrerred for now
    
    int bindIndex(const char* name);
    void bind(
        int paramNo, const void* pBlob, int nBytes, ObjectDestructor destructor
    );
    //void bind(
    //    int paramNo, const void* pBlob, uint64_t nBytes,
    //    ObjectDestructor destructor
    //);
    void bind(int paramNo, double value);
    void bind(int paramNo, int value);
    void bind(int paramNo, int64_t value);
    void bind(int paramNo);
    void bind(
        int paramNo, const char* pValue, int nBytes,
        ObjectDestructor destructor
    );

    void bind(int Paramno, uint64_t nBytes);          // Blob filled with nulls.
    void clearBindings();
    
    
    // Iteration
    
    CSqliteStatement& operator++();             // Only prefix auto inc.    
    void reset();
    bool atEnd();
    
  
    // Get data from a step result:
    
    int bytes(int col);
    int bytes16(int col);
    const void* getBlob(int col);                // Blob
    double      getDouble(int col);
    int         getInt(int col);
    int64_t     getInt64(int col);
    const unsigned char* getText(int col);
    int         columnType(int col);
   
    
    // Miscelaneous:
    
    const char* sql();                        // SQL of the statement.
    int   lastInsertId();                     // Row Id of last insert.
    void  enableRetry() {m_retriable = true;}
    void  disableRetry() {m_retriable = false;}
    
    
    // private utilities.
private:

};

#endif