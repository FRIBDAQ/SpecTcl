/*
  EventSinkPipeline.cpp - see the header file for general class information.
*/

// Copyright Notice.
static const char* Copyright =
  "OutputEventStream.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header Files.
#include <config.h>
#include "Event.h"
#include "EventList.h"

#include "EventSinkPipeline.h"
#include <algorithm>
#include <Globals.h>

#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Constructors.

/*!
   Construct the event sink pipeline.  This involves
   just initializing the anonymous sink  index to 0.
*/
CEventSinkPipeline::CEventSinkPipeline() :
  m_nSequence(0)
{
}
/*!
  Destruction of the pipeline is just a no-op however the
  destroyer may want to ensure that any dynamically created sinks
  get destroyed in order to prevent memory leaks.
*/
CEventSinkPipeline::~CEventSinkPipeline() 
{
}

/*!
  The function call operator iterates through the pipeline invoking each
  event processor in the pipeline.  Note that sink list elements are pairs
  consisting of the sink name and the sink itself.
  \param rEventList
     A set of events for the  pipeline entry to process.

*/
void CEventSinkPipeline::operator()(CEventList& rEventList) {
  for(EventSinkIterator i = CEventSinkPipeline::begin(); 
      i != CEventSinkPipeline::end(); i++) {
    CEventSink& sink(*(i->second));
    sink(rEventList);
  }
}

// Class functions:

/*!
  Appends a sink to the end of the event sink pipeline.  Note that this function
  already exists, but will be augmented with the addition of an optional event
  sink name.
  @param entry   
      Reference to an event sink to add to the pipeline.
  @param name
      Optional name of the sink  If one is not supplied by the user a
      name will be created that is unique.
 
 */
void CEventSinkPipeline::AddEventSink(CEventSink& rEventSink, const char* name) 
{

  InsertSink(rEventSink, end(), name);
}

/*!
  Inserts an event sink at a specified location in the pipeline.
  @param entry    
     The event sink to insert into the pipeline.
  @param position
     Iterator 'pointing' to the position in front of which the
     element will be inserted.
  @param name
     Name of the event sink.  Will be created if omitted.
  
*/
void CEventSinkPipeline::InsertSink(CEventSink& entry, 
				    EventSinkIterator position, 
				    const char* name)
{
  // Figure out the sink's name:
  


  string sName;
  if(name) {
    sName = name;
  } 
  else {
    sName = AssignName();
  }
  // Enter it into the pipeline before position.

  PipelineEntry item(sName, &entry);
  m_lSinks.insert(position, item);
  entry.OnAttach(*gpAnalyzer);

}
/*!
  Remove the indicated event sink from the pipeline. This code actually will
  remove all event sink pipeline elements with the same address as the
  supplied event sink.  In most cases an event sink is only reigstered once,
  so there's no difference. 
  \param rEventSink
     Reference to the event sink to remove.

   \note  The remove_if member of the std::list class is used.  We will use the
          AddressMatch predicate that is a nested class of CEventSinkPipeline
	  to remove the correct element(s).
   \note  Since the caller already has a reference to the sink it's not necessary
          to return the sink's address to him as we do in the case of the
	  other remove members.
   \note  If there is no such element in the pipeline this is a no-op that
          executes in O(n) where n is the number of elements in the pipeline.

*/
void CEventSinkPipeline::RemoveEventSink(CEventSink& rEventSink) 
{
  AddressMatch  predicate(rEventSink);
  m_lSinks.remove_if(predicate);
  rEventSink.OnDetach(*gpAnalyzer);
}

/*!
   Remove the named event sink.  This is the same as the previous
   member, however the sink to remove is specified by name. All sinks with that
   name will be removed.  In general, there should only be a single sink with 
   any specific name. 

   \param name 
      Name of the sink to remove.
   \return CEventSink*
   \retval Pointer to the last of the event sinks removed with that name.
   \retval NULL    there was no event sink with that name.

*/
CEventSink*
CEventSinkPipeline::RemoveEventSink(string name)
{
  NameMatch&  predicate(*(new NameMatch(name)));
  m_lSinks.remove_if(predicate);
  CEventSink* pSink = predicate.getMatch();
  if(pSink) {
    pSink->OnDetach(*gpAnalyzer);
  }
  delete &predicate;
  return pSink;

}
/*!
  Remove an event sink given an iterator that points to it.
  @param entry
    Event sink pipeline iterator that 'points' to the item to be
    deleted.
 
 */
CEventSink* 
CEventSinkPipeline::RemoveEventSink(EventSinkIterator entry)
{
  CEventSink*  pSink = entry->second;
  m_lSinks.erase(entry);
  pSink->OnDetach(*gpAnalyzer);
  return pSink;
}
/*!
  Locates an event sink, and returns an 'iterator' to it or end() if the itme
  idoes not exist.
  @param name
    Name of the sink to hunt for.
 
*/
CEventSinkPipeline::EventSinkIterator 
CEventSinkPipeline::FindSink(string name)
{
  NameMatch predicate(name);
  EventSinkIterator temp =  find_if(m_lSinks.begin(), m_lSinks.end(), predicate);
  return temp;
}

/*!
  Finds a specific event sink iterator given a reference to it. an Iterator to
  the element is returned, or
  end() if the sink does not exist in the pipeline.

  @param sink
    The sink to hunt for.

  \return EventSinkIterator
  \retval An iterator pointing to the element containing the sink.
  \retval end()'s return value  If not found.

*/
CEventSinkPipeline::EventSinkIterator 
CEventSinkPipeline::FindSink(CEventSink& sink)
{
  AddressMatch predicate(sink);
  return find_if(m_lSinks.begin(), m_lSinks.end(), predicate);

}

// Get info about the pipeline contents.

CEventSinkPipeline::EventSinkIterator 
CEventSinkPipeline::begin() 
{
  return m_lSinks.begin();
}

CEventSinkPipeline::EventSinkIterator 
CEventSinkPipeline::end() 
{
  return m_lSinks.end();
}

UInt_t 
CEventSinkPipeline::size() 
{
  return m_lSinks.size();
}


/// Utility functions.

/**
 * Assigns a unique name of the form Anonymous::n  where n is an number.
 */
std::string CEventSinkPipeline::AssignName()
{

  string name;
  while (1) {
    char buffer[100];
    m_nSequence++;
    snprintf(buffer, sizeof(buffer), "%d", m_nSequence);
    name  = "Anonymous::";
    name +=  buffer;

    if (FindSink(name) == end()) {
      break;
    }
  }
  return name;
}


// Internal nested class implementations.

/*!
   Address match is a predicate class that compares the event sink of a pipeline
   entry to the address of a saved sink.  It returns true if there is a match.
*/
/*! Constructor saves the sink addres.
 */
CEventSinkPipeline::AddressMatch::AddressMatch(CEventSink& rSink) :
  m_pSink(&rSink)
{
}
/*!
   Function call operator does the comparison.
*/
bool
CEventSinkPipeline::AddressMatch::operator()(PipelineEntry& rEntry)
{
  return (rEntry.second == m_pSink);
}

/*!
   Name match is a predicate that compares event sink names with
   a saved name.  The last match is stored for retrieval.
*/

/*! Constructor: Save the name and null the last match pointer.
 */
CEventSinkPipeline::NameMatch::NameMatch(string name) :
  m_pLastMatch(0),
  m_sName(name)
{}

/*!
    Check for the match, if so save the matching pointer.
*/
bool
CEventSinkPipeline::NameMatch::operator()(PipelineEntry& entry)
{
  if(entry.first == m_sName) {
    m_pLastMatch = entry.second;
    return true;
  }
  else {
    return false;
  }
}

/*!
  Return pointer to last match:
*/
CEventSink*
CEventSinkPipeline::NameMatch::getMatch() const
{
  return m_pLastMatch;
}
