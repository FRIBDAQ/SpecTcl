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

/** @file:  SaveSet.cpp
 *  @brief:  Implementation of the SpecTclDB::SaveSet class.
 */
#include "SaveSet.h"
#include "DBParameter.h"
#include "DBSpectrum.h"
#include "DBGate.h"
#include "DBApplications.h"
#include "DBTreeVariable.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "CSqliteTransaction.h"

#include <sqlite3.h>

#include <sstream>
#include <stdexcept>
#include <stddef.h>
#include <time.h>

namespace SpecTclDB{
/**
 * constructor
 *    - Locate the saveset in the connected database.
 *    = fetch the information block for the item.
 *  @param conn - references the connection to the database.
 *  @param name - the name of the save set to use.
 */
SaveSet::SaveSet(CSqlite& conn, const char* name) :
    m_connection(conn)
{
    CSqliteStatement s(
        conn,
        "SELECT id,name,timestamp from save_sets WHERE name=?"
    );
    s.bind(1, name, -1, SQLITE_STATIC);
    
    ++s;
    if(s.atEnd()) {
        std::stringstream e;
        e << "There is no save-set named: " << name;
        throw std::invalid_argument(e.str());
    }
    loadInfo(m_Info, s);
}
/**
 * constructor
 *   Same as above but we know the id:
 * @param id - the id of the save set.
 */
SaveSet::SaveSet(CSqlite& conn, int id) :
    m_connection(conn)
{
    CSqliteStatement s(
        conn,
        "SELECT id,name,timestamp from save_sets WHERE id=?"
    );
    s.bind(1, id);
    
    ++s;
    if(s.atEnd()) {
        std::stringstream e;
        e << "There is no save-set with the id : " << id;
        throw std::invalid_argument(e.str());
    }
    loadInfo(m_Info, s);
}

////////////////////////////////
// Parameter API implementation
/**
 * listParameters
 *    Return a vector of pointers to all defined parameters.
 *    Note that the pointers are to dynamically created objects
 *    that must be deleted when the caller is done with them.
 * @return std::vector<DBParameter*>  the list of parameters.
 * @note delegates to DBParameters::list
 */
std::vector<DBParameter*>
SaveSet::listParameters()
{
    return DBParameter::list(m_connection, m_Info.s_id);
}
/**
 * createParameter
 *    Wrapper for DBParameter::create  simple parameter.
 * @param name - name of the parameter.
 * @param number - parameter number.
 * @return DBParameter* - pointer to a dynamically allocated new parameter
 *         that wraps what we created.
 */
DBParameter*
SaveSet::createParameter(const char* name, int number)
{
    return DBParameter::create(m_connection, m_Info.s_id, name, number);
}
/**
 * createParameter
 *    wrapper for DBParameter::create with metadata.
 *  @param name - parameter name.
 *  @param number - parameter number.
 *  @param low  - parameter low limit.
 *  @param high - parameter high glimit.
 *  @param bins - parameter bins.
 *  @param units - parameter units.
 */
DBParameter*
SaveSet::createParameter(
    const char* name, int number,
    double low, double high, int bins, const char* units
)
{
    return DBParameter::create(
        m_connection, m_Info.s_id, name, number,
        low, high, bins, units
    );
}
/**
 * findParameter
 *    Retrieve a paramete from our save set given its name
 *  @param name - parameter name.
 *  @return DBParameter*
 */
DBParameter*
SaveSet::findParameter(const char* name)
{
    return new DBParameter(m_connection, m_Info.s_id, name);
}
/**
 * findParameter
 *    Same as above but find by parameter number.
 * @param number - parameter number to retrieve.
 * @return DBParameter*
 */
DBParameter*
SaveSet::findParameter(int number)
{
    return new DBParameter(m_connection, m_Info.s_id, number);
}
/**
 * getParameter
 *   Get a parameter given its id.  This differs from the
 *   version of findParameter above because find fetches the
 *   parameter by its parameter number while we are fetching it
 *   by its primary key (id field).
 * @param id - the id to fetch.
 * @return DBPararmeter* - pointer to dynamically allocated parameter.
 */
DBParameter*
SaveSet::getParameter(int id)
{
    return DBParameter::get(m_connection, m_Info.s_id, id);
}
///////////////////////////////
// Spectrum API implementation

/**
 * spectrumExists
 *  Wraps the DBSpectrum::exists method.
 * @param name - name of the spectrum to check on
 * @return bool
 */
bool
SaveSet::spectrumExists(const char* name)
{
    return DBSpectrum::exists(m_connection, m_Info.s_id, name);
}
/**
 * createSpectrum
 *   Wraps the DBSpectrum::create static method.
 * @param name - name of the spectrum being created.
 * @param type - type of spectrum.
 * @param parmeterNames - names of the parameters the spectrum needs.
 * @param axes - axis specifications.
 * @param datatype - datatype for each channel.
 * @return DBSpectrum* - pointer to a dynamically created spectrum.
 */
DBSpectrum*
SaveSet::createSpectrum(
    const char* name, const char* type,
    const std::vector<const char*>& parameterNames,
    const std::vector<SpectrumAxis>& axes,
    const char* datatype
)
{
    // We needed to define SpectrumAxis becaus we can't declare
    // DBSpectrum::Axis forward e.g.
    
    DBSpectrum::Axes a;
    for (int i =0; i < axes.size(); i++) {
        DBSpectrum::Axis ax;
        ax.s_low  = axes[i].s_low;
        ax.s_high = axes[i].s_high;
        ax.s_bins = axes[i].s_bins;
        a.push_back(ax);
    }
    
    return DBSpectrum::create(
        m_connection, m_Info.s_id,
        name, type, parameterNames, a, datatype
    );
}
/**
 * listSpectra
 *    Jacked for DBSpectra::list
 *
 * @return std::vector<DBSpectrum*> -the spectra in the saveset.
 */
std::vector<DBSpectrum*>
SaveSet::listSpectra()
{
    return DBSpectrum::list(m_connection, m_Info.s_id);
}
/**
 * lookupSpectrum
 *   Wrapper for spectrum loopu (name based constructor)
 * @param name - name of the spectrum.
 * @return DBSpectrum*
 */
DBSpectrum*
SaveSet::lookupSpectrum(const char* name)
{
    return new DBSpectrum(m_connection, m_Info.s_id, name);
}

//////////////////////
// Gate api:

/**
 * gateExists
 *   @const char* name - name of the gate to check out.
 *   @return bool - true if that gate exists in this save set.
 */
bool
SaveSet::gateExists(const char* name)
{
    return DBGate::exists(m_connection, m_Info.s_id, name);
}
/**
 * create1dGate
 *    Creates a 1d gate in this saveset.
 * @param name - name of the new gate.
 * @param type - gate type (e.g. "s").
 * @param params - vector of pointers to parameter names the gate needs.
 * @param low,   - gate low limit.
 * @param high   - gate high limit.
 * @return DBGate* - pointer to the new gate object that was entered in
 *                   the database.
 * @note the return value is dynamically generated and must be
 *        deleted by the caller
 */
DBGate*
SaveSet::create1dGate(
     const char* name, const char* type,
    const std::vector<const char*>& params, double low, double high
)
{
    return DBGate::create1dGate(
        m_connection, m_Info.s_id,
        name, type, params, low, high
    );
}
/**
 * create2dGate
 *   Cretes a 2d gate in this saveset.
 * @param name -name of the new gate.
 * @param type -gate type (e.g. "c").
 * @param parms - parameters the gate depends on.
 * @param points - The x/y points in the gate.
 * @return DBGate* - pointer to the newly created/entered gate object.
 * @note caller must eventually delete the returned pointer.
 */
DBGate*
SaveSet::create2dGate(
    const char* name, const char* type,
    const std::vector<const char*>& params,
    const std::vector<std::pair<double, double>>& points
    
)
{
    // Need to marshallthe points into the correct struct:
    
    DBGate::Points pts;
    for (int i =0; i < points.size(); i++) {
        pts.push_back({points[i].first, points[i].second});
    }
    return DBGate::create2dGate(
        m_connection, m_Info.s_id,
        name, type, params, pts
    );
}

/**
 * createCompoundGate
 *    Creates a gate that depends on other gates (e.g. "*" type)
 *     in this saveset.
 *
 *   @param name -new gate name.
 *   @param type - new gate type (e.g. *).
 *   @param gates - Gates this gate depends on.
 *   @return DBGate* - pointer to the gate object that encapsulates
 *                 the newly entered gate.
 *   @note The caller must eventually delete the return value.
 *   @note False and True gates are treated as compound gates
 *         that don't depend on any other gates.  Enter them using
 *         an empy gates vector
 */
DBGate*
SaveSet::createCompoundGate(
    const char* name, const char* type,
    const std::vector<const char*>& gates
)
{
    return DBGate::createCompoundGate(
        m_connection, m_Info.s_id,
        name, type, gates
    );
}
/**
 * createMaskGate
 *    Creates a new bitmask gate in this save set.
 *
 *  @param name - name of the new gate,
 *  @param type - Type of the new gate.
 *  @param parameter - name of the one parameter to check.
 *  @param imask     - bitmask to check it against.
 */
DBGate*
SaveSet::createMaskGate(
    const char* name, const char* type,
    const char* parameter, int imask
)
{
    return DBGate::createMaskGate(
        m_connection, m_Info.s_id,
        name, type, parameter, imask
    );
}

/**
 * lookupGate (overloaded)
 *    These methods lookup a gate and return its object encapsulation.
 * @param name - name of the gate.
 * @param id   - Id of the gate.
 * @return DBGate* pointer to the encaspulted gate.
 * @note The returned pointer must eventually be deleted by the caller.
 * @note Looking up a gate will return a matching gate even though
 *       it may not be in the save set described.  The gate information
 *       will, however provide the save set id which can be compared
 *       against the save set's id.
 */
DBGate*
SaveSet::lookupGate(const char* name)
{
    return new DBGate(m_connection, m_Info.s_id, name);
}
DBGate*
SaveSet::lookupGate(int id)
{
    return new DBGate(m_connection, id);   // id implies the save_set
}
/**
 * listGates
 *
 *    Returns a list of the gate objects that have been entered
 *    into this saveset.
 *  @return std::vector<DBGate*>
 *  @note the gate pointers point to objects that must eventually
 *     be deleted by the caller.
 */
std::vector<DBGate*>
SaveSet::listGates()
{
    return DBGate::listGates(m_connection, m_Info.s_id);
}

///////////////////////
// Applications API.

/**
 * applyGate
 *   Save a gate application in this save set.
 * @param gatename - name of an existing gate.
 * @param spectrum - name of an existing spectrum
 * @return DBApplication* - pointer to the encapsulating object.
 */
DBApplication*
SaveSet::applyGate(const char* gate, const char* spectrum)
{
    return DBApplication::applyGate(m_connection, m_Info.s_id, gate, spectrum);

}
/**
 * lookupApplication
 *    Look up an appliction in the save set and return
 *    its encapsulating object if found.
 *  @param gatename - name of the gate applied
 *  @param spectrum - name of the spectrum its applied to.
 *  @return DBApplication* pointer to the application object.
 */
DBApplication*
SaveSet::lookupApplication(const char* gate, const char* spectrum)
{
    return new DBApplication(m_connection, m_Info.s_id, gate, spectrum);
}

/**
 * listApplications
 *     Returns a list of application objects stored in
 *     the current saveset.
 */
std::vector<DBApplication*>
SaveSet::listApplications()
{
    return DBApplication::listApplications(
        m_connection, m_Info.s_id
    );
}

///////////////////////////////////////////////////////////////
// API for treevariables:

/**
 * createVariable
 *    Create an entry for a tree variable in this saveset.
 *  @param name -name of the variable
 *  @param value- variable value.
 *  @param units  - units.
 *  @return DBTreeVariable* - pointer to dynamically created tree variable.
 */
DBTreeVariable*
SaveSet::createVariable(const char* name, double value, const char* units)
{
    return DBTreeVariable::create(m_connection, m_Info.s_id, name, value, units);
}
/**
 * lookupVariable
 *    Return encapsulation of an existing variable.
 * @param name
 * @return DBTreeVariable*
 */
DBTreeVariable*
SaveSet::lookupVariable(const char* name)
{
    return new DBTreeVariable(m_connection, m_Info.s_id, name);
}
/**
 * exists
 *   @param name -name of the variable.
 *   @return bool - true if there's a variable with this name in the saveset.
 */
bool
SaveSet::variableExists(const char* name)
{
    return DBTreeVariable::exists(m_connection, m_Info.s_id, name);
    
}
/**
 * listVariables
 * @return std::vector<DBTreeVariable*> variables in the curent saveset.
 */
std::vector<DBTreeVariable*>
SaveSet::listVariables()
{
    return DBTreeVariable::list(m_connection, m_Info.s_id);
}
////////////////////////////////////////////////////////////
// API for storing run data.

/**
 * startRun
 *   Starts recording a run associated with this saveset.
 * @param run     - Run number.
 * @param title   - Title string.l
 * @param start   - Time of day at which the run started.
 * @return integer - id use this value to insert data that
 *                  refers to this run.  It is the id parameter
 *                  to other elements of this API.
 */
int
SaveSet::startRun(uint32_t run, const char* title, time_t start)
{
    CSqliteStatement ins(
        m_connection,
        "INSERT INTO runs (config_id, run_number, title, start_time) \
          VALUES(?, ?, ?, ?)"
    );
    ins.bind(1, m_Info.s_id);
    ins.bind(2, int(run));
    ins.bind(3, title, -1, SQLITE_STATIC);
    ins.bind(4, (int)(start));
    
    ++ins;
    return ins.lastInsertId();
}
/**
 *  endRun
 *     Provide the end of run timestamp for an existing run.
 *     This is separated from startRun to allow it to be
 *     set by sequentially procesing an event file e.g.
 * @param id   - the return value from the startRun call for this run.
 * @param endTime - end timestamp for the run.
 */
void
SaveSet::endRun(int id, time_t endtime)
{
    CSqliteStatement upd(
        m_connection,
        "UPDATE runs SET stop_time=? WHERE id=?"
    );
    upd.bind(1, (int)(endtime));
    upd.bind(2, id);
    ++upd;
}
/**
 * saveScalers
 *   Save a scaler readout instance to a run.
 * @param id       - Run identifier returned by the startRun call.
 * @param sourceid - data source id that generated the scalers.
 * @param startOffset - offset into the run the counting interval began
 *                      (see also divisor).
 * @param stopOffset  - offset into the run the counting interval ended.
 *                      (see also divisor).
 * @param divisor     - Normally, startOffset and stopOffset are
 *                      given in seconds. This field allows
 *                      for subsecond resolution and represents
 *                      an integer divisor that must be applied
 *                      to these offsets to recover seconds.
 *                      Thus, if this value is 2 e.g., the offsets
 *                      are in units of 0.5 seconds.
 * @param when        - clock time at which the scaler record was
 *                      read out.
 * @param nScalers    - number of scalers to save.
 * @param scalers     - values of the scalers.
 * @return int        - Scaler identification record.
 * @note Sure we could put scalers into a vector but; the
 *       ring items that are most likely being processed to produce
 *       these database records don't have a vector but an
 *       array. This allows fields of that ring item to be passed
 *       directly to this method.
 * @note Insertions are atomicized by using a savepoint "Scalers".
 */
int
SaveSet::saveScalers(
    int id, int sourceid,
    int startOffset, int stopOffset, int divisor, time_t when,
    int nScalers, const uint32_t* scalers
)
{
    // All of this is done within a save point so that
    // the multiple inserts are atomic.
    
    CSqliteSavePoint transaction(m_connection, "Scalers");
    int readoutId;
    try {
        // First the scaler readout root record:
        
        CSqliteStatement root(
            m_connection,
            "INSERT INTO scaler_readouts  \
            (run_id, source_id, start_offset, stop_offset, divisor, clock_time) \
            VALUES(?,?,?,?,?,?)"
        );
        root.bind(1, id);
        root.bind(2, sourceid);
        root.bind(3, startOffset);
        root.bind(4, stopOffset);
        root.bind(5, divisor);
        root.bind(6, int(when));
        ++root;
        
        readoutId = root.lastInsertId();
        
        // Now the scalers:
        
        CSqliteStatement scaler(
            m_connection,
            "INSERT INTO scaler_channels (readout_id, channel, value) \
                VALUES(?,?,?)"
        );
        scaler.bind(1, readoutId);
        for (int i =0; i < nScalers; i++) {
            scaler.bind(2, i);
            scaler.bind(3, int(scalers[i]));
            ++scaler;
            scaler.reset();
        }
        
    } catch (...) {
        transaction.scheduleRollback();
        throw;
    }                        // rollback.
    return readoutId;
    // Commit 
}
/**
 * startEvents
 *   In Sqlite3 saving a lot of records individually can be time consuming.
 *   It's more efficient/faster to queue up a bunch of records in a transaction
 *   and then commit them.  Therefore startEvents, rollbackEvents and
 *   endEvents provide a mechanism to encapsulate a SavePoint within which
 *   a bunch of events can be atomically  inserted.  The user code
 *   should look something like:
 *
 * <pre>
 *   void* handle = startEvents(id);
 *   try {
 *      for (int i =0; i < largeNumber; i++) {
 *          ...
 *         saveEvent(id ....);
 *         ... 
 *      }
 *   }
 *   catch (...) {
 *      rollbackEvents(handle); 
 *   }
 *   endEvents(handle);
 * </pre>
 *
 *  @param id    - run id - this is used to create the name of the savepoint.
 *  @return void*- handle to pass to either rollbackEvents or endEvents
 */
void*
SaveSet::startEvents(int id)
{
    std::stringstream name;
    name << "Events_" << id;
    std::string sname = name.str();
    CSqliteSavePoint* result  = new CSqliteSavePoint(m_connection, sname.c_str());
    return result;
}
/**
 * rollbackEvents
 *    Rollback a savepoint produced by startEvents.
 * @param savept - a handle returned from startEvents.  Note that once this
 *    method is called the handle becomes invalid and attempting to use it
 *    further will likely result in a segfault.
 */
void
SaveSet::rollbackEvents(void* savept)
{
    CSqliteSavePoint* p = static_cast<CSqliteSavePoint*>(savept);
    p->rollback();
    delete p;
}
/**
 * endEvents
 *    commits a savepoint produced by startEvents.
 * @param savept - a handle returned from startEvents.  Note that once
 *    method is called the handle becomes invalid and attempting to use it
 *    further will likely result in a segfault.
 */
void
SaveSet::endEvents(void* savept)
{
    CSqliteSavePoint* p = static_cast<CSqliteSavePoint*>(savept);
    p->commit();
    delete p;
}

/**
 * saveEvent
 *    Saves a single event associated with a run. Note that
 *    normally this should be part of a multi-event insertion
 *    bracketed by startEvents and endEvents.
 * @param id - run id returned from startRun.
 * @param event - Event number - used to order events.
 * @param nParams - number of parameters decoded in the event.
 * @parm paramids - Pointer to the parameter ids of the event.
 * @param params - Pointer to the parameters.
 */
void
SaveSet::saveEvent(
    int id, int event, int nParams, const int* paramids, const double* params
)
{
    // Marshall the vector of parameters:
    
    std::vector<EventParameter> blob;
    for (int i =0; i < nParams; i++) {
        blob.push_back({paramids[i], params[i]});
    }
    CSqliteStatement ins(
        m_connection,
        "INSERT INTO events (run_id, event_number, parameter_count, event_data) \
           VALUES(?,?,?,?)"
    );
    ins.bind(1, id);
    ins.bind(2, event);
    ins.bind(3, nParams);
    ins.bind(
        4, (void*)(blob.data()),
        (int)(nParams*sizeof(EventParameter)), SQLITE_STATIC
    );
    ++ins;
}
/**
 * listRuns
 *    Lists the runs that have been saved in this savesset.
 *    Not that this can be an empty list, if there are no runs saved.
 * @return std::vector<int> run numbers. that have been saved.
 *       empty if there are no run numbers in the save set.
 */
std::vector<int>
SaveSet::listRuns()
{
    std::vector<int> result;
    CSqliteStatement fet(
        m_connection,
        "SELECT run_number FROM runs WHERE config_id = ?"
    );
    fet.bind(1, m_Info.s_id);
    while(!(++fet).atEnd()) {
        result.push_back(fet.getInt(0));
    }

    return result;
}
/**
 * openRun
 *   Accesses a run in this saveset.
 *
 *  @return int  - An identifier to be used to query the run.
 *  @throw std::invalid_argument if the saveset does not store that run.
 */
int
SaveSet::openRun(int number)
{
    CSqliteStatement fet(
        m_connection,
        "SELECT  id FROM runs WHERE run_number = ? AND config_id = ?"
    );
    fet.bind(1, number);
    fet.bind(2, m_Info.s_id);
    ++fet;
    if (fet.atEnd()) {
        std::stringstream msg;
        msg << " Saveset " << m_Info.s_name
            << " does not have data for run number " << number;
        throw std::invalid_argument(msg.str());
    }
    
    return fet.getInt(0);
}
/**
 * openScalers
 *   Creates an Sqlite statement object that sequentially
 *   obtains the root records of scaler readouts for an open
 *   run.
 *
 *  @param id  - run id gotten from openRun.
 *  @return void*  handle to a statement object.
 *  @note The return value must be passed to closeScalers at some point.
 *        to release resources associated with it.
 *  @note use the return value in successive callse to readScaler
 */
void*
SaveSet::openScalers(int id)
{
    CSqliteStatement* result =
        new CSqliteStatement(
            m_connection,
            "SELECT id, source_id, start_offset, stop_offset, divisor, clock_time \
               FROM scaler_readouts WHERE run_id = ?"
        );
    result->bind(1, id);

    return result;
}
/**
 * readScaler
 *   Gets the next scaler record from the run.
 * @param context - return value from openScalers.
 * @param[out] result - Data read will be stored here.
 * @return int - 1 if there was another record, 0 if not
 * @note if the return value is 0, the contents of result are
 *       indeterminate.
 */
int
SaveSet::readScaler(void* context, ScalerReadout& result)
{
    CSqliteStatement& s(*static_cast<CSqliteStatement*>(context));
    ++s;
    if(s.atEnd()) return 0;
    int rdoid            = s.getInt(0);
    result.s_sourceId    = s.getInt(1);
    result.s_startOffset = s.getInt(2);
    result.s_stopOffset  = s.getInt(3);
    result.s_divisor     = s.getInt(4);
    result.s_time        = s.getInt(5);
    // get the readout - we assume the channels are dense.
    result.s_values.clear();
    CSqliteStatement scalers(
        m_connection,
        "SELECT value FROM scaler_channels  \
            WHERE readout_id = ? ORDER BY channel ASC"
    );
    scalers.bind(1, rdoid);
    while(!(++scalers).atEnd()) {
        result.s_values.push_back(scalers.getInt(0));
    }
    
    return 1;
}
/**
 * closeScalers
 *   destroys the statement being used to fetch scaler information.
 *
 * @param context - context returned from openScalers.
 * @note after calling this it is not valid to use context anymore.
 */
void
SaveSet::closeScalers(void *context)
{
    CSqliteStatement* p = static_cast<CSqliteStatement*>(context);
    delete p;
}
/**
 * openEvents
 *   Creates a context to  use for playing back events from
 *   a run stored in the database.
 * @param runid - Id retuned from openRun.
 * @return void* - context to pass to readEvent and closeEvents.
 * @note the context points to dynamic data. At some point it must
 *       be passed to closeEvents to properly free resources associated
 *       with it.
 */
void*
SaveSet::openEvents(int runid)
{
    CSqliteStatement* pResult =
        new CSqliteStatement(
            m_connection,
            "SELECT parameter_count, event_data FROM events \
            WHERE run_id = ? ORDER BY event_number ASC"
        );
        
    pResult->bind(1, runid);
    
    return pResult;
}
/**
 * readEvent
 *    Get the next event from the an event context returned from
 *    openEvents or indicate there are no more.
 * @param ctx  - Context gotten from openEvents.
 * @param[out] result - filled in with the event.
 * @return int  - 0 means there were no events.
 * @note if 0 is returned, the contents of result are indeterminate.
 */
int
SaveSet::readEvent(void* ctx, Event& result)
{
    CSqliteStatement& f(*(static_cast<CSqliteStatement*>(ctx)));
    
    ++f;
    if (f.atEnd()) return 0;

    
    result.clear();
    int nparams = f.getInt(0);
    const EventParameter* params= static_cast<const EventParameter*>(f.getBlob(1));
    for (int i =0; i < nparams; i++) {
        result.push_back(*params++);
    }
    
    return 1;
}
/**
 * closeEvents
 *   Releases storage and other resources associated with an event
 *   fetching context gotten from openEvents.
 *  @param context
 */
void
SaveSet::closeEvents(void* context)
{
    CSqliteStatement* s = static_cast<CSqliteStatement*>(context);
    delete s;
}
/**
 * getRunInfo
 *    Return information about a run.
 * @param id - id of the run gotten from e.g. openRun.
 * @return SpecTclDB::SaveSet::RunInfo struct.
 */
SaveSet::RunInfo
SaveSet::getRunInfo(int id)
{
    CSqliteStatement get(
        m_connection,
        "SELECT run_number, title, start_time, stop_time \
           FROM runs WHERE id=?"
    );
    get.bind(1, id);
    ++get;
    if (get.atEnd()) {
        throw std::invalid_argument("Invalid run id in SaveSet::getRunInfo");
    }
    RunInfo result;
    
    result.s_runNumber = get.getInt(0);
    result.s_title     = reinterpret_cast<const char*>(get.getText(1));
    result.s_startTime = get.getInt(2);
    result.s_stopTime  = get.getInt(3);
    
    return result;
}
////////////////////////////////////////////////////////////
// Static methods

/**
 * exists
 *   Find out if a save set exists (by name).  This is normally
 *   used to avoid collisions with existing save sets.
 * @param conn - the connection
 * @param name - the name to look for.
 */
bool
SaveSet::exists(CSqlite& conn, const char* name)
{
    // Simple way is to construct one and if it throws
    // false if not true:
    
    try {
        SaveSet s(conn, name);
        return true;                 // No exception.
    } catch (std::invalid_argument& a) {
        return false;
    }
    catch (...) {
        // Something _Bad_  -- we don't know how to test this branch.
        
        std::stringstream m;
        m << "While checking for existence of save_set: " << name
            << " an unexpected exception was caught";
        throw std::logic_error(m.str());
    }
}
/**
 * create
 *    Creates a new save set and returns a pointer to it.
 *    The save set must not yet exist in the database.
 * 
 * @param conn - database connection
 * @param name - save set name.
 * @return SaveSet* - pointer to dynamically created save set.
 * @throw std::invalid_argument - save set exists.
 */
SaveSet*
SaveSet::create(CSqlite& conn, const char* name)
{
    if (exists(conn, name)) {
        std::stringstream s;
        s << "The save set: " << name << " already exists";
        throw std::invalid_argument(s.str());
    } else {
        CSqliteStatement stmt(
            conn,
            "INSERT INTO save_sets (name, timestamp) VALUES(?,?)"
        );
        // Create the saveset in the database
        
        stmt.bind(1, name, -1, SQLITE_STATIC);
        stmt.bind(2, time(nullptr));
        ++stmt;
        
        // Return a new object.
        
        return new SaveSet(conn, name);
    }
}
/**
 * list
 *    List the set of save sets that have already been created.
 *  @param conn -sqlite connection
 *  @return std::vector<SaveSet::Info> - descriptions of
 *          save sets.
 */
std::vector<SaveSet::Info>
SaveSet::list(CSqlite& conn)
{
    std::vector<Info> result;
    
    CSqliteStatement s(
        conn,
        "SELECT id, name, timestamp FROM save_sets"
    );
    while(!((++s).atEnd())) {
        Info i;
        loadInfo(i, s);
        result.push_back(i);
    }
    return result;
}

////////////////////////////////////////////////////////////
// Private utilities
//

/**
 * setInfo
 *   Given a statment that's fetching data from save_sets,
 *   Builds the info for that row:
 *
 * @param[out] result - Info block to fill.
 * @param stmt - Sqlite statement iterated to the row we want.
 */
void
SaveSet::loadInfo(Info& result, CSqliteStatement& stmt)
{
    result.s_id   = stmt.getInt(0);
    result.s_name = reinterpret_cast<const char*>(stmt.getText(1));
    result.s_stamp= stmt.getInt64(2);
}
 
}