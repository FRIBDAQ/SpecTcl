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
#include <histotypes.h>


#include <sstream>
#include <stdexcept>

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
* exists
*    @param connection - Sqlite connection object.
*    @param sid        - save set id.
*    @param name       - spectrum name
*    @return bool      - true of the spectrum is defined in the save set.
*    @note we only look for the name in the spectrum_defs table..not
*          that we have a complete definition.
*/
bool
DBSpectrum::exists(CSqlite& connection, int sid, const char* name)
{
    CSqliteStatement s(
        connection,
        "SELECT COUNT(*) FROM spectrum_defs WHERE save_id = ? AND name = ?"
    );
    s.bind(1, sid);
    s.bind(2, name, -1, SQLITE_STATIC);
    ++s;
    return s.getInt(0) > 0;
}
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
    validateBaseInfo(connection, specinfo.s_base);
   
    
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
//////////////////////////////////////////////////////////////////////////////
// Private utility methods.

/**
 * fetchParameters
 *    Given the set of parameter names, Fetch the corresponding parameter ids.
 *    (not numbers, ids).
 * @param connection - Sqlite connection object.
 * @param sid        - save set id.
 * @param parameterNames - vector of parameter names.
 * @return Parameters (std::vector<int>)  the ids.
 * @throws std::invalid_argument at the first parameter not found.
 * @note - we could throw all the names into an WHERE name IN (...) clause
 *         but fetching them one by one here makes it easier to
 *         figure out which one is missing if one _is_ missing.
 *         This could be a later optimiziation where we'd fetch
 *         name/id pairs and then be able to see which one is missing
 *         Furthermore, since for some spectra, the order of the parameters is
 *         important (x,y e.g.) this preserves the order.
 */
DBSpectrum::Parameters
DBSpectrum::fetchParameters(
    CSqlite& connection, int sid, const std::vector<const char*>& parameterNames
)
{
    Parameters result;
    CSqliteStatement s(
        connection,
        "SELECT id FROM parameter_defs WHERE save_id = ? AND name = ?"
    );
    // We have a constant binding for the save id
    
    s.bind(1,sid);
    for (int i =0; i < parameterNames.size(); i++) {
        s.bind(2, parameterNames[i], -1, SQLITE_STATIC);
        ++s;
        if (s.atEnd()) {                          // not found.
            SaveSet set(connection, sid);
            std::stringstream msg;
            msg << "There is no parameter named " << parameterNames[i]
                << " in the save set " << set.getInfo().s_name;
            throw std::invalid_argument(msg.str());
        }
        result.push_back(s.getInt(0));
        s.reset();
    }
    
    
    return result;
}
/**
 * validateBaseInfo
 *    Do a few checks:
 *    -   The Spectrum name is unique in the save set.
 *    -   The spectrum type is legal
 *    -   The data type is legal.
 * @param connection - the sqlite3 connection object.
 * @param base       - the base info except for the spectrum id whcih is not known yet.
 * @throw std::invalid_argument if checks fail.
 */
void
DBSpectrum::validateBaseInfo(CSqlite& connection, const BaseInfo& info)
{
    if (exists(connection, info.s_saveset, info.s_name.c_str())) {
        std::stringstream msg;
        SaveSet set(connection, info.s_saveset);
        msg << info.s_name << " already exists in the save set "
            << set.getInfo().s_name;
        throw std::invalid_argument(msg.str());
    }
    
    validateSpectrumType(info.s_type.c_str());
    validateDataType(info.s_dataType.c_str());
}
/**
 * validateParameterCount
 *    Given a spectrum type and the number of proposed parameters,
 *    throws an exception if this is not a legal number of
 *    parameters.
 * @param type - spectrum type code.
 * @param n    - Number of proposed parameters
 */
void
DBSpectrum::validateParameterCount(const char* type, size_t n)
{
    bool ok(false);             // Throw at end
    
    // there are three clasess:
    // Spectra with one parameter (e.g. 1d).
    // spectra with 2 parameters (e.g. 2d)
    // spectra with no limit (e.g. summary).
    std::string stype = type;
    if (type == "1" || type == "b") {
        ok = (n == 1);
    } else if (type =="2" || type == "S") {
        ok = (n == 2);
    } else if (type == "s" || type == "g1" || type == "g2" ||
               type == "gd"  || type == "gs" || type == "m2" ||
               type == "2dmproj"
            ) {
        // For gd, 2dmproj and m2, there must be an even number of params:
        
        if (type == "gd" || type == "m2" || type == "2dmproj") {
            ok == ((n % 2) == 0);
        } else {
            ok = true;
        }
    }
    if (!ok) {
        std::stringstream msg;
        msg << n << " is not a valid number of parameters for "
            << "spectra of type " << type;
        throw std::invalid_argument(msg.str());
    }
}
/**
 * validateAxisCount
 *   for a known valid spectrum type, determine if the proposed number
 *   of axes is valid.
 * @param type    - Spectrum tpe
 * @param n       - proposed number of axis definitions.
 * @throw std::invalid_argument if not.
 */
void
DBSpectrum::validateAxisCount(const char* type, size_t n)
{
    std::string specType;         // Easier to compare.
    bool ok(false); 
    if (n > 2) {
        ok = false;               // 2 is the maximum number of axes we allow.
    } else {
        if (specType == "1" || specType == "b" || specType == "s" ||
            specType == "g1" || specType == "S" || specType == "gs" ||
            specType == "2dmproj"
        ) {
            ok = (n == 1);
        } else if (specType == "2" || specType == "g2" || specType == "gd" ||
                   specType == "m2"
        ) {
            ok = (n == 2);
        }
    }
    // Note that this code will have ok false for illegal spectrum types
    // in case the caller didn't yet validate the type.
    
    if (!ok) {
        std::stringstream msg;
        msg << n << " is not a valid number of axis specifications for spectra "
            << " of type  " << type;
        throw std::invalid_argument(msg.str());
    }
}
/**
 * validteSpectrumType
 *    Throws an std::invalid_argument if the spectrum type is not valid.
 * @param type  - spectrum type.
 */
void
DBSpectrum::validateSpectrumType(const char* type)
{
    std::string sptype=type;
    std::stringstream tstr(sptype);
    SpectrumType_t typecode;
    tstr >> typecode;
    
    if (typecode == keUnknown) {
        std::stringstream msg;
        msg << "The spectrum type code string: " << type <<" is not valid";
        throw std::invalid_argument(msg.str());
    }
    
}
/**
 * validateDataType
 *     throws a std::invalid_argument if the spectrum channel data type is not valid.
 *     Ntoe that DataType_t includes data type definitions that are not legal
 *     spectrum channel types.  Therefore we do the if chain below.
 * @param type  - the data type string.
 */
void
DBSpectrum::validateDataType(const char* type)
{
    std::string dtype = type;
    bool ok = false;
    if (dtype == "byte" || dtype == "word" || dtype == "long") ok = true;
    
    if (!ok) {
        std::stringstream msg;
        msg << type <<  " is not a valid spectrum channel data type";
        throw std::invalid_argument(msg.str());
    }
}
/**
 * enterSpectrum
 *    This rather long but straightforward method actually inserts all
 *    the rows in all the tables needed to describe the spectrum
 *    in the info block it's passed. By now the caller must have
 *    ensured that the block can be validly inserted.
 *    We will set the s_id fields of the base block and axis blocks
 *    as we go.
 *
 *    Really the caller should call this from inside a transaction which
 *    gets committed if there are no exceptions and rollback if there
 *    are exceptions.
 * @param connection   - Sqlite connection object.
 * @param[inout] info  - Spectrum information block the s_id elements
 *                       will be filled in with the actual id values.
 */
void
DBSpectrum::enterSpectrum(CSqlite& connection, Info& info)
{
    // Insert the root record in spectrum_defs (information from s_base)
    
    CSqliteStatement root(
        connection,
        "INSERT INTO spectrum_defs(save_id, name, type, datatype)  \
            VALUES(?,?,?,?)"
    );
    root.bind(1, info.s_base.s_saveset);
    root.bind(2, info.s_base.s_name.c_str(), -1, SQLITE_STATIC);
    root.bind(3, info.s_base.s_type.c_str(), -1, SQLITE_STATIC);
    root.bind(4, info.s_base.s_dataType.c_str(), -1, SQLITE_STATIC);
    ++root;                        // insert.
    info.s_base.s_id = root.lastInsertId();
    
    // add the parameters to spectrum_params
    
    CSqliteStatement param (
        connection,
        "INSERT INTO spectrum_params (spectrum_id, parameter_id) \
            VALUES (?,?)"
    );
    param.bind(1, info.s_base.s_id);
    for (int i = 0; i < info.s_parameters.size(); i++) {
        param.bind(2, info.s_parameters[i]);
        ++param;                     //  insert.
        param.reset();              // prepared for next loop.
    }
    
    // Add the axis definitions (note we need to capture the id of each.)
    
    CSqliteStatement axis(
        connection,
        "INSERT INTO axis_defs (spectrum_id, low, high, bins)   \
            VALUES(?,?,?,?)"
    );
    axis.bind(1, info.s_base.s_id);
    for (int i =0; i < info.s_axes.size(); i++) {
        axis.bind(2, info.s_axes[i].s_low);
        axis.bind(3, info.s_axes[i].s_high);
        axis.bind(4, info.s_axes[i].s_bins);
        
        ++axis;
        info.s_axes[i].s_id = axis.lastInsertId();
        
        axis.reset();
    }
}

}               // namespace SpecTcl