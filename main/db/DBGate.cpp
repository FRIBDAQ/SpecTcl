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
 *  @brief: Implementation of SpecTcl::DBGate.
 */
#include "DBGate.h"
#include "SaveSet.h"
#include "DBParameter.h"

#include <CSqlite.h>
#include <CSqliteStatement.h>
#include <CSqliteTransaction.h>
#include <set>
#include <stdexcept>
#include <sstream>


namespace SpecTcl {

/**
 * private constructor.
 *   This constructor is used by the creationals.  It is not
 *   intended for use by client and, hence, is private.
 * @param conn - the Sqlite connection to use in queries.
 * @param info - The information struct.
 */
DBGate::DBGate(CSqlite& conn, const Info& info) :
    m_connection(conn), m_Info(info) {}
    

    
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
    
    if (std::string("s") != type && std::string("gs") != type) {
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
    
    static std::set<std::string> allowedTypes = {
        "c", "gc", "b", "gb"
    };                         // note c2band is a contour.
    
    if( allowedTypes.count(std::string(type)) == 0) {
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
    
    static std::set<std::string> allowedTypes = {
        "T", "F", "-", "+", "*"
    };
    if (allowedTypes.count(std::string(type)) == 0) {
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
        CSqliteTransaction t(conn);
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
    
    static std::set<std::string> allowedTypes = {
        "em", "am", "nm"
    };
    if (allowedTypes.count(std::string(type)) == 0) {
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
        CSqliteTransaction t(conn);
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
        CSqliteTransaction t(conn);         // All enters are atomic:
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
}                                           // SpecTcl namespace.