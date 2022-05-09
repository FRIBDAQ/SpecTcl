/******************************************************************************
*
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
******************************************************************************/


/**
 * @file  ScalerProcessor.cpp  
 * @brief Analysis processor for scaler display
 * @author Ron Fox <ron@caentechnologies.com>
 */

#include "CScalerProcessor.h"
#include <TCLInterpreter.h>
#include <stdio.h>
#include "BufferTranslator.h"
#include "CRingBufferDecoder.h"
#include "RingFormatHelper.h"
#include <Globals.h>
#include <stdexcept>


/**
 * constructor
 * @param interp - interpreter to which our commands will be dispatched.
 */
ScalerProcessor::ScalerProcessor(CTCLInterpreter& interp) :
  m_Interp(interp)
{}

/**
 * destructor - noop.
 */
ScalerProcessor::~ScalerProcessor() 
{}

/**
 * onStateChange:
 *    Process Begin, Pause, End, Resume runs.
 *    - Set RunNumber
 *    - Set RunTitle
 *    - Set ElapsedRunTime
 *    - Invoke the correct function (BeginRun e.g.).
 *    - Invoke Update.
 *
 * @param type      - Type of state change
 * @param runNumber - the Run Number.
 * @param absTime   - Timestamp of the absolute time.
 * @param runTime   - Number of seconds into the run.
 * @param title     - Run title.
 * @param clientData- Unused.
 */ 

void
ScalerProcessor::onStateChange(
	CAnalysisBase::StateChangeType type, int runNumber, time_t absoluteTime, 
        float runTime, std::string title, void* clientData
)
{
  Set("RunNumber", runNumber);
  Set("ElapsedRunTime", (int)runTime);
  Set("RunTitle", title);

  std::string procName;
  std::string state;
  uint32_t sid  = getSid();

  switch (type) {
  case CAnalysisBase::Begin:
    state = "Active";
    procName = "BeginRun";
    Set("StartTime", absoluteTime);
    m_totals.clear();
    break;
  case CAnalysisBase::End:
    procName = "EndRun";
    state = "Halted";
    Set("StartTime", absoluteTime);    // I know this looks squirrely.
    break;
  case CAnalysisBase::Pause:
    procName = "PauseRun";
    state = "Paused";
    break;
  case CAnalysisBase::Resume:
    procName = "ResumeRun";
    state = "Active";
    break;
  }
  Set("ScalerRunState", state);
  m_Interp.GlobalEval(procName);
  Set("DataSource", sid);
}
/**
 * onScalers
 *   Process scaler data:
 *   - Set ElapsedRunTime
 *   - Set the ScalerDeltaTime value.
 *   - Set Scaler_Increments(i) for each element of the scalers array.
 *   - Invoke the Update command.
 *
 * @param absoluteTime - the time at which the event was emitted.
 * @param startOffset  - The counting interval start time offset.
 * @param endOffset    - The counting interval end time offset.
 * @param scalers      - Vector of scaler counts.
 * @param incremental  - Assumed to be true - NSCLDAQ is doing incremental scalers.
 * @param clientData   - Unused by this code.
 */
void
ScalerProcessor::onScalers(
        time_t absoluteTime, float startOffset, float endOffset,
        std::vector<unsigned> scalers, bool incremental, void* clientData
)
{
  Set("ElapsedRunTime", endOffset);
  Set("ScalerDeltaTime", (int)(endOffset - startOffset));

  Set("UpdateTime", absoluteTime);

  bool firstTime = m_totals.empty();
  for (int i = 0; i < scalers.size(); i++) {
    Set("Scaler_Increments", i, scalers[i]);
    if (firstTime) {
      m_totals.push_back(scalers[i]);
    } else {
      m_totals[i] += scalers[i];
    }
    Set("Scaler_Totals", i, m_totals[i]);
  }
  Set("DataSource", getScalerSid());
  m_Interp.GlobalEval("Update");

}

/*---------------------------------------------------------------------
 * Private Utilities
 *-------------------------------------------------------------------*/


/**
 * Set
 *   Simplest form of a variable set sets the variable to a string value.
 *   All strings are encapsulated in {}
 *
 * @param varName - The variable (could also be an array element).
 * @param value   - new variable value.
 * @note rather than building a CTCLVariable, we just do a set script.
 *
 */
void
ScalerProcessor::Set(const char* varName, std::string value) {
  std::string command = "set ";
  command += varName;
  command += " {";
  command += value;
  command += "}";

  m_Interp.GlobalEval(command);
  
}
/**
 * Set
 *   Same as above but the value is an integer:
 *
 * @param varName - The name to set.
 * @param value   - new value.
 */
void
ScalerProcessor::Set(const char* varName, int value)
{
  char sValue[100];
  sprintf(sValue, "%d", value);
  Set(varName, std::string(sValue));
}
/**
 * Set
 *   Same as above but the variable has an integer index.
 *
 * @param varName - Base variable name.
 * @param index   - integer index.
 * @param value   - integer value.
 */
void
ScalerProcessor::Set(const char* varName, int index, int value)
{
  char fullVarName[200];
  sprintf(fullVarName, "%s(%d)", varName, index);
  Set(fullVarName, value);
}
/**
 * getSid
 *   Return the source id from the body header.
 * @return uint32_t
 * @todo - need a get original sid for state change items (including helpers).
 */

uint32_t
ScalerProcessor::getSid()
{
  void*                   pItem = getItem();
  BufferTranslator* pTranslator = getTranslator();
  CRingFormatHelper* pHelper   = getHelper();
  return pHelper->getSourceId(pItem, pTranslator);
}
/**
 * getScalerSid
 *   Get the orignal source id of a scaler item..
 *   ALl of this works with v12 to get the original source id.
 *  @return uint32_t - the original source id if possible else the source id.
 */
uint32_t
ScalerProcessor::getScalerSid()
{
  
  void*                   pItem = getItem();
  BufferTranslator* pTranslator = getTranslator();
  CRingFormatHelper* pHelper   = getHelper();
  return pHelper->getScalerOriginalSourceId(pItem, pTranslator);
}
/**
 * getDecoder
 *   @return CRingBufferDecoder* - pointer to the current ring buffer decoder instance.
 *   @throw  std::logic_error if the current decoder is not a ring buffer decoder.
 */
CRingBufferDecoder*
ScalerProcessor::getDecoder()
{
  CRingBufferDecoder* pDecoder =
    dynamic_cast<CRingBufferDecoder*>(gpBufferDecoder);
  if (!pDecoder) {
    // non ringbuffers not supported.
    throw std::logic_error(
        "CScalerProcessor only support event files from NSCLDAQ10 and later"
    );
  }
  return pDecoder;
}
/**
 * getItem
 *   @return void* - pointer to the item being processed.
 */
void*
ScalerProcessor::getItem()
{
  return getDecoder()->getItemPointer(); 
}
/**
 *  getTranslator
 *    @return BufferTranslator - active buffer byte order traslator.
 */
BufferTranslator*
ScalerProcessor::getTranslator()
{
  return getDecoder()->getBufferTranslator();
}
/**
 * getHelper
 *
 *   @return CRingFormatHelper* - pointer to the current format helper.
 */
CRingFormatHelper*
ScalerProcessor::getHelper()
{
  return getDecoder()->getCurrentFormatHelper();
}
