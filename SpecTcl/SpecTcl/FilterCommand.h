#ifndef __FILTERCOMMAND_H  // Required for current class.
#define __FILTERCOMMAND_H

//Required for base classes.
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#define __TCLPACKAGEDCOMMAND_H
#endif

#ifndef __STL_VECTOR_H
#include <vector>
#define __STL_VECTOR_H
#endif

#ifndef __STL_STRING_H
#include <string>
#define __STL_STRING_H
#endif

#ifndef __GATEDEVENTFILTER_H
#include <GatedEventFilter.h>
#define __GATEDEVENTFILTER_H
#endif

#ifndef __FILTERDICTIONARY_H
#include <FilterDictionary.h>
#define __FILTERDICTIONARY_H
#endif

// Forward declarations.
class CTCLCommandPackage;
class CTCLInterpreter;
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
  int operator==(const CFilterCommand& aCFilterCommand) { // Legal, but weird.
    return (CTCLProcessor::operator==(aCFilterCommand));
  }
 private:
  CFilterCommand& operator=(const CFilterCommand& aCFilterCommand); // Assignment operator is illegal.

  // Additional functions.
 public:
  Int_t Create(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t Delete(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t Enable(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t Disable(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t Regate(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t File(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  Int_t List(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
  string ListFilter(const char*);
 protected:
  static eSwitches MatchSwitch(const char* pSwitch);
  static std::string Usage();
};

#endif
