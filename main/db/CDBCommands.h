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

/** @file:  CDBCommands.h
 *  @brief: Tcl command processors for database event files.
 */
#ifndef CDBCOMMAND_H
#define CDBCOMMAND_H

#include <TCLObjectProcessor.h>
#include "CDBEvents.h"
#include <tcl.h>

class CDBProcessor;


/**
 * @class CDBCommand
 *    This is a command ensemble that interfaces SpecTcl's event flow to
 *    the events part of a database.  The base command is just daqdb. It has
 *    the following subcommands:
 *
 *    - open file-name  - Opens a database also sets up the event processing
 *                        if that's  not yet been done.  If there's an existing
 *                        open file, it's closed and a new one replaces it.
 *                        The objects are disabled. first.
 *    - enable          - beginning with the next run, record events into the
 *                        database.
 *    - disable         - Starting *now* events will no longer be recorded in the
 *                        database.
 *    - close           - Close the database and teardown the event processor/sink
 *                        this also happens immediately.
 *    - autosave list   - Sets the list of spectra to autosave - empty list for none.
 *    - daqdb listruns  - lists the runs in the current event file.
 *    - daqdb play      - Playback the specified run number.
 *    - daqdb stop      - Stop playback.
 *        
 *  @note disable and close should not be performed while a run is open.
 *        the result will be that the events recorded so far will be there,
 *        but the end run time will be NULL in the Run record.
 *  @note open disables writing because otherwise replacing the database file
 *        in the middle of recording a run to the database will results in
 *        orphaned events (events without an owning event record).
 */
class CDBCommands : public CTCLObjectProcessor
{
private:
    CDBEventWriter* m_pWriter;
    CDBProcessor*   m_pEventProcessor;
    bool            m_enabled;
    int             m_nProcessorIndex;
    CDBEventPlayer* m_pPlayback;
public:
    CDBCommands(CTCLInterpreter& interp, const char* name = "daqdb");
    virtual ~CDBCommands();
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
protected:
    void dbOpen(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void dbEnable(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void dbDisable(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void dbClose(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void dbAutoSave(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void dbListRuns(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void dbPlay(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void dbStopPlayback(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
private:
    void requireOpen();
    void requireEnabled();
    void requireDisabled();
    std::string processorName();
    void makeRunInfoDict(const CDBEventWriter::RunInfo& info, CTCLObject& dict);
    
    void addKey(Tcl_Interp* pInterp, Tcl_Obj* pDict, const char* pKey, int value);
    void addKey(
        Tcl_Interp* pInterp, Tcl_Obj* pDict,
        const char* pKey, const std::string& value
    );
    void addKey(
        Tcl_Interp* pInterp, Tcl_Obj* pDict, const char* pKey, time_t value
    );
    Tcl_Obj* stringToObj(const char* pString);
    
    void setActive(CTCLInterpreter& interp, int run, const char* title="");
    void setInactive(CTCLInterpreter& interp);
    void setBuffersAnalyzed(CTCLInterpreter& interp, int n);
    
};
#endif