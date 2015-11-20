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
#include <string>

#include "CPipelineCommand.h"
#include "CEventProcessor.h"

/**
 * @file nullProcessor.cpp
 * @brief null package provides a null event processor.
 */

static const char* packageVersion="1.0";

/**
 * Implement the event processor.
 */
class CNullEventProcessor : public CEventProcessor {
public:
  CNullEventProcessor(const char* pName) :
    CEventProcessor(pName) {}
  virtual bool onEvent(void* pEvent, size_t nBytes,
		       CAnalysisPipeline* pPipe) {
    return true;
  }
};

/**
 * Nullprocessor_Init
 *
 *  * Provides the null event processor package.
 *  * Registers a null processor as an instance "null"
 *    with the processing pipeline so it can be
 *    added to the pipeline.
 */
extern "C" {
  int Nullprocessor_Init(Tcl_Interp* pInterp)
  {
    /* Provide the package */

    int status = Tcl_PkgProvide(pInterp, "null", packageVersion);
    if (status != TCL_OK) {
      return status;
    }
    CPipelineCommand* pPipe = CPipelineCommand::instance();
    pPipe->defineElement(new CNullEventProcessor("null"));
    pPipe->defineElement(new CNullEventProcessor("null1"));

    return TCL_OK;
  }
}
