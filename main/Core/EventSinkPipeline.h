/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

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


/*
  EventSinkPipeline.h
*/

// Preprocessor includes, definitions.
#ifndef EVENTSINKPIPELINE_H // Required for current class
#define EVENTSINKPIPELINE_H


#include <list>
#include <string>
#include "Event.h"
#include "EventSink.h"
#include <histotypes.h>


/*!
  Maintains the set of event sinks that process unpacked event data (parameters
  in CEvent objects).
  @author Ron Fox
  @version 1.0
*/   
class CEventSinkPipeline : public CEventSink {

  // Data types exported to the outside world:

public:
  typedef std::pair<std::string, CEventSink*>   PipelineEntry;
  typedef std::list<PipelineEntry> EventSinks;
  typedef EventSinks::iterator       EventSinkIterator;

  // Private member data.

 private:
  EventSinks     m_lSinks;	//!< The event sink list.
  UInt_t         m_nSequence;   //!< Used to allocate anonymous sink names.
  

 public:
  // Constructors.
  CEventSinkPipeline();
  virtual ~CEventSinkPipeline();

  // Operators.
  virtual void operator()(CEventList&);

  // Additional functions.
  void AddEventSink(CEventSink&, const char* name = 0); //!< Add an event sink.
  void InsertSink(CEventSink& entry, EventSinkIterator position, 
		  const char* name = 0);                //!< Insert sink at posn. 

  void RemoveEventSink(CEventSink&);                    //!< Remove given the sink 
  CEventSink* RemoveEventSink(std::string name);        //!< Remove given name.
  CEventSink* RemoveEventSink(EventSinkIterator entry);	//!< Remove given iterator.
  EventSinkIterator FindSink(std::string name);	        //!< Find sink by  name. 
  EventSinkIterator FindSink(CEventSink& sink);	        //!< Find iterator given sink
  EventSinkIterator begin();	                        //!< Begin of iteration. 
  EventSinkIterator end();	                        //!< End iteration sentinel 
  UInt_t size();		                        //!< sink count. 
protected:
  std::string AssignName();

  // The following are predicates used by the internals of the class:

  //!  Match pipeline elements with a specific sink.
  class AddressMatch 
  {
  private:
    CEventSink* m_pSink;
  public:
    AddressMatch(CEventSink& rSink);
    bool operator()(PipelineEntry& rEntry);

  };

  //! Match pipeline elements with specific name.
  class NameMatch 
  {
  private:
    static CEventSink* m_pLastMatch;
    std::string m_sName;
  public:
    NameMatch(std::string name);
    bool operator()(PipelineEntry& entry);
    CEventSink* getMatch() const;
    void reset() {m_pLastMatch = nullptr;}
  };

};

#endif
