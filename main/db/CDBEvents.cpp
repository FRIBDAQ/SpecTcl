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
#include <sstream>
#include <memory>
using namespace ufmt;
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
 * @param pSaveSet  - the save set in which the run will be stored.
 * @param run       - run number.
 * @note we get ownership of the saveset, which is assumed to be
 *      dynamically allocated.
 */
CDBEventPlayer::CDBEventPlayer(SpecTclDB::SaveSet* pSaveSet, int run) :
  m_pSaveSet(pSaveSet),
  m_run(run),
  m_eventContext(nullptr)
{
  m_runId = m_pSaveSet->openRun(m_run);
  m_eventContext = m_pSaveSet->openEvents(m_runId);
}
/**
 * destructor:
 *    Just finalize the statement.
 */
CDBEventPlayer::~CDBEventPlayer()
{
  m_pSaveSet->closeEvents(m_eventContext);
  delete m_pSaveSet;
}
/**
 * Return a const reference to the next event. Note that the
 * contents of this will be ovewritten by subsequent calls to next
 * and by destruction of this object.
 * @return CDBEventPlayer::Event
 * @retval Event with 
 */
const SpecTclDB::SaveSet::Event&
CDBEventPlayer::next()
{
  
  if (!m_pSaveSet->readEvent(m_eventContext, m_Event)) {
    m_Event.clear(); // ensure it's empty.
  }
  return m_Event;
}
///////////////////////////////////////////////////////////////////
//  CDBEventWriter implementation.
//



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
  m_pDatabase(nullptr),
  m_pSaveSet(nullptr),
  m_eventsInTransaction(batchSize),
  m_eventsInCurrentTransaction(0),
  m_eventsInRun(0),
  m_nCurrentRun(-1),
  m_nRunId(-1),
  m_pInterpreter(nullptr)
{
    SpecTcl* pApi = SpecTcl::getInstance();
    
    m_pInterpreter = pApi->getInterpreter();
    m_pInterpreter->GlobalEval("package require dbconfig");   // Gets sqlite3 too.
    
    // Create the database schema and open it in C++ and Tcl:
    
    SpecTclDB::CDatabase::create(databaseFile);
    m_pDatabase = new SpecTclDB::CDatabase(databaseFile);
    
    std::stringstream connect;
    connect << "DBTcl connect " << databaseFile;
    m_dbCommand = m_pInterpreter->GlobalEval(connect.str());

    
    
}
/**
 * destructor
 *    - Close the Tcl database handle.
 *    - finalize the prepared statements.
 *    - close the sqlite C api handle.
 */
CDBEventWriter::~CDBEventWriter()
{
  std::stringstream closecmd;
  closecmd << m_dbCommand << " destroy";
  m_pInterpreter->GlobalEval(closecmd.str());
  delete m_pDatabase;
  delete m_pSaveSet;           // Delete nullptr is no-op.
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
    
    if ((m_savesetCommand != "") && (pBody->s_runNumber == m_nCurrentRun)) {
        return;
    }
    
    m_nCurrentRun = pBody->s_runNumber;        // Update the run number.
    m_eventsInRun = 0;
    
    
    std::string configId;
    {
        std::stringstream cmd;
        std::stringstream cfgname;
        cfgname << "run-" << pBody->s_runNumber;
        m_configName = cfgname.str();
        cmd << "dbconfig::saveConfig " << m_dbCommand << " " << m_configName;
        m_savesetCommand = m_pInterpreter->GlobalEval(cmd.str());  
    }
    m_pSaveSet = m_pDatabase->getSaveSet(m_configName.c_str());
    m_nRunId   = m_pSaveSet->startRun(
        m_nCurrentRun, pBody->s_title, pBody->s_Timestamp
      );
  }
  catch (std::exception& e) {

    std::cerr << " CDBEvents::BeginRun failed: " << e.what() << std::endl;
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
    
    if ((m_configName == "") || (m_nRunId < 0)) {
        return;
    }
    const StateChangeItemBody* pBody =
        reinterpret_cast<const StateChangeItemBody*>(getBody(pStateTransition));
    
    // If a transaction is in progress, finish it:
    
    if (m_eventsInCurrentTransaction > 0) {
        
        m_eventsInCurrentTransaction = 0;
        m_pSaveSet->endEvents(m_eventTransactionContext);
        m_eventTransactionContext = nullptr;
      }
    
    // Add the end run time to the run record.
    
    m_pSaveSet->endRun(m_nRunId, pBody->s_Timestamp);
    
    // Auto save the spectra -- before the configuration id gets set invalid.
    
    saveSpectra();
    
    // Set the state to an invalid run:
    
    m_nRunId = -1;
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
    
    if (m_nRunId < 0) {
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
    
   
    const ScalerItemBody* pScalerBody=
      reinterpret_cast<const ScalerItemBody*>(getBody(pScaler));
                                                                         ;
    
    // We're in a transaction pretty gauranteed because event writing puts us
    // there so we don't need to worry about atomicity:
    
    
  m_pSaveSet->saveScalers(
      m_nRunId, sourceId,
      pScalerBody->s_intervalStartOffset, pScalerBody->s_intervalEndOffset, pScalerBody->s_intervalDivisor,
      pScalerBody->s_timestamp,
      pScalerBody->s_scalerCount, pScalerBody->s_scalers
  );
    
    
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
    if ((!m_pSaveSet) || (m_nRunId < 0)) {
        throw std::logic_error("CDBEventWriter::event called without a current run");
    }
    // If we don't have a current id, assume that we joined in the
    // middle of the run and ignore:
    //
    if (m_nRunId < 0) {
        return;
    }
    
    
    DopeVector& dope = pEvent->getDopeVector();
    CEvent& e(*pEvent);
    int n = dope.size();
    if (n) {                              // Empty events don't count.
        if (m_eventsInCurrentTransaction == 0) {
            m_eventTransactionContext = m_pSaveSet->startEvents(m_nRunId);
        }
        // Marshall the dope vector and associated parameters
        // into flat arrays:

        std::unique_ptr<int> paramids(new int[n]);     // To automate destruction.
        std::unique_ptr<double> params(new double[n]);
        
        for (int i =0; i < n; i++) {
          int pno = dope[i];
          paramids.get()[i] = pno;
          params.get()[i]   = (*pEvent)[pno];
        }
        m_pSaveSet->saveEvent(
          m_nRunId, m_eventsInRun, n, paramids.get(), params.get()
        );
        
        m_eventsInCurrentTransaction++;
        m_eventsInRun++;
        
        // Commit a batch if appropriate.
        
        if(m_eventsInCurrentTransaction >= m_eventsInTransaction) {
            m_eventsInCurrentTransaction = 0;
            m_pSaveSet->endEvents(m_eventTransactionContext);
            m_eventTransactionContext = nullptr;
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
    
    // Need all savesets so we can iterate over them:
    
    auto savesets = m_pDatabase->getAllSaveSets();
    for (int i = 0; i < savesets.size(); i++) {
      auto runs = savesets[i]->listRuns();    // Run numbers in save-set
      
      // Iterate over the runs in a saveset:
      
      for (int r =0; r < runs.size(); r++) {
        int id = savesets[i]->openRun(runs[r]);
        auto info = savesets[i]->getRunInfo(id);
        RunInfo run;
        run.s_runNumber = runs[r];
        run.s_config    = savesets[i]->getInfo().s_name;
        run.s_title     = info.s_title;
        run.s_start     = info.s_startTime;
        run.s_end       = info.s_stopTime;
        run.s_hasEnd    = info.s_stopTime == 0 ? false : true;
        
        result.push_back(run);
      }
      
      delete savesets[i];         // Savesets were dynamically allocated. 
    }
    
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
 * @note While databases are allowed by the low level API to have
 * more than one run per save-set, SpecTcl's use of the database
 * does not. We therefore just use the first saveset that has
 * a run matching the run number given.
 * 
 */
CDBEventPlayer*
CDBEventWriter::playRun(int run)
{
  auto saveSets = m_pDatabase->getAllSaveSets();
  SpecTclDB::SaveSet* pSaveSet(nullptr);
  for (int s =0; s < saveSets.size(); s++) {
    auto runs = saveSets[s]->listRuns();
    bool found = false;
    for (int i =0; i < runs.size(); i++) {
      if (runs[i] == run) {
        found = true;
        pSaveSet = saveSets[s];
      }
    }
    if (!found) delete saveSets[s];
  }
  if (!pSaveSet) {
    std::stringstream msg;
    msg << "Can't find run" << run << " in the database " << m_dbName;
    throw std::invalid_argument(msg.str());
  }
  return new CDBEventPlayer(pSaveSet, run);
}
/////////////////////////////////////////////////////////////////////////////
// Private utilities


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
    baseCommand += m_savesetCommand;
    baseCommand += " ";
    
    
    for (int i =0; i < m_autoSaveSpectra.size(); i++) {
        std::string command(baseCommand);
        command += m_autoSaveSpectra[i];
        m_pInterpreter->GlobalEval(command);
    }
}
