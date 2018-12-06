/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file: CEventBuilderEventProcessor.h
 *  @brief: Event processor that handles event built data.
 */
#ifndef CEVENTBUILDEREVENTPROCESSOR_H
#define CEVENTBUILDEREVENTPROCESSOR_H

#include "EventProcessor.h"

#include <map>
#include <string>
#include <stdint.h>

class CTreeParameter;


/**
 * @class CEventBuilderEventProcessor
 *    This class provides a top  level event processor for ring items
 *    that come from the NSCL event builder pipeline.   It supports:
 *
 *  - Registration of event processors for each source id.
 *    Note that for hierarchically built data you can register another of
 *    these.
 *  - Maintenance of a set of diagnostic parameters that run across
 *    the fragments.  These include time stamp differences,  event number
 *    event fragment counts and number of times each source contributes
 *    a fragment..
 */

class CEventBuilderEventProcessor : public CEventProcessor
{
    // Private data types:
private:
    struct SourceData {
        unsigned          s_sourceId;
        CTreeParameter*   s_SourcePresent;
        CEventProcessor*  s_processor;
        
        SourceData() : s_sourceId(0), s_SourcePresent(0), s_processor(0) {}
    };
    
    typedef std::map<unsigned, SourceData> HandlerMap;
    typedef std::map<unsigned, CTreeParameter*> TimeDifferences;
    typedef std::map<unsigned, TimeDifferences> TimeDifferenceMatrix;
    // Private data members:
private:
    double               m_ClockMHz;
    HandlerMap           m_sourceHandlers;
    TimeDifferenceMatrix m_timeDifferenceParams;
    CTreeParameter*      m_sourceCount;
    CTreeParameter*      m_unrecognizedSourceCount;
    CTreeParameter*      m_eventNumber;
    CTreeParameter*      m_seconds;
    std::string          m_baseName;     // Construct parametrs from this.
    unsigned             m_nEvents;      // Number of events.
    
    // Canonicals:
    
public:
    CEventBuilderEventProcessor(double clockMHz, std::string baseName);
    virtual ~CEventBuilderEventProcessor();
    
    // Event processor methods we need to override - all of them since we
    // have to pass control over to the registered event processor methods:
    
    virtual Bool_t operator()(const Address_t pEvent,
                              CEvent& rEvent,
                              CAnalyzer& rAnalyzer,
                              CBufferDecoder& rDecoder); // Physics Event.
  
    // Functions:
    virtual Bool_t OnAttach(CAnalyzer& rAnalyzer); // Called on registration.
    virtual Bool_t OnDetach(CAnalyzer& rAnalyzer);
    virtual Bool_t OnBegin(CAnalyzer& rAnalyzer,
                           CBufferDecoder& rDecoder); // Begin Run.
    virtual Bool_t OnEnd(CAnalyzer& rAnalyzer,
                         CBufferDecoder& rBuffer); // End Run.
    virtual Bool_t OnPause(CAnalyzer& rAnalyzer,
                           CBufferDecoder& rDecoder); // Pause Run.
    virtual Bool_t OnResume(CAnalyzer& rAnalyzer,
                            CBufferDecoder& rDecoder); // Resume Run.
    virtual Bool_t OnOther(UInt_t nType,
                           CAnalyzer& rAnalyzer,
                           CBufferDecoder& rDecoder); // Unrecognized buftype.
  
    virtual Bool_t OnEventSourceOpen(std::string name);
    virtual Bool_t OnEventSourceEOF();
    virtual Bool_t OnInitialize();
    
public:
    
    // Additional public methods:

public:    
    void addEventProcessor(unsigned sourceId, CEventProcessor& processor);
    bool haveEventProcessor(unsigned sourceId);
    
    // Utilities:
    
private:
    void computeTimestampDifferences(
        const std::vector<std::pair<unsigned, uint64_t> >& stamps
    );
    void addEventSourceParameters(unsigned id);
    std::string tdiffParamName(unsigned sid1, unsigned sid2);
};

#endif