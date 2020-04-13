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

/** @file:  DBTreeVariable.cpp
 *  @brief: Implement the SpecTcl::DBTreeVariable class
 */

#include "DBTreeVariable.h"
#include "SaveSet.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include <sstream>
#include <stdexcept>

namespace SpecTcl {
//////////////////////////////////////////////////////////////////
// Constructors.


/**
 * constructor - private
 *    Used by static creationals to create an object given we
 *    already have it's info built:
 * @param conn - Sqlite connection object.
 * @param Info - Info block to use for this object.
 */
DBTreeVariable::DBTreeVariable(CSqlite& conn, const Info& info) :
    m_connection(conn), m_Info(info)
{}

///////////////////////////////////////////////////////////////
// Object methods.



////////////////////////////////////////////////////////////////
// Static methods.

/**
 * exists
 *    @param conn   - connection.
 *    @param saveid - saveset id.
 *    @param name   - name of the treevariable.
 */
bool
DBTreeVariable::exists(CSqlite& conn, int saveid, const char* name)
{
    SaveSet s(conn, saveid);       // Throws if no such saveset.
    
    CSqliteStatement q(
        conn,
        "SELECT COUNT(*) FROM treevariables WHERE save_id = ? AND name = ?"
    );
    q.bind(1, saveid);
    q.bind(2, name, -1, SQLITE_STATIC);
    ++q;
    
    return (q.getInt(0) > 0);
}
/**
 * create
 *    Create a tree variable.
 *  @param conn - Sqlite connection object.
 *  @param saveid - Saveset id.
 *  @param name   - variable name.
 *  @param value  - current value.
 *  @para  units  - units text (defaults to an empty string)
 *  @return DBTreeVarialbe* - pointer to an object encapsulating the newly
 *         created variable.
 *  @throw std::invalid_argument if the variable already has been saved
 *            in the svae set.
 *  @note the return value is new'd into existence and mus eventually be
 *         deleted by the caller.
 */
DBTreeVariable*
DBTreeVariable::create(
        CSqlite& conn, int saveid, const char* name, double value,
        const char* units
)
{
    SaveSet set(conn, saveid);              // Checks save set.
    if(exists(conn, saveid, name)) {
        std::stringstream msg;
        msg << "The save set " << set.getInfo().s_name
            << " already has a treevariable named; " << name;
        throw std::invalid_argument(msg.str());
    }
    
    CSqliteStatement insert(
        conn,
        "INSERT INTO treevariables (save_id, name, value, units) \
            VALUES(?,?,?,?)"
    );
    insert.bind(1, saveid);
    insert.bind(2, name, -1, SQLITE_STATIC);
    insert.bind(3, value);
    insert.bind(4, units, -1, SQLITE_STATIC);
    ++insert;
    Info info;
    info.s_id      = insert.lastInsertId();
    info.s_saveset = saveid;
    info.s_name    = name;
    info.s_value   = value;
    info.s_units   = units;
    
    return new DBTreeVariable(conn, info);
}
}                         // namespace SpecTcl