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
#include "SpecTclDatabase.h"
#include "SaveSet.h"

class CEvent;
class CTCLInterpreter;
struct sqlite3;
struct sqlite3_stmt;





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

private:
    SpecTclDB::SaveSet* m_pSaveSet;
    
    int            m_run;
    int            m_runId;
    void*          m_eventContext;
    std::string    m_Title;           // Title of current run.
    SpecTclDB::SaveSet::Event m_Event;
    
public:
    CDBEventPlayer(SpecTclDB::SaveSet* pSaveSet, int run);
    ~CDBEventPlayer();
    
    const SpecTclDB::SaveSet::Event& next();    // Empty means no more in the run.
    std::string getTitle() const {return m_Title; }

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
    SpecTclDB::CDatabase* m_pDatabase;
    SpecTclDB::SaveSet*  m_pSaveSet;
    std::string        m_configName;
    std::vector<std::string> m_autoSaveSpectra;  // Names of spectra to auto save.
    unsigned           m_eventsInTransaction;     // More efficient to batch
    unsigned           m_eventsInCurrentTransaction; // events into a transaction.
    unsigned           m_eventsInRun;             // Event number in the run.
    int                m_nCurrentRun;            // Current run number.
    int                m_nRunId;
    
    CTCLInterpreter*   m_pInterpreter;          // Some things are easier in Tcl.
    std::string        m_dbCommand;             // Database command.
    std::string        m_savesetCommand;        // save set command.
    void*              m_eventTransactionContext;
    
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
    std::string getDbPath() { return m_dbName; }
private:
    
    void requireItem(const RingItem* pItem, unsigned itemType);
    const void* getBody(const RingItem* pItem);
    void saveSpectra();
    
};

#endif
