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

#ifndef _TCLINTERPRETER_H
#include <TCLInterpreter.h>
#endif

#ifndef _TCL_H
#include <tcl.h>
#ifndef _TCL_H
#define _TCL_H
#endif
#endif

#ifndef __STD_EXCEPTION
#include <exception>
#ifndef __STD_EXCEPTION
#define __STD_EXCEPTION
#endif
#endif

#ifndef __STD_STRING
#include <string>
#ifndef __STD_STRING
#define __STD_STRING
#endif
#endif

/**
 * @file CSpecTclInterpreter.h
 * @brief Provides a singleton CTCLInterpreter that wraps the apps main interp.
 */

#ifndef _CSPECTCLINTERPRETER_H
#define _CSPECTCLINTERPRETER_H

/**
 * @class CSpecTclInterpreter
 *
 *   This class is a CTCLInterpreter that is used to wrap the SpecTcl 
 *   application interpreter.   This is a singleton class/object.   Normally the package initialization
 *   module will call setInterp and then call instance() to create the singleton the
 *   first time so that commands can be registered on it.
 */
class CSpecTclInterpreter : public CTCLInterpreter
{

  // Class level data.
private:
  static Tcl_Interp*          m_pInterp;   // Underlying interpreter.
  static CSpecTclInterpreter* m_pInstance; // The singleton instance.

  // Construction is private:

private:
  CSpecTclInterpreter();

  // Static public methods for setting up and getting the instance:

public:
  static void setInterp(Tcl_Interp* pInterp);
  static CSpecTclInterpreter* instance();

  // The remaining methods come from this being a CTCLInterpreter.
};

// Exceptions we might throw:

class spectcl_interp_exception : public std::exception
{
private:
  std::string m_message;

public:
  spectcl_interp_exception(std::string msg) throw() :
    m_message(msg) {}
  ~spectcl_interp_exception() throw() {}

  const char* what() const throw() {return m_message.c_str();}
};

#endif
