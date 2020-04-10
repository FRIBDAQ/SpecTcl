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
#include <string.h>


///////////////////////////////////////////////////////////////////
// CDBEventPlayer implementation.
//

/**
 * constructor
 *    Save the database and run number and fetch the
 *   runid.  If we can't, throw std::invalid_argument.
 *   If we can, construct and prepare the retrieval statement for
 *   next to step in.
 *
 * @param pDatabase - pointer to the databaes handle.
 * @param run       - run number.
 */
CDBEventPlayer::CDBEventPlayer(sqlite3* pDatabase, int run) :
  m_pDatabase(pDatabase),
  m_pRetriever(nullptr),
  m_run(run),
  m_runId(-1)
{
  sqlite3_stmt* pGetRunId;
  CDBEventWriter::checkStatus(sqlite3_prepare(
      m_pDatabase,
      "SELECT id, title FROM runs WHERE run_number = :run", -1, &pGetRunId,
      nullptr
  ));
  CDBEventWriter::checkStatus(sqlite3_bind_int(pGetRunId, 1, run));
  int status = sqlite3_step(pGetRunId);
  if (status == SQLITE_ROW) {
    m_runId = sqlite3_column_int(pGetRunId, 0);
    m_Title = reinterpret_cast<const char*>(sqlite3_column_text(pGetRunId, 1));
    CDBEventWriter::checkStatus(sqlite3_finalize(pGetRunId));
      
    // Now prepare and bind the m_pRetriever used by next:
    
    CDBEventWriter::checkStatus(sqlite3_prepare(
      m_pDatabase,
      "SELECT event_number, parameter_count, event_data \
        FROM events WHERE run_id = :id",
        -1, &m_pRetriever, nullptr
    ));
  
    CDBEventWriter::checkStatus(sqlite3_bind_int(m_pRetriever, 1, m_runId));
          
    
    // All done.
    
  } else {
    throw std::invalid_argument("No such run in database");
  }
  
}
/**
 * destructor:
 *    Just finalize the statement.
 */
CDBEventPlayer::~CDBEventPlayer()
{
  sqlite3_finalize(m_pRetriever);
}
/**
 * Return a const reference to the next event. Note that the
 * contents of this will be ovewritten by subsequent calls to next
 * and by destruction of this object.
 * @return CDBEventPlayer::Event
 * @retval Event with 
 */
const CDBEventPlayer::Event&
CDBEventPlayer::next()
{
  
  if (sqlite3_step(m_pRetriever) == SQLITE_ROW) {
      // How many parameters do we have?
      
      int m_eventNumber = sqlite3_column_int(m_pRetriever, 0);
      int paramCount = sqlite3_column_int(m_pRetriever, 1);
      m_currentEvent.resize(paramCount);    // make the storage.
      const void* pBlob = sqlite3_column_blob(m_pRetriever, 2);
      memcpy(
        m_currentEvent.data(), pBlob,
        paramCount*sizeof(DBEvent::blobElement)
      );
      
  }  else {
    m_currentEvent.clear();   // Empty event signals end of iteration.
  }
  return m_currentEvent;
}
///////////////////////////////////////////////////////////////////
//  CDBEventWriter implementation.
//

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
    m_eventsInCurrentTransaction(0),
    m_eventInRun(0),
    m_nCurrentRun(0)
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
        sqlite3_prepare(
            m_pSqlite,
            "INSERT INTO events \
              (run_id, event_number, parameter_count,  event_data) \
              VALUES (:run, :eno, :nparam, :event)",
            -1, &m_pInsert, nullptr
        )
    );
    // Prepare the transaction/commit statements:
    checkStatus(
      sqlite3_prepare(m_pSqlite, "BEGIN TRANSACTION", -1, &m_pTransaction, nullptr)
    );
    checkStatus(
      sqlite3_prepare(m_pSqlite, "COMMIT", -1, &m_pCommit, nullptr)
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
    
    
    sqlite3_finalize(m_pTransaction);
    sqlite3_finalize(m_pInsert);
    sqlite3_finalize(m_pCommit);
    sqlite3_close(m_pSqlite);

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
 * 
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
    
    // If we've already got a configuration id and the
    // run number from this state change body matches our current
    // run number, we assume this is event built data for which
    // we've already seen the first begin run and ignore this:
    // If we have a configuration going, however, but this is a
    // different run number, we assume that the prior run didn't
    // end with end runs so wwe make a new configuration:
    
    if ((m_nConfigId > 0) && (pBody->s_runNumber == m_nCurrentRun)) {
        return;
    }
    
    m_nCurrentRun = pBody->s_runNumber;        // Update the run number.
    
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
    checkStatus(sqlite3_bind_int(m_pInsert, 1, m_nCurrentRunId));  // same for all events.
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
    
    // Note that if we see an end run, with no open run,
    // we'll just return assuming this is an additional end run from
    // event built data... or we joined in the middle.
    
    if ((m_nConfigId < 0) || (m_nCurrentRunId < 0)) {
        return;
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
{
    // If we don't have a current run id just return
    // assuming we joined in the middle:
    
    if (m_nCurrentRunId < 0) {
        return;
    }
    
    if (pScaler->s_header.s_type != PERIODIC_SCALERS) {
      throw std::invalid_argument("Non scaler ring item passed to CDBEventWriter::scaler");
    }
    // Get the source id... if there's no body header we use zero:
    
    uint32_t sourceId = 0;
    if (pScaler->s_body.u_noBodyHeader.s_mbz != 0) {
        sourceId = pScaler->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId;
    }
    // Locate the scaler body. Note that while we don't, yet have
    // scalers with body header extensions, we'll assume that at some point
    // we might:
    
    const uint32_t* pBodyHeaderSize = reinterpret_cast<const uint32_t*>(
        &(pScaler->s_body.u_noBodyHeader.s_mbz)
    );
    const ScalerItemBody* pScalerBody(0);
    if (*pBodyHeaderSize == 0) {
        pScalerBody = reinterpret_cast<const ScalerItemBody*>(pBodyHeaderSize+1);
    } else {
        uint32_t nBytes = *pBodyHeaderSize;
        const uint8_t* p8 = reinterpret_cast<const uint8_t*>(pBodyHeaderSize);
        p8 += nBytes;
        pScalerBody = reinterpret_cast<const ScalerItemBody*>(p8);
    }
    // We're in a transaction pretty gauranteed because event writing puts us
    // there so we don't need to worry about atomicity:
    // Scalers are assumed to be at low rate so we prep/bind/execute here.
    
    // Root record for the readout:
    
    sqlite3_stmt* pRoot;
    checkStatus(sqlite3_prepare(
        m_pSqlite,
        "INSERT INTO scaler_readouts                                          \
            (run_id, source_id, start_offset, stop_offset, divisor, clock_time)      \
            VALUES(:run, :src, :start, :stop, :divisor, :clock)",
        -1, &pRoot, nullptr
    ));
    checkStatus(sqlite3_bind_int(pRoot, 1, m_nCurrentRunId));
    checkStatus(sqlite3_bind_int(pRoot, 2, sourceId));
    checkStatus(sqlite3_bind_int(pRoot, 3, pScalerBody->s_intervalStartOffset));
    checkStatus(sqlite3_bind_int(pRoot, 4, pScalerBody->s_intervalEndOffset));
    checkStatus(sqlite3_bind_int(pRoot, 5, pScalerBody->s_intervalDivisor));
    checkStatus(sqlite3_bind_int(pRoot, 6, pScalerBody->s_timestamp));
    checkStatus(sqlite3_step(pRoot), SQLITE_DONE);
    checkStatus(sqlite3_finalize(pRoot));
    
    // Reference id for the scaler items:
    
    sqlite3_int64 lastid = sqlite3_last_insert_rowid(m_pSqlite);
    
    // Each Scaler Item:
    
    sqlite3_stmt* pChannel;
    checkStatus(sqlite3_prepare(
        m_pSqlite,
        "INSERT INTO scaler_channels (readout_id, channel, value)          \
            VALUES (:rdoid, :channel, :value)", -1, &pChannel, nullptr
    ));
    // We can bind the rdo id now and leave it bound through the
    // loop over channels:
    
    checkStatus(sqlite3_bind_int64(pChannel, 1, lastid));
    const uint32_t* pValues = pScalerBody->s_scalers;
    
    for (int i =0; i < pScalerBody->s_scalerCount; i++) {
        checkStatus(sqlite3_bind_int(pChannel, 2, i));
        checkStatus(sqlite3_bind_int(pChannel, 3, *pValues++));
        checkStatus(sqlite3_step(pChannel), SQLITE_DONE);
        checkStatus(sqlite3_reset(pChannel));
    }
    checkStatus(sqlite3_finalize(pChannel));
}

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
    // If we don't have a current id, assume that we joined in the
    // middle of the run and ignore:
    //
    if (m_nCurrentRunId < 0) {
        return;
    }
    
    
    DopeVector& dope = pEvent->getDopeVector();
    CEvent& e(*pEvent);
    int n = dope.size();
    if (n) {                              // Empty events don't count.
        if (m_eventsInCurrentTransaction == 0) {
            checkStatus(sqlite3_step(m_pTransaction), SQLITE_DONE);
            checkStatus(sqlite3_reset(m_pTransaction));
        }
        
        checkStatus(sqlite3_bind_int(m_pInsert, 2, m_eventInRun));
        DBEvent::Event  blob;
        for (int i =0; i < n; i++) {
            DBEvent::blobElement param;
            int pno = dope[i];
            param.s_parameterNumber = pno;
            param.s_parameterValue  = e[pno];
            blob.push_back(param);
        }
        checkStatus(sqlite3_bind_int(m_pInsert, 2, m_eventInRun));
        checkStatus(sqlite3_bind_int(m_pInsert, 3, n));
        checkStatus(sqlite3_bind_blob(
          m_pInsert, 4, blob.data(), n*sizeof(DBEvent::blobElement),
          SQLITE_STATIC
        ));
        checkStatus(sqlite3_step(m_pInsert), SQLITE_DONE);
        checkStatus(sqlite3_reset(m_pInsert));
        
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
/**
 * listRuns
 *    Provides a list of all the runs that are in database.
 * @return std::vector<RunInfo> (note this could be empty).
 */
std::vector<CDBEventWriter::RunInfo>
CDBEventWriter::listRuns()
{
    std::vector<RunInfo> result;
    sqlite3_stmt* pList;
    checkStatus(
        sqlite3_prepare(
            m_pSqlite,
            "SELECT run_number, name, title, start_time, stop_time \
                    FROM runs                                      \
                    INNER JOIN save_sets ON runs.config_id = save_sets.id;",
            -1, &pList, nullptr
        )
    );
    int stat;
    while ((stat = sqlite3_step(pList)) == SQLITE_ROW) {
        RunInfo record;
        record.s_runNumber = sqlite3_column_int(pList, 0);
        record.s_config    =
            reinterpret_cast<const char*>(sqlite3_column_text(pList, 1));
        record.s_title     =
            reinterpret_cast<const char*>(sqlite3_column_text(pList, 2));
        record.s_start     = sqlite3_column_int(pList, 3);
        
        // The s_end could be a null if the run never ended:
        
        int haveEnd       = sqlite3_column_type(pList, 4);
        if (haveEnd == SQLITE_NULL) {
            record.s_hasEnd = false;
        } else {
            record.s_hasEnd = true;
            record.s_end   = sqlite3_column_int(pList, 4);
        }
        result.push_back(record);
    }
    checkStatus(sqlite3_finalize(pList));
    
    return result;
}
/**
 * playRun
 *    Returns an event player from which events can be gotten.
 *    At some point this may become a base class from which
 *    classes can be derived to put some criteria on the
 *    events to select.  At this point all events in the run will
 *    be selected.
 * @param run - number of the run in the database to select.
 * @return CDBEventPlayer* the caller owns and must dispose of this object.
 */
CDBEventPlayer*
CDBEventWriter::playRun(int run)
{
  return new CDBEventPlayer(m_pSqlite, run);
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
