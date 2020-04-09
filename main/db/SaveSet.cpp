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
 *  @brief:  Implementation of the SpecTcl::SaveSet class.
 */
#include "SaveSet.h"
#include "DBParameter.h"
#include "DBSpectrum.h"
#include "DBGate.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "CSqliteTransaction.h"
#include <sqlite3.h>

#include <sstream>
#include <stdexcept>
#include <time.h>

namespace SpecTcl{
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
        stmt.bind(2, time_t(nullptr));
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