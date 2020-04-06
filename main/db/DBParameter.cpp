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

/** @file:  DBParameter.cpp
 *  @brief:  Implements SpecTcl::DBParameter class.
 */
#include "DBParameter.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "SaveSet.h"

#include <sstream>
#include <stdexcept>

namespace SpecTcl {
/**
 * constructor (public)
 *    Fetches information about a parameter in a save set
 *    given its name.
 *  @param connection - sqlite connection
 *  @param saveid     - save set id.
 *  @param name       - Name of the parameter.
 */
DBParameter::DBParameter(CSqlite& connection, int saveid, const char* name) :
    m_connection(connection)
{
    CSqliteStatement s(
        m_connection,
        "SELECT * FROM parameter_defs WHERE save_id = ? AND name = ?"
    );
    s.bind(1, saveid);
    s.bind(2, name, -1, SQLITE_STATIC);
    ++s;
    if(s.atEnd()) {
        SaveSet set(m_connection, saveid);
        auto i = set.getInfo();
        // no match!
        
        std::stringstream msg;
        msg << "There is no parameter named " << name << " in saveset named: "
            << i.s_name;
        throw std::invalid_argument(msg.str());
    }
    fillInfo(s);
}
/*
 * constructor
 *   Fetches information about a parameter by number.  I'd love to factor out
 *   the common logic for the this and the constructor above but the
 *   error message for the exception is dynamic and I don't want to always
 *   build it as that's probably time consuming.
 * @param conn - connection object.
 * @param sid  - save set id.
 * @param number - the parameter number (not id).
 */
DBParameter::DBParameter(CSqlite& conn, int sid, int number) :
    m_connection(conn)
{
    CSqliteStatement s(
        m_connection,
        "SELECT * FROM parameter_defs WHERE save_id = ? AND number = ?"
    );
    s.bind(1, sid);
    s.bind(2, number);
    ++s;
    if (s.atEnd()) {
        SaveSet set(m_connection, sid);
        auto& i = set.getInfo();
        std::stringstream msg;
        msg << "There is no parameter numbered: " << number
            << " in saveset: " << i.s_name;
        throw std::invalid_argument(msg.str());
    }
    fillInfo(s);
}
/**
 * constructor (Private)
 *    This is a private constructor that constructs the
 *    object from its Info block that, presumably was
 *    retrieved from the database.  Used by e.g. create.
 *    
 *   @param connection - the connection to the database
 *   @param info       - The info block describing the parameter.
 */
DBParameter::DBParameter(CSqlite& connection, const Info& info) :
    m_connection(connection),
    m_Info(info)
{
        
}


///////////////////////////////////////////////////////////////////
// Static methods:

/**
 * exists
 *    See if a parameter exists in a save set:
 * @param connection - Database connection object.
 * @param sid        - save set id.
 * @param name       - parameter name.
 * @param number     - parameter number
 */
bool
DBParameter::exists(CSqlite& connection, int sid, const char* name)
{
    CSqliteStatement s(
        connection,
        "SELECT COUNT(*) FROM parameter_defs      \
            WHERE save_id = ? AND name = ?"
    );
    s.bind(1, sid);
    s.bind(2, name, -1, SQLITE_STATIC);
    ++s;                                 // Get the count.
    
    int count = s.getInt(0);
    return (count > 0);
    
}
bool
DBParameter::exists(CSqlite& connection, int sid, int number)
{
    CSqliteStatement s(
        connection,
        "SELECT COUNT(*) FROM parameter_defs      \
            WHERE save_id = ? AND number = ?"
    );
    s.bind(1, sid);
    s.bind(2, number);
    ++s;                                 // Get the count.
    
    int count = s.getInt(0);
    return (count > 0);
    
}
/**
 * create (simple)
 *    Creates a parameter that has no metadata.
 * @param connection - connection to the database.
 * @param sid        - Save set id to make the parameter in.
 * @param name       - name of the new parameter.
 * @param number     - parameter number
 * @return SpecTcl::DBParameter*  - pointer to the object describing this thing.
 * @throw std::invalid_argument if the parameter name already exists in the saveset.
 * @note we can use the private Info based constructor to create the parameter
 * @note The returned parameter object was created with new and therefore the
 *       caller must delete it when they're done with that.
 * @note destroying DBParameter objects does nothing to the underlying database
*/
DBParameter*
DBParameter::create(CSqlite& conn, int sid, const char* name, int number)
{
    checkCreateOk(conn, sid, name, number);
    
    // Insert the database record:
    
    CSqliteStatement s(
        conn,
        "INSERT INTO parameter_defs (save_id, name, number) VALUES (?,?,?)"
    );
    s.bind(1, sid);
    s.bind(2, name, -1, SQLITE_STATIC);
    s.bind(3, number);
    ++s;
    
    Info i;
    i.s_id = s.lastInsertId();
    i.s_savesetId = sid;
    i.s_name = name;
    i.s_number = number;
    i.s_haveMetadata = false;
    return new DBParameter(conn, i);
}
/**
 * create
 *    Create parameter with full metadata
 *
 * @param conn      - connection to the database.
 * @param sid        - Save set id to make the parameter in.
 * @param name       - name of the new parameter.
 * @param number     - parameter number
 * @param low        - parameter low limit.
 * @param high       - parameter high limit.
 * @param bins       - suggested parametr binning.
 * @param units      - parameter units of measure.
 * @return SpecTcl::DBParameter*  - pointer to the object describing this thing.
 * @throw std::invalid_argument if the parameter name already exists in the saveset.
 * @note we can use the private Info based constructor to create the parameter
 * @note The returned parameter object was created with new and therefore the
 *       caller must delete it when they're done with that.
 * @note destroying DBParameter objects does nothing to the underlying database
 */
DBParameter*
DBParameter::create(
    CSqlite& conn, int sid, const char* name, int number,
    double low, double high, int bins, const char* units
)
{
    checkCreateOk(conn, sid, name, number);
    
    CSqliteStatement s(
        conn,
        "INSERT INTO parameter_defs \
            (save_id, name, number, low, high, bins, units) \
            VALUES (?,?,?,?,?,?,?)"
    );
    s.bind(1, sid);
    s.bind(2, name, -1, SQLITE_STATIC);
    s.bind(3, number);
    s.bind(4, low);
    s.bind(5, high);
    s.bind(6, bins);
    s.bind(7, units, -1, SQLITE_STATIC);
    ++s;
    
    Info i;
    i.s_id        = s.lastInsertId();
    i.s_savesetId = sid;
    i.s_name      = name;
    i.s_number    = number;
    i.s_haveMetadata = true;
    i.s_low       = low;
    i.s_high      = high;
    i.s_bins      = bins;
    i.s_units     = units;
    
    return new DBParameter(conn, i);
    
}
/**
 * get
 *    Return the parameter that has the given id.
 *    @note throws std::invalid_argument for most errors.
 *  @param conn -connection object.
 *  @param sid  - Save set id.
 *  @param id   - id (primary key) of the parameter.
 *  @return DBParameter* - dynamically allocated parameter object.
 */
DBParameter*
DBParameter::get(CSqlite& conn, int sid, int id)
{
    SaveSet s(conn, sid);              // Checks the save set exists.
    CSqliteStatement f(
        conn,
        "SELECT * FROM parameter_defs WHERE id = ?"
    );
    f.bind(1, id);
    ++f;
    if (f.atEnd()) {
        std::stringstream msg;
        msg << "There is no parameter with the id " << id
            << " in the save set " << s.getInfo().s_name;
        throw std::invalid_argument(msg.str());
    }
    DBParameter* result = new DBParameter(conn);
    result->fillInfo(f);
    
    return result;
}

/**
 * list
 *    Return a list of the parameters in a save set:
 * @param conn - the connection object.
 * @param number - Save set number
 * @return std::vector<DBParameter*>
 */
std::vector<DBParameter*>
DBParameter::list(CSqlite& connection, int sid)
{
    // This next line throws if the save set is invalid:
    
    SaveSet set(connection, sid);
    
    std::vector<DBParameter*> result;
    CSqliteStatement s(
        connection,
        "SELECT number FROM parameter_defs WHERE save_id = ?"
    );
    s.bind(1, sid);
    while(!((++s).atEnd())) {
        int number = s.getInt(0);
        result.push_back(new DBParameter(connection, sid, number));
    }
    
    return result;
}
//////////////////////////////////////////////////////////////////////////
// Utilities
 
/**
 * checkCreateOk
 *    Checks that it's ok to create a parameter in a saveset.
 *    throws std::invalid_argument if not.
 * @param conn -the connection.
 * @param sid - the save set id.
 * @param name -the parameter name.
 * @param number - the parameter number
 */
void
DBParameter::checkCreateOk(CSqlite& conn, int sid, const char* name, int number)
{
    SaveSet set(conn, sid);               // Checks existence of saveset.
    if (exists(conn, sid, name) ) {
        std::stringstream msg;
        auto info = set.getInfo();
        msg << "A parameter named: " << name << " is already defined in"
            << " the save set: " << info.s_name;
        throw std::invalid_argument(msg.str());
    }
    if (exists(conn, sid, number)) {
        std::stringstream msg;
        auto info = set.getInfo();
        msg << "There's already a parameter numbered " << number
            << " in the save set " << info.s_name;
        throw std::invalid_argument(msg.str());
    }    
}
/**
 * fillInfo
 *    Fill our info block from the results of a SELECT * FROM parameter_defs
 * @param stmt - the statment doing the fetch
 */
void
DBParameter::fillInfo(CSqliteStatement& stmt)
{
    m_Info.s_id        = stmt.getInt(0);
    m_Info.s_savesetId = stmt.getInt(1);
    m_Info.s_name      = reinterpret_cast<const char*>(stmt.getText(2));
    m_Info.s_number    = stmt.getInt(3);
    if (stmt.columnType(4) == CSqliteStatement::null) {
        m_Info.s_haveMetadata = false;
    } else {
        m_Info.s_haveMetadata = true;
        m_Info.s_low   = stmt.getDouble(4);
        m_Info.s_high  = stmt.getDouble(5);
        m_Info.s_bins  = stmt.getInt(6);
        m_Info.s_units = reinterpret_cast<const char*>(stmt.getText(7));
    }
}
}                                // SpecTcl namespace.