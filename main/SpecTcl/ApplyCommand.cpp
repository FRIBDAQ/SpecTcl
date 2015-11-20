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

// Class: CApplyCommand
// Implements the apply command.
// apply applies a single gate to a set of spectra.
// The syntax of this command is:
//    apply gatename spectrum1 [spectrum2 ...]
//        to apply a gate to several spectra or:
//    apply -list  spectrum [spectrum2...]
//        to list the gate applied on each of the selected
//       spectra.
//  Note that spectra can only have a single gate applied.
//  Applying a gate to a spectrum which is gated, replaces that
//  spectrum's gate... Since gates can be arbitrarily complex entities,
//  in practice, this is not a problem.
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
#include "ApplyCommand.h"    				
#include "GatePackage.h"


#include <TCLInterpreter.h>
#include <TCLString.h>
#include <TCLResult.h>

#include <assert.h>
#include <vector>
#include <string.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


static const  char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved ApplyCommand.cpp \\n";

// Functions for class CApplyCommand

///////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                int nParams, char* pParams[])
//  Operation Type: 
//      evaluation
//.
int CApplyCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			      int nParams, char* pParams[])  
{
  // Called to implement the apply command.
  // This command both applies gates and lists
  //  the applications.
  //
  // Formal Parameters:
  //       CTCLInterpreter& rInterp:
  //          Refers to the Tcl Interpreter which invoked us.
  //       CTCLResult& rResult:
  //           Refers to the result string.
  //      int nParam:
  //           Number of parameters which are in the command string.
  //      char* pArgs[]:
  //         pointer to array of null terminated parameter strings.
  // 
  nParams--;
  pParams++;
  //
  //  Ensure that we have at least one parameter:
  //
  if(nParams < 1) {
    rResult = Usage();
    rResult += "\n Insufficient parameters";
    return TCL_ERROR;
  }
  // Distinguish between a -list and application of gates:
  //
  if(strcmp(pParams[0], "-list") == 0) {// List gates.
    nParams--;
    pParams++;
    return ListApplications(rInterp, rResult, nParams, pParams);
  }
  else {			        // Apply gates to spectra.

    return ApplyGate(rInterp, rResult, nParams, pParams);
  }

}
////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//      ApplyGate(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                UInt_t nArgs, char*  pArgs[])
//  Operation Type: 
//      Subfunction.
//
Int_t CApplyCommand::ApplyGate(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			       UInt_t nArgs, char*  pArgs[])  
{
  // Processes the  apply gate spectrum_list
  // SpecTcl command. 
  //
  //  Formal Parameters:
  //        CTCLIntepreter& rInterp:
  //            The interpreter which is running the command.
  //        CTCLResult& rResult:
  //             The result string for the command/interpreter.
  //             This will be empty on success.  On failure, it will
  //             contain a list of 2 element sublists.  Each 
  //             sublist will contain
  //                  Name of a failing spectrum
  //                  Reason for failure.
  //        UInt_t nPars, char* pArgs[]:
  //             Number of parameters and the list of parameters in
  //             the command tail.  The first parameter should be a
  //             gate name.  Subsequent parameters should be names
  //             of spectra on which to apply the gate.
  // 
  // Returns:
  //    TCL_OK  - Success.
  //    TCL_ERROR - failure.
  //

  if(nArgs < 2) {		// Must have gate and at least one spectrum.
    rResult = Usage();
    rResult += "\n Unsufficient command parameters";
    return TCL_ERROR;
  }
  // Stringize the gate name and then cycle through the spectra.
 
  Bool_t AnyFailed = kfFALSE;
  CTCLString         ResultString;
  string GateName(*pArgs);
  CGatePackage& Package((CGatePackage&)getMyPackage());

  pArgs++;
  nArgs--;
  while(nArgs) {
    string Spectrum(*pArgs);
    CTCLString FailureReason;
    if(!Package.ApplyGate(FailureReason, GateName, Spectrum)) {
      ResultString.StartSublist();
      ResultString.AppendElement(FailureReason);
      ResultString.AppendElement(GateName);
      ResultString.AppendElement(Spectrum);
      AnyFailed = kfTRUE;
    }
    nArgs--;
    pArgs++;
  }
  if(AnyFailed) {
    rResult = (const char*)ResultString;
    return TCL_ERROR;
  }
  else {
    return TCL_OK;
  }
}
/////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ListApplications(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                      UInt_t nArgs, char* pArgs[])
//  Operation Type: 
//     Subfunction.
//
Int_t 
CApplyCommand::ListApplications(CTCLInterpreter& rInterp, CTCLResult& rResult, 
				UInt_t nArgs, char* pArgs[])  
{
  // Processes the apply -list command which 
  // lists gates applied to spectra.
  // the following forms are accepted:
  //     apply -list           
  //         lists all spectra and their gates.
  //     apply  -list [pattern]   
  //          lists gates applied specified pattern.
  // Formal Parameters:
  //     CTCLInterpreter& rInterp:
  //         Reference to an interpreter object which
  //         is running the command.
  //      CTCLResult& rResult:
  //         Reference to the result string of which
  //         contains the following:
  //         On Success:
  //             A list of 2 item sublists.  Each sublist
  //             contains:
  //                  { Spectrum_name  Applied_gate_name}
  //         On Failure:
  //             A list of 2 item sublists.  Each sublist contains:
  //                   { Spectrum_name WhyFailed}
  //             there will be one element for each failure.
  //       UInt_t nArgs, char* pArgs[]:
  //            remaining parameter list.   This is either
  //            empty (full list), or contains the set of spectra
  //            about which to inquire.
  // Returns:
  //     TCL_OK      - Success.
  //     TCL_ERROR - Failure.
  
  vector<string> Spectra;
  CGatePackage& Package((CGatePackage&)getMyPackage());

  // The only difference between no more parameters and parameters
  // is how the Spectra vector is filled:
  const char* pattern = "*";
  if (nArgs != 0) 
    {
      pattern = pArgs[0];
    }

  CHistogrammer* pHist = Package.getHistogrammer();
  SpectrumDictionaryIterator p = pHist->SpectrumBegin();
  while(p != pHist->SpectrumEnd()) {
    const char* name = (((*p).second)->getName()).c_str();
    if (Tcl_StringMatch(name, pattern))
    {
      Spectra.push_back((*p).second->getName());
    }
    p++;
    
  }
  // Now Spectra contains the names of the histograms we want application
  // information about, the rest is common code:

  vector<string>::iterator p2   = Spectra.begin();
  Bool_t            SomeFailed = kfFALSE;
  CTCLString        Failures, Successes;
  
  while(p2 != Spectra.end()) {
    CTCLString Result;
    if(Package.ListAppliedGate(Result, *p2)) { // List worked.
      Successes.StartSublist();
      Successes.AppendElement(*p2);
      Successes.AppendElement(Result);
      Successes.EndSublist();
      Successes.Append("\n");
    }
    else {
      SomeFailed = kfTRUE;
      Failures.StartSublist();
      Failures.AppendElement(*p2);
      Failures.AppendElement(Result);
      Failures.EndSublist();
      Failures.Append("\n");
    }
    p2++;
  }
  if(SomeFailed) {
    rResult = (const char*)Failures;
    return TCL_ERROR;
  }
  else {
    rResult = (const char*)Successes;
    return TCL_OK;
  }
  assert(0);
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//   Usage()
// Operation type:
//    protected utility (static).
//
string
CApplyCommand::Usage()
{
  string Use;
  Use +=   " Usage\n";
  Use +=   "   apply gate spectrum1 [spectrum2 ...]\n";
  Use +=   "   apply -list [pattern]\n";
  return Use;
}
