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

static TCLPLUS::UInt_t nSwitches = sizeof(SwitchTable)/sizeof(SwitchEntry);


TCLPLUS::Int_t ChannelCommand::EvalIndex(CTCLInterpreter* pInterp, string& index) {
  TCLPLUS::Int_t result;
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
//     operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv)
//  Operation Type: 
//     Command processor
int 
ChannelCommand::operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv)  
{
  // Processes the chan command.
  // The command options are parsed an
  // the Get or Set procedure is executed
  // accordingly.
  //
  // Formal Paramters:
  //    CTCLInterpreter&  rInterp:
  //        References the interpreter on which the command runs.
  //    std::vector<CTCLObject>& objv
  //       Object encapsulated command words.
  // Returns:
  //     TCL_OK         - Success.
  //     TCL_ERROR - Failure.
  //

  // To make the port simple; reconstruct nArgs, pArgs:

  std::vector<std::string> words;
  std::vector<const char*> pWords;
  for (auto& word: objv) {
    words.push_back(std::string(word));
  }
  for (auto& word : words) {
    pWords.push_back(word.c_str());
  }
  int nArgs = words.size();
  auto pArgs = pWords.data();

  nArgs--; 
  pArgs++;
  
  // There must be at least one more parameter...
  
  if(nArgs <= 0) {
    Usage(rInterp);
    return TCL_ERROR;
  }
  // and it must be a switch:
  
  switch(ParseSwitch(*pArgs)) {
  case kSetSwitch:
    nArgs--;
    pArgs++;
    return Set(&rInterp, nArgs, pArgs);
  case kGetSwitch:
    nArgs--;
    pArgs++;
    return Get(&rInterp, nArgs, pArgs);
  default:
    Usage(rInterp);
    return TCL_ERROR;
  }
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Get(CTCLInterpreter* pInterp,  UInt_t nArgs, const char* pArgs[])
//  Operation Type: 
//     interface.
TCLPLUS::UInt_t 
ChannelCommand::Get(CTCLInterpreter* pInterp, TCLPLUS::UInt_t nArgs, const char* pArgs[])  
{
  // Parses the chan -get command and initiates the get of a channel value.
  //
  // Formal Paramters:
  //     CTCLInterpreter* pInterp:
  //        Points to the interpreter which is running the command.
  //     UInt_t nArgs:
  //        # parameters in the argument tail.
  //     const char* pArgs[]:
  //         Array of parameters.
  // Returns:
  //     TCL_OK         - Success, result is the channel value.
  //     TCL_ERROR - Failure, result is failure reason.
  //
  
  if(nArgs != 2) {		// Must be spectrum name and channel indexes.
    Usage(*pInterp);
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
  StringArray        StringIndices;
  vector<TCLPLUS::UInt_t>     NumericIndices;
  TCLPLUS::Int_t              nIndex;
  std::string rResult;
  //IndexObject.Bind(pInterp);

  ParamList.Split(StringIndices);
  for(TCLPLUS::UInt_t i = 0; i < StringIndices.size(); i++) {
    
    nIndex = EvalIndex(pInterp, StringIndices[i]); // perform string -> integer conversion
    
    if(nIndex < 0) {		   //  Array indices must be positive... 
      char MoreUsage[100];
      Usage(*pInterp);
      rResult = pInterp->GetResultString();
      sprintf(MoreUsage, "\n Index value %d must be positive and isn't", 
	      nIndex);
      rResult += MoreUsage;
      pInterp->setResult(rResult);
      return TCL_ERROR;
    }
    NumericIndices.push_back(static_cast<TCLPLUS::UInt_t>(nIndex));
  }
  // Now the parameters are marshalled. We need to ask our package to do 
  // the get for us.
  // 
  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  if(rPack.GetChannel(*pInterp, SpectrumName, NumericIndices)) 
    return TCL_OK;
  else
    return TCL_ERROR;
  pInterp->setResult("BUG - Report that: Control fell through the end of CChannelCommand::Get");
  return TCL_ERROR;
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     Set(CTCLInterpreter* pInterp, UInt_t nArgs, char* pArgs[])
//  Operation Type: 
//     Interface.
TCLPLUS::UInt_t ChannelCommand::Set(CTCLInterpreter* pInterp, TCLPLUS::UInt_t nArgs, const char* pArgs[])  
{
  // Sets the value of a spectrum channel to a 
  // specific number.
  //
  // Formal Parameters:
  //      CTCLInterpreter* pInterp:
  //         Pointer to the interpreter running the command.
  //      UInt_t nArgs:
  //          Number of parameters in the command tail.
  //      const char* pArgs[]:
  //          Parameters in the command tail.
  // Returns:
  //     TCL_OK         - success, the result is the new channel value.
  //     TCL_ERROR - failure, the result is the reason for the failure.
  
  if(nArgs != 3) {		// Must be spectrum name channel, value.
    Usage(*pInterp);
    return TCL_ERROR;
  }
  // The parameters are the spectrum name and a TCL formatted list of
  // channel indices.

  string SpectrumName(*pArgs);
  pArgs++;
  CTCLList           ParamList(pInterp, *pArgs);
  pArgs++;

  StringArray        StringIndices;
  vector<TCLPLUS::UInt_t>     NumericIndices;
  TCLPLUS::Int_t              nIndex;
  std::string    rResult;
  const char* Value = *pArgs;	// String representation of the value to set.
  errno             = 0;
  TCLPLUS::UInt_t      nValue= strtoul(Value, NULL, 0);
  if ((nValue ==0 ) && (errno != 0)) {
    char message[100];
    sprintf(message,"%s is not a valid channel value", Value);
    pInterp->setResult(message);
    return TCL_ERROR;
  }
  
  //IndexObject.Bind(pInterp);		// an index object.
  ParamList.Split(StringIndices);
  for(TCLPLUS::UInt_t i = 0; i < StringIndices.size(); i++) {
    //IndexObject = StringIndices[i]; // String -> TCL Object.
    //nIndex      = IndexObject;      // TCL Object -> Integer.
    
    nIndex = EvalIndex(pInterp, StringIndices[i]);  // perform string -> integer conversion
    //                     with ExprLong instead of a TCLObject.  will replace
    //                     variables etc. and do the conversion.
    
    if(nIndex < 0) {		   //  Array indices must be positive... 
      char MoreUsage[100];
      Usage(*pInterp);
      rResult =  pInterp->GetResultString();
      sprintf(MoreUsage, "\n Index value %d must be positive and isn't", 
	      nIndex);
      rResult += MoreUsage;
      pInterp->setResult(rResult);
      return TCL_ERROR;
    }
    NumericIndices.push_back(static_cast<TCLPLUS::UInt_t>(nIndex));
  }
  // Now set the channel using the package's facility.

  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  if(rPack.SetChannel(*pInterp, SpectrumName, NumericIndices, 
		      static_cast<TCLPLUS::ULong_t>(nValue))) {
    return TCL_OK;
  }
  else {
    return TCL_ERROR;
  }
  pInterp->setResult("BUG Report that: Control fell through to the end of CChannelCommand::Set");
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
  for(TCLPLUS::UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(pSwitch, pSwitches->pText) == 0) 
      return pSwitches->eValue;
    pSwitches++;
  }
  return ChannelCommand::kNotSwitch;
  
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//    void Usage(CTCLInterpreter& rInterp)
// Operation type:
//    protected utility.
//
void
ChannelCommand::Usage(CTCLInterpreter& rInterp)
{
  std::string rResult;
  rResult += "Usage:\n";
  rResult += "   channel -get spectrumname { indices }\n";
  rResult += "   channel -set spectrumname { indices } NewValue\n";
  rInterp.setResult(rResult);
}
