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
#define __STL_VECTOR
#endif


#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

// forward definitions:

class CEvent;
class CEventList;
class CXdrOutputStream;
class CDictionaryException;
   
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
   Bool_t m_fEnabled;                 //!< Filter only works when enabled.
   string m_sFileName;                //!< Name of output file.
   vector<string> m_vParameterNames;  //!< Names of parameters in output.
   vector<UInt_t> m_vParameterIds;    //!< Ids of params in output stream.
   CXdrOutputStream* m_pOutputEventStream; //!< Output file.

 public:
      // Constructors and other canonical operations.
 
  CEventFilter();
  CEventFilter(string& rFileName);
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
   
  vector<string> getParameterNames() const {
    return m_vParameterNames;
  }
  vector<UInt_t> getParameterIds() const {
      return m_vParameterIds;
   }
  Bool_t CheckEnabled() const {
      return m_fEnabled;
  }
  string getFileName() const {
      return m_sFileName;
  }
   CXdrOutputStream* getOutputStream() {      // not const.
      return m_pOutputEventStream;
   }
   // Mutators.
protected:
  // Operations on the class.
public:

   void setParameterNames(const vector<string>& names);
   void setOutputStream(CXdrOutputStream* str);

   void Enable();                        //!< Enable the filter.
   void Disable();                       //!< Disable the filter.
   void setFileName(string&);            //!< Choose a new filename.

  
   virtual void operator()(CEventList& rEvents);  //!< Process list of events.
   virtual void operator()(CEvent& rEvent);      //!< Process single event.
  //protected:
protected:
   // Behavioral override hooks.

   virtual Bool_t CheckCondition(CEvent& rEvent) = 0;
   virtual void FormatEventDescription();   //!< Describe event in output file
   virtual void FormatOutputEvent(CEvent& rEvent); //!< Format a filtered event

// utilities:

   static string DefaultFilterFilename();    //!< Return default filter filename.
   void NamesToIds();                       //!< Translate param names -> ids.
   vector<string> IdsToNames() 
              throw (CDictionaryException); //!< and back again.
   static void setBit(unsigned* bits, unsigned offset); //!< set a bit.
};


#endif
