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

#include <config.h>
#include "DocumentationCallback.h"
#include <histotypes.h>
#include <TCLVariable.h>
#include <TCLTracedVariable.h>
#include <TCLInterpreter.h>
#include <VariableTraceCallback.h>
#include "DocumentationException.h"
#include <CBufferProcessor.h>
#include <buffer.h>
#include <string.h>
#include <TranslatorPointer.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
   Construct a documentation callback.  Documentation callbacks are associated with
   a TCL Interpreter.  The interpreter is either one that the user already has in mind,
   or one that is created for the sole purpose of maintaining the reconstructed TCL
   variables in the documentation buffers. 

   \param pInterp (CTCLInterpreter* [default = NULL]):
       If this is non NULL the user is supplying an interpreter into which the
       documentation buffer scripts will be interpreted.  If the interpreter is null,
       the construtor will create an interpreter that will only be used for that
       purpose.

*/
CDocumentationCallback::CDocumentationCallback(CTCLInterpreter* pInterp) :
  m_pInterpreter(pInterp),
  m_fCaptiveInterp(false)
{
  if(!m_pInterpreter) {		// Create a 'captive' interpreter.
    Tcl_Interp* pRawInterp = Tcl_CreateInterp();
    Tcl_Init(pRawInterp);
    m_pInterpreter = new CTCLInterpreter(pRawInterp);
    
    m_fCaptiveInterp = true;	// Flag the interpreter needs deletion on ~
  }
}
/*!
   Destructor.  The lists and maps will take care of themselves, since the caller
  is responsible for their content.  If the interpreter was captive, it must be
  destroyed.
*/
CDocumentationCallback::~CDocumentationCallback()
{
  if (m_fCaptiveInterp) {
    Tcl_DeleteInterp(m_pInterpreter->getInterpreter());
    delete m_pInterpreter;
  }
}

/*!
   Get the value of a variable in the interpreter.
   \param name (string):
      Name of the variable to get.
   \return string
   \retval the value of the variable
   \throws CDocumentationException if the variable does not exist.
*/
string
CDocumentationCallback::getValue(string name) const
{
  CTCLVariable var(m_pInterpreter, name, false);
  const char*  pValue = var.Get();
  if(!pValue) {
    throw CDocumentationException(CDocumentationException::NoSuchVariable,
				  "getting its value");
  }
  else {
    return string(pValue);
  }
}
/*!
   Get the value of an element of an array.  When using this function it is important
   to remember that TCL arrays are associative, that is they have string indices,
   rather than numeric.
   \param name (string):
      Name of the array.
   \param element (string):
       Name of the element of the array (index).
   \return string
   \retval the value of the variable.
   \throws CDocumentationException if the variable does not exist.
*/
string
CDocumentationCallback::getElementValue(string name, string element) const
{
  CTCLVariable var(m_pInterpreter, name, false);
  const char* pValue = var.Get(TCL_GLOBAL_ONLY, (char*)element.c_str());
  if(!pValue) {
    throw CDocumentationException(CDocumentationException::NoSuchElement,
				  "getting its value");
  }
  else {
    return string(pValue);
  }

}
/*!
  Add a variable change handler.  Variable change handlers get called when a
  variable is changed (obviously).  This is essentially a trace on write only.
  The implementation involves creating a CTCLTracedVariable and packaging the 
  user's CVariableTraceCallback object in it.
  \param name (string):
     The name of the variable or array element to track.  If an array element,
     use TCL array notation to specify it e.g. (name = "EPICS_DATA(K12TC12);").
  \param callback (CVariableTraceCallback&):
     Reference to the callback object the user wants us to invoke.  The
     callback object is tracked in a map indexed by variable name.. each map
     entry contains a list of callback pointers.

   \note  The callback object must have a lifetime at least as long as it is
   registered on for the variable.
*/
void
CDocumentationCallback::addChangeHandler(string                  name, 
					 CVariableTraceCallback& callback)
{
  CTCLTracedVariable* pTracer = new CTCLTracedVariable(m_pInterpreter,
						       name,
						       callback,
						       TCL_TRACE_WRITES);
  m_Callbacks[name].push_back(pTracer);
}
/*!
   Remove a variable changed callback.  A previously registered callback is
   removed from the set of callbacks on a variable. 
   \param name  (string)
      Name of the command or array element on which the callback was registered.
      see addChangeHandler for more information about how to specify these.
   \param callback (CVariableTraceCallback&)
      Reference to the callback that is registered.
   \throw CDocumentationExcetpion - if the callback is not registered on the
   specified command.
*/
void
CDocumentationCallback::removeChangeHandler(string                  name,
					    CVariableTraceCallback& callback)
{
  CallbackList& listOfCallbacks(m_Callbacks[name]);
  CallbackListIterator p = listOfCallbacks.begin();
  while(p != listOfCallbacks.end()) {
    CTCLTracedVariable* pVariable = *p;
    if (&callback == &(pVariable->getCallback())) {
      listOfCallbacks.erase(p);	// Remove.
      delete pVariable;
      return;
    }
    p++;
  }
  // If control passes here, we were not able to locate a matching callback.
  
  throw CDocumentationException(CDocumentationException::NoSuchHandler,
				"Deleting a callback");
}
/*!
    Return a begin of iteration iterator to the callback map. Note that dereferencing
    this iterator gives a pair<string, CallbackList>  Where the strig is the name
    of a variable or elemement, and CallbackList is a list of CTCLTracedVariables
    whose callback objects are registered via addChangeHandler to fire when a
    variable has changed.
*/
CDocumentationCallback::TraceIterator
CDocumentationCallback::begin()
{
  return m_Callbacks.begin();
}
/*!
   Returns an end of iteration iterator to the callback map.  See 
   CDocumentationCallback::begin() for more information about what this
   returns.
*/
CDocumentationCallback::TraceIterator
CDocumentationCallback::end()
{
  return m_Callbacks.end();
}
/*!
  Returns the number of elements in the trace map.  Note that since this map
  contains lists of callbacks, this is not the same as returning the number of
  callbacsk that have been registered.
*/
int
CDocumentationCallback::size()
{
  return m_Callbacks.size();
}

/*!
  This member is called whenever a documentation buffer is received.
  The header of the documentation buffer indicates how many scriptlets are
  present.  The body then contains sets of null terminated strings.
  Each string is a little scriptlet.  The body contents are somewhat complicated
  by the fact that, if necessary, strings are padded with an additional  nulll to 
  ensure that each string starts on a word boundary.
  \param type (unsigned int)
     Type of buffer being processed. (ignored).
  \param buffer (const void*)
     Ordinary pointer to the buffer...we'll convert this into a translating pointer
     in order to deal with the header.  The buffer contents are (I think)
     byte streams  and therefore don't require translation.

   You may notice that we don't do anything with the callbacks.  TCL's trace
   facility will do that autonomously.

 */
void
CDocumentationCallback::operator()(unsigned int type,
				   const void*  pBuffer)
{
  BufferTranslator* pTranslator = CBufferProcessor::getTranslatingPointer(pBuffer);
  TranslatorPointer<UShort_t>  p(*pTranslator);

  int nItems = p[6];
  const char* pRaw = (const char*)pBuffer;

  pRaw += sizeof(bheader);	// Looking at the body now...

  for(int i =0; i < nItems; i++) {
    m_pInterpreter->GlobalEval(pRaw);
    int scriptLength = strlen(pRaw) +1;	 // Include the null
    if(scriptLength %  2) {
      scriptLength++; // And, if necessary a second null
    }
    pRaw += scriptLength;
  }

}
