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

/** @file:  DBGate.h
 *  @brief: Database encapsulation of a gate.
 */
#ifndef DBGATE_H
#define DBGATE_H
#include <string>
#include <vector>

class CSqlite;
class CSqliteStatement;

namespace SpecTclDB {
   
/**
 * @class DBGate
 *   This class encapsulates a gate.  Note that gates are pretty
 *   complicated things.  There is a type dependency that determines
 *   the sorts of dependent things their root table entry (gate_defs)
 *   has linked to it.  Primarily there are three type of gates:
 *   - Gates that have associated points.
 *   - Gates that have associated gates.
 *   - Gates that are masks and have an associated mask.
 * @note the c2band gate is not directly stored in the database.  Instead
 *       the equivalent contour is stored.  An attempt to store a c2band
 *       will result in an error.
 *
 *    Let's look at the dependent data for each of these types of gates:
 *
 *    - Gates with associated points have parameters, and points.  These
 *      are in  the table (gate_paramters and gate_points).
 *    - Gates with associated gates have only gates in the component_gates
 *      table. Note that since gate ids are stored in that table,
 *      we only allow a compound gate to be stored if its component gates have
 *      been stored.  This requires the client code to do gate dependency
 *      analysis.
 *    - mask gates have a parameter (gate_parameters) and a mask (gate_masks).
 *
 *    To make all of this easier, we'll be providing different creationals
 *    for each of these classifications of gate types.
 *
 */
class DBGate {
    
    public:
        // public types:
        
        typedef std::vector<const char*> NameList;
        typedef std::vector<int>         IdList;
        
        typedef enum _BasicGateType {point, compound, mask}
            BasicGateType;   // over all gate types.  See comments above.
        // All gates have this information:
        
        struct BaseInfo {
            int           s_id;                   // Id in gate_defs table.
            int           s_saveset;              // id of saveset.
            std::string   s_name;                 // gate name.
            std::string   s_type;                 // detailed gate type.
            BasicGateType s_basictype;            // what to expect.
            
            // We need some canonicals to properly copy the strings.
            
            BaseInfo(){}
            BaseInfo(const BaseInfo& rhs) {
                copyIn(rhs);
            }
            BaseInfo& operator=(const BaseInfo& rhs) {
                if (this != &rhs) {
                    copyIn(rhs);
                }
                return *this;
            }
            void copyIn(const BaseInfo& rhs) {
                s_id       = rhs.s_id;
                s_saveset  = rhs.s_saveset;
                s_name     = rhs.s_name;
                s_type     = rhs.s_type;
                s_basictype= rhs.s_basictype;
            }
        };
        // Some gates have parameters.  These are stored as the ids of the
        // parameter, not the names or numbers.
        
        
        
        // Points:
        //   Note 1d slices, gamma slices only x is meaningful.
        
        struct Point {
            double    s_x;
            double    s_y;
        };
        typedef std::vector<Point> Points;
        
        // Ok here's the gate information:
        
        struct Info {
            BaseInfo       s_info;
            IdList         s_parameters;
            IdList         s_gates;
            Points         s_points;
            int            s_mask;
            // We need some canonicals because of the vectors and strings in info.
            
            Info(){}
            Info(const Info& rhs) {
                copyIn(rhs);
            }
            Info& operator=(const Info& rhs) {
                if (this != &rhs) {
                    copyIn(rhs);
                }
                return *this;
            }
            void copyIn(const Info& rhs) {
                s_info       = rhs.s_info;
                s_parameters = rhs.s_parameters;
                s_gates      = rhs.s_gates;
                s_points     = rhs.s_points;
                s_mask       = rhs.s_mask;
            }
        };
        private:
            CSqlite&  m_connection;
            Info     m_Info;
        
        // Canonicals:
        
        
        private:                            // Disallowed:
            DBGate(const DBGate&);
            DBGate& operator=(const DBGate&);
            int operator==(const DBGate&);
            int operator!=(const DBGate&);
            
        // Constructor used by creational methods:
            
            DBGate(CSqlite& conn, const Info& info);
            
        // Public constructors are lookups:
        
        public:
            DBGate(CSqlite& conn, int saveid, const char* name);
            DBGate(CSqlite& conn, int id);
        
        // Object methods:
        
        const Info& getInfo() const { return m_Info; }
        std::vector<std::string> getParameters();
        std::vector<std::string> getGates();
        Points   getPoints();
        int      getMask();
        
        // Static methods:
        
        static bool exists(CSqlite& conn, int saveid, const char* name);
        static DBGate* create1dGate(
            CSqlite& conn, int saveid,
            const char* name, const char* type,
            const NameList& params, double low, double high
        );
        static DBGate* create2dGate(
            CSqlite& conn, int saveid,
            const char* name, const char* type,
            const NameList& params, const Points& points
        );
        static DBGate* createCompoundGate(
            CSqlite& conn, int saveid,
            const char* name, const char* type,
            const NameList& gates
        );
        static DBGate* createMaskGate(
            CSqlite& conn, int saveid,
            const char* name, const char* type,
            const char* pName, int imask
        );
        static std::vector<DBGate*> listGates(CSqlite& conn, int saveid);
        
        // Utility methods:
        
        private:
            // Creation support.
            
            static DBGate* createPointGate(
                CSqlite& conn, int saveid,
                const char* name, const char* type,
                const NameList& params, const Points& points
            );
            static void checkName(CSqlite& conn, int saveid, const char* name);
            static int gateId(CSqlite& conn, int saveid, const char* name);
            static void enterBase(CSqlite& conn,  BaseInfo& baseInfo);
            static IdList
                enterParams(CSqlite& conn, int sid, int gid, const NameList& params);
            static void enterPoints(CSqlite& conn, int gid, const Points& pts);
            static IdList enterDependentGates(
                CSqlite& conn, int sid, int gid, const NameList& gates    
            );
            static void enterMask(
                CSqlite& conn, int gid, int mask
            );
            // For lookups:
            
            static BasicGateType basicType(const char* gateType);
            static void loadBase(CSqliteStatement& stmt, BaseInfo& info);
            static void loadPointGate(CSqlite& conn, Info& info);
            static void loadCompoundGate(CSqlite& conn, Info& info);
            static void loadMaskGate(CSqlite& conn, Info& info);
            
            static void loadInfo(
                CSqlite& conn, Info& info
            );
            
};      
}                                                   //namespace SpecTclDB.


#endif