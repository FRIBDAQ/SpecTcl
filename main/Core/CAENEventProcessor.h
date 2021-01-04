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
 *    Memory management of the mappers is a bit problematic.
 *    We can't provide some virtual function that's called from the
 *    destructor because destructors don't support polymporphism.
 *    We are therefore going to require that all parameter mappers are
 *    dynamically created via new.  In practice, this not a huge problem.
 *
 *   @note the parser is declared as protected. This allows for derived
 *         classes to directly manipulate it (e.g. to provide a
 *         time scale factor for externally clocked applications).
 */
class CAENEventProcessor : public CEventProcessor
{
private:
    CAENParser*                      m_pParser;
    bool                             m_testMode;
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
    void addParameterMap(int sid, CAENParameterMap* map, int mult=1);
    void disposeMapEntry(int sid, CAENParameterMap* pMap);
    void setTestMode() {m_testMode = true;}
};

#endif