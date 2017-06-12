/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008.

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

// Class: ChannelCommand
// parses the chan command.  The chan command
// has the following formats:
//
//  chan -get spname { indices }    
//      Returns the channel from spname selected
//      by the indices list.
//  chan -set spname { indices }  value
//      Sets the channel in spname selected
//      by indices to value.
//
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
// Modified:
//    Chase Bolen (added TCL evaluation of indices in EvalIndex(...)) 4/20/00
//    
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include <config.h>
#include "ChanCommand.h"    				
#include "SpectrumPackage.h"

#include <TCLList.h>
#include <TCLObject.h>
#include <TCLException.h>
#include <Exception.h>
#include <tcl.h>
#include <vector>
#include <string.h>

#include <errno.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// 

struct SwitchEntry {
  const char*                pText;
  ChannelCommand::Switches  eValue;
};

static SwitchEntry SwitchTable[] = {
  {"-set",     ChannelCommand::kSetSwitch} ,
  {"-get",     ChannelCommand::kGetSwitch}
};

static UInt_t nSwitches = sizeof(SwitchTable)/sizeof(SwitchEntry);


Int_t ChannelCommand::EvalIndex(CTCLInterpreter* pInterp, string& index) {
  Int_t result;
  try {
    result =  pInterp->ExprLong(index.c_str());  //get the long result of evaluating
  } //                                                   the expression
  catch (CTCLException *err) {
    CTCLException e(*pInterp, err->getReason(),
		    "attempting to evaluate 'channel' command indices.");
    throw e;
  }
  return result;
}


// Functions for class ChannelCommand

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[])
//  Operation Type: 
//     Command processor
int 
ChannelCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			   int nArgs, char* pArgs[])  
{
  // Processes the chan command.
  // The command options are parsed an
  // the Get or Set procedure is executed
  // accordingly.
  //
  // Formal Paramters:
  //    CTCLInterpreter&  rInterp:
  //        References the interpreter on which the command runs.
  //     CTCLResult&  rResult:
  //        References the resutl associated with rInterp.
  //      int nArgs:
  //        Number of command line parameters.
  //     char* pArgs[]:
  //        Array of pointers to command line parameters.
  // Returns:
  //     TCL_OK         - Success.
  //     TCL_ERROR - Failure.
  //

  nArgs--; 
  pArgs++;
  
  // There must be at least one more parameter...
  
  if(nArgs <= 0) {
    Usage(rResult);
    return TCL_ERROR;
  }
  // and it must be a switch:
  
  switch(ParseSwitch(*pArgs)) {
  case kSetSwitch:
    nArgs--;
    pArgs++;
    return Set(&rInterp, rResult, nArgs, pArgs);
  case kGetSwitch:
    nArgs--;
    pArgs++;
    return Get(&rInterp, rResult, nArgs, pArgs);
  default:
    Usage(rResult);
    return TCL_ERROR;
  }
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Get(CTCLInterpreter* pInterp, CTCLResult& rResult, UInt_t nArgs, const char* pArgs[])
//  Operation Type: 
//     interface.
UInt_t 
ChannelCommand::Get(CTCLInterpreter* pInterp, CTCLResult& rResult, 
		    UInt_t nArgs, char* pArgs[])  
{
  // Parses the chan -get command and initiates the get of a channel value.
  //
  // Formal Paramters:
  //     CTCLInterpreter* pInterp:
  //        Points to the interpreter which is running the command.
  //     CTCLResult& rResult:
  //        Refers to the result string.. if successful,this will be the
  //        textual equivalent of the channel value.
  //     UInt_t nArgs:
  //        # parameters in the argument tail.
  //     const char* pArgs[]:
  //         Array of parameters.
  // Returns:
  //     TCL_OK         - Success, result is the channel value.
  //     TCL_ERROR - Failure, result is failure reason.
  //
  
  if(nArgs != 2) {		// Must be spectrum name and channel indexes.
    Usage(rResult);
    return TCL_ERROR;
  }
  // The parameters are the spectrum name and a TCL formatted list of
  // channel indices.
  // Note that while it looks like just a bunch of assignments are going
  // on below, what's actually happening is that the assignemnt operator
  // of CTCLObject does conversions to the appropriate destination type.
  //
  string SpectrumName(*pArgs);
  nArgs--;
  pArgs++;
  CTCLList           ParamList(pInterp, *pArgs);
  //CTCLObject         IndexObject;
  StringArray        StringIndices;
  vector<UInt_t>     NumericIndices;
  Int_t              nIndex;

  //IndexObject.Bind(pInterp);
  ParamList.Split(StringIndices);
  for(UInt_t i = 0; i < StringIndices.size(); i++) {
    //IndexObject = StringIndices[i]; // String -> TCL Object.
    //nIndex      = IndexObject;      // TCL Object -> Integer.


    nIndex = EvalIndex(pInterp, StringIndices[i]); // perform string -> integer conversion
    //                     with ExprLong instead of a TCLObject.  will replace
    //                     variables etc. and do the conversion.

    if(nIndex < 0) {		   //  Array indices must be positive... 
      char MoreUsage[100];
      Usage(rResult);
      sprintf(MoreUsage, "\n Index value %d must be positive and isn't", 
	      nIndex);
      rResult += MoreUsage;
      return TCL_ERROR;
    }
    NumericIndices.push_back((UInt_t)nIndex);
  }
  // Now the parameters are marshalled. We need to ask our package to do 
  // the get for us.
  // 
  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();
  if(rPack.GetChannel(rResult, SpectrumName, NumericIndices)) 
    return TCL_OK;
  else
    return TCL_ERROR;
  rResult = "BUG - Report that: Control fell through the end of CChannelCommand::Get";
  return TCL_ERROR;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Set(CTCLInterpreter* pInterp, CTCLResult& rResult, UInt_t nArgs, char* pArgs[])
//  Operation Type: 
//     Interface.
UInt_t ChannelCommand::Set(CTCLInterpreter* pInterp, CTCLResult& rResult, UInt_t nArgs, char* pArgs[])  
{
  // Sets the value of a spectrum channel to a 
  // specific number.
  //
  // Formal Parameters:
  //      CTCLInterpreter* pInterp:
  //         Pointer to the interpreter running the command.
  //      CTCLResult&       rResult:
  //          Refers to the result string.
  //      UInt_t nArgs:
  //          Number of parameters in the command tail.
  //      const char* pArgs[]:
  //          Parameters in the command tail.
  // Returns:
  //     TCL_OK         - success, the result is the new channel value.
  //     TCL_ERROR - failure, the result is the reason for the failure.
  
  if(nArgs != 3) {		// Must be spectrum name channel, value.
    Usage(rResult);
    return TCL_ERROR;
  }
  // The parameters are the spectrum name and a TCL formatted list of
  // channel indices.

  string SpectrumName(*pArgs);
  pArgs++;
  CTCLList           ParamList(pInterp, *pArgs);
  pArgs++;

  StringArray        StringIndices;
  vector<UInt_t>     NumericIndices;
  Int_t              nIndex;

  const char* Value = *pArgs;	// String representation of the value to set.
  errno             = 0;
  UInt_t      nValue= strtoul(Value, NULL, 0);
  if ((nValue ==0 ) && (errno != 0)) {
    char message[100];
    sprintf(message,"%s is not a valid channel value", Value);
    rResult = message;
    return TCL_ERROR;
  }
  
  //IndexObject.Bind(pInterp);		// an index object.
  ParamList.Split(StringIndices);
  for(UInt_t i = 0; i < StringIndices.size(); i++) {
    //IndexObject = StringIndices[i]; // String -> TCL Object.
    //nIndex      = IndexObject;      // TCL Object -> Integer.
    
    nIndex = EvalIndex(pInterp, StringIndices[i]);  // perform string -> integer conversion
    //                     with ExprLong instead of a TCLObject.  will replace
    //                     variables etc. and do the conversion.
    
    if(nIndex < 0) {		   //  Array indices must be positive... 
      char MoreUsage[100];
      Usage(rResult);
      sprintf(MoreUsage, "\n Index value %d must be positive and isn't", 
	      nIndex);
      rResult += MoreUsage;
      return TCL_ERROR;
    }
    NumericIndices.push_back((UInt_t)nIndex);
  }
  // Now set the channel using the package's facility.

  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();
  if(rPack.SetChannel(rResult, SpectrumName, NumericIndices, 
		      (ULong_t)nValue)) {
    return TCL_OK;
  }
  else {
    return TCL_ERROR;
  }
  rResult = "BUG Report that: Control fell through to the end of CChannelCommand::Set";
  return TCL_ERROR;
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//    Switches ParseSwitch(const char* pSwitch)
// Operation Type:
//    Protected Utility.
//
ChannelCommand::Switches
ChannelCommand::ParseSwitch(const char* pSwitch)
{
  SwitchEntry* pSwitches = SwitchTable;
  for(UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(pSwitch, pSwitches->pText) == 0) 
      return pSwitches->eValue;
    pSwitches++;
  }
  return ChannelCommand::kNotSwitch;
  
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//    void Usage(CTCLResult& rResult)
// Operation type:
//    protected utility.
//
void
ChannelCommand::Usage(CTCLResult& rResult)
{
  rResult += "Usage:\n";
  rResult += "   channel -get spectrumname { indices }\n";
  rResult += "   channel -set spectrumname { indices } NewValue\n";

}
