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

/** @file:  SpecTclDatabase.h
 *  @brief: Main data base class.
 */
#ifndef SPECTCLDATABASE_H
#define SPECTCLDATABASE_H

#include "CSqlite.h"
#include <vector>

// Time we started putting SpecTcl stuff in it's own namespace.

namespace SpecTclDB {
    class SaveSet;
    
    /**
     * @class CDataBase
     *     This class is the top level API into a SpecTcl configuration
     *     database.   It provides:
     *     -   Methods for creating new databases.
     *     -   Methods for Opening existing databases.
     *     -   Methods for listing and creating savesets.
     */
    class CDatabase {
    private:
        CSqlite m_connection;
        
    public:
        static void create(const char* database);
        
        // Create/Retreive save sets from the database:
        
        SaveSet* createSaveSet(const char* name);
        SaveSet* getSaveSet(const char* name);
        SaveSet* getSaveSet(int id);
        std::vector<SaveSet*> getAllSaveSets();
    public:
        CDatabase(const char* database);
        virtual ~CDatabase() {};
        
        // Forbidden canonicals.
        
    private:
        CDatabase(const CDatabase&);
        CDatabase& operator=(const CDatabase&);
        int operator==(const CDatabase&);
        int operator!=(const CDatabase&);
        
        // utilities:
        
        void checkTables();
    };
    
}                         // SpecTclDB namespace.


#endif