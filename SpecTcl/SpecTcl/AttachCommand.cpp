//  CAttachCommand.cpp
// Implements the 'attach' TCL command.  This command 
// Connects SpecTcl to various data sources.
// It has the following formats:
//
//   attach  -file   filename  [size]
//   attach  -tape   devicename
//   attach  -pipe   [-size nwords]  command_string    
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//  Change Log:
//      July 14, 1999  Ron Fox
//           Replace online support with pipe support.
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include "AttachCommand.h"                               
#include "TCLInterpreter.h"
#include "TCLCommandPackage.h"
#include "TCLResult.h"
#include "DataSourcePackage.h"

#include <histotypes.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static const char* Copyright = 
"AttachCommand.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Static data declarations:

struct SwitchDef {
  char*     pName;
  CAttachCommand::Switch_t  Value;
};

static const SwitchDef SwitchTable[] = {
  { "-file",    CAttachCommand::keFile },
  { "-tape",    CAttachCommand::keTape },
  { "-pipe",  CAttachCommand::kePipe },
  { "-size",  CAttachCommand::keBufferSize }
};

static const UInt_t nSwitches = sizeof(SwitchTable)/sizeof(SwitchDef);

// Functions for class CAttachCommand

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                     int nArgs, char* pArgs[] )
//  Operation Type:
//     Function operator.
//
int 
CAttachCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			   int nArgs, char* pArgs[]) 
{
// Parses and implements the attach TCL Command.
// 
// Formal Parameters:
//    CTCLInterpreter& rInterp:
//        Refers to the interpreter which is running this command.
//    CTCLResult& rResult:
//        Refers to the result string associated with the interpreter.
//    int nArgs, char* pArgs[]:
//        Argument list for the command parameters.

  nArgs--;
  pArgs++;

  // Require at least 2 additional parameters.

  if(nArgs < 2) {
    Usage(rResult);
    return TCL_ERROR;
  }

  // Determine what the first switch is:
  //

  char* pSwitch = pArgs[0];
  nArgs--;
  pArgs++;

  switch(ParseSwitch(pSwitch)) {
  case keFile:
    return AttachFile(rResult, nArgs, pArgs);

  case keTape:
    return AttachTape(rResult, nArgs, pArgs);

  case kePipe:
    return AttachPipe(rResult, nArgs, pArgs);

  case keNotSwitch:
    rResult  = "Invalid command Switch: ";
    rResult += pSwitch;
    rResult += "\n";
    Usage(rResult);
    return TCL_ERROR;
  }
  assert(0);			// Should not get here.

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int AttachFile ( CTCLResult& rResult, int nArgs, char* pArgs[] )
//  Operation Type:
//     sub function
//
int 
CAttachCommand::AttachFile(CTCLResult& rResult, int nArgs, char* pArgs[]) 
{
// Attaches a file as SpecTcl's data source.
//
// Formal Parameters:
//
//    CTCLResult& rResult
//        Refers to the command interpreter result string.
//    int nArgs:
//        Number of command line parameters.
//    char* pArgs[]:
//       Array of pointers to the command line parameters

  if(nArgs < 1) {
    Usage(rResult);
    return TCL_ERROR;
  }

  // This generates an attach and an open of the disk file.
  // the first additional parameter is the filename, and the second,
  // the blocksize.  If the blocksize is missing, then knDefaultBuffersize
  // is used.

  UInt_t nBlockSize = knDefaultBufferSize;
  char* pFilename = pArgs[0];
  nArgs--;
  pArgs++;
  if(nArgs) {
    Int_t nSize = atoi(pArgs[0]);
    nArgs--;
    pArgs++;
    if(nArgs) {			// Too many parametesr.
      Usage(rResult);
      return TCL_ERROR;
    }
    if(nSize > 0) 
      nBlockSize = (UInt_t)nSize;
    else {
      rResult = "Block size must be a postive integer.\n";
      Usage(rResult);
      return TCL_ERROR;
    }
  }

  // Now pFilename and nBlockSize are all set up for the open.
  // Try the attach, and if it works, then do the open:
  //
  CDataSourcePackage& rPack = (CDataSourcePackage&)getMyPackage();
  Int_t stat                = rPack.AttachFileSource(rResult);
  if(stat != TCL_OK) 
    return stat;
  
  return rPack.OpenSource(rResult, pFilename, nBlockSize);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int AttachTape ( CTCLResult& rResult, int nArgs, char* pArgs[] )
//  Operation Type:
//     subfunction
//
int 
CAttachCommand::AttachTape(CTCLResult& rResult, int nArgs, char* pArgs[]) 
{
//  Attaches a tape data source to SpecTcl.
//  Note that the attachment of tape data sources
//  enables the tape commands.
//
// Formal Parameters:
//     CTCLResult& rResult:
//        Tcl interpreter result string.
//     int nArgs, char* pArgs[]:
//        command line parameters.

  if(nArgs != 1) {
    Usage(rResult);
    return TCL_ERROR;
  }
  // This one is just an attach.  Opens are done using the tape -open 
  // command.

  CDataSourcePackage& rPack = (CDataSourcePackage&)getMyPackage();
  return rPack.AttachTapeSource(rResult, pArgs[0]);
  
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int AttachPipe ( CTCLResult& rResult, int nArgs, char* pArgs[] )
//  Operation Type:
//     Subfunction
//
int 
CAttachCommand::AttachPipe(CTCLResult& rResult, int nArgs, char* pArgs[]) 
{
//  Attaches a data source which comes through a pipe file.
//  These are programs which generate data on the fly and pipe
//  it to us.  For example, an online data source.
//  SpecTcl histogrammer.  
//
//   Formal Parameters:
//         CTCLResult& rResult:
//              Results of the attempts.
//         int nArgs, char* pArgs[]
//              Parameter list.
// Returns:
//     TCL_OK        - If success.
//     TCL_ERROR     - If Failure.
  
  std::string Command;

  if(nArgs < 1) {
    Usage(rResult);
    return TCL_ERROR;
  }

  // This maps into an attach and an open.   Information for the open
  // is taken as follows:
  //   If pArgs[0] is the switch -size then pArgs[1] is the blocksize, 
  //   and all remaining arguments are the command string.
  //   If not,all remaining parameters are the command string.

  UInt_t nBlockSize = knDefaultBufferSize;

  char* pSwitch = pArgs[0];
  switch(ParseSwitch(pSwitch)) {
  case keBufferSize:			// Size is provided.
    if(nArgs < 3) {		// Need at least 3 total parameters.
      Usage(rResult);
      return TCL_ERROR;
    }
    nBlockSize  = atoi(pArgs[1]);
    if(nBlockSize == 0) {	// illegal and atoi failed.
      Usage(rResult);
      return TCL_ERROR;
    }
    nArgs -= 2;			// Adjust remaining parameter count.
    pArgs += 2;
  
  case keNotSwitch:		// Remainder is command line.
    if(nArgs < 1) {
      Usage(rResult);
      return TCL_ERROR;
    }
    Command = ConcatenateParameters(nArgs, pArgs);
    break;
  default:			// Unexpected switch.
    Usage(rResult);
    return TCL_ERROR;
  }


  // Now we're ready to try the attach, and if that is successful, the
  // open.
  //
  CDataSourcePackage& rPack = (CDataSourcePackage&)getMyPackage();
  int stat                  = rPack.AttachPipeSource(rResult);
  if(stat != TCL_OK) 
    return stat;

  return rPack.OpenSource(rResult,Command.c_str() , nBlockSize);


}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     void     Usage(CTCLResult& rResult)
//  Operation Type:
//     Protected utility.
//
void
CAttachCommand::Usage(CTCLResult& rResult)
{
  rResult += "Usage:\n";
  rResult += "   attach -file   Filename [blocksize]\n";
  rResult += "   attach -tape   DeviceName\n";
  rResult += "   attach -pipe [-size nBytes] command string\n";
  rResult += "\n Attach attaches various data sources to SpecTcl\n";
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    Switch_t ParseSwitch(char* pSwitch)
//  Operation Type:
//    Protected Utility.
//
CAttachCommand::Switch_t
CAttachCommand::ParseSwitch(char* pSwitch)
{
  for(UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(SwitchTable[i].pName, pSwitch) == 0)
      return SwitchTable[i].Value;
  }
  return keNotSwitch;
}
