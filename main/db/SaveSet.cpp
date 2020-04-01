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
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "CSqliteTransaction.h"
#include <sqlite3.h>

#include <sstream>
#include <stdexcept>

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
    getInfo(m_Info, s);
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
SaveSet::getInfo(Info& result, CSqliteStatement& stmt)
{
    result.s_id   = stmt.getInt(0);
    result.s_name = reinterpret_cast<const char*>(stmt.getText(1));
    result.s_stamp= stmt.getInt64(2);
}
 
}