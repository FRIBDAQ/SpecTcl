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

/** @file:  CAENParser
 *  @brief: Parses events from CAEN digitizer modules (event built).
 *  
 */
#ifndef CAENPARSER_H
#define CAENPARSER_H

#include "CAENHit.h"
#include "CAENModuleHits.h"

#include <map>
#include <vector>

/**
 * @class CAENParser
 *    Produces a set of CAENModules stuffed wit hits
 *    from an event that comes from the event builder.
 *    Important features of this class:
 *    -   Source ids that are not known are ignored allowing
 *        for hybrid data from CAEN and non CAEN digitizers.
 *    -   Each source id must be described by providing:
 *       * The source id for that module.
 *       * The DPP firmware type expected for that module.
 *    - Once an event is parsed, what's made available to the
 *      client is a vector of pointers to modules that have hits.
 *      This improves the performance of converting the hits into
 *      parameters.
 */
class CAENParser
{
    // Data types:
private:
    struct  ModuleInfo {
        CAENHit::HitType   s_hitType;
        CAENModuleHits     s_module;
        int                s_multiplier;
        ModuleInfo(int sid) : s_module(sid) {}
        ModuleInfo() : s_module(0) {}
        ModuleInfo(const ModuleInfo& rhs) {
            copyIn(rhs);
        }
        ModuleInfo& operator=(const ModuleInfo& rhs) {
            if (&rhs != this) copyIn(rhs);
            return *this;
        }
        private:
            void copyIn(const ModuleInfo& rhs) {
                s_hitType = rhs.s_hitType;
                s_module  = rhs.s_module;
                s_multiplier = rhs.s_multiplier;
            }
    };
    // Member data:
private:
    std::map<int, ModuleInfo>    m_modules;
    std::vector<CAENModuleHits*> m_hitModules;
public:
    CAENParser();
    virtual ~CAENParser();
    
    void operator()(void* pEventBody);
    void clear();
    const std::vector<CAENModuleHits*>& getHitModules() const;
    void addModule(int sid, CAENHit::HitType type, int multiplier=0);
private:
    CAENHit* makeHit(const ModuleInfo& info);
    void destroyHits(CAENModuleHits& module);
};

#endif