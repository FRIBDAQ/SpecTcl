/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2026.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
        Ron Fox
        FRIB
        Michigan State University
        East Lansing, MI 48824-1321
*/
/**
 *  @file DBMetadata.cpp
 *  @brief Implement the class that manages metadata. See Issue #299 
 *  @author Ron Fox<rfoxkendo@gmail.com>
 */

 #include "DBMetadata.h"
 #include "CSqlite.h"
 #include "CSqliteStatement.h"
 #include "CSqliteException.h"
 #include "CSqliteTransaction.h"
 #include "SaveSet.h"
 #include "DBParameter.h"
 #include "DBSpectrum.h"
 #include "DBGate.h"
 #include "DBTreeVariable.h"
 #include <sqlite3.h>
 #include <stdexcept>
namespace SpecTclDB {
 /**
  *  constructor
  *     @param connection - connects to the database
  *     @param sveid      - the save set id.
  *     @throws CSqliteException or the connection is closed.
  *     @throws std::invalid_argument if the saveset does not exist.
  */
DBMetadata::DBMetadata(CSqlite& connection, int saveid) :
    m_connection(connection), m_saveId(saveid) {
    // Validate the save id.

    SaveSet svset(connection, saveid);    // throws if no such saveset.
}

/**
 * selectParameter
 *    Selects the parameter for which following metadata operations are
 * going to be done.   This does two things:
 * -  Sets the type of metadata to 'parameter'
 * -  Sets the m_fkId attribute to the id of the 
 * parameter selected.
 * 
 * @param parname - name of the parameter.
 * @param std::exception derived exception if the parameter does not exist.
 */
void
DBMetadata::selectParameter(const char* parname) {
    DBParameter param(m_connection, m_saveId, parname);
    m_type = "parameter";
    m_fkId = param.getInfo().s_id;

}
/**
 * selectSpectrum
 *    Selects the spectrum for which subsequent metadata operations will
 * be done.  m_type gets set to 'spectrum' and m_fkId to the id of the
 * spectrum.
 * 
 * @param specname - name of the specturm.
 * @throw std::exception derived exception if the spectrum does not exist.
 */
void
DBMetadata::selectSpectrum(const char* specname) {
    DBSpectrum spec(m_connection, m_saveId, specname);
    m_type = "spectrum";
    m_fkId = spec.getInfo().s_base.s_id;
}
/**
 * selectGate
 *   Selects the gate for which subsequent metadata operations will be
 * performed.  m_type gets set to 'gate' and the m_fkId to the id of that gate.
 * 
 * @param gateName - name of the gate to associate.
 * @throws std::exception derived exception if the gate does not exist
 *  in our saveset.
 */
void
DBMetadata::selectGate(const char* gateName) {
    DBGate gate(m_connection, m_saveId, gateName);
    m_type = "gate";
    m_fkId = gate.getInfo().s_info.s_id;
}

/** 
 * selectTreevar.
 *    Selects a treevariable for which subsequent metatadata operations will be
 * performed.  m_type is set to 'treevariable', m_fkId to the id of that
 * tree variable.
 * 
 * @param tvname - name of the tree variable.
 * @throws std::exception derived exception if there's no such variable.
*/
void
DBMetadata::selectTreevar(const char* tvname) {
    DBTreeVariable tv(m_connection, m_saveId, tvname);
    m_type = "treevariable";
    m_fkId = tv.getInfo().s_id;
}

/**
 * setMetadataItem
 *    Sets a metadata item for the currently selected object.  
 * If the metadata item already exists it is overwitten.
 * If not it is created.
 * 
 * @param name - name of the metadata item.
 * @param value -the value to give to it.
 */
void
DBMetadata::setMetadataItem(const char* name, const char* value) {
    CSqliteTransaction begin(m_connection);                   // All or nothing.

    // rid ourselves of any prior item:

    CSqliteStatement deleter(m_connection,
        "DELETE FROM metadata WHERE                          \
            type=? AND item_id = ?                          \
            AND name=?"
    );
    deleter.bind(1, m_type.c_str(), -1, SQLITE_STATIC);
    deleter.bind(2, m_fkId);
    deleter.bind(3, name, -1, SQLITE_STATIC);
    ++deleter;                                  // Runs the delete.

    // Insert the metadata:

    CSqliteStatement inserter(m_connection,
        "INSERT INTO metadata (type, item_id, name, value) VALUES (?,?,?,?)"
    );
    inserter.bind(1, m_type.c_str(), -1, SQLITE_STATIC);
    inserter.bind(2, m_fkId);
    inserter.bind(3, name, -1, SQLITE_STATIC);
    inserter.bind(4, name, -1, SQLITE_STATIC);

}
/**
 * getMetadataValue
 *    Get the value of a single metadata item for the established object.
 * 
 * @param name - name of the metadata item to get.
 * @return std::string - value of the item.
 * @throw std::invalid_argument  - if there's no such item.
 */
std::string
DBMetadata::getMetadataValue(const char* name) {
    CSqliteStatement stmt(m_connection,
        "SELECT value FROM metadata                 \
            WHERE type=? AND item_id=? AND name=?   \
        "
    );
    stmt.bind(1, m_type.c_str(), -1, SQLITE_STATIC);
    stmt.bind(2, m_fkId);
    stmt.bind(3, name, -1, SQLITE_STATIC);

    // one or none:

    ++stmt;
    if (!stmt.atEnd()) {
        std::string result = reinterpret_cast<const char*>(stmt.getText(0));
        return result;
    } else {
        throw std::invalid_argument("no such metadata item");
    }
}
/**
 * dumpMetadata 
 *    Gets all the metadata for the selected item.  The metadata will be ordered by name
 * ascending.
 * 
 *  @return std::vector<std::pair<std::stirng, std::string>>  The first item of each pair is
 * the name of a metadtaa item, the second its value.
 */
std::vector<std::pair<std::string, std::string>>
DBMetadata::dumpMetadata() {
    CSqliteStatement stmt(m_connection,
        "SELECT name, value FROM metadata \
            WHERE type=? and item_id=? \
            ORDER BY name ASC"
    );

    stmt.bind(1, m_type.c_str(), -1, SQLITE_STATIC);
    stmt.bind(2, m_fkId);

    std::vector<std::pair<std::string, std::string>> result;
    while (!(++stmt).atEnd()) {
        std::string name  = reinterpret_cast<const char*>(stmt.getText(0));
        std::string value = reinterpret_cast<const char*>(stmt.getText(1));

        result.push_back(std::pair(name, value));
    }
    return result;
}

}    // namespace SpecTclDB.