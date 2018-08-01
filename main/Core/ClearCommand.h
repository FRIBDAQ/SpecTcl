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


//  CClearCommand.h:
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
*///
//    This file defines the CClearCommand class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef CLEARCOMMAND_H  //Required for current class
#define CLEARCOMMAND_H
                               //Required for base classes
#include "TCLPackagedCommand.h"
#include <vector>
#include <string>
#include <set>

class CTCLInterpreter;
class CTCLResult;
class CSpectrum;
                               
class CClearCommand  : public CTCLPackagedCommand        
{
public:
  // Switch set:
  enum Switch {
    keAll,
    keId,
    keChannels,
    keStatistics,
    keNotSwitch
  };


public:
  CClearCommand (CTCLInterpreter* pInterp,
		 CTCLCommandPackage& rPack) :
    CTCLPackagedCommand("clear", pInterp, rPack)
  { }        
  virtual ~CClearCommand ( ) 
  { }				//Destructor	
			//Copy constructor - illegal
private:
  CClearCommand (const CClearCommand& aCClearCommand );
public:
			//Operator= Assignment Operator - illegal

private:
  CClearCommand& operator= (const CClearCommand& aCClearCommand);
  int operator==(const CClearCommand& rhs) const;
  int operator!=(const CClearCommand& rhs) const;
public:

  // Operations.

public:
  virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			    int nArgs, char* pArgs[])  ;
protected:
  static void   Usage(CTCLResult& rResult);
  static Switch MatchSwitch(const char* pSwitchText);

private:
  void clearAll(std::set<Switch> switches);
  int  clearIdList(
    CTCLResult& rResult, std::set<Switch> switches,
    std::vector<std::string> idStrings
  );
  int clearNameList(
    CTCLResult& rResult, std::set<Switch> switches,
    std::vector<std::string> names
  );
  std::pair<bool, bool> clearWhat(std::set<Switch> switches);

  void clearSpectrum(std::pair<bool, bool> clearFlags, CSpectrum* pSpec);
};

#endif
