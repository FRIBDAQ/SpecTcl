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
#define __STL_VECTOR
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
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
public:

			//Operator== Equality Operator - legal but...
  
  int operator== (const CSpectrumCommand& aCSpectrumCommand)
  { 
    return ((CTCLPackagedCommand::operator== (aCSpectrumCommand)));
  }                             
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

  void          SortSpectraById(std::vector<std::string>& rvProperties);
  void          SortSpectraByName(std::vector<std::string>& rvProperties);

  static void   VectorToResult(CTCLResult& rResult, 
			       std::vector<std::string>& rvStrings);
public:
  UInt_t      ExtractId(const std::string& rProperties);
  std::string ExtractName(const std::string& rProperties);
};

#endif
