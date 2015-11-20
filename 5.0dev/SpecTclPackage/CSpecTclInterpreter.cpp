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
 * @file CSpecTclInterpreter.cpp
 * @brief implement the spectcl interpreter singleton.
 */
#include "CSpecTclInterpreter.h"

// class level data:

Tcl_Interp*          CSpecTclInterpreter::m_pInterp(0);
CSpecTclInterpreter* CSpecTclInterpreter::m_pInstance(0);


/*--------------------------------------------------------------------------
 * Static methods
 */

/**
 * instance
 *
 *   If necessary create a new instance of the singleton, if not, just
 *   return a pointer to the existing instance.
 *
 *  @return CSpecTclInterpreter*
 *
 *  @throw spectcl_interp_exception - underlying interpreter has not yet been set.
 */
CSpecTclInterpreter*
CSpecTclInterpreter::instance()
{
  // Throw an exception if the underlying interpreter has not yet been set:

  if(!m_pInterp) {
    throw spectcl_interp_exception("CSpecTclInterpreter::instance - application interpeter not set");
  }

  // If necessary, create the singleton instance...

  if (!m_pInstance) {

    m_pInstance = new CSpecTclInterpreter;
  }

  // Return the singleton instance.

  return m_pInstance;
}
/**
 * setInterp
 *    Set the current interpreter.
 *
 *  @param pInterp -  pointer to the application interpreter on which SpecTcl will run.
 */
void
CSpecTclInterpreter::setInterp(Tcl_Interp* pInterp)
{
  if (m_pInterp) {
    throw spectcl_interp_exception("CSpecTclInterpreter::setInterp - application interpreter already set");
  }
  m_pInterp = pInterp;
}


/**
 * Construction:
 *
 */
 CSpecTclInterpreter::CSpecTclInterpreter() :
   CTCLInterpreter(m_pInterp) {}
