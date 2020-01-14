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
private:
    std::string nextCommand();
    void checkStatus(int status, int expected=-1);
    void requireItem(const RingItem* pItem, unsigned itemType);
    const void* getBody(const RingItem* pItem);
    void saveSpectra();
    
};

#endif