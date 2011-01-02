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


#ifndef __TCLPROCESSOR_H
#define __TCLPROCESSOR_H

#ifndef TCLINTERPRETEROBJECT_H
#include <TCLInterpreterObject.h>
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif


#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

// Forward classes

class CTCLInterpreter;
class CTCLResult;
class CTCLCompatibilityProcessor;

/*!
   This class is an abstract base class for commands written against the
   argc/argv interface of Tcl/Tk.  Note that since this interface is
   supposedly  due to be deprecated with Tcl 9.0, this is actually
   written with the aid of an adaptor class (CTCLCompatibilityProcessor)
   to work with only the object interface.

   This is a complete rewrite of this class since SpecTcl-3.0
   however it should be compatible with older uses.

*/
class CTCLProcessor : public CTCLInterpreterObject
{
  // Data members:
private:
  std::string                  m_Command;	   // (initial) Name of the command.
  CTCLCompatibilityProcessor*  m_pObjectProcessor; // Adaptor.

  // Constructors and other canonicals.
public:
  CTCLProcessor(const std::string sCommand, CTCLInterpreter* pInterp);
  CTCLProcessor(const char*       pCommand, CTCLInterpreter* pInterp);
  virtual ~CTCLProcessor();

private:
  CTCLProcessor(const CTCLProcessor& rhs);
  CTCLProcessor& operator=(const CTCLProcessor& rhs);
  // 
  // these used to be public but I doubt they are really useful.
  //
  int operator==(const CTCLProcessor& rhs) const;
  int operator!=(const CTCLProcessor& rhs) const;
public:

  // Selectors we must retain for compatibility with the old use
  // we omitted begin()/end() as they no longer have meaning,
  // and will have to deal with any usage we see in SpecTcl e.g.

  std::string getCommandName() const;

  // Operations and overrides:

  virtual int operator()(CTCLInterpreter& rInterpreter,
			 CTCLResult&      rResult,
			 int argc, char** argv) = 0; // pure.
  virtual void OnDelete();

  void Register();
  void Unregister();
  void RegisterAll();		// Not really different than register now.
  void UnregisterAll();

  // The functions below should be refactorable into the interpreter object:

  static  std::string ConcatenateParameters (int nArguments, 
					      char* pArguments[])  ;
  int ParseInt (const char* pString, int* pInteger)  ;
  int ParseInt (const std::string& rString, int* pInteger) {
    return ParseInt(rString.c_str(), pInteger);
  }

  int ParseDouble (const char* pString, double* pDouble)  ;
  int ParseDouble (const std::string& rString, double* pDouble) {
    return ParseDouble(rString.c_str(), pDouble);
  }

  int ParseBoolean (const char* pString, Bool_t* pBoolean)  ;
  int ParseBoolean (const std::string& rString, Bool_t* pBoolean) {
    return ParseBoolean(rString.c_str(), pBoolean);
  }

  static int MatchKeyword(std::vector<std::string>& MatchTable, 
			  const std::string& rValue, 
			  int NoMatch = -1);
  virtual void preCommand();	// Called just prior to operator()
  virtual void postCommand();	// Called on return from operator()
  virtual void preDelete();	// Ditto for OnDelete().
  virtual void postDelete();
			  
  CTCLInterpreter* Bind(CTCLInterpreter& binding);
  CTCLInterpreter* Bind(CTCLInterpreter* binding);


  // Utilities available for derived classes.
protected:

  void NextParam(int& argc, char**& argv) {
    argc--;
    argv++;
  }
};
#endif
