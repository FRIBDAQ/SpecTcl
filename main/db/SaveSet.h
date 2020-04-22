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

namespace SpecTclDB {
    class DBParameter;
    class DBSpectrum;
    class DBGate;
    class DBApplication;   
    class DBTreeVariable;
    
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
            if (&rhs != this) copyIn(rhs);
            return *this;
        }
        void copyIn(const Info& rhs) {
            s_id = rhs.s_id;
            s_name = rhs.s_name;
            s_stamp = rhs.s_stamp;
        }
        };
        struct SpectrumAxis {
            double s_low;
            double s_high;
            int    s_bins;
        };
        // Stuff having to do with run playback:
        
        struct RunInfo {
            int         s_runNumber;
            std::string s_title;
            time_t      s_startTime;
            time_t      s_stopTime;
            
        };
        struct ScalerReadout {
            int    s_sourceId;
            int    s_startOffset;
            int    s_stopOffset;
            int    s_divisor;
            time_t s_time;
            std::vector<int> s_values;
        };
        struct EventParameter {   // Event blobs are a soup of these.
            int     s_number;
            double  s_value;
        };
        typedef std::vector<EventParameter> Event;
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
        
        // Parameter API:
        
        std::vector<DBParameter*> listParameters();
        DBParameter* createParameter(const char* name, int number);
        DBParameter* createParameter(
            const char* name, int number,
            double low, double high, int bins, const char* units
        );
        DBParameter* findParameter(const char* name);
        DBParameter* findParameter(int number);
        DBParameter* getParameter(int id);
        
        // Spectrum API:
        
        bool spectrumExists(const char* name);
        DBSpectrum* createSpectrum(const char* name, const char* type,
                const std::vector<const char*>& parameterNames,
                const std::vector<SpectrumAxis>& axes,
                const char* datatype="long"
        );

        std::vector<DBSpectrum*> listSpectra();

        DBSpectrum* lookupSpectrum(const char* name);
    
        // Gate api:
        
        bool gateExists(const char* name);
        DBGate*  create1dGate(
            const char* name, const char* type,
            const std::vector<const char*>& params, double low, double high
        );
        DBGate* create2dGate(
            const char* name, const char* type,
            const std::vector<const char*>& params,
            const std::vector<std::pair<double, double>>& points
        );
        DBGate* createCompoundGate(
            const char* name, const char* type,
            const std::vector<const char*>& gates
        );
        DBGate* createMaskGate(
            const char* name, const char* type,
            const char* parameter, int imask
        );
        DBGate* lookupGate(const char* name);
        DBGate* lookupGate(int id);
        std::vector<DBGate*> listGates();
        
        // Gate application api:
        
        DBApplication* applyGate(const char* gate, const char* spectrum);
        DBApplication* lookupApplication(const char* gate, const char* spectrum);
        std::vector<DBApplication*> listApplications();
        
        // Treevariable API

        DBTreeVariable* createVariable(
            const char* name, double value, const char* units=""
        );
        DBTreeVariable* lookupVariable(const char* name);
        bool variableExists(const char* name);
        std::vector<DBTreeVariable*> listVariables();
        
        // Event recording in the database.
        
        int startRun(
            uint32_t run, const char* title, time_t start
        );
        void endRun(int id, time_t endtime);
        int saveScalers(
            int id, int sourceid,
            int startOffset, int stopOffset, int divisor, time_t when,
            int nScalers, const uint32_t* scalers
        );
        void* startEvents(int id);
        void  rollbackEvents(void* savept);
        void  endEvents(void* savept);
        void  saveEvent(
            int id,  int event, int nParams,
            const int* paramids, const double* params
        );
        
        std::vector<int> listRuns();
        
        // Accessing runs
        
        int openRun(int number);
        
        void* openScalers(int runid);
        int   readScaler(void* context, ScalerReadout& result);
        void  closeScalers(void* context);
        void* openEvents(int runid);
        int   readEvent(void* context, Event& result);
        void closeEvents(void* context);
        
        RunInfo getRunInfo(int id);
        
        
    // Static methods:
    public:
        static bool exists(CSqlite& conn, const char* name);
        static SaveSet* create(CSqlite& conn, const char* name);
        static std::vector<Info> list(CSqlite& conn);
    // Utilities:
    
    private:
        static void loadInfo(Info& result, CSqliteStatement& stmt);
    };
}                                  // SpecTclDB 

#endif