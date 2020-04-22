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

/** @file:  DBGate.cpp
 *  @brief: Implementation of SpecTclDB::DBGate.
 */
#include "DBGate.h"
#include "SaveSet.h"
#include "DBParameter.h"

#include <CSqlite.h>
#include <CSqliteStatement.h>
#include <CSqliteTransaction.h>
#include <CSqliteWhere.h>
#include <set>
#include <stdexcept>
#include <sstream>


namespace SpecTclDB {

static std::set<std::string> allowed1dTypes  = {"s", "gs"};
static std::set<std::string> allowed2dTypes = {
        "c", "gc", "b", "gb"
    };  
static std::set<std::string> allowedCompoundTypes = {
        "T", "F", "-", "+", "*"
    };
static std::set<std::string> allowedMaskTypes = {
        "em", "am", "nm"
    };

/**
 * private constructor.
 *   This constructor is used by the creationals.  It is not
 *   intended for use by client and, hence, is private.
 * @param conn - the Sqlite connection to use in queries.
 * @param info - The information struct.
 */
DBGate::DBGate(CSqlite& conn, const Info& info) :
    m_connection(conn), m_Info(info) {}
    

/**
 * public constructor (retrieval)
 *    Wraps an existing gate in an object.
 *
 *  @praam connn - sqlite connection object.
 *  @param saveid  - saveid Save set id.
 *  @param name    -  name of the gate.
 */
DBGate::DBGate(CSqlite& conn, int saveid, const char* name) :
    m_connection(conn)
{
    // Validate the saveset:
    
    SaveSet s(conn, saveid);       // Throws if no such.
    CSqliteStatement retrieve(
        conn,
        "SELECT * from gate_defs WHERE name = ? AND saveset_id = ?"
    );
    retrieve.bind(1, name, -1, SQLITE_STATIC);
    retrieve.bind(2, saveid);
    ++retrieve;
    
    // If we're at the end there's no such gate:
    
    if (retrieve.atEnd()) {
        std::stringstream msg;
        msg << "Save set " << s.getInfo().s_name
            << " does not have a gate named: " << name;
        throw std::invalid_argument(msg.str());
    }
    loadBase(retrieve, m_Info.s_info);
    loadInfo(conn, m_Info);
}
/**
 * public constructor (retrieve).
 *    Same as above but retrieves by the id of the gate.
 * @param conn - sqlite connection object.
 * @param id     - id of the gate to retrieve from that saveset.
 * @note the id implies a save-set since it's globally unique
 */
DBGate::DBGate(CSqlite& conn, int id) :
    m_connection(conn)
{

    CSqliteStatement retrieve(
        conn,
        "SELECT * from gate_defs WHERE id = ?"
    );
    retrieve.bind(1, id);
    ++retrieve;
    // If we're at the end there's no such gate:
    
    if (retrieve.atEnd()) {
        std::stringstream msg;
        msg << "There is no gate with the id " << id;
        throw std::invalid_argument(msg.str());
    }
    loadBase(retrieve, m_Info.s_info);
    loadInfo(conn, m_Info);
}
/**
 * getParameters
 *    Return a String vector. containing the names of the parameters
 *    the gateNameList depends on.  If the gate does not depend on
 *    parameters an std::invalid_argument exception is thrown.
 * @return std::vector<std::string>
 */
std::vector<std::string>
DBGate::getParameters()
{
   std::vector<std::string> result;
    
    // Does this gate have parameters:
    
    auto btype = m_Info.s_info.s_basictype;
    if (btype != point && btype != mask) {
        std::stringstream msg;
        msg << "Gate " << m_Info.s_info.s_name << " is of type "
            << m_Info.s_info.s_type << " which does not depend on parameters";
        throw std::invalid_argument(msg.str());
    }
    // Fetch the parameter names:
    // We're going to do the query based on the parameter ids present in the
    // info struct.  This is proof against some other weirdness in how
    // these associate with the gate.
    
    std::string query =
        "SELECT name FROM gate_parameters AS gp\
         INNER JOIN parameter_defs AS pd ON pd.id = gp.parameter_id \
         WHERE parent_gate = ? AND ";
    CInFilter in("gp.parameter_id", m_Info.s_parameters);
    query += in.toString();
    query += " ORDER BY gp.id ASC";  // Same order as in the definition.
    
    CSqliteStatement fetch(m_connection, query.c_str());
    fetch.bind(1, m_Info.s_info.s_id);
    while (!(++fetch).atEnd()) {
        result.push_back(reinterpret_cast<const char*>(fetch.getText(0)));
    }
    return result;
}
/**
 * getGates
 *   Returns the names of all of the gates that this gate depends on.
 *   Throws an exception if this gate does not depend on other gates.
 * @return std::vector<std::string>
 */
std::vector<std::string>
DBGate::getGates()
{
    std::vector<std::string> result;
    
    // Only compound gates depend on gates:
    
    if (m_Info.s_info.s_basictype != compound) {
        std::stringstream msg;
        msg << "Gate: " << m_Info.s_info.s_name
            <<  " is a " << m_Info.s_info.s_type << " gate."
            <<  " This gate type does not depend on other gates";
        throw std::invalid_argument(msg.str());
    }
    // T/F gates don't have gate name dependencies:
    
   if (m_Info.s_gates.size() > 0) {
      
      std::string query =
          "SELECT gd.name FROM component_gates AS cg \
              INNER JOIN gate_defs AS gd ON gd.id = cg.child_gate\
              WHERE parent_gate = ? AND ";
      CInFilter in("cg.child_gate", m_Info.s_gates);
      query += in.toString();
      query += " ORDER BY cg.id ASC";           // Same as defined order.
      CSqliteStatement q(m_connection, query.c_str());
      q.bind(1, m_Info.s_info.s_id);
      while(!(++q).atEnd()) {
          result.push_back(reinterpret_cast<const char*>(q.getText(0)));
      }
    }
   return result;
}
/**
 * getPoints
 *    Get the points from a point gate.
 * @return SpecTclDB::DBGate::Points
 */
DBGate::Points
DBGate::getPoints()
{
    Points result;
    
    // Validate gate type:
    
    if (m_Info.s_info.s_basictype != point) {
        std::stringstream msg;
        msg << "The gate: " << m_Info.s_info.s_name <<  " is of type "
            << m_Info.s_info.s_type << " which does not have associated points";
        throw std::invalid_argument(msg.str());
    }
    CSqliteStatement fetch(
        m_connection,
        "SELECT x,y from gate_points WHERE gate_id = ? ORDER BY id ASC"
    );
    fetch.bind(1, m_Info.s_info.s_id);
    
    while(!(++fetch).atEnd()) {
        result.push_back({fetch.getDouble(0), fetch.getDouble(1)});
    }
    
    return result;
}
/**
 * getMask
 *   Retrieve the mask for a mask gate
 * @return int
 */
int
DBGate::getMask()
{
    // Only mask types have a mask:
    
    if (m_Info.s_info.s_basictype != mask) {
        std::stringstream msg;
        msg << "The gate: " << m_Info.s_info.s_name <<  " is of type "
            << m_Info.s_info.s_type << " which does not have an associated bitmask";
        throw std::invalid_argument(msg.str());
    }
    CSqliteStatement fetch(
        m_connection,
        "SELECT mask FROM gate_masks WHERE parent_gate = ?"
    );
    fetch.bind(1, m_Info.s_info.s_id);
    ++fetch;
    if (fetch.atEnd()) {
        std::stringstream msg;
        msg << "The gate " << m_Info.s_info.s_name
            << " is of type " << m_Info.s_info.s_type
            << " it should have but does not have an associated mask";
        throw std::logic_error(msg.str());
    }
    return fetch.getInt(0);
}
/////////////////////////////////////////////////////////////
//  static methods implementations

/**
 * exists
 *   @praam conn - database connection object.
 *   @param sid  - save set id.
 *   @param name - Name of the gate.
 *   @return bool - true if the gate exists.
 *   @throw std::invalid_argument - if the save set does not exist.
 */
bool
DBGate::exists(CSqlite& conn, int saveid, const char* name)
{
    SaveSet s(conn, saveid);           // Throws if no such saveset.
     try {
        gateId(conn, saveid, name);
        return true;
     } catch(...) {
        return false;
     }
     throw std::logic_error("DBGate::exists - should not fall through!!!");
}
/**
 * create1dGate
 *    Create a 1d gate.  A 1d gate is a special example of
 *    a point gate that has exactly two points whose y
 *    coordinates are meaningless and whose x coordinates in
 *    turn are the gate's low an high limits.
 *    Only slice ("s") and Gamma slice ("gs") gates are allowed.
 *
 * @param conn    - Sqlite connection object.
 * @param saveid  - Saveset id.
 * @param name    - Name of new gate (must be unique.)
 * @param type    - gate type string.
 * @param params  - Gate parameters.
 * @param low, high - gate limits.
 * @return DBGate* - Pointer to resulting gate (dynamically allocated)
 */
DBGate*
DBGate::create1dGate(
    CSqlite& conn, int saveid,
    const char* name, const char* type,
    const NameList& params, double low, double high
)
{
    // Type must be either "s" or "gs"
    
    if (allowed1dTypes.count(std::string(type)) != 1) {
        std::stringstream msg;
        msg << "creat1dGate must have either  's' or a 'gs' type. Got: "
            << type;
        throw std::invalid_argument(msg.str());
                        
    }
    
    // Marshall the low/high into points:
    
    Points pts;
    pts.push_back({low, 0.0});
    pts.push_back({high, 0.0});
    return createPointGate(conn, saveid, name, type, params, pts);
    
}
/**
 * create2dGate
 *   Create a gate on a 2-dimenstional space.   This is
 *   essentially create1dGate with
 *   - more legal gate types.
 *   - an arbitrary number of points.
 * @param conn - Sqlite databae connection object.
 * @param saveid - Id of save set in which the gate will be saved.
 * @param name    - Name of the new gate.
 * @param type    - Type of the new gate.
 * @param params  - Parameters involved in the gate.
 * @param points  - Points involved in the gate.
 * @return DBGate* - pointer to the dynamically created new gate.
 */
DBGate*
DBGate::create2dGate(
    CSqlite& conn, int saveid,
    const char* name, const char* type,
    const NameList& params, const Points& points
)
{
    // Check the gate type and then use the common code
    // with 1d gates (createPointGate).
    
                           // note c2band is a contour.
    
    if( allowed2dTypes.count(std::string(type)) == 0) {
        std::stringstream msg;
        msg << type << " is not an allowed 2d gate type";
        throw std::invalid_argument(msg.str());
    }
    
    return createPointGate(conn, saveid, name, type, params, points);
}
/**
 * createCompoundGate
 *    Create a gate that depends on other gates.
 *
 * @param conn - the database connection object.
 * @param saveid  - Saveset into which the gate will be entered.
 * @param type    - Gate type.
 * @param gates   - Name of dependent gates.
 * @return DBGate*  - Pointer to the dynamically allocated gate object.
 */
DBGate*
DBGate::createCompoundGate(
    CSqlite& conn, int saveid,
    const char* name, const char* type,
    const NameList& gates
)
{
    // Check against legal type:
    
    
    if (allowedCompoundTypes.count(std::string(type)) == 0) {
        std::stringstream msg;
        msg << type << " is not a compound gate";
        throw std::invalid_argument(msg.str());
    }
    checkName(conn, saveid, name);
    
    // Fill in the info record
    
    Info info;
    info.s_info.s_id      = -1;
    info.s_info.s_saveset = saveid;
    info.s_info.s_name    = name;
    info.s_info.s_type    = type;
    info.s_info.s_basictype = compound;
    
    // enter the gate inside a transaction:
    
    {
        CSqliteSavePoint t(conn, "entergate");
        try {
            enterBase(conn, info.s_info);
            info.s_gates = enterDependentGates(
                conn, saveid, info.s_info.s_id, gates
            );
        }
        catch (...) {
            t.rollback();
            throw;                // Let the callers handle the actual error.
        }
    }                             // commits here.
    return new DBGate(conn, info);
}
/**
 * createMaskGate
 *    Create a gate with a mask value.
 * @param conn - Sqlite connection object.
 * @param saveid - the save set id.
 * @param name   - name of the new gate.
 * @param pName  - Name of the parameter.
 * @paran imask   - Mask value.
 * @return DBGate* - pointer to the dynamically created gate encapsulation.
 */
DBGate*
DBGate::createMaskGate(
    CSqlite& conn, int saveid,
    const char* name, const char* type,
    const char* pName, int imask
)
{
    // Do the necessary error checking
    
    
    if (allowedMaskTypes.count(std::string(type)) == 0) {
        std::stringstream msg;
        msg << type << " is not an allowed mask gate type";
        throw std::invalid_argument(msg.str());
    }
    checkName(conn, saveid, name);
    
    // Fill in the base info struct:
    
    Info info;
    info.s_info.s_id        = -1;
    info.s_info.s_saveset   = saveid;
    info.s_info.s_name      = name;
    info.s_info.s_type      = type;
    info.s_info.s_basictype = mask;
    info.s_mask             = imask;
    
    // Create a NameList containing the one parameter:
    
    NameList params = {pName};
    
    // Do the gate entry  in a transaction:
    
    {
        CSqliteSavePoint t(conn, "entergate");
        try {
            enterBase(conn, info.s_info);
            info.s_parameters = enterParams (
                conn, saveid, info.s_info.s_id, params
            );
            enterMask(conn, info.s_info.s_id, imask);
        }
        catch (...) {
            t.rollback();
            throw;
        }
    }
    return new DBGate(conn, info);
}
/**
 * listGates
 *    Returns a list of the gates for the specified save set id.
 * @param conn   - sqlite connection object.
 * @param saveid - Id of the saveset we're querying.
 * @return std::vector<DBGate*>  - vector of dynamically allocated gates
 *                 that are in the saveset.
 */
std::vector<DBGate*>
DBGate::listGates(CSqlite& conn, int saveid)
{
    SaveSet s(conn, saveid);              // validates the saveset.
    std::vector<DBGate*> result;
    CSqliteStatement lister(
        conn,
        "SELECT * FROM gate_defs WHERE saveset_id = ? ORDER BY id ASC"
    );
    lister.bind(1, saveid);
    while(!(++lister).atEnd()) {
        Info gateInfo;
        loadBase(lister, gateInfo.s_info);
        loadInfo(conn, gateInfo);
        
        result.push_back(new DBGate(conn, gateInfo));
    }
    return result;
}
 
//////////////////////////////////////////////////////////////
// Utility method implementations.


/**
 * createPointGate
 *    Common code to create a point gate once the gate type has been
 *    verified as good:
 *
 * @param conn - connection object.
 * @param saveid  - save set into which the gate is being entered.
 * @param type    - type of gate.
 * @param params  - parameters the gate depends on.
 * @param points  - gate points.
 * @return dynamically created gate object.
 * 
 */
DBGate*
DBGate::createPointGate(
    CSqlite& conn, int saveid,
    const char* name, const char* type,
    const NameList& params, const Points& points
)
{
    
    checkName(conn, saveid, name);
    // Create the info block:
    
    Info info;
    info.s_info.s_id      = -1;
    info.s_info.s_saveset = saveid;
    info.s_info.s_name    = name;
    info.s_info.s_type    = type;
    info.s_info.s_basictype = point;
    info.s_points = points;
    {
        CSqliteSavePoint t(conn, "entergate");         // All enters are atomic:
        try {
           enterBase(conn, info.s_info);
           info.s_parameters =
           enterParams(conn, saveid, info.s_info.s_id, params);
           enterPoints(conn, info.s_info.s_id, info.s_points);
           
        } catch(...) {             // something failed.
            t.rollback();          // rollback the transaction.
            throw;                 // rethrow for reporting/handling.
        }
    }                   // Implicit commit.
    
    
    return new DBGate(conn, info);
    
}
/**
 * checkName
 *    Throws an exception if the proposed gate name already exists in the
 *    saveset.
 * @param conn   - Sqlite connection.
 * @param saveid - save set.
 * @param name   - proposed name
 */
void
DBGate::checkName(CSqlite& conn, int saveid, const char* name)
{
    if (exists(conn, saveid, name) ) {    // Throws if bad saveset.
        SaveSet sset(conn, saveid);
        std::stringstream msg;
        msg << name << " Is a gate that's already defined in " 
            << sset.getInfo().s_name;
            
        throw std::invalid_argument(msg.str());
    }
}

/**
 * gateId
 *    Given a gate name, returns the id of the gate in the saveset.
 *
 *  @param conn - connection object.
 *  @param sid  - Save set id.
 *  @param name - Gate name.
 *  @return int - gate id.
 *  @throw std::invalid_argument if errors.
 */
int
DBGate::gateId(CSqlite& conn, int saveid, const char* name)
{
    
    CSqliteStatement s(
        conn,
        "SELECT id FROM gate_defs WHERE name = ? AND saveset_id = ?"
    );
    s.bind(1, name, -1, SQLITE_STATIC);
    s.bind(2, saveid);
    
    ++s;
    if (s.atEnd()) {
        SaveSet svset(conn, saveid);
        std::stringstream msg;
        msg << "Save set " << svset.getInfo().s_name <<  " has no gate named "
            << name;
        throw std::invalid_argument(msg.str());
    }
    
    return s.getInt(0);
}
/**
 * enterBase
 *    Given the base information struct, enters it in the
 *    gate_defs table and  sets its s_id element.
 *  @praam conn - Sqlite connection object.
 *  @param[inout] baseInfo - reference to the information to go in
 *            the root record.
 * @note This should normally execute in a transaction so that
 *       all inserts associated with this gate are one atomic unit.
 */
void
DBGate::enterBase(CSqlite& conn, BaseInfo& baseInfo)
{
    CSqliteStatement ins(
        conn,
        "INSERT INTO gate_defs (saveset_id, name, type) VALUES (?,?,?)"
    );
    ins.bind(1, baseInfo.s_saveset);
    ins.bind(2, baseInfo.s_name.c_str(), -1, SQLITE_STATIC);
    ins.bind(3, baseInfo.s_type.c_str(), -1, SQLITE_STATIC);
    ++ins;                // do the insert or throw.
    
    baseInfo.s_id = ins.lastInsertId(); 
}
/**
 * enterParams
 *   Enter the parameters associated with a gate into the
 *   gate_parameters table.  Each parameter is looked up
 *   and its id is inserted in the table.
 *
 * @param conn - connection object.
 * @param sid  - save set id (needed to lookup parametes sensibly).
 * @param gid  - Gate id to associate with the parameters.
 * @param params - vector of paramter _names_
 * @return IdList - list of parameter ids corresponding to params. names.
 * @note this should be called within a transaction usually so that all
 *        insertion operations related to making a gate are atomic.
 */
DBGate::IdList
DBGate::enterParams(CSqlite& conn, int sid, int gid, const NameList& params)
{
    IdList result;
    CSqliteStatement ins(
        conn,
        "INSERT INTO gate_parameters (parent_gate, parameter_id) VALUES (?,?)"
    );
    ins.bind(1, gid);                 // Constant.
    for (int i =0; i < params.size(); i++) {
        DBParameter p(conn, sid, params[i]);     // Throws if no such.
        ins.bind(2, p.getInfo().s_id);
        ++ins;
        result.push_back(p.getInfo().s_id);
        ins.reset();
    }
    
    return result;
}
/**
 * enterPoints
 *    Enters the gate points associated with a point gate.
 * @param conn  - sqlite connection.
 * @param gid   - Id of the gate these points go with.
 * @param pts   - the points themselves.
 * @note This method should be called from inside a transaction
 *       to ensure that all inserts associated with a gate are atomic.
 */
void
DBGate::enterPoints(CSqlite& conn, int gid, const Points& pts)
{
    CSqliteStatement s(
        conn,
        "INSERT INTO gate_points (gate_id, x, y) VALUES(?,?,?)"
    );
    s.bind(1, gid);
    for (int i =0; i < pts.size(); i++) {
        s.bind(2, pts[i].s_x);
        s.bind(3, pts[i].s_y);
        ++s;
        s.reset();
    }
}
/**
 * enterDependentGates
 *    This enters the gates a compound gate depends on into  the
 *    component_gates table.  This method should be called inside
 *    a transaction so that all of the operations needed to
 *    enter a gate are an atomic group.
 *
 *  @param conn - the connection
 *  @param sid  - save set id.
 *  @param gates - names of all the gates.
 *  @return IdList - list of the entered gates.
 */
DBGate::IdList
DBGate::enterDependentGates(
    CSqlite& conn, int sid, int gid, const NameList& gates    
)
{
    // Generate the list of ids:
    
    IdList result;
    for (int i = 0; i < gates.size(); i++) {
        result.push_back(gateId(conn, sid, gates[i]));
    }
    
    // enter the gate ids in component_gates
    
    CSqliteStatement ins(
        conn,
        "INSERT INTO component_gates (parent_gate, child_gate)   \
            VALUES (?,?)"
    );
    ins.bind(1, gid);                  // Fixed for all insertions.
    for (int i =0; i < result.size(); i++) {
        ins.bind(2, result[i]);
        ++ins;
        ins.reset();
    }
    
    // Return the id list to the caller.
    
    return result;
}
/**
 * enterMask
 *    For a gate with masks, enter the mask in the gate_masks table.
 *  @param conn - SQLITE connection object.
 *  @param gid  - Gate's id.
 *  @param mask - mask value.
 */
void
DBGate::enterMask(CSqlite& conn, int gid, int mask)
{
    CSqliteStatement insert(
        conn,
        "INSERT INTO gate_masks (parent_gate, mask) VALUES (?,?)"
    );
    insert.bind(1, gid);
    insert.bind(2, mask);
    ++insert;
}
/**
 * basicType
 *    Given a gate type string returns the enum that
 *    provides the basic gate type.
 *
 * @param gateType - the gate type (e.g. "c" for contour)
 * @return BasicGateType - the type of gate from the database point of view.
 */
DBGate::BasicGateType
DBGate::basicType(const char* gateType)
{
    std::string t(gateType);
    if ((allowed1dTypes.count(t) == 1) || (allowed2dTypes.count(t) == 1)) {
        return point;
    }
    if (allowedCompoundTypes.count(t) == 1) {
        return compound;
    }
    if (allowedMaskTypes.count(t) ==1) {
        return mask;
    }
    std::stringstream msg;
    msg << t << "is not a recognized gate type";
    throw std::invalid_argument(msg.str());
}
/**
 * loadBase
 *    Given an sqlite statement of the form
 *    SELECT * from gate_defs... loads
 *    the base part of info struct from the data
 *    in the current row of the statement.
 * @param stmt - The Sqlite statement object.
 * @param[out] info - the struct to load.
 */
void
DBGate::loadBase(CSqliteStatement& stmt, BaseInfo& info)
{
    info.s_id      = stmt.getInt(0);
    info.s_saveset = stmt.getInt(1);
    info.s_name    = std::string(reinterpret_cast<const char*>(stmt.getText(2)));
    info.s_type    = std::string(reinterpret_cast<const char*>(stmt.getText(3)));
    info.s_basictype = basicType(info.s_type.c_str());
}
/**
 * loadPointGate
 *    Given an info struct whos base info was filled in previously,
 *    and is known to be a point gate, fill in the rest of the
 *    struct (specifically the parameters and Points).
 *
 * @param conn - sqlite connection object.
 * @param[inout] info - The gate information struct.
 */
void
DBGate::loadPointGate(CSqlite& conn, Info& info)
{
    int id  = info.s_info.s_id;             // Gate id is a fine lookup key.
    
    CSqliteStatement par(
        conn,
        "SELECT parameter_id FROM gate_parameters WHERE parent_gate = ?"
    );
    par.bind(1, id);
    while(!(++par).atEnd()) {
        info.s_parameters.push_back(par.getInt(0));
    }
    // Now the points:
    
    CSqliteStatement pts(
        conn,
        "SELECT x,y FROM gate_points WHERE gate_id = ?"
    );
    pts.bind(1, id);
    while (!(++pts).atEnd()) {
        Point p = {pts.getDouble(0), pts.getDouble(1)};
        info.s_points.push_back(p);
    }
}
/**
 * loadCompoundGate
 *   GIven an info struct with the base part filled in that has been
 *   determined to be for a compound gate, fills in the rest of the
 *   struct (specifically the gates the compound depends on).
 *
 * @param conn - Sqlite connection object.
 * @param[inout] info - the info object.
 */
void
DBGate::loadCompoundGate(CSqlite& conn, Info& info)
{
    int id = info.s_info.s_id;            // Gate id is a fine lookup key.
    
    CSqliteStatement gates(
        conn,
        "SELECT child_gate FROM component_gates WHERE parent_gate = ?"
    );
    gates.bind(1, id);
    while(!(++gates).atEnd()) {
        info.s_gates.push_back(gates.getInt(0));
    }
}
/**
 * loadMask
 *    Given an info struct with the base part filled in,
 *    that has been determined to be a mask gate, retrieves
 *    the rest of that struct; specifically the parameter and
 *    the mask
 * @param conn  - The database connection object.
 * @param[inout] info  - The partially filled in info struct.
 */
void
DBGate::loadMaskGate(CSqlite& conn, Info& info)
{
    int id = info.s_info.s_id;
    
    CSqliteStatement par(
        conn,
        "SELECT parameter_id FROM gate_parameters WHERE parent_gate = ?"
    );
    par.bind(1, id);
    ++par;
    info.s_parameters.push_back(par.getInt(0));
    
    CSqliteStatement mask(
        conn,
        "SELECT mask FROM gate_masks WHERE parent_gate = ?"
    );
    mask.bind(1, id);
    ++mask;
    info.s_mask = mask.getInt(0);
}
/**
 * loadInfo
 *    Given an info block that's got it's base part filled in, invokes
 *    the gate type dependent code to load the rest of the info struct.
 * @param conn - sqlite connection object.
 * @param[inout] info - info block.
 */
void
DBGate::loadInfo(CSqlite& conn, Info& info)
{
    switch (info.s_info.s_basictype) {
    case point:
        loadPointGate(conn, info);
        break;
    case compound:
        loadCompoundGate(conn, info);
        break;
    case mask:
        loadMaskGate(conn, info);
        break;
    default:
        {
            std::stringstream msg;
            msg << "Unclassifiable gate type: " << info.s_info.s_type
                << "  while trying to load information about that gate";
            throw std::invalid_argument(msg.str());
        }
    }
}
}                                           // SpecTclDB namespace.