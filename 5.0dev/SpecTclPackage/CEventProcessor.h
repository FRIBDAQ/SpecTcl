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
#ifndef _CEVENTPROCESSOR_H
#define _CEVENTPROCESSOR_H

/**
 * @file CEventProcessor.h
 * @brief abstract base class for elements of the event processing pipeline.
 */

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

#ifndef _STDLIB_H
#include <stdlib.h>		// For size_t
#ifndef _STDLIB_H
#define _STDLIB_H
#endif
#endif

class CAnalysisPipeline;

/**
 * @class CEventProcessor
 *
 *  This is the abstract base class for all elements of the analysis pipeline.
 *  Event processors have names and a virtual method: onEvent that is
 *  called for each item received from the decoder.
 */
class CEventProcessor
{
  // Class level data.
private:
  static unsigned m_NameCount;

  // Local per object data

private:
  std::string m_name;

  // public methods:

public:
  CEventProcessor(const char* pName = 0);

  // Base class methods:

  std::string getName() const;

  // Inteface:

  virtual bool onEvent(void* pEvent, size_t nBytes, CAnalysisPipeline* pipeline) = 0;
};

#endif
