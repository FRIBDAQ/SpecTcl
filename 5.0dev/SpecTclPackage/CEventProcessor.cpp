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

/**
 * @file CEventProcessor.cpp
 * @brief Implementation of the CEventProcessor base class methods.
 */

#include "CEventProcessor.h"
#include <sstream>

// Static member data:

unsigned CEventProcessor::m_NameCount(0);

/**
 * constructor
 *    Constructs the m_Name of the object.  If a name is supplied,
 *    that is used.  If not, then a name is created by using the
 *    m_NameCount : "EventProcessor_${m_NameCount}"  where the
 *    ${} construct has Tcl like meaning.
 *
 * @param pName - The name, NULL causes the name to be constructed
 *                by us.
 */
CEventProcessor::CEventProcessor(const char* pName)
{
  if (pName) {
    m_name = pName;
  } else {
    std::stringstream s;
    s << "EventProcessor_" << m_NameCount++;
    m_name = s.str();
  }
}
/**
 * getName
 *   Returns the name of the event processor.
 */
std::string
CEventProcessor::getName() const
{
  return m_name;
}
