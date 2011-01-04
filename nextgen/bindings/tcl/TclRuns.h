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


#ifndef __TCLRUNS_H
#define __TCLRUNS_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

class CHandleManager;

/**
 ** This command implements the SpecTcl run command
 ** This is a command ensemble that manages the runs table in the experiment
 ** database.
 ** The following subcommands are supported:
 ** \verbatim
 **    run create handle number title ?options?
 **    run end    handle number ?[clock scan parseabletimestring]?
 **    run list   handle
 **
 ** \endverbatim
 ** - handle - is an experiment database handle gotten from expopen or expcreate
 ** - title  - Is a title string for the run. It must be quoted if it contains whitespace or other
 **            tcl special characters e.g. {[Oxygen beam] $1,000,00 run} has $ and []'s which must
 **            be disabled via {}'s.
 ** - options- are name value pairs. the legal names are -start -end which provide
 **            the start and end times of a run (note that run creat inserts a default start time of
 **            now if not provided.  The value for these options is a time in seconds from the
 **            epoch (e.g. [clock seconds] for now.  If you have  date/time string use
 **            [clock scan] to convert it to a usable value.
 **
 ** The create command returns the run number of the created run.  The
 ** list command produces a Tcl list of the known runs.  The list is in a form that can be
 ** used with array set to get an array indexed by run number.  The payload of each array element is
 ** itself a list of the form {title start-time end-time}  where null elements in the database table
 ** are returned as empty elements.
 **
 */
class CTclRuns : public CTclDBCommand
{
  // Types private to this class:

private:
  struct parsedOptions {
    bool   s_haveStart;
    time_t s_start;
    bool   s_haveEnd;
    time_t s_end;
    bool   s_haveUnrecognized;
    std::string s_unrecognized;
  };

  // Canonicals
public:
  CTclRuns(CTCLInterpreter& interp);
  virtual ~CTclRuns();

  // Interface for CTCLObject Processor
public:
  int operator()(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv);
  // subcommand handlers.
private:

  int create(CTCLInterpreter& interp, 
	     std::vector<CTCLObject>& objv) throw(std::string);
  int end(CTCLInterpreter& interp, 
	  std::vector<CTCLObject>& objv) throw(std::string);
  int list(CTCLInterpreter& interp, 
	  std::vector<CTCLObject>& objv) throw(std::string);

  // utilities:

  void parseOptions(parsedOptions& result,
		    CTCLInterpreter& interp,std::vector<CTCLObject>& objv,
		    unsigned startIndex) throw(std::string);

};

#endif 
