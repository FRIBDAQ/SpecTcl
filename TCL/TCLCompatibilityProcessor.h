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


#ifndef __TCLCOMPATIBILITYPROCESSOR_H
#define __TCLCOMPATIBILITYPROCESSOR_H

#ifndef __TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif


#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

// Forward class definitions:

class CTCLProcessor;
class CTCLObject;
class CTCLInterpreter;

/*!

   This is an adapter pattern class between object based commands
   and the classic argc/argv command interface.  See the 
   CTCLProcessor class for more information.   The idea is that
   a CTCLProcessor will contain one of these which will marshall
   the object interface parameters to the argc/argv interface,
   create at CTCLResult object and then dispatch to the
   CTCLProcessor so that old commands written using the
   argc/argv interface can continue to work even if 
   Tcl 9.0 deprecates or even eliminates this interface.

*/
class CTCLCompatibilityProcessor : public CTCLObjectProcessor
{
private:
  // Member data:

  CTCLProcessor& m_ActualCommand;

public:
  // Constructors and other canonicals.

  CTCLCompatibilityProcessor(CTCLProcessor& actualCommand);
  virtual ~CTCLCompatibilityProcessor();
	
  // Disallowed canonicals:
private:		      
  CTCLCompatibilityProcessor(const CTCLCompatibilityProcessor& rhs);
  CTCLCompatibilityProcessor& operator=(const CTCLCompatibilityProcessor& rhs);
  int operator==(const CTCLCompatibilityProcessor& rhs) const;
  int operator!=(const CTCLCompatibilityProcessor& rhs) const;
public:

  // overrides to the CTCLObjectProcessor definitions (adaptors).
public:
  virtual int operator()(CTCLInterpreter& interp,
			 STD(vector)<CTCLObject>& objv);
  virtual void onUnregister();
  
};

#endif
