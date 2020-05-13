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
# @file   CSqliteTransaction.h
# @brief  Sqlite transaction support class
# @author <fox@nscl.msu.edu>
*/


#ifndef __CSQLITETRANSACTION_H
#define __CSQLITETRANSACTION_H

#include <exception>
#include <string>

class CSqlite;

/**
 * @class CSqliteTransaction
 *
 * This provides a class whose instances start a transtation and, when
 * destroyed, unless otherwise directed commit the transaction.  This is
 * best shown via a set of code fragments:
 *
 * Normal use to begin and commit a transaction:
 * \code
 {
    CTransaction begin(db);               // Starts the transaction.
    ...
    ... 
 }                                   // Commits the transaction.
 
 *  \code
 *
 *  Explicitly rolling back a transaction:
 *
 *  \code
{
    CTransaction begin(db);
    ...
    begin.rollback();               // Rollback right here and now.
    ...
}                                   // Nothing happens at destruct time.                             
*  \code
*
*  Scheduling a deferred rollback:
*
*\code
{
    CTransaction begin(db);             // transaction starts.
    ...
    begin.scheduleRollback();       // Mark transaction for rollback.
    ....
}                                   // Transaction actually rolled back here.
 * \code
*/

class CSqliteTransaction {
public:
    typedef enum _transactionState {
        active, rollbackpending, completed
    } TransactionState;
private:    
    CSqlite&         m_db;
    TransactionState m_state;
    std::string      m_startCommand;
    std::string      m_rollbackCommand;
    std::string      m_commitCommand;
public:
    CSqliteTransaction(CSqlite& db);
    CSqliteTransaction(
        CSqlite& db, const char* start, const char* rollback, const char* commit
    );
    virtual ~CSqliteTransaction();
    
    // Operations on the transaction:
    
    void start();
    void rollback();                // Rollback now.
    void scheduleRollback();                // Rollback on destruction
    void commit();                          // early commit.
    TransactionState state() const {return m_state;}
    
public:
    // This exception is thrown when you do somethign stupid with the transaction
    // state ...e.g. commit() a transaction that has rollbackpending or is completed.
    
    class CException : public std::exception
    {
        private:
           std::string m_message;
        public:
            CException(std::string message) noexcept;
            CException(const CException& rhs) noexcept;
        public:
            CException& operator=(const CException& rhs) noexcept;
            virtual const char* what() const noexcept;
    };
};

/**
 * @class CSqliteSavePoint
 *
 *      Support for the SAVEPOINT, RELEASE, ROLLBACK
 *      form of transactions.
 *      The strategy is to construct a transaction with the right commands.
 */
class CSqliteSavePoint : public CSqliteTransaction
{
public:
    CSqliteSavePoint(CSqlite& db, const char* name);

private:
    static std::string startCommand(const char* name);
    static std::string rollbackCommand(const char* name);
    static std::string commitCommand(const char* name);
};

#endif                  // __CSQLITETRANSACTION_H