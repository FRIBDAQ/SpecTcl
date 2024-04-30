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
static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";
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
#include <config.h>
#include "UngateCommand.h"    				
#include "GatePackage.h"

#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <TCLString.h>
#include <TCLResult.h>

#include <string>
#include <assert.h>
#include <string.h>
#include <vector>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


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
CUngateCommand::operator()(CTCLInterpreter& rInterpreter, std::vector<CTCLObject>& objv)  
{
  // Evaluates the ungate command.
  //
  // Formal Parameters:
  //     CTCLInterpreter& rInterpeter:
  //         The interpreter on which the command is executing
  //          The result string will be set via rInterpreter.setResult().  This willl contain either:
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

  // marshall stuff up into nArgs/pArgs to make the port lazy.,
  
  std::vector<std::string> words;
  std::vector<const char*> pWords;

  // do in two loops to sidestep the lifetime issues with std::vector::back().c_str()...

  for (auto& word : objv) {
      words.push_back(std::string(word));
  }
  auto nArgs = words.size();

  for (int i = 0; i < nArgs; i++) {
    pWords.push_back(words[i].c_str());
  }
  auto pArgs = pWords.data();

  nArgs--;
  pArgs++;
  std::string rResult;                           // Also port laziness.
  //
  // Need at least one spectrum name on the list:
  //
  if(nArgs < 1) {
    rResult = Usage();
    rResult += "\nAt least one spectrum name is required";
    rInterpreter.setResult(rResult);
    return TCL_ERROR;
  }
  //  Now ungate each spectrum...
 
  CGatePackage& Package(*(CGatePackage*)getPackage());
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
  rInterpreter.setResult(rResult);
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
