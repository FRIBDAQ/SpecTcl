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

/** @file:  DBApplications.cpp
 *  @brief: Implements the DBApplications class.
 */

#include "DBApplications.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "DBSpectrum.h"
#include "DBGate.h"

namespace SpecTcl {

/**
 * constructor (private)
 *    Construts after insert:
 * @param conn - Sqlite connection object.
 * @pram info  - Info block.
 */
DBApplication::DBApplication(CSqlite& conn, const Info& info) :
    m_connection(conn), m_Info(info)
{}

/////////////////////////////////////////////////////////////////
// Static methods:

/**
 * applyGate
 *   Applies a gate to a specific spectrum in a save set.
 * @param conn - connection.
 * @param svaeid - save set.
 * @param gateName   - gate name in the saveset.
 * @param spectrumName - Spectrum name in the save set.
 * @return DBApplication*  - Pointer to an encapsulaton of the
 *                    entered application.
 * @note the return value is dynamically allocate and must be deleted.
 */
DBApplication*
DBApplication::applyGate(
    CSqlite& conn, int saveid, const char* gateName, const char* spectrumName
)
{
    DBSpectrum spec(conn, saveid, spectrumName);
    DBGate     gate(conn, saveid, gateName);
    
    Info info;
    info.s_gateid = gate.getInfo().s_info.s_id;
    info.s_spectrumid = spec.getInfo().s_base.s_id;
    
    CSqliteStatement ins(
        conn,
        "INSERT INTO gate_applications (spectrum_id, gate_id) VALUES (?,?)"
    );
    ins.bind(1, info.s_spectrumid);
    ins.bind(2, info.s_gateid);
    ++ins;
    info.s_id = ins.lastInsertId();
    
    return new DBApplication(conn, info);
}
}                            // SpecTcl namespace.