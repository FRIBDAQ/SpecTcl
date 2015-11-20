/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2013.

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
 * @file CSpectrumCommand.cpp
 * @brief Implementation of the CSpectrumCommand command.
 */

#include "CSpectrumCommand.h"
#include "CSpecTclInterpreter.h"

/**
 *  The instance variable:
 */

CSpectrumCommand* CSpectrumCommand::m_pInstance(0);


/**
 * Construction (private)
 *
 *   Construction just uses the base class constructor to do all
 *   the dirty work:
 *
 * @param interp - interpreter on which we get registered.
 * @param command - Name of the command to create.
 */
CSpectrumCommand::CSpectrumCommand(CTCLInterpreter& interp, std::string command) :
  CTCLObjectProcessor(interp, command, true)
{}

/**
 * Destruction (private)
 *
 *  Destruction is a no-op since the base class does all the right stuff.
 */
CSpectrumCommand::~CSpectrumCommand() {}

/**
 * instance
 *
 * @return CSpectrumCommand* - Pointer to the singleton instance.
 */
CSpectrumCommand*
CSpectrumCommand::instance()
{
  if (!m_pInstance) {
    m_pInstance = new CSpectrumCommand(*CSpecTclInterpreter::instance(), "::spectcl::spectrum");
  }
  return m_pInstance;

}
