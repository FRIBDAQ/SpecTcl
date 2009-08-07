#ifndef __CCONSTPROCESSOR_H
#define __CCONSTPROCESSOR_H

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


#ifndef __TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif

/*!
   Class that implements the const command.  This has the following forms:

\literal
   const ?-and?   outparam outvalue [list inputparams]
   const -delete outparam
\endliteral
   The first form of the command creates a new const parameter.  If -and is present,
   all of the inputparams must be present to define the outparam, else any of the inputparams 
   being present will define outparam.   When defined, outparam will have the value outvalue.
   For many applications, this value will be 1, which will allow rate computations to be
   performed.

*/
class CConstProcessor : public CTCLInterpreterObject
{
  // Constructors and canonicals:
public:
  CConstProcessor(CTCLInterpreter& interp);
  virtual ~CConstProcessor();

  // illegal canonicals:

private:
  CConstProcessor(const CConstProcessor& rhs);
  CConstProcessor& operator=(const CConstProcessor& rhs);
  int operator==(const CConstProcessor& rhs) const;
  int operator!=(const CConstProcessor& rhs) const;

  // Dispatcher for the commands:

protected:
  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);

  // Helpers:

private:
  int create(CTCLInterpreter& interp,
	       std::vector<CTCLObject>& objv) const;
  int destroy(CTCLInterpreter& interp,
	      std::vector<CTCLObject>& objv) const;

  static std::string usage() const;
};

#endif
