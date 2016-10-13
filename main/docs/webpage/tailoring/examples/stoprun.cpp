//  CStopRun.cpp
// Command processor to stop runs.  This is assumed
// to be a member of a CRunControlPackage. class.
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

#include "StopRun.h"
#include "TCLInterpreter.h"
#include "TCLResult.h"                               
#include "RunControlPackage.h"

static const char* Copyright = 
"CStopRun.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CStopRun

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterpreter,
//                      CTCLResult& rResult, 
//                      int nArguments, char* pArguments[] )
//  Operation Type:
//     Command Processor
//
int 
CStopRun::operator()(CTCLInterpreter& rInterpreter, 
		     CTCLResult& rResult, 
		     int nArguments, char* pArguments[]) 
{
// Called to process the stop command.
// If the run is already stopped this is an error.
// If the run is not stopped, then it is stopped and
// the RunState variable is updated.
// note that much of this is done via calls to the
// member functions of the CRunControlPackage
// class of which this object is assumed to be a member.
//
// Formal Parameters:
//     CTCLInterpreter& rInterpreter:
//         Refers to the command interpreter which is running.
//     CTCLResult&       rResult:
//         Refers to the interpreters result string.
//     int nArguments:
//        Number of command parameters (should be 1).
//     char* pArguments[]:
//        Array of command parameters.  Note that pArguments[0] is our
//       command name.

  CRunControlPackage& rMyPack((CRunControlPackage&) getMyPackage());

  if(nArguments != 1) {
    rResult = "Usage:\n    stop\n";
    return TCL_ERROR;
  }
  if( rMyPack.isRunning()) {
    rMyPack.StopRun();
    return TCL_OK;
  }
  else {
    rResult = "Run is already halted";
    return TCL_ERROR;
  }
  
}

