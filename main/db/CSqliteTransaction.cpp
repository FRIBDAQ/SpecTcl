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
# @file   CSqliteTransaction.cpp
# @brief  Implement auto-commit transaction and its associated exception.
# @author <fox@nscl.msu.edu>
*/

#include "CSqliteTransaction.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include <sstream>
/*----------------------------------------------------------------------------*/
/**
 * Implement the exception class used by the transaction methods:
 */

/**
 * constructor
 *  @param message - the error message
 */
CSqliteTransaction::CException::CException(std::string message) noexcept :
    m_message(message) {}
    
/**
 * copy constructor
 * @param rhs - the exception from which we are constructed.
 */
CSqliteTransaction::CException::CException(const CException& rhs) noexcept :
    m_message(rhs.m_message) {}

/**
 * assignmentCSq
 *  @param rhs - The object being assigned to this.
 *  @return *this
 */
CSqliteTransaction::CException&
CSqliteTransaction::CException::operator=(const CException& rhs) noexcept 
{
    m_message = rhs.m_message;
    return *this;
}

/**
 * what
 *  @return const char*  - the error message string.
 */
const char*
CSqliteTransaction::CException::what() const noexcept
{
    return m_message.c_str();
}

/*----------------------------------------------------------------------------*/
/**
 * Implement the CSqliteTransaction class itself.
 */
/**
 * constructor
 *    This construtor allows us to be encapsulated in a CSavepoint class:
 *  @param db       - database.
 *  @param startCmd    - Command to start the transaction.
 *  @param rollback - Command to rollback the transaction.
 *  @param commit   - Command to commit the transaction
 */
CSqliteTransaction::CSqliteTransaction(
    CSqlite& db, const char* startCmd, const char* rollback, const char* commit
) :
    m_db(db), m_state(active),
    m_startCommand(startCmd),
    m_rollbackCommand(rollback),
    m_commitCommand(commit)
{
    start();
}
/**
 * constructor
 *    Save the database, set the transaction state to active,
 *    and exectue a BEGIN TRANSACTION
 * @param db - The database on which the transaction is starting:
 */
CSqliteTransaction::CSqliteTransaction(CSqlite& db) :
    m_db(db), m_state(active),
    m_startCommand("BEGIN DEFERRED TRANSACTION"),
    m_rollbackCommand("ROLLBACK TRANSACTION"),
    m_commitCommand("COMMIT TRANSACTION")
{
    start();  
}


/**
 * destructor
 *    Dispose of the transaction properly:
 *    - Active COMMIT
 *    - rollbackpending ROLLBACK
 *    - completed do nothing.
 */
CSqliteTransaction::~CSqliteTransaction()
{
    if (m_state == active) {
        commit();
    } else if (m_state == rollbackpending) {
        rollback();
    }

}
/**
 * start
 *    Initiate the transaction
 */
void
CSqliteTransaction::start()
{
    CSqliteStatement::execute(m_db, m_startCommand.c_str());
}
/**
 * rollback
 *    Rollback the transaction now.  The state is set to completed so
 *    destruction is a no-op.
 */
void CSqliteTransaction::rollback()
{
    switch(m_state) {
        case active:
        case rollbackpending:
            {
                // Some cases can be multiple commands
                
                m_state = completed;
                std::string command;
                std::istringstream f(m_rollbackCommand);
                while (getline(f, command, ';')) {
                    CSqliteStatement::execute(m_db, command.c_str());
                }
               
            }
            break;
        case completed:
            throw CException("Rollback attemped on completed transaction");
            break;
    }
}
/**
 * scheduleRollback
 *   Mark the transaction to be rolled back on destruction.
 */
void CSqliteTransaction::scheduleRollback()
{
    switch(m_state) {
        case active:
            m_state = rollbackpending;
        case rollbackpending:
            break;
        case completed:
            throw CException("Attempted scheduleRollback on completed transaction");
    }
    
}
/**
 * commit
 *    Do a commit now.  The state of the transaction is set to completed so
 *    the destructor is a no-op.
 */
void CSqliteTransaction::commit()
{
    switch (m_state) {
        case active:
            m_state = completed;
            CSqliteStatement::execute(m_db, m_commitCommand.c_str());    
            break;
        case completed:
            throw CException("Attempting to commmit a completed transaction");
            break;
        case rollbackpending:
            throw CException("Attempting to commit when rollback is pending");
            break;
    }
}

/**
 *  Implement a save point:
 */

/**
 * constructor
 *    @param db - The database handle (reference).
 *    @param name - The name of the save point.
 */
CSqliteSavePoint::CSqliteSavePoint(CSqlite& db, const char* name) :
    CSqliteTransaction(
        db, startCommand(name).c_str(), rollbackCommand(name).c_str(),
        commitCommand(name).c_str()
    )
{}


/**
 * startCommand
 *    Create the string SAVEPOINT name
 *
 *  @param name -name of the savepoint.
 *  @return std::string
 */
std::string
CSqliteSavePoint::startCommand(const char* name)
{
    std::string result = "SAVEPOINT ";
    result            += name;
    return result;
}
/**
 * rollbackCommand
 *    Create the string:  ROLLBACK TRANSACTION TO SAVEPOINT name
 *
 *  @param name
 *  @return std::string
 */
std::string
CSqliteSavePoint::rollbackCommand(const char* name)
{
    std::string result = "ROLLBACK TRANSACTION TO SAVEPOINT ";
    result += name;
    result += "; ";
    result += commitCommand(name);
    return result;
}

/**
 *  commitCommand
 *     Create the string RELEASE SAVEPOINT name
 *
 *   @param name - the name of the save point to 'commit'.
 *   @return std::string
 */
std::string
CSqliteSavePoint::commitCommand(const char* name)
{
    std::string result = "RELEASE SAVEPOINT ";
    result += name;
    return result;
}