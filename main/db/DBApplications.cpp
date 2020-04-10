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
#include "SaveSet.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "DBSpectrum.h"
#include "DBGate.h"

#include <sstream>
#include <stdexcept>

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

/**
 * constructor (public)
 *    Retrieve an appliction's information and wrap it in
 *    a DBApplication object.
 * @param conn  - database connection object.
 * @param savid - saveset id.
 * @param gateName - name of the gate.
 * @param specName - Name of the spectrum.
 */
DBApplication::DBApplication(
    CSqlite& conn, int saveid,
    const char* gateName, const char* spectrumName
) :
    m_connection(conn)
{
    // Get the spectrum and gate name information:
    
    SaveSet s(conn, saveid);
    auto spectrum = s.lookupSpectrum(spectrumName);
    DBGate* gate(nullptr);
    try {
        gate    = s.lookupGate(gateName);
    }
    catch (...) {
        delete spectrum;      // No memory leak here.
        throw;
    }
    
    CSqliteStatement ins(
        conn,
        "SELECT id, spectrum_id, gate_id FROM  gate_applications \
        WHERE spectrum_id = ? AND gate_id = ?"
         
    );
    ins.bind(1, spectrum->getInfo().s_base.s_id);
    ins.bind(2, gate->getInfo().s_info.s_id);
    delete spectrum;
    delete gate;
    ++ins;
    if (ins.atEnd()) {
        std::stringstream msg;
        msg << " The saveset " << s.getInfo().s_name
            << " does not have an application of "
            << gateName << " to " << spectrumName;
        throw std::invalid_argument(msg.str());
    }
    m_Info.s_id = ins.getInt(0);
    m_Info.s_spectrumid = ins.getInt(1);
    m_Info.s_gateid = ins.getInt(2);
    
}
/**
 * getGateName
 *    Returns the name associated with the gate we're holding.
 * @return std::string - name of applied gate.
 */
std::string
DBApplication::getGateName()
{
    CSqliteStatement f(
        m_connection,
        "SELECT name FROM gate_defs WHERE id = ?"
    );
    f.bind(1, m_Info.s_gateid);
    ++f;
    if (f.atEnd()) {
        throw std::logic_error("Can't find gate name for gate id");
    }
    return std::string(reinterpret_cast<const char*>(f.getText(0)));
}
/**
 * getSpectrumName
 *
 * @return std::string - name of the spectrum the gate is applied on.
 */
std::string
DBApplication::getSpectrumName()
{
    CSqliteStatement f(
        m_connection,
        "SELECT name FROM spectrum_defs WHERE id = ?"
    );
    f.bind(1, m_Info.s_spectrumid);
    ++f;
    if (f.atEnd()) {
        throw std::logic_error("Can't find spectrum name for gate id");
    }
    return std::string(reinterpret_cast<const char*>(f.getText(0)));
}
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
/**
 * listApplications
 *    List the gate applications that are relevant for a specific
 *    save set.  This means requiring that the gate and spectrum
 *    live in the selected save set.  This can be assured by
 *    doing a join on the spectrum_defs table and applying that
 *    requirement there.
 *
 *  @param conn    Database connectoin object.
 *  @param sid     saveid   Save set id.
 *  @return std::vector<DBApplication*> - vector of matching
 *                 application objects.
 *  @note the elements of the vector must be deleted.
 */
std::vector<DBApplication*>
DBApplication::listApplications(CSqlite& conn, int saveid)
{
    SaveSet set(conn, saveid);     // Throws if bad saveset.
    std::vector<DBApplication*> result;
    
    CSqliteStatement fetch(
        conn,
        "SELECT ga.id, gate_id, spectrum_id FROM gate_applications AS ga \
         INNER JOIN spectrum_defs AS sd ON sd.id = ga.spectrum_id       \
         WHERE sd.save_id = ? ORDER BY ga.id ASC"
    );
    fetch.bind(1, saveid);
    
    while(!(++fetch).atEnd()) {
        Info info = {fetch.getInt(0), fetch.getInt(1), fetch.getInt(2)};
        result.push_back(new DBApplication(conn, info));
    }
    
    return result;
}

}                            // SpecTcl namespace.