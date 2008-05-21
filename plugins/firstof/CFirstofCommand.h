/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __CFIRSTOFCOMMAND_H
#define __CFIRSTOFCOMMAND_H

#ifndef __TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif

#include  __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

class CTCLInterpreter;
class CTCLObject;

/*!
   Adds the command 'firstof' to the interpreter.  The command adds an event processor
   that produces  new parameters from a list of existing parameters.  The new parameter
   is the first valid parameter from the parameters in the list.
   This is useful in e.g. producing an x or y paramteer from a dsssd from the strip
   parameters.

   The two new parameters produced are the value of the first parameter and the offset
   of into the parameter list of the first valid parameter.  This allows the production
   of hit spectra as well as hit energy spectra.

   Form of the command:

   \verbatim
     firstof valueparam hitnumparam [list p1 p2...]
   \endverbatim

   At present, there's no way to list the parameters so created nor
   edit them.
*/
class CFirstofCommand : public CTCLObjectProcessor
{
  // Canonical member functions>
public:
  CFirstofCommand(CTCLInterpreter& interp, std::string name = std::string("firstof"));
  virtual ~CFirstofCommand();

private:
  CFirstofCommand(const CFirstofCommand& rhs);
  CFirstofCommand& operator=(const CFirstofCommand& rhs);
  int operator==(const CFirstofCommand& rhs) const;
  int operator!=(const CFirstofCommand& rhs) const;
public:

  // The command interface:

public:
  int operator()(CTCLInterpeter&          interp,
		 std::vector<CTCLObject>& objv);
 
  // utilities.
private:
  std::string Usage() const;
};


#endif
