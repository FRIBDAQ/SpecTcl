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

#ifndef __TCLLOADEVENTS_H
#define __TCLLOADEVENTS_H



#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif


typedef struct _ParameterData_ ParameterData, *pParameterData;

/**
 * This command loads an event database with new events.
 * Note that the underlying primitive does this operation within a transaction making
 * the entire load both atomic and an all or nothing operation.
 * The form of the command is:
 * 
\verbatim
 
spectcl::loadevents exphandle evhandle events

\endverbatim

 * Where:
 *   - exphandle is a handle to an already open experiment database (allows us to use
 *     symbolic parametern ames in the events)
 *   - evhandle is a handle to an already open event database.
 *   - events is a Tcl list that contains the events to load.  Each event is itelf a list of
 *     the form:
 *       {trigger_number {parameter value} {parameter value}....}
 *     where:
 *     - trigger_number is a number tha uniquely identifies the event (normally an incrementing
 *        value.  
 *     - 'parammeter' identifies the parameter to load.  This must be the name of a parameter
 *        that is defined in the experiment database open on exphandle.
 *     - 'value' is the floating point value of that parameter.
 * @note   The experiment and event databases specified must have the same uuid..that is the
 *         experiment database must be the experiment that corresponds to the event database.
 * @note All parameter names are checked to be valid parameters prior to attempting an insert.
 */
class CTclLoadEvents : public CTclDBCommand
{
  /* canonicals: */

public:
  CTclLoadEvents(CTCLInterpreter& interp);
  ~CTclLoadEvents();

  /* Public methods: */

public:
  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);

  /* Utility methods: */

private:
  int marshallEvents(void* pExperiment, CTCLObject& eventList, pParameterData* ppData);

  
};

#endif
