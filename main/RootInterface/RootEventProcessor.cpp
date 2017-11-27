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

/** @file:  RootEventProcessor.h
 *  @brief: RootEventProcessor.cpp
 */

#include "RootEventProcessor.h"
#include "RootTreeSink.h"
#include <BufferDecoder.h>
#include <Exception.h>
#include <stdexcept>

#include <iostream>

/**
 *  The event processor lives for the life of the program once
 *  created. Sure we should do something at destruction time about hanging
 *  sinks but there's not really much point to it as destruction coincides
 *  with the death of the program.
 */

/**
 * OnBegin
 *    Invokes the OnBegin method for all of the event sinks that have
 *    been defined.  This is done so that they can compute/open a new
 *    TFile.
 *
 * @param rA  - references the analyzer object.  We don't need this.
 * @param rB  - references the buffer decoder object. We'll get run number
 *              and title from this.
 * @return kfTRUE - unless an exception is thrown in which case kfFALSE is
 *              returned along with a message to stderr.
 */
Bool_t
RootEventProcessor::OnBegin(CAnalyzer& rA, CBufferDecoder& rB)
{
    Bool_t result = kfTRUE;
    std::string message;
    
    UInt_t run        = rB.getRun();
    std::string title = rB.getTitle();
    
    try {
        for (auto p = m_sinks.begin(); p != m_sinks.end(); p++)  {
            p->second->OnBegin(run, title);
        }
    }
    catch (CException& e) {
        message = "CException caught RootEventProcessor::OnBegin ";
        message += e.ReasonText();
        result  = kfFALSE;
    }
    catch (std::exception& e) {
        message = "std::exception caught RootEventProcessor::OnBegin ";
        message += e.what();
        result  = kfFALSE;
    }
    catch (const char* msg) {
        message = "const char* exception caught RootEventProcessor::OnBegin ";
        message += msg;
        result  = kfFALSE;
    }
    catch (std::string msg) {
        message = "std::string exception caught RootEventProcessor::OnBegin ";
        message += msg;
        result  = kfFALSE;
    }
    catch (...) {
        message = "Unanticipated exception caught by RootEventProcessor::OnBegin";
        result  = kfFALSE;
    }
    if (!result) {
        std::cerr << message << std::endl;
    }
    return result;
}
/**
 * OnEnd
 *    Called at the end of the run.  We invoke this method in all of the
 *    event sinks.
 *
 * @param rA  - Reference to the analyzer.
 * @param rB  - Reference to the buffer decoder.
 * @note - both parameters are unused.
 * @return kfTRUE unless an exception is thrown and then kfFALSE.
 * 
 */
Bool_t
RootEventProcessor::OnEnd(CAnalyzer& rA, CBufferDecoder& rB)
{
    Bool_t result = kfTRUE;
    std::string message;
    
    try {
        for (auto p = m_sinks.begin(); p != m_sinks.end(); p++) {
            p->second->OnEnd();  
        }
    }
    catch (CException& e) {
        message = "CException in RootEventProcessor::OnEnd - ";
        message += e.ReasonText();
        result  = kfFALSE;
    }
    catch (std::exception & e) {
        message = "std::exception caught in RootEventProcessor::OnEnd - ";
        message += e.what();
        result  = kfFALSE;
    }
    catch (const char* msg) {
        message = "const char* caught in RootEventProcessor::OnEnd - ";
        message += msg;
        result = kfFALSE;
    }
    catch (std::string msg) {
        message = "std::string caught in RootEventProcessor::OnEnd - ";
        message += msg;
        result += kfFALSE;
    }
    catch (...) {
        message += "Unanticipated exception type caught in RootEventProcessor::OnEnd";
        result = kfFALSE;
    }
    
    if (!result) {
        std::cerr << message << std::endl;
    }
    return result;
}
/**
 * addTreeSink
 *    Add a new tree sink to the managed sinks.
 *
 *   @param name - name of the sink.
 *   @param sink - pointer to the sink to and.
 *   @throw std::invalid_argument - if the name already exists.
 */
void
RootEventProcessor::addTreeSink(const char* name, RootTreeSink* sink)
{
    // If exists that's a problem:
    
    if (m_sinks.find(name) != m_sinks.end()) {
        std::string msg = name;
        msg += " already exists.";
        throw std::invalid_argument(msg);
    }
    m_sinks[name] = sink;
}
/**
 * removeSink
 *    Removes a sink by name from the collection of sinks.
 *
 * @param name - name of the sink to remove.
 * @return RootTreeSink* - pointer to the removed sink.  Up to the caller to do
 *                         any storage management (e.g. delete).
 * @throw std::invalid_argument if there's no such item.
 */
RootTreeSink*
RootEventProcessor::removeTreeSink(const char* name)
{
    auto p = m_sinks.find(name);
    if (p == m_sinks.end()) {
        // not found.
        
        std::string msg = name;
        msg            += " not found";
        throw std::invalid_argument(msg);
    }
    RootTreeSink* result = p->second;
    m_sinks.erase(p);
    
    return result;
    
}
/**
 * begin
 *    Return a const iterator to the beginning of the collection.
 *  @return std::map<std::string, RootTreeSink*>::const_iterator
 */
std::map<std::string, RootTreeSink*>::const_iterator
RootEventProcessor::begin() const
{
    return m_sinks.begin();
}
/**
 * end
 *  @return std::map<std::string, RootTreeSink*>::const_iterator -
 *          const iterator just off the end of the collection of sinks.
 *
 */
std::map<std::string, RootTreeSink*>::const_iterator
RootEventProcessor::end() const
{
    return m_sinks.end();
}
