/*    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/
/*
  Author:
      Ron Fox
      NSCL
      Michigan State University
      East Lansing, MI 48824-1321
*/
#ifndef DOCUMENTATIONCALLBACK_H
#define DOCUMENTATIONCALLBACK_H

#include <CBufferCallback.h>
#include <map>
#include <list>
#include <string>

// Forward declarations.

class CTCLTracedVariable;
class CTCLInterpreter;
class CVariableTraceCallback;

/*!
   The documentation callback class can be registered on a buffer analysis
   framework to process RUNVARBF and STATEVARBF buffers to recreate the
   TCL Variables they maintain.

*/
class CDocumentationCallback : public CBufferCallback
{
public:
  typedef std::list<CTCLTracedVariable*>     CallbackList;
  typedef CallbackList::iterator        CallbackListIterator;
  typedef std::map<std::string, CallbackList>     CallbackMap;
  typedef CallbackMap::iterator         TraceIterator;
private:

  CTCLInterpreter* m_pInterpreter; //!< TCL interpreter on which vars are made.
  bool             m_fCaptiveInterp;
  CallbackMap      m_Callbacks;	   //!< Map of callback lists on variables.
public:
  CDocumentationCallback(CTCLInterpreter* pInterp = NULL);
  virtual ~CDocumentationCallback();
private:
  CDocumentationCallback(const CDocumentationCallback& rhs);
  CDocumentationCallback& operator=(const CDocumentationCallback& rhs);
  int operator==(const CDocumentationCallback& rhs) const;
  int operator!=(const CDocumentationCallback& rhs) const;
public:

  // Selectors:

  CTCLInterpreter* getInterpreter() {
    return m_pInterpreter;
  }

  // Class operations:

  std::string getValue(std::string name) const;
  std::string getElementValue(std::string name, std::string element) const;
  void addChangeHandler(std::string name, CVariableTraceCallback& callback);
  void removeChangeHandler(std::string name, CVariableTraceCallback& callback);

  //!< Access to the callbacks.  Note TraceIterator points to a list<> of callbacks.

  TraceIterator begin();
  TraceIterator end();
  int           size();
 

  // Overridable and overrides.

  virtual void operator()(unsigned int type, const void* pBuffer);
  
};


#endif
