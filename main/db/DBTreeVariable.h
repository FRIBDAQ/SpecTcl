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

/** @file:  DBTreeVariable.h
 *  @brief: Encapsulate database records representing tree variables.
 */
#ifndef DBTREEVARIABLE
#define DBTREEVARIABLE
#include <vector>
#include <string>

class CSqlite;

namespace SpecTcl {
    
/**
 * @class DBTreeVariable
 *    This class encapsulates database records that represent a SpecTcl
 *    treevariable.  Tree variables are Tcl variables with a touch of metadata
 *    (units of measure).  In SpecTcl, there is a class encapsulation that makes
 *    them easy to use in C++ code as steering variables.  The type
 *    of a tree variable is _double_
 */
class DBTreeVariable {
public:
    struct Info {
        int         s_id;
        int         s_saveset;
        std::string s_name;
        double      s_value;
        std::string s_units;
        
        void copyIn(const Info& rhs) {
            s_id      = rhs.s_id;
            s_saveset = rhs.s_saveset;
            s_name    = rhs.s_name;
            s_value   = rhs.s_value;
            s_units   = rhs.s_units;
        }
        Info() {}
        Info(const Info& rhs) {
            copyIn(rhs);
        }
        Info& operator=(const Info& rhs) {
            if (this != &rhs) {
                copyIn(rhs);
            }
            return *this;
        }
    };
private:
    CSqlite& m_connection;
    Info     m_Info;
    
    // Canonicals:
public:
    DBTreeVariable(CSqlite& connection, int saveid, const char* varname);
private:
    DBTreeVariable(CSqlite& connection, const Info& info);    // Used internally.
    
    DBTreeVariable(const DBTreeVariable& rhs);         // forbidden
    DBTreeVariable& operator=(const DBTreeVariable& rhs);         // forbidden
    int operator==(const DBTreeVariable& rhs);         // forbidden
    int operator!=(const DBTreeVariable& rhs);         // forbidden
    
    // object methods:
public:
    const Info& getInfo() const { return m_Info; }
    std::string getName() const;
    double      getValue() const;
    std::string getUnits() const;
    
    // Static methods:
    
    bool exists(CSqlite conn, int saveid, const char* name);
    static DBTreeVariable* create(
        CSqlite& conn, int saveid, const char* name, double value,
        const char* units=""
    );
   std::vector<DBTreeVariable*> list(CSqlite& conn, int saveid);
   
   // Private utilities:

private:
    
};
}                              // namespace SpecTcl


#endif