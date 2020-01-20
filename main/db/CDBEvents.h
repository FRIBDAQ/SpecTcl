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

/** @file:  CDBEvents.h
 *  @brief: Store (Retrivev?) events from the sqlite config database.0
 */
#ifndef CDBEVENTS_H
#define CDBEVENTS_H

#include <vector>
#include <string>
#include <stdint.h>
#include <DataFormat.h>

class CEvent;
class CTCLInterpreter;
struct sqlite3;
struct sqlite3_stmt;

// These structs define the blob  data and how it's presented
// back to the client.

namespace DBEvent {
    typedef struct _blobELement {
        uint32_t s_parameterNumber;
        double   s_parameterValue;
    } blobElement, *pBlobelement;
    
    typedef std::vector<blobElement>  Event, *pEvent;
}

/**
 * @class CDBEventPlayer
 *    Provides a class that allows iteration through events.
 *    The representation of an event is a vector of
 *    pair<int, double> where the int is the parameter
 *    number and the double is the value of that parameter
 *    for that event.  This is surfficiently use neutral
 *    that it can be used in pretty much any analysis system
 *    (e.g. could make dataframes or root trees etc).
 */
class CDBEventPlayer {
public:
    typedef DBEvent::Event Event, *pEvent;
private:
    sqlite3*       m_pDatabase;
    sqlite3_stmt*  m_pRetriever;
    int            m_run;
    int            m_runId;
    
    int            m_eventNumber;
    DBEvent::Event m_currentEvent;     // So we can avoid copy.
    
public:
    CDBEventPlayer(sqlite3* pDatabase, int run);
    ~CDBEventPlayer();
    
    const Event& next();            // Empty means no more in the run.

};

/**
 * @class CDBEventWriter
 *    Provides a class for storing  events in an sqlite3
 *    database.  We assume that the dbconfig Tcl package is
 *    can be loaded/required as we're going to use it in non-rate critical stuff.
 *
 * Several runs can be stored in the same database.  run has associated
 * with it a configuration stored at the time the run started.
 * At the end of the run, optionally a set of spectra can be saved as well.
 */
class CDBEventWriter {
public:
    typedef struct _RunInfo {
        int             s_runNumber;
        std::string     s_config;
        std::string     s_title;
        time_t          s_start;
        time_t          s_end;
        bool            s_hasEnd;
    } RunInfo, *pRunInfo;
private:
    std::string        m_dbName;                 // name of the database.
    CTCLInterpreter*   m_pInterp;                // For dbconfig use.
    
    sqlite3*           m_pSqlite;                // Sqlite handle.
    std::string        m_dbCommand;
    sqlite3_stmt*      m_pTransaction;           // prepared transaction starter.
    sqlite3_stmt*      m_pInsert;                // Insertion prepared statement.
    sqlite3_stmt*      m_pCommit;                // Commit prepared statement.
    
    int64_t           m_nCurrentRunId;          // Id of current run.
    int64_t           m_nConfigId;              // Id of configuration.
    std::string       m_configName;
    
    std::vector<std::string> m_autoSaveSpectra;  // Names of spectra to auto save.
    unsigned           m_eventsInTransaction;     // More efficient to batch
    unsigned           m_eventsInCurrentTransaction; // events into a transaction.
    unsigned           m_eventInRun;             // Event number in the run.
    
    static int         m_dbCmdIndex;            // uniquifier for database command.
    
public:
    CDBEventWriter(const char* databaseFile, unsigned batchSize = 500);
    virtual ~CDBEventWriter();
    
    void beginRun(const RingItem* pStateTransition);
    void endRun(const RingItem*   pStateTransition);
    void scaler(const RingItem*   pScaler);
    void event(CEvent*      pEvent);
    
    //  For autosaving spectra at end of run.
    
    void setAutoSaveSpectra(const std::vector<std::string>& spectra);
    void addAutoSaveSpectrum(const char* name);
    void removeAutoSaveSpectrum(const char* name);
    
    // For query and playback:
    
    std::vector<RunInfo> listRuns();
    CDBEventPlayer* playRun(int run);

    
    static void checkStatus(int status, int expected=-1);
private:
    std::string nextCommand();
    
    void requireItem(const RingItem* pItem, unsigned itemType);
    const void* getBody(const RingItem* pItem);
    void saveSpectra();
    
};

#endif
