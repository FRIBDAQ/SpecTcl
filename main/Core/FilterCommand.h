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

#include <TCLObjectProcessor.h>
#include <MPITclCommand.h>
#include <string>
#include <GatedEventFilter.h>
#include <FilterDictionary.h>

// Forward declarations.

class CTCLInterpreter;
class CTCLObject;
class CGatedEventFilter;
class CFilterDictionary;

// Note that the filter command only  runs in the
// Event Sink Pipeline rank in mpiSpecTcl.
// 

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

class CFilterCommandActual : public CTCLObjectProcessor {
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
  //  CFilterCommandActual(CTCLInterpreter& rInterp) : CTCLProcessor("filter", &rInterp);
  CFilterCommandActual(CTCLInterpreter& rInterp, const char* cmd = "::spectcl::serial::filter");
  ~CFilterCommandActual();

  // Unimplemented operations:
 private:
  CFilterCommandActual (const CFilterCommandActual& aCFilterCommandActual ); // Copy constructor is illegal.


  CFilterCommandActual& operator=(const CFilterCommandActual& aCFilterCommandActual); // Assignment operator is illegal.
  int operator==(const CFilterCommandActual& rhs) const;
  int operator!=(const CFilterCommandActual& rhs) const;

  // Operators.
 public:
  virtual int operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv);

  
  // Additional functions.
 public:
  Int_t Create(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]);
  Int_t Delete(CTCLInterpreter& rInterp,  int nArgs, const char* pArgs[]);
  Int_t Enable(CTCLInterpreter& rInterp,  int nArgs, const char* pArgs[]);
  Int_t Disable(CTCLInterpreter& rInterp,  int nArgs, const char* pArgs[]);
  Int_t Regate(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]);
  Int_t File(CTCLInterpreter& rInterp,  int nArgs, const char* pArgs[]);
  Int_t List(CTCLInterpreter& rInterp,  int nArgs, const char* pArgs[]);
  Int_t Format(CTCLInterpreter& rInterp, int nargs, const char* pArgs[]);

  std::string ListFilter(const std::string& rName,
		    CGatedEventFilter* pFilter);
  std::string ListFilter(const std::string& rName);
 protected:
  static eSwitches MatchSwitch(const char* pSwitch);
  static std::string Usage();
  static std::string SinkName(std::string filterName);
};

class CFilterCommand : public CMPITclCommand {
public:
  CFilterCommand(CTCLInterpreter& rInterp);
  ~CFilterCommand() {}
};

#endif
