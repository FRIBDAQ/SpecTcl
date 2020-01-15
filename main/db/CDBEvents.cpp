/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CDBEvents.cpp 
 *  @brief: Implement storage and retreival of events from database.
 */
#include "CDBEvents.h"
#include <DataFormat.h>
#include <Event.h>
#include <TCLInterpreter.h>
#include <sqlite3.h>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <SpecTcl.h>
#include <iostream>

int CDBEventWriter::m_dbCmdIndex(0);

/**
 ** constructor
 *   - Save the database name.
 *   - Get the SpecTcl interpreter and
 *      - load the sqlite3 and dbconfig packages.
 *      - Create the database in tcl.
 *      - Create the schema for the database.
 *   - Open the database in C++.
 *   - Set the events per transaction, and current transaction count.
 *   - Set the current run number id to -1 to indicate no current run.
 *
 *   @param databaseFile - path to the database file.
 *   @param batchSize    - Number of events written per commit (optional).
 */
CDBEventWriter::CDBEventWriter(const char* databaseFile, unsigned batchSize) :
    m_dbName(databaseFile),
    m_pInterp(nullptr),
    m_pSqlite(nullptr),
    m_pTransaction(nullptr),
    m_pInsert(nullptr),
    m_pCommit(nullptr),
    m_nCurrentRunId(-1),
    m_eventsInTransaction(batchSize),
    m_eventsInCurrentTransaction(0)
{
    SpecTcl* pApi = SpecTcl::getInstance();
    
    m_pInterp = pApi->getInterpreter();
    m_pInterp->GlobalEval("package require dbconfig");   // Gets sqlite3 too.
    m_dbCommand = nextCommand();
    
    // Open the database in Tcl and create the schema:
    
    {
        std::string sqliteCmd = "sqlite3 ";
        sqliteCmd += m_dbCommand;
        sqliteCmd += " ";
        sqliteCmd += m_dbName;
        m_pInterp->GlobalEval(sqliteCmd);
    
    }
    {
        std::string schemaCmd = "dbconfig::makeSchema ";
        schemaCmd += m_dbCommand;
        m_pInterp->GlobalEval(schemaCmd);
    }
    // Open the database in C/C++ and create the prepared statements:
    
    checkStatus(sqlite3_open(m_dbName.c_str(), &m_pSqlite));
    checkStatus(
        sqlite3_prepare(m_pSqlite, "BEGIN TRANSACTION", -1, &m_pTransaction, nullptr)
    );
    checkStatus(
        sqlite3_prepare(
            m_pSqlite,
            "INSERT INTO events (run_id, event_number,  parameter_num,  parameter_value) \
                                VALUES (:run, :eno, :pno, :val)",
            -1,&m_pInsert, nullptr
        )
    ); 
    checkStatus(
        sqlite3_prepare(m_pSqlite, "COMMIT TRANSACTION", -1, &m_pCommit, nullptr)
    );
}
/**
 * destructor
 *    - Close the Tcl database handle.
 *    - finalize the prepared statements.
 *    - close the sqlite C api handle.
 */
CDBEventWriter::~CDBEventWriter()
{
    std::string closecmd(m_dbCommand);
    closecmd += " ";
    closecmd += "close";
    m_pInterp->GlobalEval(closecmd);
    
    
    checkStatus(sqlite3_finalize(m_pTransaction));
    checkStatus(sqlite3_finalize(m_pInsert));
    checkStatus(sqlite3_finalize(m_pCommit));
    checkStatus(sqlite3_close(m_pSqlite));

    sqlite3_close(m_pSqlite);
    
}
/**
 * beginRun
 *     This must be called in response to a begin run.
 *     -  Require the ring item be a begin run item.
 *     -  Save the current SpecTcl configuration (get the id from  that).
 *     -  Prepare the statement to insert the runs table entry.
 *     -  Bind the columns to data in the ring item.
 *     -  Step the statement.
 *     -  Clear the statement bindings.
 *     -  Get the last row id and save it in m_currentRunId.
 * @note we can't save the run end time until endRun is called.
 * @param pStateTransition - Pointer to the ring item desscribing the begin run.
 * @throw std::invalid_argument if the ring item isnt a begin run.
 */
void
CDBEventWriter::beginRun(const RingItem* pStateTransition)
{
  try {
    requireItem(pStateTransition, BEGIN_RUN);
    const StateChangeItemBody* pBody =
        static_cast<const StateChangeItemBody*>(getBody(pStateTransition));
        
    // Save the current configuration:
    
    int configId;
    {
        std::stringstream cmd;
        std::stringstream cfgname;
        cfgname << "run-" << pBody->s_runNumber;
        m_configName = cfgname.str();
        cmd << "dbconfig::saveConfig " << m_dbCommand << " " << m_configName;
        std::string tclResult = m_pInterp->GlobalEval(cmd.str());
        cmd.str(tclResult);
        cmd >> configId;
        
    
    }
    m_nConfigId = configId;
    
    // Create the run record:
    
    sqlite3_stmt* pRun;
    checkStatus(
        sqlite3_prepare(
            m_pSqlite,
            "INSERT INTO runs (config_id, run_number, title, start_time) \
                    VALUES(:configId, :runnum, :title, :start)",
            -1, &pRun, nullptr
        )
    );
    checkStatus(sqlite3_bind_int(pRun, 1, m_nConfigId));
    checkStatus(sqlite3_bind_int(pRun, 2, pBody->s_runNumber));
    checkStatus(sqlite3_bind_text(
        pRun,3, pBody->s_title, -1, SQLITE_STATIC
    ));
    checkStatus(sqlite3_bind_int(pRun, 4, pBody->s_Timestamp));
    checkStatus(sqlite3_step(pRun), SQLITE_DONE);
    checkStatus(sqlite3_finalize(pRun));
    
    m_nCurrentRunId = sqlite3_last_insert_rowid(m_pSqlite);
    m_eventInRun    = 0;
  }
  catch (std::exception& e) {
    std::cerr << "CDBEventWriter failed in begin run: " << e.what();
    throw;
  }
  
}
/**
 * endRun
 *    -  Ensure we actually have a run open.
 *    -  Add the end timestamp to the record.
 *    -  Save any spectra the user wants saved.
 *    -  Set the book keeping parameters so that we don't have an open run.
 *
 *  @param pStateTransition - pointer to the end run ring item.
 *  @throw std::logic_error if the run is not open.
 */
void
CDBEventWriter::endRun(const RingItem* pStateTransition)
{
  try {
    requireItem(pStateTransition, END_RUN);
    if ((m_nConfigId < 0) || (m_nCurrentRunId < 0)) {
        throw std::logic_error("CDBEventWriter::endRun called with no open run");
    }
    const StateChangeItemBody* pBody =
        reinterpret_cast<const StateChangeItemBody*>(getBody(pStateTransition));
    
    // If a transaction is in progress, finish it:
    
    if (m_eventsInCurrentTransaction > 0) {
        checkStatus(sqlite3_step(m_pCommit), SQLITE_DONE);
        checkStatus(sqlite3_reset(m_pCommit));
        m_eventsInCurrentTransaction = 0;
    }
    
    // Add the end run time to the run record.
    
    sqlite3_stmt* pEnd;
    checkStatus(
        sqlite3_prepare(
            m_pSqlite, "UPDATE runs SET stop_time=:end WHERE id = :id", -1, &pEnd, nullptr
        )
    );
    checkStatus(sqlite3_bind_int(pEnd, 1, pBody->s_Timestamp));
    checkStatus(sqlite3_bind_int(pEnd, 2, m_nCurrentRunId));
    checkStatus(sqlite3_step(pEnd), SQLITE_DONE);
    checkStatus(sqlite3_finalize(pEnd));
    
    // Auto save the spectra -- before the configuration id gets set invalid.
    
    saveSpectra();
    
    // Set the state to an invalid run:
    
    m_nCurrentRunId = -1;
    m_nConfigId     = -1;
    m_eventsInCurrentTransaction = 0;
    
  }
  catch (std::exception& e) {
    std::cerr << "CDBEventWriter::endRun failed : " << e.what();
    throw;
  }
  
}
/**
 * scaler
 *    Logs a scaler item in to the database.
 *    This is not currently supported and does nothing.
 *
 * @param pScaler - ostensibly a pointer to a scaler ring item.
 */
void
CDBEventWriter::scaler(const RingItem* pScaler)
{}

/**
 * event
 *     Logs an event into the database.
 *     - Ensure a run is open else scream.
 *     - If m_eventsInCurrentTransaction is - start a transaction.
 *     - For each valid parameter (using the dope vector), add an entry to the
 *       events table.
 *     - Increment the transaction index and if its >= the limit commit.
 *
 *  @param  pEvent - pointer to the CEvent for this event.
 */
void
CDBEventWriter::event(CEvent* pEvent)
{
  try {
    if ((m_nConfigId < 0) || (m_nCurrentRunId < 0)) {
        throw std::logic_error("CDBEventWriter::event called without a current run");
    }
    DopeVector& dope = pEvent->getDopeVector();
    CEvent& e(*pEvent);
    int n = dope.size();
    if (n) {                              // Empty events don't count.
        if (m_eventsInCurrentTransaction == 0) {
            checkStatus(sqlite3_step(m_pTransaction), SQLITE_DONE);
            checkStatus(sqlite3_reset(m_pTransaction));
        }
        checkStatus(sqlite3_bind_int(m_pInsert, 1, m_nCurrentRunId));
	checkStatus(sqlite3_bind_int(m_pInsert, 2, m_eventInRun));
        for (int i =0; i < n; i++) {
            int pno = dope[i];
            double value = e[pno];
            
            checkStatus(sqlite3_bind_int(m_pInsert, 3, pno));
            checkStatus(sqlite3_bind_double(m_pInsert, 4, value));
            
            checkStatus(sqlite3_step(m_pInsert), SQLITE_DONE);
            checkStatus(sqlite3_reset(m_pInsert));
        }
        m_eventsInCurrentTransaction++;
	m_eventInRun++;
        
        // Commit a batch if appropriate.
        
        if(m_eventsInCurrentTransaction >= m_eventsInTransaction) {
            m_eventsInCurrentTransaction = 0;
            checkStatus(sqlite3_step(m_pCommit), SQLITE_DONE);
            checkStatus(sqlite3_reset(m_pCommit));
        }
    }
  }
  catch (std::exception& e) {
    std::cerr << "CDBEventWriter failed in event: " << e.what();
    throw;
  }

}

/**
 * setAutoSaveSpectra
 *    Provides a new set of spectra that will be auto saved
 *    into the run configuration when the run end is seen.
 *    Any existing set of autosave spectra are forgotten.
 *
 *  @param spectra - vector of spectra to save.
 *  
 */
void
CDBEventWriter::setAutoSaveSpectra(const std::vector<std::string>& spectra)
{
    m_autoSaveSpectra = spectra;
}
/**
 * addAutoSaveSpectrum
 *    Add a new spectrum to the set that will be saved at the end of the run.
 *
 *  @param name - name of the spectrum to add to the autosave set.
 */
void
CDBEventWriter::addAutoSaveSpectrum(const char* name)
{
    m_autoSaveSpectra.push_back(name);
}
/**
 * removeAutoSaveSpectrum
 *    If the spectrum named is in the save set, it is removed.
 *
 * @param name - the spectrum to remove from the save set.
*/
void
CDBEventWriter::removeAutoSaveSpectrum(const char* name)
{
    std::string sName(name);
    auto p = std::find(m_autoSaveSpectra.begin(), m_autoSaveSpectra.end(), sName);
    if (p != m_autoSaveSpectra.end()) m_autoSaveSpectra.erase(p);

}
/////////////////////////////////////////////////////////////////////////////
// Private utilities

/**
 * nextCommand
 *    Return the command string to use for the next command interface.
 *
 * @return std::string
 */
std::string
CDBEventWriter::nextCommand()
{
    std::stringstream sResult;
    sResult << "nscldaq_sqlitedb_" << m_dbCmdIndex++;
    return sResult.str();
}
/**
 * checkStatus
 *   Checks for the status of an Sqlite operation matching
 *   what's expected (default SQLITE_OK).  On mismatch throw an std::logic_error.
 *
 * @param status - the status value.
 * @param expected - the expected value (-1 -> SQLITE_OK)
 * @throw std::logic_error if mismatch.
 */
void
CDBEventWriter::checkStatus(int status, int expected)
{
    if (expected == -1) expected = SQLITE_OK;
    if (status != expected) {
        std::stringstream msg;
        msg << "sqlite3 function call failed: " << sqlite3_errstr(status);
        throw std::logic_error(msg.str());
    }
}
/**
 * requireItem
 *    Requires that a ring item be of a specific type.  Mismatches
 *    throw an error.
 *
 * @param pItem - poilnter to the ring item.
 * @param itemType - expected item type.
 * @throw std::invalid_argument if unexpected type.
 */
void
CDBEventWriter::requireItem(const RingItem* pItem, unsigned itemType)
{
    if (pItem->s_header.s_type != itemType) {
        std::stringstream msg;
        msg << "Invalid ring item type.  Expected " << itemType << " got "
            << pItem->s_header.s_type;
        throw std::invalid_argument(msg.str());
    }
}
/**
 * getBody
 *    Given a ring item returns a pointer to the ring item body.
 *    The assumption is that this is nscldaq-11 or later data.
 * @param pITem - pointer to the item.
 * @return const void* - pointer to the body (skipping body header).
 */
const void*
CDBEventWriter::getBody(const RingItem* pItem)
{
  
  if (pItem->s_body.u_noBodyHeader.s_mbz == 0) {
    return pItem->s_body.u_noBodyHeader.s_body;
  } else {
    // We want to allow for body  header extensions so:

    uint32_t size = pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size;
    const uint8_t* p    = reinterpret_cast<const uint8_t*>(&pItem->s_body.u_hasBodyHeader);
    p += size;
    return p;
   
  }
  
}
/**
 * saveSpectra
 *    Use Tcl to save the spectra in the auto save list.
 */
void
CDBEventWriter::saveSpectra()
{
    std::string baseCommand("dbconfig::saveSpectrum ");
    baseCommand += m_dbCommand;
    baseCommand += " ";
    baseCommand += m_configName;
    baseCommand += " ";
    
    for (int i =0; i < m_autoSaveSpectra.size(); i++) {
        std::string command(baseCommand);
        command += m_autoSaveSpectra[i];
        m_pInterp->GlobalEval(command);
    }
}
