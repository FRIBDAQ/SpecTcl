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
*/
#include <tcl.h>
#include "CEventProcessor.h"
#include "CPipelineCommand.h"
#include "CParameterDictionary.h"
#include "CPipelineCommand.h"

#include <iostream>

/**
 * @file tpdump.cpp
 * @brief loadable package supplying a tree parameter dump to stdout.
 */

class CTpDump : public CEventProcessor {
public:
  CTpDump() : CEventProcessor("tpdump") {}
  virtual bool onEvent(void* pEvent, size_t nBytes, CAnalysisPipeline* pPipe);
};
/**
 * CTpDump::onEvent
 *
 *   Traverse the tree parameter dictionary for each value produce a line
 *   on stdout of the form name: value.   If the tree parameter is not
 *   valid then the text "not set" is output instead of the value.
 *
 *
 * @param pEvent - Pointer to the event 
 * @param nBytes - Number of bytes in the event.
 * @param pPipe  - Pointer to the analysis pipeline.
 *
 * @return true
 */
bool
CTpDump::onEvent(void* pEvent, size_t nBytes, CAnalysisPipeline* pPipe)
{
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator p = pDict->begin();
  while (p != pDict->end()) {
    std::string                          name  = p->first;
    CParameterDictionary::pParameterInfo pInfo = p->second;
    std::cerr << name << ": ";
    if (pInfo->s_value.isvalid()) {
      std::cerr <<  (double)pInfo->s_value;
    } else {
      std::cerr << "not set";
    }
    std::cerr << std::endl;
    
    p++;
  }
  
  return true;
}

/**
 * Init for the pacakge
 */
extern "C" {
  int Tpdump_Init(Tcl_Interp* pInterp) {
    int status = Tcl_PkgProvide(pInterp, "tpdump", "1.0");
    if (status != TCL_OK) {
      return status;
    }

    // Register  CTpDump instance with the pipeline command
    // so it can go into the analysis pipeline.

    CPipelineCommand* pPipe = CPipelineCommand::instance();
    pPipe->defineElement(new CTpDump);

    return TCL_OK;

  }
}
