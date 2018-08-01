/**

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

##
# @file   CAnalysisBase.h
# @brief  Base class for user written analysis.
# @author <fox@nscl.msu.edu>
*/
#ifndef CANALYSISBASE_H
#define CANALYSISBASE_H

#include <vector>
#include <string>
#include <time.h>
#include <stdexcept>

/**
 * @class CAnalysisBase
 *    This class is a base class for analysis that might take place within or
 *    external to SpecTcl but is independent of SpecTcl.  Any interaction with
 *    SpecTcl will have a sample driver event processor that can be used to plug this
 *    class into SpecTcl as well as an offline file reading framework
 *    that can be used to drive identical analysis from file.
 *
 *    This base class is not abstract.  Its methods are virtual but stubs
 *    so that the user only needs to derive and implement the methods they
 *    need.
 *
 *    Interfacing with SpecTcl is also possible by providing a 'client data'
 *    parameter in all of the methods.  The SpecTcl driver can make this pass
 *    e.g. the event pointer....and of course users of the tree parameter framework
 *    can use that to gain access to the data..however reliance on these
 *    methods clearly makes use of this class for ad-hoc offline
 *    analysis unusable.
 */
class CAnalysisBase
{
public:
    typedef enum _StateChangeType {
        Begin, End, Pause, Resume
    } StateChangeType;
    
    typedef enum _StringListType {
        PacketTypes, MonitoredVariables, RunVariables
    } StringListType;
    
public:
    virtual void onStateChange(
        StateChangeType type, int runNumber, time_t absoluteTime, float runTime,
        std::string title, void* clientData
    );
    virtual void onScalers(
        time_t absoluteTime, float startOffset, float endOffset,
        std::vector<unsigned> scalers, bool incremental, void* clientData
    );

    virtual void onStringLists(
        StringListType type, time_t absoluteTime, float runTime,
        std::vector<std::string> strings, void* clientData
    );
    
    virtual unsigned onEvent(void* pEvent, void* clientData);
    
    // Methods throw this to report an error that the framework
    // can continue after.
    
    class NonFatalException : public std::runtime_error {
        public:
            explicit NonFatalException(const std::string& whatarg) :
                std::runtime_error(whatarg) {}
        
    };
    // Methods throw this for errors that require the analysis
    // to be aborted (whatever that means in the context of the
    // framework)
    
    class FatalException : public std::runtime_error {
        public:
            explicit FatalException(const std::string& whatarg) :
                std::runtime_error(whatarg) {}
    };
};

#endif