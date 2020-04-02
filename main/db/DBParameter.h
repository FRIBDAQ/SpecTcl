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

/** @file:  DBParameter.h
 *  @brief: Provide CRud interface for parameters in a saveset.
 */
#ifndef DBPARAMETER_H
#define DBPARAMETER_H
#include <string>
#include <vector>

class CSqlite;
namespace SpecTcl {
    class SaveSet;
    
    /**
     * @class DBParameter
     *    Allows creation and retrieval of a parameter
     *    from a save set.
     */
    class DBParameter {
    public:
        struct Info {
            int         s_id;
            int         s_savesetId;
            std::string s_name; 
            int         s_number;
            bool        s_haveMetadata;
            double      s_low;
            double      s_high;
            int         s_bins;
            std::string s_units;
            Info() {}
            Info(const Info& rhs) {
                copyIn(rhs);
            };
            Info& operator=(const Info& rhs) {
                copyIn(rhs);
                return *this;
            }
            void copyIn(const Info& rhs) {
                s_id = rhs.s_id;
                s_savesetId = rhs.s_savesetId;
                s_name = rhs.s_name;
                s_number = rhs.s_number;
                s_haveMetadata = rhs.s_haveMetadata;
                if(s_haveMetadata) {    // Don't matter if it doesn't.
                    s_low = rhs.s_low;
                    s_high = rhs.s_high;
                    s_units = rhs.s_units;
                }
            }
        };
    private:
        CSqlite& m_connection;
        Info     m_Info;
    public:
        // Constructors retrieve.
        
        DBParameter(CSqlite& conn, int saveid, const char* name);
        virtual ~DBParameter();
       // Construct from info (used internally).
        
    private:
        DBParameter(CSqlite& connection, const Info& info);
        
        // Object methods:
    public:
        
        const Info& getInfo() { return m_Info; }
        
        // Disallowed canonicals:
        
    private:
        DBParameter(const DBParameter& );
        DBParameter& operator=(const DBParameter& );
        int operator==(const DBParameter& );
        int operator!=(const DBParameter& );

 

        
        // Static methods
        
    public:
        static bool exists(CSqlite& connection, int sid, const char* name);
        static DBParameter* create(
            CSqlite& connection, int sid, const char* name, int number
        );
        static DBParameter* create(
            CSqlite& connection, int sid, const char* name, int number,
            double low, double high, int bins, const char* units
        );
        static std::vector<DBParameter*> list(CSqlite& connection);
        
    };
}

#endif