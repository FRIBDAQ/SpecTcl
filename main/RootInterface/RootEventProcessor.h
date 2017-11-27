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
 *  @brief: Event processor that passes begin/end information to sinks.
 */
#ifndef ROOTEVENTPROCESSOR_H
#define ROOTEVENTPROCESSOR_H

#include <EventProcessor.h>
#include <string>
#include <map>

class RootTreeSink;

/**
 * @class RootEventProcessor
 *    This class is an event processor that will be registered with SpecTcl
 *    when the first tree is created.  It maintains a set of named tree
 *    sinks.
 *    The reason we need an event processor for this is that event sinks are
 *    not aware of run starts/stops and we want our sinks to be aware of them
 *    so that they know when to create new files and recreate their trees.
 */
class RootEventProcessor : public CEventProcessor
{
private:
    std::map<std::string, RootTreeSink*> m_sinks;
public:
    virtual Bool_t OnBegin(CAnalyzer& rA, CBufferDecoder& rB);
    virtual Bool_t OnEnd(CAnalyzer& rA, CBufferDecoder& rB);
    
    void addTreeSink(const char* name, RootTreeSink* sink);
    RootTreeSink* removeTreeSink(const char* name);
    
    std::map<std::string, RootTreeSink*>::const_iterator begin() const;
    std::map<std::string, RootTreeSink*>::const_iterator end() const;
    
    
};

#endif