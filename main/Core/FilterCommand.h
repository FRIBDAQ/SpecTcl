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

#ifndef FILTERCOMMAND_H  // Required for current class.
#define FILTERCOMMAND_H

//Required for base classes.

#include "TCLPackagedCommand.h"
#include <string>
#include <GatedEventFilter.h>
#include <FilterDictionary.h>

// Forward declarations.
class CTCLCommandPackage;
class CTCLInterpreter;
class CGatedEventFilter;
class CFilterDictionary;

/*!
   Implements the SpecTcl \em filter command. This command has the
   following format:
   \verbatim

   filter [-new] filtername gatename {par1 par2 ...}
   filter -delete filtername
   filter -enable filtername
   filter -disable filtername
   filter -regate filtername gatename
   filter -file filename filtername
   filter -list ?glob-pattern?
   filter -format filtername format

   \endverbatim

    and is used to manipulate pre-sort filter sets.   A filter is a gate,
    a set of parameters and an output file.  Events which make the filter's
    gate true have the specified parameters written to the output file.
    filters can also be disabled if desired.

*/

class CFilterCommand : public CTCLProcessor {
  // Internal class definitions:
 public:
  enum eSwitches {
    keNew,
    keDelete,
    keEnable,
    keDisable,
    keRegate,
    keFile,
    keList,
    keFormat,
    keNotSwitch
  };

  // Constructors.
 public:
  //  CFilterCommand(CTCLInterpreter& rInterp) : CTCLProcessor("filter", &rInterp);
  CFilterCommand(CTCLInterpreter& rInterp);
  ~CFilterCommand();
 private:
  CFilterCommand (const CFilterCommand& aCFilterCommand ); // Copy constructor is illegal.

  // Operators.
 public:
  virtual int operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);

 private:
  CFilterCommand& operator=(const CFilterCommand& aCFilterCommand); // Assignment operator is illegal.
  int operator==(const CFilterCommand& rhs) const;
  int operator!=(const CFilterCommand& rhs) const;

  // Additional functions.
 public:
  Int_t Create(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t Delete(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t Enable(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t Disable(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t Regate(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t File(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t List(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t Format(CTCLInterpreter& rInterp, CTCLResult& rResult, int Nargs, char* pArgs[]);

  std::string ListFilter(const std::string& rName,
		    CGatedEventFilter* pFilter);
  std::string ListFilter(const std::string& rName);
 protected:
  static eSwitches MatchSwitch(const char* pSwitch);
  static std::string Usage();
  static std::string SinkName(std::string filterName);
};

#endif
