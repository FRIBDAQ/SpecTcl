/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


 
// Class: ChannelCommand                     //ANSI C++
//
// parses the chan command.  The chan command
// has the following formats:
//
//  chan -get spname { indices }    
//      Returns the channel from spname selected
//      by the indices list.
//  chan -set spname { indices }  value
//      Sets the channel in spname selected
//      by indices to value.
//
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// Modified:
//     Chase Bolen (added TCL evaluation of indices in EvalIndex(...)) 4/20/00
//
// (c) Copyright NSCL 1999, All rights reserved ChanCommand.h
//

#ifndef CHANCOMMAND_H  //Required for current class
#define CHANCOMMAND_H

                               //Required for base classes
#include "TCLPackagedObjectProcessor.h"
                               
class ChannelCommand  : public CTCLPackagedObjectProcessor
{                       
public:
  enum Switches {		// Command switches in enum form.
    kSetSwitch,			// -set
    kGetSwitch,			// -get
    kNotSwitch			// Not a recognized switch.
  };

public:

// Constructors and other cannonical operations:

   ChannelCommand (CTCLInterpreter* pInterp) :
    CTCLPackagedObjectProcessor(*pInterp, "channel", true)
  { }
  
  virtual ~ChannelCommand ( )  // Destructor 
  { }  

private:
  ChannelCommand (const ChannelCommand& aChannelCommand );
  TCLPLUS::Int_t EvalIndex(CTCLInterpreter*, std::string&);
  ChannelCommand& operator= (const ChannelCommand& aChannelCommand);
  int operator==(const ChannelCommand& rhs) const;
  int operator!=(const ChannelCommand& rhs) const;
public:
	
  // operations:       
public:

  virtual   int operator() (CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv)    ;

  TCLPLUS::UInt_t Get (CTCLInterpreter* pInterp, TCLPLUS::UInt_t nArgs, const char* pArgs[])    ;

  TCLPLUS::UInt_t Set (CTCLInterpreter* pInterp, TCLPLUS::UInt_t nArgs, const char* pArgs[])    ;

protected:
  Switches ParseSwitch(const char* pSwitch);
  void Usage(CTCLInterpreter& rInterp);

};

#endif
