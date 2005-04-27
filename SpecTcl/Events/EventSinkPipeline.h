/*
  EventSinkPipeline.h
*/

// Preprocessor includes, definitions.
#ifndef __EVENTSINKPIPELINE_H // Required for current class
#define __EVENTSINKPIPELINE_H


#ifndef __STL_LIST
#include <list>
#ifndef __STL_LIST
#define __STL_LIST
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __EVENT_H
#include "Event.h"
#define __EVENT_H
#endif

#ifndef __EVENTSINK_H
#include "EventSink.h"
#define __EVENTSINK_H
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
#endif


/*!
  Maintains the set of event sinks that process unpacked event data (parameters
  in CEvent objects).
  @author Ron Fox
  @version 1.0
*/   
class CEventSinkPipeline : public CEventSink {

  // Data types exported to the outside world:

public:
  typedef STD(pair)<STD(string), CEventSink*>   PipelineEntry;
  typedef STD(list)<PipelineEntry> EventSinks;
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
  CEventSink* RemoveEventSink(STD(string) name);        //!< Remove given name.
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
    CEventSink* m_pLastMatch;
    STD(string) m_sName;
  public:
    NameMatch(STD(string) name);
    bool operator()(PipelineEntry& entry);
    CEventSink* getMatch() const;
  };

};

#endif
