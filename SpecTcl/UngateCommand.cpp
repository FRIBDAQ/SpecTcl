// Class: CUngateCommand
// Removes the gating condition from a spectrum.
// The gate on a spectrum is replaced with a gate which
// is always true.
//   The form of the command is:
//        ungate spec1 [spec2 ...]
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
#include "UngateCommand.h"    				
#include "GatePackage.h"

#include <TCLInterpreter.h>
#include <TCLString.h>
#include <TCLResult.h>

#include <string>
#include <assert.h>
#include <string.h>
#include <vector>

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved UnGateCommand.cpp \\n";

// Functions for class CUngateCommand
//////////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//         int operator()(CTCLInterpreter& rInterpreter, CTCLResult& rResult, 
//                        int nArgs, char* pArgs[])
//  Operation Type: 
//         Evaluation operator.
//
int 
CUngateCommand::operator()(CTCLInterpreter& rInterpreter, CTCLResult& rResult,
			   int nArgs, char* pArgs[])  
{
  // Evaluates the ungate command.
  //
  // Formal Parameters:
  //     CTCLInterpreter& rInterpeter:
  //         The interpreter on which the command is executing
  //     CTCLResult& rResult:
  //          The result string.  This willl contain either:
  //           Success:
  //                 empty.
  //           Failure:
  //                 List of 2 element sublists.
  //                 each sublist contains: {Spectrum_name FailureReason}
  //                 for each failed removal.. note that all spectra not in the
  //                 failure list will have been ungated.
  //    int nArgs, char* pArgs[]:
  //           Parameter list of the command.
  //
  
  nArgs--;
  pArgs++;
  //
  // Need at least one spectrum name on the list:
  //
  if(nArgs < 1) {
    rResult = Usage();
    rResult += "\nAt least one spectrum name is required";
    return TCL_ERROR;
  }
  //  Now ungate each spectrum...
 
  CGatePackage& Package((CGatePackage&)getMyPackage());
  Bool_t AnyFailed = kfFALSE;
  CTCLString Errors;
  do {
    CTCLString Failure;
    if(!Package.Ungate(Failure, string(*pArgs))) {
      AnyFailed = kfTRUE;
      Errors.StartSublist();
      Errors.AppendElement(*pArgs);
      Errors.AppendElement(Failure);
      Errors.EndSublist();
    }

    nArgs--;
    pArgs++;
  } while(nArgs > 0);
  
  // Return the proper error information:

  rResult = (const char*)Errors;
  return AnyFailed ? TCL_ERROR : TCL_OK;
}
////////////////////////////////////////////////////////////////////////////////
//
// Function:
//    string Usage()
// Operation Type:
//    static, protected utility
//
string
CUngateCommand::Usage()
{
  string Use;
  Use += "Usage:\n";
  Use += "   ungate spectrum1 [spectrum2 ...] \n";
  return Use;
}
