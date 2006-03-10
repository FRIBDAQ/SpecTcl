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

static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";

// Class: CPseudoCommand
// Processes the pseudo parameter manipulation command.
//  This command has the following syntaxes:
//
//  pseudo name { dependent_params } { body }
//  pseudo -list [names... ]
//  pseudo -delete [names...]
//
//   The defining command's three parameters
// are the name of a previously declared but unfilled
// parameter, a list of dependent parameters and a
// procedure body to evaulate the parameter.
// In fact, SpecTcl will create a procedure named
//  name_Procedure  where name is the pseudo's
// name.
//   Example:
//
//      parameter p1  0 9
//      parameter p2  1 9
//      parameter sum 100 10
//      pseudo sum { p1 p2 } { return [expr $p1 + $p2 ] }
//
//    This will create a procedure:
//      proc sum_Procedure { p1 p2 } {
//                           return [expr $p1 + $p2]}
//   for each event, the values of p1 and p2 will be passed
//   in to this procedure and the return value will
//   be placed in the parameter bound to Sum at the
//   time the pseudo was defined.  Note that p1 and p2
//   can be pseudos as well as long as their evaluation
//   scripts are defined prior to Sum's.
//
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include <config.h>
#include "PseudoCommand.h"    				
#include "ParameterPackage.h"
#include <TCLList.h>
#include <TCLResult.h>
#include <histotypes.h>

#include <vector>
#include <list>
#include <string.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved PseudoCommand.cpp \n";

// static data:

struct SwitchDefinition {
  char*                          pSwitchName;
  CPseudoCommand::SwitchValue_t  eValue;
};

static SwitchDefinition SwitchTable[] = {
  {"-new",    CPseudoCommand::SwNew},
  {"-list",   CPseudoCommand::SwList},
  {"-delete", CPseudoCommand::SwDelete},
  {"-byid",   CPseudoCommand::SwById},
  {"-byname", CPseudoCommand::SwByName}
};
static UInt_t nSwitches = sizeof(SwitchTable)/sizeof(SwitchDefinition);


// Functions for class CPseudoCommand

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                int nArgs, char* pArgs[])
//  Operation Type: 
//     evaluation
int CPseudoCommand::operator()(CTCLInterpreter& rInterp, 
			       CTCLResult& rResult, 
			       int nArgs, char* Args[])  
{
  // Called for top level processing of the pseudo
  // command.  See the module comment header for
  // recognized syntaxes.
  //  
  //  We recognize the form of the command and
  //  invoke either Create, List or Delete as appopriate.
  //
  // Formal Parameters:
  //     CTCLInterpreter& rInterp:
  //         Refers to the interpreter evaluating this cmd.
  //     CTCLResult& rResult:
  //          Refers to the result string to be returned from 
  //          this execution.
  //      int nArgs, char* pArgs[]
  //          Parameters passed to the command.
  //  Returns:
  //     TCL_OK         - if successful.
  //     TCL_ERROR - if not.
  //  The result string on exit (an out parameter) depends on the 
  //   subfunction invoked.
  //
  char** pArgs = Args;
  nArgs--;			// Command line arguments include the
  pArgs++;			// the command name.

  if(!nArgs) {
    Usage(rResult);
    return TCL_ERROR;
  }

  // Determine the subfunction and invoke it;

  switch(ParseSwitch(*pArgs)) {
  case SwNew:
    nArgs--;
    pArgs++;
  case SwNotSwitch:		// Create.
    return Create(rInterp, rResult, nArgs, pArgs);
  case SwList:			// List
    nArgs--;
    pArgs++;
    return List(rInterp, rResult, nArgs, pArgs);
  case SwDelete:		// Delete
    nArgs--;
    pArgs++;
    return Delete(rInterp, rResult, nArgs, pArgs);
  default:			// Inappropriate switch at this point.
    Usage(rResult);
    return TCL_ERROR;
  }
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Create(CTCLInterpreter& rInterp, CTCLResult& rResult, UInt_t nArgs, char* pArgs[])
//  Operation Type: 
//     SubFunction
Int_t 
CPseudoCommand::Create(CTCLInterpreter& rInterp, CTCLResult& 
		       rResult, UInt_t nArgs, char** pArgs)  
{
  // Creates the pseudo parameter.
  // See module header for the command syntax.
  // 
  //     Formal Parameters:
  //           CTCLInterpeter& rInterp:
  //                Interpreter running the command.
  //           CTCResult& rResult:
  //                Result string returned.
  //           UInt_t nArg:
  //                 No. parameters in command tail.
  //           char* pArgs[]:
  //                Pointer to tail arguments.
  // Returns:
  //    TCL_OK          - Success.
  //                      Name of the procedure created.  
  //                      This allows for interactive testing.
  //                          
  //    TCL_ERROR  - Failed.
  //                 Failure reason text.
  //
  
  // the command line should contain exactly 3 more parameters:
  //   Pseudo name, Depedent parameters, Procedure body.
  //

  if(nArgs != 3) {
    Usage(rResult);
    return TCL_ERROR;
  }

  char* pPseudoName = pArgs[0];	 // Pseudo name.
  CTCLList Parameters(&rInterp,
		      pArgs[1]); // List of names of dependent parameters.
  char* pBody       = pArgs[2];  // Script body.

  // The Parameter names are pulled out into a vector, from a properly
  // formatted TCL List:

  StringArray DependentParameters;
  Parameters.Split(DependentParameters);

  // Now ask our package to make the pseudo.

  CParameterPackage& rPack = (CParameterPackage&)getMyPackage();
  return rPack.AddPseudo(rResult, pPseudoName, DependentParameters, pBody);

}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     List(CTCLInterpreter& rInterp, CTCLResult& rResult,
//          UInt_t nArgs, char* pArgs[])
//  Operation Type: 
//     SubFunction
UInt_t CPseudoCommand::List(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			    UInt_t nArgs, char** pArgs)  
{
  // Lists the set of pseudos which have been
  // defined or a subset of the pseudos selected
  // on the command line.  Listing is in definition order
  //    BUGBUG  [later implement -byname -byid???].
  //
  // Formal Parameters:
  //    CTCLInterpreter& rInterp:
  //       Refers to the interpreter executing this command.
  //    CTCLResult&       rResult:
  //       Refers tothe reseult string.
  //    UInt_t nArgs:
  //       number of arguments in the command tail.
  //     char* pArgs[]:
  //        arguments in the command tail.
  // Returns:
  //    TCL_OK:      - If worked.  Each pseudo is an element of a properly 
  //                   formatted TCL list,  each Pseudo is itself a list 
  //                   containing:
  //                         pseudo name, dependent parameters (sublist), body.
  //     TCL_ERROR - If any pseudo could not be listed (only relevant if 
  //                 parameters are explicitly requested... then returns a 
  //                 list of failing parameters and the reasons.
  //
  
  // There are two possiblities... list all pseudos or list all which have
  // been requested.
  // in both cases we set up a loop over  a list
  // of pseudo parameter names.
  //

  CParameterPackage& rPack = (CParameterPackage&)getMyPackage();

  list<string> PseudoNames;
  char* pattern = "*";
  if(nArgs != 0) {		// List specific parameters.
   pattern = pArgs[0];
  }
  rPack.GetPseudoNames(PseudoNames);

  //
  // Iterate through all pseudos getting descriptions to add to the
  // output string and error string.
  //

  list<string>::iterator pName = PseudoNames.begin();
  CTCLString   oks;
  CTCLString   errors;
  UInt_t       nErrors = 0;
  while(pName != PseudoNames.end()) {
    string Description;
    const char* name = (*pName).c_str();
    if( Tcl_StringMatch(name, pattern))
      {
	if(rPack.DescribePseudo(*pName, Description) == TCL_OK) 
	  {
	    oks.AppendElement(Description);
	  }
	else 
	  {
	    errors.AppendElement(Description);
	    nErrors++;
	  }
      }
    pName++;
  }
  // Set the result string according to the error count, and return:

  rResult = (char*)(nErrors ? (const char*)errors : (const char*)oks);
  return (nErrors ? TCL_ERROR : TCL_OK);

}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Delete(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//            UInt_t nArgs, char** pArgs)
//  Operation Type: 
//     SubFunction
UInt_t CPseudoCommand::Delete(CTCLInterpreter& rInterp, CTCLResult& rResult,
			      UInt_t nArgs, char** pArgs)  
{
  // Deletes an existing pseudo parameter
  // definition script.
  //   
  // Formal Parameters:
  //    CTCLInterpreter& rInterp:
  //        Interpreter which is executing the cmd.
  //    CTCLResult&      rResult:
  //        Result string returned from the cmd.
  //    UInt_t nArgs:
  //        No. of args in command tail.
  //    char* pArgs[]:
  //        Argument strings in the command tail.
  // Returns:
  //     TCL_OK:          -  Parameter deleted, empty result.
  //     TCL_ERROR:  -  Deletion failed, result is textual reason.
  //
  
  if(nArgs <= 0) {		// Must have at least one item to delete.
    Usage(rResult);
    return TCL_ERROR;
  }
  CParameterPackage& rPack = (CParameterPackage&)getMyPackage();
  CTCLString errors;
  UInt_t     nErrors = 0;
  while(nArgs) {
    string Result;
    if(rPack.DeletePseudo(string(*pArgs), Result) != TCL_OK) {
      nErrors++;
      errors.AppendElement(Result);
    }

    nArgs--;
    pArgs++;
  }
  rResult = (const char*)errors;
  return (nErrors ? TCL_ERROR : TCL_OK);
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Usage(CTCLResult& rResult)
//  Operation Type: 
//     Factorization
//
void CPseudoCommand::Usage(CTCLResult& rResult)  
{
  // Appends to the input result string the command usage
  //
  //  Formal Parameters:
  //     CTCLResult& rResult:
  //          Result string.

  rResult += "Usage: \n";
  rResult += "   pseudo name { depemendent_params } { proc_body }\n";
  rResult += "   pseudo -list [pattern]\n";
  rResult += "   pseudo -delete name1 [name2 ... ]\n";
  rResult += "  name             - Name of a Pseudo parameter\n";
  rResult += "  dependent_params - names of parameters on which a pseudo\n";
  rResult += "                     depends\n";
  rResult += "  proc_body        - Tcl script body to return the value of\n";
  rResult += "                     pseudo computed in terms of\n";
  rResult += "                     dependent_params\n";
   
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ParseSwitch(const char* pSwitch)
//  Operation Type: 
//     Protected utility
CPseudoCommand::SwitchValue_t 
CPseudoCommand::ParseSwitch(const char* pSwitch)  
{
  // attempts to match a text string against
  // one of the possible switches.  The corresponding
  // enumerated const is returned or knNotSwitch
  // if there is no match
  //
  // Formal Parameters:
  //    const char* pSwitch
  //       Potential switch string.
  // Returns
  //    See comments.

  SwitchDefinition* pDef = SwitchTable;
  for(UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(pSwitch, pDef->pSwitchName) == 0) {
      return pDef->eValue;
    }
    pDef++;
  }
  return SwNotSwitch;
}


