//  CStartRun.cpp
     // Provides a command to start analyzing an offline run
     // of data.  Note that the online sources start themselves 
     // automatically, and are always considered active.
     
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include "StartRun.h"
#include "TCLInterpreter.h"
#include "TCLResult.h"
#include "RunControlPackage.h"


static const char* Copyright = 
"CStartRun.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CStartRun

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterpreter, CTCLResult& rResult, int nArguments, char* pArguments[] )
//  Operation Type:
//     State Transition
//
int 
CStartRun::operator()(CTCLInterpreter& rInterpreter, CTCLResult& rResult, int nArguments, char* pArguments[]) 
{
// Initiates a start of run.  The RunState 
// variable is updated to indicate that the
// run is active.. Note that if the state of the
// run was initially active, an error is signalled
// with appropriate message text. Much of this is done
// by callling back to our package's members.
//
// Formal Parameters:
//     CTCLInterpreter& rInterpreter:
//          Interpreter on which the command is running.
//     CTCLResult& rResult:
//          An object representing the current interpreter's result
//          string.
//     int nArguments:
//          Number of command line arguments  (should be 1).
//     char* pArguments[]:
//           Array of parameters.  Note that pArguments[0] is the
//           command name.
// Exceptions:  
  CRunControlPackage& rMyPack((CRunControlPackage&) getMyPackage());

  if(nArguments != 1) {
    rResult = "Usage:\n    stop\n";
    return TCL_ERROR;
  }

  if( !rMyPack.isRunning()) {
    rMyPack.StartRun();
    return TCL_OK;
  }
  else {
    rResult = "Run is already active";
    return TCL_ERROR;
  }

}
