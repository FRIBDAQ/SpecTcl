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

class CFilterCommand : public CTCLProcessor {
  // Internal class definitions:
 public:
  enum eSwitches {
    keNew,
    keDelete,
    keEnable,
    keDisable,
    keRegate,
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
  Int_t List(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]);
 protected:
  static eSwitches MatchSwitch(const char* pSwitch);
  static std::string Usage();
};

#endif
