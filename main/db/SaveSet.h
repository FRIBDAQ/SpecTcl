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

/** @file:  SaveSet.h
 *  @brief: Implement save sets.
 */
#ifndef SAVESET_H
#define SAVESET_H
#include <time.h>
#include <string>
#include <vector>
class CSqlite;
class CSqliteStatement;

namespace SpecTcl {
    /**
     * @class SaveSet
     *    This class encapsulates a save set and all the things
     *    you can do to it _except_ playing back events.
     *
     */
    class SaveSet {
    public:
        struct Info {
            int         s_id;
            std::string s_name;   // requires copy construction etc.
            time_t      s_stamp;
        Info() {}
        Info(const Info& rhs) {
            copyIn(rhs);
        }
        Info& operator=(const Info& rhs) {
            copyIn(rhs);
            return *this;
        }
        void copyIn(const Info& rhs) {
            s_id = rhs.s_id;
            s_name = rhs.s_name;
            s_stamp = rhs.s_stamp;
        }
        };
    private:
        CSqlite& m_connection;
        Info     m_Info;
    public:
        SaveSet(CSqlite& conn, const char* name);     // Construct given name
        SaveSet(CSqlite& conn, int id);
    
    // Forbidden canonicals:
    private:
        SaveSet(const SaveSet&);
        SaveSet& operator=(const SaveSet&);
        int operator==(const SaveSet&);
        int operator!=(const SaveSet&);
        
    // Object methods:
    public:
        const Info& getInfo() {return m_Info;}
    
    // Static methods:
    public:
        static bool exists(CSqlite& conn, const char* name);
        static SaveSet* create(CSqlite& conn, const char* name);
        static std::vector<Info> list(CSqlite& conn);
    // Utilities:
    
    private:
        static void loadInfo(Info& result, CSqliteStatement& stmt);
    };
}

#endif