/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/**
* @file  DBEvents.h
* @brief Header for SpecTcl event processor to put data to databases.
*/
#ifndef DBEVENTS_H
#define DBEVENTS_H
#include <EventSink.h>

class CDBEventWriter;

/**
 * @class CDBSink
 *    This class is an event sink.  It is involved in putting unpacked events
 *    into a database.
 *
 * @note this is paired with a CDBEventProcessor which handles non event data
 *       and is smart enough to tell us when to start/stop.
 */
class CDBSink : public CEventSink {
private:
    CDBEventWriter* m_pWriter;
    bool            m_saving;
public:
    CDBSink(CDBEventWriter* pWriter);
    
    virtual void operator()(CEventList& rEvents);
    
    // Things that get called by the event processor:
    
    void enable();
    void disable();
    void setWriter(CDBEventWriter* pWriter);
};

#endif