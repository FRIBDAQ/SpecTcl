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

#include <config.h>
#include "ScalerProcessorCallbacks.h"
#include <TCLInterpreter.h>
#include <TCLVariable.h>
#include <CBufferProcessor.h>
#include <TranslatorPointer.h>
#include <string>
#include <buftypes.h>
#include <buffer.h>
#include <stdint.h>

#include <histotypes.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/// Utility function(s)

// Set a TCL variable to a value...based on the format string supplied.
//
static void
SetVariable(CTCLVariable& rVariable, const char* format, void* value)
{
  char buffer[1000];
  snprintf(buffer, sizeof(buffer), format, value);
  rVariable.Set(buffer, TCL_GLOBAL_ONLY);

}

// Global eval a script tolerating exceptions:

static void
TolerantEval(CTCLInterpreter* pInterp, const char* pScript)
{

  try {
    pInterp->GlobalEval(pScript);
  }
  catch(...) {
  }

}

// Create however many zeroed totals elements as needed:

static void
CreateTotals(CTCLInterpreter* pInterp, int num)
{
  CTCLVariable totals(pInterp, "Scaler_Totals", kfFALSE);

  for(int i =0; i < num; i++) {
    char index[100];
    snprintf(index, sizeof(index), "%d", i);
    if(!totals.Get(TCL_GLOBAL_ONLY, index)) {
      totals.Set(index, const_cast<char*>("0"));	// Create if fetch fails.
    }
  }
}
// Set an array element with an integer subscript:

static void
SetArrayElement(CTCLVariable& rArray, int index, const char* format, void* value)
{
  char indexBuffer[100];
  char valueBuffer[1000];

  snprintf(indexBuffer, sizeof(indexBuffer), "%d", index);
  snprintf(valueBuffer, sizeof(valueBuffer), format, value);

  rArray.Set(indexBuffer, valueBuffer);
}

//////////////////////////////////  Implementation of CSpecTclControlBufferCallback
// See the ScalerProcessorCallbacks.h header for more information.

/*!
  Construct a callback for control buffers.
  \param pProcessor (CBufferProcessor*):
     Pointer to the buffer processor that can call us.
  \param pInterpreter (CTCLInterpreter*):
     Pointer to the interpreter that we will interact with.

 */
CSpecTclControlBufferCallback::CSpecTclControlBufferCallback(CBufferProcessor* pProcessor,
							     CTCLInterpreter*  pInterpreter) :
  m_pProcessor(pProcessor),
  m_pInterpreter(pInterpreter)
{
}

/*!
  Destruction of a buffer processor is currently a no-op... but supplied in case
  there are actions done up the class hierarchy of callbacks.
*/
CSpecTclControlBufferCallback::~CSpecTclControlBufferCallback()
{
}
/*!
   Copy construction is a shallow copy of the members.
*/
CSpecTclControlBufferCallback::CSpecTclControlBufferCallback(const CSpecTclControlBufferCallback& rhs) :
  m_pProcessor(rhs.m_pProcessor),
  m_pInterpreter(rhs.m_pInterpreter)
{

}

/*!
  Assignment is just a shallow copy of the member data:
*/
CSpecTclControlBufferCallback&
CSpecTclControlBufferCallback::operator=(const CSpecTclControlBufferCallback& rhs)
{
  if(this != &rhs) {
    CBufferCallback::operator=(rhs);
    m_pProcessor    = rhs.m_pProcessor;
    m_pInterpreter  = rhs.m_pInterpreter;
  }
  return *this;
}
/*!  Equality is defined as the member pointers being equal to each other.
 */
int
CSpecTclControlBufferCallback::operator==(const CSpecTclControlBufferCallback& rhs) const
{
  return ((CBufferCallback::operator==(rhs))              &&
	  (m_pProcessor   == rhs.m_pProcessor)              &&
	  (m_pInterpreter == rhs.m_pInterpreter));
}
/*! Inequality is the logial inverse of equality.
 */
int
CSpecTclControlBufferCallback::operator!=(const CSpecTclControlBufferCallback& rhs) const
{
  return !(*this == rhs);
}


/*!
    This member is called whenever a control buffer is seen.
    - We set the TCL variable ElapsedRunTime to the elapsed time in the buffer.
    - We set the TCL Variable RunNumber to the current run number.
    - We set the TCL Variable ScalerRunState as appropriate to the buffer type.
      this is a slight incompatibility 
      with the scaler interface, necessary because the RunState variable is used
      for other purposes within SpecTcl.
    - We set the RunTitle TCL Variable to the title in the buffer.
    - If this is a begin run we set RunStartTime to the absolute timestamp, and
      zero out the Scaler_Totals array.
    - If this is an endrun whe set RunEndTime to the absolute timestamp.
    - If defined, we call the following TCL procedures:
      - BeginRun (on begin runs)
      - EndRun (on end runs)
      - PauseRun (on pause run buffers)
      - ResumeRun (on resume run buffers).
      - Update regardless of the buffer type.

      \param nType (unsigned int):
         The type of the buffer being processed.
       \param pBuffer (const void*):
          A pointer to the buffer being processed.

     \note We really don't have to decode the buffer as prior to us,
     the standard callbacks for the processor were called to pull out everything
     we need.  We just get it from the processor.
*/
void
CSpecTclControlBufferCallback::operator()(unsigned int nType, const void* pBuffer)
{
  // These are the variables we'll need to map to:
  // Note that the scaler increment and scaler total arrays will be manipulated
  // via script-lets.

  CTCLVariable ElapsedRunTime(m_pInterpreter, "ElapsedRunTime", kfFALSE);
  CTCLVariable RunNumber(m_pInterpreter, "RunNumber", kfFALSE);
  CTCLVariable ScalerRunState(m_pInterpreter, "ScalerRunState", kfFALSE);
  CTCLVariable RunTitle(m_pInterpreter, "RunTitle", kfFALSE);
  CTCLVariable RunStartTime(m_pInterpreter, "RunStartTime", kfFALSE);
  CTCLVariable RunEndTime(m_pInterpreter, "RunEndTime", kfFALSE);


  // Do the stuff that's independent of buffer type:

  // Elapsed run time does have to be picked out of the buffer:

  BufferTranslator* bt = CBufferProcessor::getTranslatingPointer(pBuffer);
  TranslatorPointer<unsigned short> p(*bt);
  p += 16;                     // Skip over the header.
  p += 80/sizeof(unsigned short); // Dirty but there's no def for title len.

  TranslatorPointer<unsigned long> elapsed(p); 
  
  SetVariable(ElapsedRunTime, "%d", (void*)*elapsed);

  // All the other stuff comes from the buffer:

  SetVariable(RunNumber, "%d", (void*)static_cast<uint64_t>(m_pProcessor->runNumber()));
  SetVariable(RunTitle,  "%s", (void*)m_pProcessor->Title().c_str());
  
	      

  // Do the buffer type dependent stuff:

  switch (nType) {
  case BEGRUNBF:		// Begin run buffer:
    ScalerRunState.Set("Active");
    RunStartTime.Set(m_pProcessor->runStartTime().c_str());
    TolerantEval(m_pInterpreter,
      "foreach idx [array names Scaler_Increments] {set Scaler_Increments($idx) 0}");
    TolerantEval(m_pInterpreter,
      "foreach idx [array names Scaler_Totals] {set Scaler_Totals($idx) 0.0}");
    TolerantEval(m_pInterpreter, "BeginRun");
    break;
  case ENDRUNBF:
    ScalerRunState.Set("Halted");
    RunEndTime.Set(m_pProcessor->runEndTime().c_str());
    TolerantEval(m_pInterpreter, "EndRun");
    break;
  case PAUSEBF:
    ScalerRunState.Set("Paused");
    TolerantEval(m_pInterpreter, "PauseRun");
    break;
  case RESUMEBF:
    ScalerRunState.Set("Active");
    TolerantEval(m_pInterpreter, "ResumeRun");
    break;
  default:
    throw string("Invalid buffer type gotten in CScalerControlBufferCallback");
  }
  TolerantEval(m_pInterpreter, "Update");
  delete bt;			// The factory dynamically allocates this.
}


/////////////////////////////////   Implementation of CSpecTclScalerBufferCallback
// See the ScalerProcessorCallbacks.h header for more information.

/*!
  Construct a scaler processor callback.  This callback gets registered
  for scaler buffers only.

  \param pProcessor (CBufferProcessor* [in]):
      Pointer to the buffer processor that is managing us.  We'll use it's
      services in our callback so we maintain it.
  \param pInterpreter (CTCLInterpreter* [in]):
      Pointer to the TCL interpreter in which we are maintaining our variables.

*/
CSpecTclScalerBufferCallback::CSpecTclScalerBufferCallback(CBufferProcessor* pProcessor,
							   CTCLInterpreter*  pInterpreter) :
  m_pProcessor(pProcessor),
  m_pInterpreter(pInterpreter)
{
}
/*!
  Destructor for the object.  For now, since the objects we have pointers to
  are managed by external forces, this is a no-op.
*/
CSpecTclScalerBufferCallback::~CSpecTclScalerBufferCallback()
{
}
/*!
    Copy construction is a shallow member copy.
*/
CSpecTclScalerBufferCallback::CSpecTclScalerBufferCallback(const CSpecTclScalerBufferCallback& rhs) :
  m_pProcessor(rhs.m_pProcessor),
  m_pInterpreter(rhs.m_pInterpreter)
{
}
/*!
   assignment is also a shallow member copy:
*/
CSpecTclScalerBufferCallback&
CSpecTclScalerBufferCallback::operator=(const CSpecTclScalerBufferCallback& rhs)
{
  if (this != &rhs) {
    CBufferCallback::operator=(rhs);
    m_pProcessor   = rhs.m_pProcessor;
    m_pInterpreter = rhs.m_pInterpreter; 
  }
  return *this;
}
/*!
   Equality is defined as shallow member equality.
*/
int
CSpecTclScalerBufferCallback::operator==(const CSpecTclScalerBufferCallback& rhs) const
{
  return (CBufferCallback::operator==(rhs)                   &&
          (m_pProcessor    == rhs.m_pProcessor)              &&
	  (m_pInterpreter  == rhs.m_pInterpreter));
}
/*!
  Inequality is the logical inverse of equality.
*/
int
CSpecTclScalerBufferCallback::operator!=(const CSpecTclScalerBufferCallback& rhs) const
{
  return !(*this == rhs);
}

/*!
    The callback:
    It will run after the standard scaler callback,
  pull data from the processor context and maintain:
  - Scaler_Totals     - Total scaler counts this run.
  - Scaler_Increments - The increments this last scaler interval.
  - ScalerDeltaTime   - The length of the most recent time interval.
  - ElapsedRunTime    - The duration of the run.
  In addition, the callback will invoke the Update proc if it's defined.

  \note  Any existing Scaler_Totals get initialized to zero at the beginning of
         the run.  At the beginning of the first run, however, this array is
	 not defined, and therefore will be created by us to the appropriate
	 size.

*/
void
CSpecTclScalerBufferCallback::operator()(unsigned int nType, const void* pBuffer)
{
  int nScalers = m_pProcessor->scalerCount();
  CreateTotals(m_pInterpreter, nScalers); // If necessary create totals entries.

  // Now create TCL Variable objects for the variables we care about...

  CTCLVariable Totals(m_pInterpreter, "Scaler_Totals", kfFALSE);
  CTCLVariable Increments(m_pInterpreter, "Scaler_Increments", kfFALSE);
  CTCLVariable DeltaTime(m_pInterpreter, "ScalerDeltaTime", kfFALSE);
  CTCLVariable ElapsedTime(m_pInterpreter, "ElapsedRunTime", kfFALSE);

  // Set the variables...

  SetVariable(DeltaTime, "%d", 
	      (void*)(m_pProcessor->lastIntervalEnd() - m_pProcessor->lastIntervalStart()));
  SetVariable(ElapsedTime, "%d", (void*)m_pProcessor->lastIntervalEnd());
  //
  // The increments and totals:
  //
  for (int i = 0;  i < nScalers; i++) {
    SetArrayElement(Increments, i, "%d", (void*)m_pProcessor->lastIncrement(i));

    //       Below is not allowed: Casts of float -> void*.
    //    SetArrayElement(Totals,     i, "%f", (void*)m_pProcessor->Total(i));

    char buffer[1000];
    snprintf(buffer, sizeof(buffer), "%f", m_pProcessor->Total(i));
    char index[100];
    snprintf(index, sizeof(index), "%d", i);
    Totals.Set(index, buffer);

  }
  

  // Execute the Update script tolerantly.

  TolerantEval(m_pInterpreter, "Update");
}
  
