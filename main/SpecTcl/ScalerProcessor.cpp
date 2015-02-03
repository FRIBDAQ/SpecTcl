
/*
    This software is Copyright by the Board of Trustees of Michigan
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

// Implement the CScalerProcessor class.  This class can be used to export scaler
// data from NSCL buffer structured data analysis by registering it as an event
// processor.

#include <config.h>

#include "ScalerProcessor.h"

#include <TCLInterpreter.h>
#include "ScalerProcessorCallbacks.h"
#include <CBufferProcessor.h>
#include <Analyzer.h>
#include <TCLAnalyzer.h>
#include <BufferDecoder.h>
#include <Event.h>
#include <Globals.h>

#include <buftypes.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
   Construct a buffer processor.  At this point in time, the construction
   cannot rely on the interpreter being created, so we only create a buffer
   processor, and initialize everything else to null. OnAttach is where the
   interpreter will be located, the callbacks created and registered.
*/
CScalerProcessor::CScalerProcessor() :
  m_pProcessor(new CBufferProcessor),
  m_pInterpreter(0),
  m_pControlCallback(0),
  m_pScalerCallback(0)
{
}
/*!
  Copy construction.   All of our callaback elements support copy construction,
  so we will do a deep copy just for fun... with the exception of the
  interpreter object which will be shared between the two
  objects.    We allow for the fact that the
  m_pProcessor may be the only object that has been instantiated...
*/
CScalerProcessor::CScalerProcessor(const CScalerProcessor& rhs) :
  m_pProcessor(rhs.m_pProcessor),
  m_pInterpreter(0),
  m_pControlCallback(0),
  m_pScalerCallback(0)
{
  if (rhs.m_pInterpreter) m_pInterpreter = rhs.m_pInterpreter;
  if (rhs.m_pControlCallback) {
    m_pControlCallback = new CSpecTclControlBufferCallback(*(rhs.m_pControlCallback));
  }
  if (rhs.m_pScalerCallback) {
    m_pScalerCallback = new CSpecTclScalerBufferCallback(*(rhs.m_pScalerCallback));
  }
}
/*!
   Destruction... The interpreter is allowed to live after us and thererfore left
   alone.  Since all members get initialized to 0 and delete 0 is a no-op it's
   safe to just delete the rest of the members:

*/
CScalerProcessor::~CScalerProcessor()
{
  delete m_pProcessor;
  delete m_pControlCallback;
  delete m_pScalerCallback;
}

/*!
   Assignment: Same strategy as copy construction...except that we must delete
   the targets first since we are fully constructed at this stage:
*/
CScalerProcessor&
CScalerProcessor::operator=(const CScalerProcessor& rhs)
{
  if(this != &rhs) {
    CEventProcessor::operator=(rhs);

    delete m_pProcessor;
    delete m_pControlCallback;
    delete m_pScalerCallback;
    
    m_pProcessor       = rhs.m_pProcessor;
    m_pInterpreter     = rhs.m_pInterpreter;
    m_pControlCallback = new CSpecTclControlBufferCallback(*(rhs.m_pControlCallback));
    m_pScalerCallback  = new CSpecTclScalerBufferCallback(*(rhs.m_pScalerCallback));

      

  }
  return *this;
}

/*!
   Equality is defined as the interpreter pointers are the same and
   all non null object pointers point to equal objects.
   for null pointers, NULL == NULL and NULL != any non null.
*/
int
CScalerProcessor::operator==(const CScalerProcessor& rhs) const
{
  // For this comparison it's easiest to enumerate the cases of
  // inequality and let true be when all of these fail.


  if(CEventProcessor::operator!=(rhs))     return 0;

  if(m_pInterpreter != rhs.m_pInterpreter)  return 0;
  if(m_pProcessor   != rhs.m_pProcessor)    return 0;

  // Control callback:

  if(!CallbacksEqual<CSpecTclControlBufferCallback>(m_pControlCallback, 
						    rhs.m_pControlCallback)) {
    return 0;
  }
  if(!CallbacksEqual<CSpecTclScalerBufferCallback>(m_pScalerCallback,  
						   rhs.m_pScalerCallback)) {
    return 0;
  }

  // All other checks for inequality failed we're equal!!

  return -1;




}
/*!
   Inequality is the logical inverse of equality:
*/
int
CScalerProcessor::operator!=(const CScalerProcessor& rhs) const
{
  return !(*this == rhs);
}
////////////////////// Class overrides ////////////////////////////

/*!
  OnAttach does the final stage of the two phase construction of
  the object.  OnAttach is called when the callback is registered
  on the analysis pipeline.  At this time, we know we have interpreters
  and so forth and can initialize the remaining elements if they
  have not already been initialized e.g. via assingment.

  \param rAnalyzer (CAnalyzer& [in]):
      Reference to the analyzer object.  This is actualy
      a CTCLAnalyzer and we'll get the interpreter from it.

  
*/
Bool_t
CScalerProcessor::OnAttach(CAnalyzer& rAnalyzer)
{
  CTclAnalyzer &Analyzer(dynamic_cast<CTclAnalyzer&>(rAnalyzer));

  if(!m_pInterpreter) {
    m_pInterpreter = Analyzer.getInterpreter();
  }
  if(!m_pControlCallback) {
    m_pControlCallback = new CSpecTclControlBufferCallback(m_pProcessor,
							   m_pInterpreter);
  }
  if(!m_pScalerCallback) {
    m_pScalerCallback = new CSpecTclScalerBufferCallback(m_pProcessor,
							 m_pInterpreter);
  }
  // Register the callbacks:

  m_pProcessor->addCallback(SCALERBF, *m_pScalerCallback);
  m_pProcessor->addCallback(BEGRUNBF, *m_pControlCallback);
  m_pProcessor->addCallback(ENDRUNBF, *m_pControlCallback);
  m_pProcessor->addCallback(PAUSEBF,   *m_pControlCallback);
  m_pProcessor->addCallback(RESUMEBF, *m_pControlCallback);

  return kfTRUE;
}

/*!
  OnBegin is called for a begin run... for this as for all of the
  buffer in call functions, we just call ProcessBuffer.

 */
Bool_t
CScalerProcessor::OnBegin(CAnalyzer& rANalyzer, CBufferDecoder& rDecoder)
{
  ProcessBuffer(rDecoder);
  return kfTRUE;
}
/*!
   Called for an end run buffer, delegates to ProcessBuffer:
*/
Bool_t
CScalerProcessor::OnEnd(CAnalyzer& rANalyzer, CBufferDecoder& rDecoder)
{
  ProcessBuffer(rDecoder);
  return kfTRUE;
}
/*! Called for a pause run buffer.  Delegates to ProcessBuffer:
 */
Bool_t
CScalerProcessor::OnPause(CAnalyzer& rANalyzer, CBufferDecoder& rDecoder)
{
  ProcessBuffer(rDecoder);
  return kfTRUE;
}
/*!
  Called for A resume buffer.  Delegates to processbuffer:
*/
Bool_t
CScalerProcessor::OnResume(CAnalyzer& rANalyzer, CBufferDecoder& rDecoder)
{
  ProcessBuffer(rDecoder);
  return kfTRUE;
}
/*!
  Called for any other non -event buffer. (this includes scaler buffers!!).
  Delegates to processbuffer:

*/
Bool_t
CScalerProcessor::OnOther(UInt_t nType, 
			CAnalyzer& rANalyzer, CBufferDecoder& rDecoder)
{
  ProcessBuffer(rDecoder);
  return kfTRUE;
}
  


///////////////////// Utility functions ///////////////////////////

// Process a buffer.  The processing is forwarded to m_pProcessor
// which arranges for our callbacks to be invoked at appropriate times:
//
void
CScalerProcessor::ProcessBuffer(CBufferDecoder& rDecoder)
{
  const void* pBuffer = rDecoder.getBuffer();

  (*m_pProcessor)(pBuffer);

}

// Utility function to compare callbacks for equality given a pair of
// pointers either of which may be null:

template<class T>
bool CScalerProcessor::CallbacksEqual(T* c1, T* c2) const
{
  if(c1) {
    if(!c2 || (*c1 != *c1)) {
      return false;
    }
    else {
      return true;
    }
  } else {
    if(c2) {
      return false;
    } 
    else {
      return true;
    }
  }
  return true;			// I think this is not necessary.
}
