// Class: CGateCommand
// Implements the gate command.
// This command creates gates and enters them
// in the gate dictionary.
//    gate name type contents
//             type is one of {cut, band, contour, not, and, or, true, false}
//             contents is a list whose structure depends on the gate type:
//                  cut - { parameter { xlow xhigh}}
//                  band
//                  contour { {xpar ypar}  { {x1 y1} {x2 y2} {x3 y3}...}
//                  not     gatename
//                  and,
//                  or       {gate1 gate2 ...}
//  gate -list [-byid]
//       Lists gates in dictinoary
//  gate -delete [-id] gatelist
//       Replaces gate with deleted gate.
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

#include "GateCommand.h"    				
#include "GatePackage.h"
#include "GateFactory.h"

#include <Point.h>

#include <TCLList.h>
#include <TCLString.h>
#include <TCLResult.h>
#include <Exception.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <vector>
#include <string>

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved GateCommand.cpp \\n";

// Static/local definitions:
//

const struct SwitchList {
  char* pName;
  CGateCommand::Switches eValue;
} SwitchTable[] = {
  { "-new"   , CGateCommand::newgate },
  { "-delete", CGateCommand::deletegate },
  { "-list"  , CGateCommand::listgates },
  { "-id"    , CGateCommand::id } ,
  { "-byid"  , CGateCommand::byid }
};
static const UInt_t nSwitches = sizeof(SwitchTable) / 
                                sizeof(SwitchList);


CGateCommand::GateFactoryTable GateTable[] = {
  { "*"     , CGateFactory::and,         kfTRUE,  0, kfFALSE},
  { "b"     , CGateFactory::band,        kfFALSE, 2, kfFALSE},
  { "c2band", CGateFactory::bandcontour, kfTRUE,  0, kfFALSE},
  { "c"     , CGateFactory::contour,     kfFALSE, 2, kfFALSE},
  { "s"     , CGateFactory::cut,         kfFALSE, 1, kfFALSE},
  { "-deleted-", CGateFactory::deleted,  kfTRUE,  0, kfFALSE},
  { "F",      CGateFactory::falseg,       kfTRUE,  0, kfFALSE},
  { "T",      CGateFactory::trueg,        kfTRUE,  0, kfFALSE},
  { "-",      CGateFactory::not,         kfTRUE,  0, kfFALSE},
  { "+",      CGateFactory::or,          kfTRUE,  0, kfFALSE},
  { "gs",     CGateFactory::gammacut,     kfFALSE,  0, kfTRUE},
  { "gb",     CGateFactory::gammaband,    kfFALSE,  0, kfTRUE},
  { "gc",     CGateFactory::gammacontour, kfFALSE,  0, kfTRUE}
};
static const UInt_t nGateTypes =
                      sizeof(GateTable)/sizeof(CGateCommand::GateFactoryTable);

static const  char* pUsage[] = {
  "Usage:\n"
  "     gate [-new] name type { description }\n",
  "     gate -delete [-id] Gate1 [Gate2 ... }\n",
  "     gate -list [-byid]\n"
};
static const UInt_t nUsageLines = (sizeof(pUsage) / sizeof(char*));

// Functions for class CGateCommand

//////////////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//          operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                     int nArgs, char* pArgs[])
//  Operation Type: 
//          evaulation
//
int 
CGateCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			 int nArgs, char* pArgs[])  
{
  // Called to evaluate the "gate" command.
  // This command creates, lists or deletes a gate.
  //
  // Formal Parameters:
  //    CTCLInterpreter& rInterp:
  //        Refers to the interpreter which is invoking us.
  //     CTCLResult& rResult:
  //         Result string of operation...name of gate on success.
  //     int nArgs:
  //        Number of command line arguments.
  //     char* pArgs[]
  //        Points to the array of parameter strings.
  
  nArgs--; 
  pArgs++;			// Skip the command name.
  if(nArgs  < 1) {		// Must be at least one parameter:
    rResult = Usage();
    return TCL_ERROR;
  }
  // Now decode the switch and figure out what we got:

  switch(MatchSwitches(*pArgs)) {
  case newgate:
    nArgs--;
    pArgs++;
  case notswitch:		// Make a new gate:
    return NewGate(rInterp, rResult, nArgs, pArgs);
  case deletegate:		// Delete gates:
    nArgs--;
    pArgs++;
    return DeleteGates(rInterp, rResult, nArgs, pArgs);
  case listgates:
    nArgs--;
    pArgs++;
    return ListGates(rInterp, rResult, nArgs, pArgs);
  default:
    rResult = Usage();
    return TCL_ERROR;		// Bad switch in context.
  }
  assert(0);			// Should not pass control to here.
}
/////////////////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//       NewGate(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//               UInt_t nArgs, char* pArgs[])
//  Operation Type: 
//      Subfuction
//
Int_t 
CGateCommand::NewGate(CTCLInterpreter& rInterp, 
			    CTCLResult& rResult, UInt_t nArgs, char* pArgs[])  
{
  // Performs the  gate -new subcommand.
  // This command creates a new gate by invoking the
  //  gate factory class to create an appropriate gate.
  //  The gate is then added by CGatePackage::AddGate()
  // 
  // Formal Parameters:
  //      CTCLInterpreter& rInterp:
  //            Reference to the interpreter running us.
  //    CTCLResult& rResult:
  //            Reference to the result string for this command.
  //            on success, this is the name of the created gate, else
  //           the failure reason.
  //  int nArgs, char* pArgs[]
  //      Pointer to the remaining command parameters 
  //      (pArgs[0] should be the gate name).
  //
  
  if(nArgs != 3) {		// Must be exactly 3 parameters....
    rResult = Usage();
    return TCL_ERROR;
  }
 // Need to figure out how to parse the gate description... the first two items
  // are fixed, name and type, the last is a list with gate type 
  // dependent contents.
  //

  const char* pName = *pArgs;
  pArgs++;
  const char* pType = *pArgs;
  pArgs++;
  const char* pList = *pArgs;

  // The gate table is used to drive the rest of the parse:

  CGate*  pGate;
  CGatePackage& rPackage((CGatePackage&)getMyPackage());
  CGateFactory Factory(rPackage.getHistogrammer());

  GateFactoryTable* pItem = MatchGateType(pType);
  if(!pItem) {
    rResult = Usage();
    rResult += "\n  Invalid gate type";
    return TCL_ERROR;
  }
  GateFactoryTable& Item(*pItem);

  if(Item.fGateList) {		// The list is just a list of gates.
    CTCLList GateList(&rInterp, pList);
    StringArray Gates;
    if(GateList.Split(Gates) != TCL_OK) {
      rResult =  Usage();
      rResult += "List of gates had incorrect format\n";
      return TCL_ERROR;
    }
    try {
      pGate = Factory.CreateGate(Item.eGateType,
				 Gates);
    }
    catch(CException& rExcept) {
      rResult = Usage();
      rResult += rExcept.ReasonText();
      return TCL_ERROR;
    }
  }

  else if(!Item.fNoParams) {	// List is parameters followed by point list.
    CTCLList List(&rInterp, pList);
    StringArray Description;
    List.Split(Description);	// Bust the list apart.
    if(Description.size() != (Item.nParameters+1)) {
      rResult = Usage();
      rResult += "Incorrect description list format";
      return TCL_ERROR;
    }

    vector<string> Parameters;
    // Pull out the parameters
    for(UInt_t nPar = 0; nPar < Item.nParameters; nPar++) {
      Parameters.push_back(Description[nPar]);
    }
    // The last element of the description is a point list:
    CTCLList Points(&rInterp, Description[Item.nParameters]);
    vector <string> PointString;
    Points.Split(PointString);	// Point string contains textualized points.
    vector<CPoint> PointValues;	// Filled in below.
    
    // If the gate is 1-d, then each point just contains an X-coordinate.

    if(Item.nParameters == 1) {
      for(UInt_t nPoint = 0; nPoint < PointString.size(); nPoint++) {
	Int_t x;
	if(sscanf(PointString[nPoint].c_str(), "%d", &x) == 0) {
	  rResult = Usage();
	  rResult += "\nInvalid point string in description";
	  rResult += PointString[nPoint];
	  return TCL_ERROR;
	}
	PointValues.push_back(CPoint(x,0));
      }
    }
    else {
      // Otherwise a point is a list containing x,y...
      for(UInt_t nPoint = 0; nPoint < PointString.size(); nPoint++) {
	Int_t x,y;
	CTCLList Point(&rInterp, PointString[nPoint]);
	vector<string> coords;
	Point.Split(coords);
	if(coords.size() != 2) {
	  rResult = Usage();
	  rResult += "\nInvalid point string in description  ";
	  rResult += Point.getList();
	  return TCL_ERROR;
	}
	UInt_t s1 = sscanf(coords[0].c_str(), "%d", &x);
	UInt_t s2 = sscanf(coords[1].c_str(), "%d", &y);
	
	if((s1 != 1)  || (s2 != 1) ) {
	  rResult = Usage();
	  rResult += "\nInvalid point string in description  ";
	  rResult += Point.getList();
	  return TCL_ERROR;
	}
	CPoint pt(x,y);
	PointValues.push_back(pt);
      }
    }
    try {
      pGate = Factory.CreateGate(Item.eGateType, Parameters, PointValues);
    }
    catch(CException& rExcept) {
      rResult = Usage();
      rResult += rExcept.ReasonText();
      return TCL_ERROR;
    }
  }
  else {        // List is just a point list, possibly followed by spec list
    CTCLList List(&rInterp, pList);
    StringArray Description;
    List.Split(Description);

    vector<CPoint> PointValues;	// Filled in below
    vector<string> SpecValues;  // Filled in further below
    UInt_t nPoint = 0;
    if(Item.eGateType == CGateFactory::gammacut) {
      Int_t x1, x2;
      Int_t i = sscanf(Description[nPoint].c_str(), "%d %d", &x1, &x2);
      if(i != 2) {
	rResult = Usage();
	rResult += "\nInvalid point string in description";
	rResult += Description[nPoint];
	return TCL_ERROR;
      }
      PointValues.push_back(CPoint(x1,0));
      PointValues.push_back(CPoint(x2,0));
      if(PointValues.size() != 2) {
	rResult = Usage();
	rResult += "\nInvalid point string in description";
	rResult += Description[nPoint];
	return TCL_ERROR;
      }

      if(Description.size() == 2) {
	CTCLList Specs(&rInterp, Description[1]);
	vector<string> SpecString;
	Specs.Split(SpecString);
	for(UInt_t k = 0; k < SpecString.size(); k++) {
	  SpecValues.push_back(SpecString[k]);
	}
      }
    }
    
    else {     // Otherwise a point is a list containing x,y...
      CTCLList Points(&rInterp, Description[nPoint]);
      vector<string> PointString;
      Points.Split(PointString);
      for(UInt_t i = 0; i < PointString.size(); i++) {
	CTCLList Point(&rInterp, PointString[i]);
	vector<string> coords;
	Point.Split(coords);
	if(coords.size() != 2) {
	  rResult = Usage();
	  rResult += "\nInvalid point string in description  ";
	  rResult += Point.getList();
	  return TCL_ERROR;
	}
	Int_t x,y;
	UInt_t s1 = sscanf(coords[0].c_str(), "%d", &x);
	UInt_t s2 = sscanf(coords[1].c_str(), "%d", &y);
	if(s1 != 1 || s2 != 1) {
	  rResult = Usage();
	  rResult += "\nInvalid point string in description  ";
	  rResult += Point.getList();
	  return TCL_ERROR;
	}
	CPoint pt(x, y);
	PointValues.push_back(pt);
      }
      
      if(Description.size() == 2) { // means there are spectrum listed, too
	CTCLList Specs(&rInterp, Description[1]);
	vector<string> SpecString;
	Specs.Split(SpecString);
	for(UInt_t k = 0; k < SpecString.size(); k++) {
	  SpecValues.push_back(SpecString[k]);
	}
      }
    }
    
    try {
      pGate = Factory.CreateGate(Item.eGateType, PointValues, SpecValues);
    }
    catch(CException& rExcept) {
      rResult = Usage();
      rResult += rExcept.ReasonText();
      return TCL_ERROR;
    }
  }
  
  // Now try to enter the gate in the dictionary:
  
  if(rPackage.AddGate(rResult, string(pName), pGate)) {
    return TCL_OK;
  }
  else {
    return TCL_ERROR;
  }
  assert(0);
}
////////////////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//      ListGates(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                UInt_t  nArgs, char* pArgs[])
//  Operation Type: 
//      SubFunction
//
Int_t CGateCommand::ListGates(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			      UInt_t  nArgs, char* pArgs[])  
{
  // Returns a textually formatted list of gates
  // in the gate dictionary.
  //
  // Formal Parameters:
  //     CTCLInterpreter& rInterp:
  //          Refers to the interpreter.
  //    CTCLResult& rResult
  //        Refers to the result string.
  //   int nArgs, char* pArgs[]:
  //       Remaining command line arguments (should  either be empty or
  //       -byid switch).
  //
  CTCLString ResultString;
  CGatePackage& Package((CGatePackage&)getMyPackage());


  switch(nArgs) {
  case 0:
    ResultString = Package.ListGates();
    break;
  case 1:
    if(MatchSwitches(*pArgs) != byid) {
      rResult = Usage();
      rResult += "\n The following switch may only be -byid: ";
      rResult += *pArgs;
      return TCL_ERROR;
    }
    ResultString = Package.ListGatesById();
    break;
  default:
    rResult = Usage();
    rResult += "\nIncorrect number of parameters";
    return TCL_ERROR;
  }
  rResult = (const char*)(ResultString);
  return TCL_OK;
  
}
/////////////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//       DeleteGates(CTCLInterpreter& rInterp, CTCLResult& rRestul, 
//                   UInt_t nArgs, char* pArgs[])
//  Operation Type: 
//       Subfunction
//
Int_t 
CGateCommand::DeleteGates(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			  UInt_t nArgs, char* pArgs[])  
{
  // Deletes a gate or a set of gates.
  //
  // Formal Parameters:
  //    CTCLInterpreter& rInterp, CTCLResult& rResult:
  //           TCL interpreter junk.
  //   UInt_t nArgs:
  //         Count of the parameter tail.
  //    char* pArgs[]:
  //         Pointer to the command tail.
  //         Either points to a list of gates, or -id followed by a list of gates.
  //
  
  if(nArgs == 0) {		// Not allowed.
    rResult = Usage();
    rResult += "\nMust at least be a gate to delete.";
    return TCL_ERROR;
  }
  // What we do depends on whether or not the next item is a -id switch:

  CGatePackage& Package((CGatePackage&)getMyPackage());
  if(MatchSwitches(*pArgs) == id) { // Remaining list is a set of ids...
    vector<UInt_t> Ids;
    Bool_t ConvertFailed = kfFALSE;
    for(UInt_t i = 0; i < nArgs; i++) {
      Int_t n;
      if(sscanf(*pArgs, "%d", &n) != 1) {
	CTCLString error;
	error.StartSublist();
	error.AppendElement(*pArgs);
	error.AppendElement("Invalid Gate Id string");
	error.EndSublist();
	rResult += (const char*)error;
	ConvertFailed = kfTRUE;
      }
      else if(n < 0) {
	CTCLString error;
	error.StartSublist();
	error.AppendElement(*pArgs);
	error.AppendElement(" Gate ID cannot be negative");
	error.EndSublist();
	rResult += (const char*)error;
	ConvertFailed = kfTRUE;
      }
      else {			// n  is a good gate id:
	Ids.push_back((UInt_t)n);
      }
      pArgs++;
    }
    if(Package.DeleteGates(rResult, Ids) && (!ConvertFailed)) {
      return TCL_OK;
    }
    else {
      return TCL_ERROR;
    }
  }
  else {			// List is all gate names.
    vector<string> Names;
    for(UInt_t i = 0; i < nArgs; i++) {
      Names.push_back(*pArgs); // All strings are legal names.
      pArgs++;
    }
    if(Package.DeleteGates(rResult, Names)) {
      return TCL_OK;
    }
    else {
      return TCL_ERROR;
    }
  }
  assert(0);
}
//////////////////////////////////////////////////////////////////////////////////
//
// Function:
//    Switches MatchSwitches(char* pKey)
// Operation type:
//    Protected utility.
//
CGateCommand::Switches
CGateCommand::MatchSwitches(char* pKey)
{
  // attempts to match the switch text pointed to by pKey with
  // a valid gate command switch.
  //   Implicit input:
  //        SwitchTable  - Lookup table.
  //        nSwitches    - Number of entries in the table..

  for(UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(pKey, SwitchTable[i].pName) == 0) {
      return SwitchTable[i].eValue;
    }
  }
  return notswitch;
}
//////////////////////////////////////////////////////////////////////////////////////
//
// Function:
//    string   Usage()
// Operation Type:
//    Protected utility.
//
string
CGateCommand::Usage()
{
  // Provides a string which contains Usage information for the
  // gate command.  
  // Implicit inputs:
  //    pUsage:
  //       Pointer to a series of character strings which 
  //       contain the useage info.
  //    nUsageLines:
  //       Number of strings pointed to by pUsage.
  //

  string Use;
  const char**  p = pUsage;
  for(UInt_t i = 0; i < nUsageLines; i++) {
    Use += *p++;
  }
  return Use;
}
//////////////////////////////////////////////////////////////////////////
// Function:
//    GateFactoryTable* MatchGateType(const char* pGateType)
// Operation type:
//    Protected static utility function:
//
CGateCommand::GateFactoryTable*
CGateCommand::MatchGateType(const char* pGateType)
{
  // Returns a pointer to the parse description of a gate type
  // The GateTable is searcched for an entry which matches
  // the pGateType and a pointer to that entry is returned.
  // Note that if no match is found, an null pointer is returned.
  //
  // Formal parameters:
  //     const char* pGateType:
  //           Pointer to the type of gate to match.
  // Implicit Inputs:
  //     GateTable - Table of gate descriptions.
  //     ngateTypes- Number of entries in GateTable.
  // Returns:
  //     Pointer to the matching entry, or kpNULL
  //

  CGateCommand::GateFactoryTable* p = GateTable;
  for(UInt_t i = 0; i < nGateTypes; i++,p++) {
    if(strcmp(pGateType, p->pGateType) == 0) {
      return p;
    }
  }

  return (GateFactoryTable*)kpNULL;
}
