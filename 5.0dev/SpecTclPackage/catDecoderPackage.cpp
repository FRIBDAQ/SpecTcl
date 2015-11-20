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
#include <iostream>

#include "CPipelineCommand.h"
#include "CDataSource.h"
#include "CDecoder.h"
#include "CEventProcessor.h"

/**
 * @file catDecoderPackage.cpp
 * @brief Implements the catdecoder package which provides a 
 *        decoder that outputs data on stdout.
 */


static const char* packageVersion="1.0";


/**
 * Define and implement the CatDecoder class:
 */
class CatDecoder : public CDecoder
{
private:
  bool m_echo;
  std::string line;
  Tcl_Channel m_stdOut;
public:
  CatDecoder(bool echo = true) : m_echo(echo) {
    m_stdOut = Tcl_GetStdChannel(TCL_STDERR);
  }
  virtual void   onData(CDataSource* pSource) {
    line = "";
    char c;
    while (pSource->read(&c, 1) == 1) {
      line += c;
      if (c == '\n') break;
    }
    if ( m_echo && (line.size() > 0)) {	//  here's the cat part
      Tcl_WriteChars(m_stdOut, line.c_str(), -1);
      Tcl_Flush(m_stdOut);
    }
  }
  virtual size_t getItemSize() {
    return line.size();
  }
  virtual void*  getItemPointer() {
    if (line.size()  > 0) {
      return const_cast<char*>(line.c_str());
    } else {
      return reinterpret_cast<char*>(0);
    }
  }
  virtual bool   next() { return false;}
};

/**
 * Define and implement the cat event processor:
 */
class CatProcessor : public CEventProcessor
{
  Tcl_Channel m_stdOut;
public:
  CatProcessor(const char* pName) : CEventProcessor(pName) {
    m_stdOut = Tcl_GetStdChannel(TCL_STDERR);
  }
  virtual bool onEvent(void* pEvent, size_t nBytes, CAnalysisPipeline* pPipe) {
    Tcl_WriteChars(m_stdOut, reinterpret_cast<char*>(pEvent), nBytes);
    Tcl_Flush(m_stdOut);
    return true;
  }
};

/**
 * Catdecoder_init
 *
 *   Provides and initializes the package:
 *
 *   *  Provides the package to the interpreter.
 *   *  Creates a cat decoder and makes it available to the pipeline commande
 *      as a 'cat' decoder.
 */
extern "C" {
  int
  Catdecoder_Init(Tcl_Interp* pInterp)
  {
    int status;
    
    /* Provide the package:   */

    status = Tcl_PkgProvide(pInterp, "catdecoder", packageVersion);
    if (status != TCL_OK) {
      return status;
    }

    
    CPipelineCommand* pPipe = CPipelineCommand::instance();
    pPipe->defineDecoder("cat", new CatDecoder);
    pPipe->defineDecoder("lines", new CatDecoder(false));
    pPipe->defineElement(new CatProcessor("cat"));
   
    return TCL_OK;
  }
}
