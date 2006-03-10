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

//  CSpectrumCommand.h:
//
//    This file defines the CSpectrumCommand class.
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

#ifndef __SPECTRUMCOMMAND_H  //Required for current class
#define __SPECTRUMCOMMAND_H
                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif
              
#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

class CTCLInterpreter;
class CTCLResult;
class CTCLCommandPackage;                  
                               
class CSpectrumCommand  : public CTCLPackagedCommand        
{
public:
  enum Switch {
    keNew,
    keList,
    keId,
    keDelete,
    keAll,
    keById,
    keShowGate,
    keNotSwitch
  };
public:
			//Constructor with arguments
  CSpectrumCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage ) :
    CTCLPackagedCommand("spectrum", pInterp, rPackage)
  { }
  virtual ~CSpectrumCommand ( ) { }       //Destructor	

			//Copy constructor - Illegal
private:
  CSpectrumCommand (const CSpectrumCommand& aCSpectrumCommand );
public:

			//Operator= Assignment Operator - Illegal;
private:
  CSpectrumCommand& operator= (const CSpectrumCommand& aCSpectrumCommand);
  int operator==(const CSpectrumCommand& rhs);
  int operator!=(const CSpectrumCommand& rhs);
public:

  // Operations:

public:  
  virtual   int operator() (CTCLInterpreter& rInterpreter, CTCLResult& rResult,
			    int  nArgs, char* pArgs[])  ;
  Int_t New (CTCLInterpreter& rInterpreter, CTCLResult& rResult,
	     int nArgs, char* pArgs[])  ;
  Int_t List (CTCLInterpreter& rInterp, CTCLResult& rResult, 
	      int nArgs, char* Args[]);
  Int_t Delete (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		int nArgs, char* pArgs[])  ;
protected:
  static void   Usage(CTCLResult& rResult);
  static Switch MatchSwitch(const char* pSwitch);

  void          SortSpectraById(STD(vector)<STD(string)>& rvProperties);
  void          SortSpectraByName(STD(vector)<STD(string)>& rvProperties);

  static void   VectorToResult(CTCLResult& rResult, 
			       STD(vector)<STD(string)>& rvStrings);
public:
  UInt_t      ExtractId(const STD(string)& rProperties);
  STD(string) ExtractName(const STD(string)& rProperties);
};

#endif
