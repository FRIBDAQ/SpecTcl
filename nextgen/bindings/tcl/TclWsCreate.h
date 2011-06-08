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

#ifndef _TCLWSCREATE_H
#define _TCLWSCREATE_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif



/**
 * Implement the spectcl::wsCreate command.
 * This command is responsible for creating workspaces.
 * A workspace defines and contains analysis products.
 * A workspace is associated with an experiment and, potentially
 * a set of run databases.
 * The syntax of this command is:
 * \verbatim
::spectcl::wsCreate exphandle path
 * \endverbatim
 *
 * Where:
 *   - exphandle - is a handle open on the experiment database to which this
 *                 workspace will be associated.
 *   - path      - is the filesystem path that is the name of the new database file that
 *                 will be created.
 */
class CTclWsCreate : public CTclDBCommand
{
  /* Canonicals */
public:
  CTclWsCreate(CTCLInterpreter& interp);
  virtual ~CTclWsCreate();

private:
  CTclWsCreate& operator=(const CTclWsCreate&);
  int operator==(const CTclWsCreate&) const;
  int operator!=(const CTclWsCreate&) const;

  /* Command entry point: */

public:
  int  operator()(CTCLInterpreter& interp,
		  std::vector<CTCLObject>& objv);
 
};
#endif
