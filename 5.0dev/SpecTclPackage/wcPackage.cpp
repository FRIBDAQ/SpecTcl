/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include <tcl.h>
#include "CEventProcessor.h"
#include "CParameter.h"
#include "CPipelineCommand.h"

#include <string.h>
#include <stdlib.h>


/**
 * @file wcPackage.cpp
 * @brief Loadable package that supplied a word counting event processor.
 */

/**
 * The decoder
 */
class WcProcessor : public CEventProcessor {
  CParameter& m_param;
public:
  WcProcessor(const char* pName);
  virtual bool onEvent(void* pEvent, size_t nBytes, CAnalysisPipeline* pPipe);

};
/**
 * WcProcessor::onEvent
 *    use strtok to count the whitespaced tokens in the event which is assumed
 *    to be a string.
 *
 * @param pEvent - Pointer to the event 
 * @param nBytes - Number of bytes in the event.
 * @param pPipe  - Pointer to the analysis pipeline.
 *
 * @return true
 */
bool
WcProcessor::onEvent(void* pEvent, size_t nBytes, CAnalysisPipeline* pPipe)
{
  // pCopy will be a null terminated string copy of pEvent.  Needed since
  // - strtok needs a null terminated string.
  // - strtok will modify the input string.
  //
  char* pCopy = new char[nBytes+1];
  memset(pCopy, 0, nBytes+1);
  memcpy(pCopy, pEvent, nBytes);

  const char* delim= " \t";
  int result  = 0;
  if (strtok(pCopy, delim)) {
    result++;
    while (strtok(NULL, delim)) {
      result++;
    }
  } 
  m_param = result;

  return true;
}
/**
 * WcProcessor constructor just make the tree parameter:
 */
WcProcessor::WcProcessor(const char* pName) :
  CEventProcessor("wc"),
  m_param(*new CParameter(pName)) {}

/**
 * wc_init
 *
 * Package initialization
 * - Create an event processor that puts wc into 'wordcount'
 * - register it as a pipeline processing element.
 */
extern "C" {
  int Wcpackage_Init (Tcl_Interp* pInterp)
  {
    int status;

    status = Tcl_PkgProvide(pInterp, "wc", "1.0");
    if (status != TCL_OK) {
      return status;
    }
    CPipelineCommand* pPipe = CPipelineCommand:: instance();
    pPipe->defineElement(new WcProcessor("wordcount"));

    return TCL_OK;

  }
}
