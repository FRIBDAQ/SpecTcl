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

/** @file:  DBSpectrum.cpp
 *  @brief: Implement the database spectrum class.
 */
#include "DBSpectrum.h"
#include "SaveSet.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "CSqliteTransaction.h"

namespace SpecTcl {
/**
 * constructor (private)
 *    Used to construct from the information block after
 *    creation e.g.
 * @param connection -the database connection object.
 * @param info       - The information block.
 */
DBSpectrum::DBSpectrum(CSqlite& connection, const Info& info) :
    m_conn(connection), m_Info(info)
{}


//////////////////////////////////////////////////////////////
// Static methods:

/**
 * create
 *    Create a new spectrum in the database and return an object
 *    representing it cached in memory.
 * @param connection   - Database connection object.
 * @param sid          - Save set into which the definition is saved.
 * @param name         - Name of the spectrum, must not exist in saveset.
 * @param parameterNames - Vector of parameter names.
 * @param axes         - Axis definitions.
 * @param datatype     - Channel data type.
 * @return SpecTcl::DBSpectrum* Pointer to a dynamically created spectrum.
 * 
 */
DBSpectrum*
DBSpectrum::create(
    CSqlite& connection, int sid, const char* name, const char* type,
    const std::vector<const char*>& parameterNames,
    const Axes& axes,
    const char* datatype
)
{
    Info specinfo;       // We'll build up the definition here.
    SaveSet s(connection, sid);   // Throws if there's no such saveset.
    
    // Fill in what we can of the base part of the info and
    // validate it:
    
    specinfo.s_base.s_saveset   = sid;
    specinfo.s_base.s_name      = name;
    specinfo.s_base.s_type      = type;
    specinfo.s_base.s_dataType  = datatype;
    validateBaseInfo(connection, sid, specinfo.s_base);
    validateSpectrumType(type);
    validateDataType(datatype);
    
    // each spectrum type has an allowable number of parameters:
    // We need to also convert the parameter names to ids.
    
    validateParameterCount(type, parameterNames.size());
    specinfo.s_parameters = fetchParameters(connection, sid, parameterNames);
    
    // Each spectrum type as a specific number of allowed axes:
    
    validateAxisCount(type, axes.size());
    specinfo.s_axes = axes;
    
    // We enter the spectrum in a transaction.
    // If an exception is thrown, rollback otherwise commit:
    
    {
        CSqliteTransaction t(connection);
        try {
            enterSpectrum(connection, specinfo);
        }
        catch (...) {
            t.scheduleRollback();        // Destruction will rollback.
            throw;                       // Someone else can handle the actual error.
        }
        // Destruction of the transaction means commit if no rollback scheduled.
    }
    // Now create/return the object - specinfo's ids were filled in by enterSpectrum.
    
    return new DBSpectrum(connection, specinfo);
}


}               // namespace SpecTcl