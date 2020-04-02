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
{}


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
}                                // SpecTcl namespace.