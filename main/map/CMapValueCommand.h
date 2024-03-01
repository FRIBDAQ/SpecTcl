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
#ifndef CMAPVALUECOMMAND_H
#define CMAPVALUECOMMAND_H


#include <TCLObjectProcessor.h>
#include <map>

/*!
  This class provides the mapvalue command.  The mapvalue command provides a mechanism
  to define event processors that map an input parameter to an output parameter
  via an integer->floating point map.  One sample use case for this would be to 
  map detector segment numbers to positions.  This mapping could take into account
  cable weirdnesses as well as the geometry of the detector.
  Syntax:

\verbatim
mapvalue inparam outparam  map

\endverbatim
  where:
  - inparam is the name of an existing defined parameter that will be the source
    parameter for the event processor.
  - outparam is the name of an output parameter (A tree parameter will be made for it)
    that the result of the map will be stored in.
  - map is a Tcl list of in/out pairs. The first element of each pair must be an integer,
    while the second can be a float.  Suppose, for example, our input number is a segment
    from an 8 segment detector for which the cables are reversed relative to the position.
    we could define mapvalue segment position  {{0 7} {1 6} {2 5} {3 4} {4 3} {5 2} {6 1} {7 0}}
    an event with the parameter "segment" having a value of 2 would result in parameter
    "position" having a value of 5.
*/

class CMapValueCommand : public CTCLObjectProcessor
{
public:
  CMapValueCommand(CTCLInterpreter& interp);
  virtual ~CMapValueCommand();

  virtual int operator()(CTCLInterpreter&         interp, 
			 std::vector<CTCLObject>& objv);
private:
  std::map<int, float> createMap(CTCLInterpreter& interp, CTCLObject& mapList);
  std::string          usage();
};

#endif
