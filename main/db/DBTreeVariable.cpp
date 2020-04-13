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

/**
 * constructor - retrieval
 *   Retrieves a record from the tree variables in a save set
 *   and wraps it in an object.
 * @param conn   - sqlite connection object.
 * @param saveid - Save set id.
 * @param name   - variable name.
 * @throw std::invalid_argument if the save id is invalid or the variable
 *       is not saved in it.
 */
DBTreeVariable::DBTreeVariable(
    CSqlite& connection, int saveid, const char* varname
) : m_connection(connection)
{
    SaveSet s(connection, saveid);
    CSqliteStatement retrieve(
        connection,
        "SELECT * from treevariables WHERE save_id = ? and name= ?"
    );
    retrieve.bind(1, saveid);
    retrieve.bind(2, varname, -1, SQLITE_STATIC);
    
    ++retrieve;
    if (retrieve.atEnd()) {
        std::stringstream msg;
        msg << "There is no variable named; " << varname
            << " in the save set named; " << s.getInfo().s_name;
        throw std::invalid_argument(msg.str());
    }
    loadInfo(retrieve, m_Info);
}
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
/**
 * list
 *    Returns a vector of pointers to dynamically created DBTreeVariables
 *    that encapsulate all of the tree variables saved in a saveset:
 * @param conn - Sqlite connection object.
 * @param saveid - Save set id.
 * @return std::vector<SpecTcl::DBTreeVariable*> -
 * @note The caller must, at some point, execute a delete on the
 *        pointers in the vector returned by this method.
 */
std::vector<DBTreeVariable*>
DBTreeVariable::list(CSqlite& conn, int saveid)
{
    SaveSet saveset(conn, saveid);         // Throws if invalid saveset.
    std::vector<DBTreeVariable*> result;
    CSqliteStatement s(
        conn,
        "SELECT * from treevariables WHERE save_id =? ORDER BY id ASC"
    );
    s.bind(1, saveid);
    while(!(++s).atEnd()) {
        Info i;
        loadInfo(s, i);
        
        
        result.push_back(new DBTreeVariable(conn, i));
    }
    return result;
}
///////////////////////////////////////////////////////////////
// Private utilities.

/**
 * loadInfo
 *    Loads an info struct with the data from the current
 *    cursor position of a SELECT* FROM treevariables ... statement
 */
void
DBTreeVariable::loadInfo(CSqliteStatement& stmt, Info& info)
{
        info.s_id      = stmt.getInt(0);
        info.s_saveset = stmt.getInt(1);
        info.s_name    = reinterpret_cast<const char*>(stmt.getText(2));
        info.s_value   = stmt.getDouble(3);
        info.s_units   = reinterpret_cast<const char*>(stmt.getText(4));
}
}                         // namespace SpecTcl