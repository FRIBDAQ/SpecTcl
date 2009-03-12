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

#ifndef __CSTACKMAPCOMMAND_H
#define __CSTACKMAPCOMMAND_H

#ifndef __TCLOBJECTCOMMAND_H
#include <TCLObjectProcessor.h>
#endif


#ifndef __CPARAMMAPCOMMAND_
#include "CParamMapCommand.h"
#endif



class CTCLObject;
class CTCLInterpreter;



/*!
  This class implements the Tcl stackMap command extension to SpecTcl.
  This extension is responsible for producing the data structures that
  drive the unpacking of VM-USB events.   Each stack consists of a vector of
  pointers to AdcMapping structs.  The map contains a code used to select the
  unpacker for that module.  It also contains an array of parameter ids corresponding
  to the channels of the module.

  The form of the command is:

  \verbatim
  stackMap stacknum modules
  \endverbatim
  
  where:
  - stacknum - is the number of the VM-USB stack being described.
  - modules  - is a list of modules in the stack in the order in which they are
               read.

  If stackMap is repeated for a stack that's already defined, the 
  stack mapping is silently replaced.


*/
class CStackMapCommand : public CTCLObjectProcessor
{
  // Exported data:
public:
  typedef std::vector<CParamMapCommand::AdcMapping*> stackMap;

  // Private data:

  static stackMap m_stacks[8];		// There are 8 stacks... no more no less.

  // Constructors and canonicals:

  CStackMapCommand(CTCLInterpreter& interp, std::string name = std::string("stackMap"));
  virtual ~CStackMapCommand();

private:
  CStackMapCommand(const CStackMapCommand& rhs);
  CStackMapCommand& operator=(const CStackMapCommand& rhs);
  int operator==(const CStackMapCommand& rhs) const;
  int operator!=(const CStackMapCommand& rhs) const;

public:
  static const stackMap& getMap(int number);
  

  // Virtual function overrides:

protected:
  
  virtual int operator()(CTCLInterpreter& interp,
			 STD(vector)<CTCLObject>& objv);


  // Utilities:


private:
  static std::string Usage();
  static void clearMap(stackMap& map);
};

#endif
