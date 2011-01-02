/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#ifndef _TCLPARAMETER_H
#define _TCLPARAMETER_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif



class CHandleManager;


/** 
 ** This class provides Tcl bindings to the parameter API of the
 ** experiment database.  The parameter command is a command ensemble:
 ** \verbatim
 **   parameter create exphandle name ?options?
 **   parameter list   exphandle ?pattern?
 **   parameter alter  exphandle name ?limitoptions?
 ** \endverbatim
 ** - exphandle - is an experiment database handle from expcreate or expopen.
 ** - name      - is the name of a parameter.
 ** - pattern   - is an optional glob pattern.  Only parameters that match that pattern will be
 **               returned.  If not supplied, the pattern defaults to *.
 ** In the definition above options are:
 ** - -units optional parameter units. Value is a string
 ** - -low   optional parameter range low limit. Value is a double.
 ** - -hi    optinoal parameter range high limit. Value is a double.
 ** 
 ** limitoptions are the -low and -hi options above.
 **
 ** - create - Makes a new parameter.  It is an error to attempt to create an existing parameter
 **            On success, result is the name of the parameter created.
 ** - list     Lists parameters that match the pattern as described above.  The result is
 **            A Tcl list (possibly empty), one element per matching parameter.
 **            Each list element is a sublist containing
 **        - parameter name.
 **        - units string (empty if no units defined)
 **        - low limit (empty if no low limit defined)
 **        - high limit (empty if no high limit defined).
 */
class CTclParameter : public CTclDBCommand
{
  // private data:

  struct parsedOptions {
    bool          s_haveUnits;
    std::string   s_unitsString;
    bool          s_haveHigh;
    double        s_high;
    bool          s_haveLow;
    double        s_low;
    bool          s_haveUnrecognized;
    std::string   s_unrecognized;
  };

  // Canonicals
public:
  CTclParameter(CTCLInterpreter& interp);
  virtual ~CTclParameter();

  // Interface for CTCLObject Processor
public:
  int operator()(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv);
  // subcommands:

private:
  int create(CTCLInterpreter& interp,
	     std::vector<CTCLObject>& objv) throw(std::string);
  int list(CTCLInterpreter& interp,
	   std::vector<CTCLObject>& objv)throw(std::string);
  int alter(CTCLInterpreter& interp,
	    std::vector<CTCLObject>& objv)throw(std::string);



				       

  void parseOptions(parsedOptions&           result,
		    CTCLInterpreter&         interp, 
		    std::vector<CTCLObject>& objv,
		    unsigned                 startIndex) throw(std::string);

  void throwBadOption(parsedOptions          options,
		      std::string            subcommand) throw(std::string);
  
};

#endif
