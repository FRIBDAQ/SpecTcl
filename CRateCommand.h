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

#ifndef __CRATECOMMAND_H
#define __CRATECOMMAND_H

#ifndef __TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif

class CTCLInterpreter;
class CTCLObject;


/*!
  This class provides a new SpecTcl command: 'rate' to SpecTcl.
  rate generates and manipulates objects that keep track 
  of the rate at which spectra increment.  All types of 
  spectra are currently supported.  The
  formats for the command are:
\verbatim
   rate create spectrum      - Creates a new rate computer.
   rate delete spectrum      - Destroys any rate computer for the spectrum.
   rate list                 - List information about all the rate
                               computers.
\endverbatim
*/
class CRateCommand : public CTCLObjectProcessor 
{
public:
  CRateCommand(CTCLInterpreter& interp);
  virtual ~CRateCommand();
private:
  CRateCommand(const CRateCommand& rhs);
  CRateCommand& operator=(const CRateCommand& rhs);
  int operator==(const CRateCommand& rhs) const;
  int operator!=(const CRateCommand& rhs) const;

public:
  virtual int operator()(CTCLInterpreter& interp,
			 STD(vector)<CTCLObject>& objv);
protected:
  int createRate(CTCLInterpreter& interp,
	     STD(vector)<CTCLObject>& objv);
  int deleteRate(CTCLInterpreter& interp,
	     STD(vector)<CTCLObject>& objv);
  int listRates(CTCLInterpreter& interp,
		STD(vector)<CTCLObject>& objv);

private:
  STD(string) Usage();
};

#endif
