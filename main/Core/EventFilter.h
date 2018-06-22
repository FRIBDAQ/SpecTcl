/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/*
  EventFilter.h
*/

// Preprocessor includes, definitions.
#ifndef __EVENTFILTER_H //Required for current class
#define __EVENTFILTER_H

#ifndef __EVENTSINK_H
#include <EventSink.h>
#define __EVENTSINK_H
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __DICTIONARYEXCEPTION_H
#include <DictionaryException.h>
#endif

// forward definitions:

class CEvent;
class CEventList;
class CFilterOutputStage;

// class CDictionaryException;
   
// Class.
/*! 
    Abstract base class for event filters.  An event filter is an event sink
    that produces an output stream that is a subset of the input event stream.
    The subsetting has two selections
  
    - Events - Only events that satisfy a gate are output on the output stream.
    - Parameters - Only the specified subset of parameters are output on the
      output stream.
      
*/
class CEventFilter : public CEventSink {

   // Attributes.
 
protected:
   Bool_t m_fEnabled;                           //!< Filter only works when enabled.
   std::string              m_sFileName;        //!< Name of output file.
   std::vector<std::string> m_vParameterNames;  //!< Names of parameters in output.
   std::vector<UInt_t>      m_vParameterIds;    //!< Ids of params in output stream.
   CFilterOutputStage*      m_pOutput;          //!< Output stage (formatter).

 public:
      // Constructors and other canonical operations.
 
  CEventFilter();
  CEventFilter(std::string& rFileName);
  virtual ~CEventFilter();

 // Copies etc. are not allowed:
 
 private:
   CEventFilter& operator=(const CEventFilter&);
   CEventFilter(const CEventFilter& rhs);
   int operator==(const CEventFilter& rhs);
   int operator!=(const CEventFilter& rhs);
 public:

  // Selectors.
public:
   
  std::vector<std::string> getParameterNames() const {
    return m_vParameterNames;
  }
  std::vector<UInt_t> getParameterIds() const {
      return m_vParameterIds;
   }
  Bool_t CheckEnabled() const {
      return m_fEnabled;
  }
  std::string getFileName() const {
      return m_sFileName;
  }
  CFilterOutputStage* getOutputStream() {
    return m_pOutput;
  }

   // Mutators.
protected:
  // Operations on the class.
public:

  void setParameterNames(const std::vector<std::string>& names);
  void setOutputStream(CFilterOutputStage* str);
  
  void Enable();                        //!< Enable the filter.
  void Disable();                       //!< Disable the filter.
  void setFileName(std::string&);       //!< Choose a new filename.
  void setOutputFormat(CFilterOutputStage* format);
  std::string outputFormat() const;

  
   virtual void operator()(CEventList& rEvents);  //!< Process list of events.
   virtual void operator()(CEvent& rEvent);      //!< Process single event.
  //protected:
protected:
   // Behavioral override hooks.

   virtual Bool_t CheckCondition(CEvent& rEvent) = 0;

// utilities:

   static std::string DefaultFilterFilename();    //!< Return default filter filename.
   void NamesToIds();                       //!< Translate param names -> ids.
   std::vector<std::string> IdsToNames(); //!< and back again.
};


#endif
