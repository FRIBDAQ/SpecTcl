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

#ifndef _TCLWSSPECTYPES_H
#define _TCLWSSPECTYPES_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

// forward declarations:

typedef struct _spectcl_spectrum_type spectcl_spectrum_type;


/**
 * Implement the ::spectcl::wsSpecTypes command.  This command returns the
 * set of spectrum types that are currently known to the database schema
 * The form of this command is:
 \verbatim
::spectcl::wsSpecTypes exp-handle ?attach-point?
\endverbatim
 * Where:
 *   - exp-handle - is a handle to an experiment database that has a workspace
 *                  attached to it.
 *   - attach-point - is an optional parameter that indicates where the workspace
 *                    is attached. If omitted, the workspace is assumed to be attached
 *                    at WORKSPACE.
 * 
 * The command returns a list.  Each element of the list is itslef a two element list
 * containing:
 *    - the spectrum type (used in defining a spectrum).
 *    - a description of what that spectrum type means.
 *
 * The idea is that the spectrum type is brief to make spectrum generating commands
 * concise while the description is 'human readable'.
 */

class CTclWsSpecTypes : public CTclDBCommand
{
  /* Canonicals */
public:
  CTclWsSpecTypes(CTCLInterpreter& interp);
  virtual ~CTclWsSpecTypes();

private:
  CTclWsSpecTypes& operator=(const CTclWsSpecTypes&);
  int operator==(const CTclWsSpecTypes&) const;
  int operator!=(const CTclWsSpecTypes&) const;

  /* Command entry point: */

public:
  int  operator()(CTCLInterpreter& interp,
		  std::vector<CTCLObject>& objv);

  // Utilities:

private:
  std::string marshallList(CTCLInterpreter& interp, spectcl_spectrum_type** types);
};


#endif
