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

}                                // SpecTcl namespace.