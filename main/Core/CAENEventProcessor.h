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
#ifndef CAENEVENTPROCESSOR_H
#define CAENEVENTPROCESSOR_H
/** @file:  CAENEventProcessor.h
 *  @brief: Process hits from an event file containing CAEN digitizers.
 */

#include <EventProcessor.h>
#include <map>
class CAENParser;
class CAENParameterMap;

/**
 * @class CAENEventProcessor
 *
 *    This class brings together the mechanics of parsing an
 *    event that contains CAEN hits.  No assumption is made
 *    that CAEN hits are all there are.  The CAEN digitizers
 *    could have been built with non CAEN digitizer modules
 *    (See CAENParser).  The actual processing of the event is
 *    rather trivial.  The more exciting part is setting up the parser.
 *    We'll get into that in a bit.
 *
 *    Parsing consists of running the event through the parser and
 *    then taking the hits from each module and running *them*
 *    through the approriate module parameter map.  The tricky
 *    part is how to use this class to link the modules to their unpackers
 *    This is done by deriving a sublcass from this class and
 *    writing a constructor that does exactly that.
 *
 *    The base class provides methods for adding a mapper and
 *    describing the source id it handles.  The user's constructor
 *    must create these mappers and call those methods.
 *    Those calls build up a map of sourcid -> mapper which
 *    is used in the second stage of processing.
 *
 *    In order to avoid problems with memory leaks if parameter
 *    maps are dynamically created, we provide the virtual method
 *    disposeMapEntry called by the destructor.  The method is called
 *    for each parameter map that was entered.  If a map was
 *    dynamically allocated it can be deleted here.  The default
 *    implementation assumes this is not necessary and does nothing.
 *
 *   @note the parser is declared as protected. This allows for derived
 *         classes to directly manipulate it (e.g. to provide a
 *         time scale factor for externally clocked applications).
 */
class CAENEventProcessor : public CEventProcessor
{
private:
    CAENParser*                      m_pParser;
protected:
    std::map<int, CAENParameterMap*> m_pMaps;
public:
    CAENEventProcessor();
    virtual ~CAENEventProcessor();
    
    virtual Bool_t operator()(
        const Address_t pEvent,
        CEvent& rEvent,
        CAnalyzer& rAnalyzer,
        CBufferDecoder& rDecoder
    );
    
protected:
    void addParameterMap(int sid, CAENParameterMap* map);
    virtual void disposeMapEntry(int sid, CAENParameterMap* pMap);
};

#endif