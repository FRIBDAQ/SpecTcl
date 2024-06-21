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

/*
  Change Log:
  $Log$
  Revision 5.4  2007/05/16 15:41:32  ron-fox
  Debugged gate tracing

  Revision 5.3  2005/09/22 12:40:37  ron-fox
  Port in the bitmask spectra

  Revision 5.2  2005/06/03 15:19:26  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.6  2005/05/27 17:47:38  ron-fox
  Re-do of Gamma gates also merged with Tim's prior changes with respect to
  glob patterns.  Gamma gates:
  - Now have true/false values and can therefore be applied to spectra or
    take part in compound gates.
  - Folds are added (fold command); and these perform the prior function
      of gamma gates.


  Revision 5.1.2.2  2005/03/15 17:28:52  ron-fox
  Add SpecTcl Application programming interface and make use of it
  in spots.

  Revision 5.1.2.1  2004/12/15 17:24:04  ron-fox
  - Port to gcc/g++ 3.x
  - Recast swrite/sread in terms of tcl[io]stream rather than
    the kludgy thing I had done of decoding the channel fd.
    This is both necessary due to g++ 3.x's runtime and
    nicer too!.

  Revision 5.1  2004/11/29 16:56:10  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.3.2.1  2004/10/22 19:19:40  ron-fox
   Force gamma gate creation to throw gate factory exceptions when the gate
   spectra don't have names of spectra that exist.

  Revision 4.3  2004/02/03 21:10:07  ron-fox
  Fix Bug 75 get -delete -byid gives error message.
  Forgot to bump argv and decrement argc after parsing -id keyword.
  In addition, both branches of the id decode returned failure indicators...
  even if the parse succeeded.

  Revision 4.2  2003/04/15 19:25:21  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs. Modifications support the input, listing and application information when gate coordinates are floating point.

*/

#include <config.h>
#include "GateCommand.h"    				
#include "GatePackage.h"
#include "GateFactory.h"
#include "GateFactoryException.h"


#include <Point.h>

#include <TCLList.h>
#include <TCLString.h>
#include <TCLInterpreter.h>
#include <Exception.h>
#include <SpecTcl.h>
#include <TCLObject.h>
#include <Histogrammer.h>
#include <SpecTcl.h>
#include <GateContainer.h>

#include <iostream>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

#include <TclPump.h>
#include <Globals.h>
#include <stdexcept>


static const  char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved GateCommand.cpp \\n";

// Local class:

class CMyGateObserver : public CGateObserver
{
private:
  CGateCommand*  m_pCommand;
public:
  CMyGateObserver(CGateCommand* command) :
    m_pCommand(command) {}
  virtual void onAdd(string name, CGateContainer& item) {
    m_pCommand->invokeAddScript(name);
  }
  virtual void onRemove(string name, CGateContainer& item) {
    m_pCommand->invokeDeleteScript(name);
  }
  virtual void onChange(string name, CGateContainer& item) {
    m_pCommand->invokeChangedScript(name);
  }
};




// Static/local definitions:
//

/*
   Maps between switches on the command line and the CGateCommand::Switches
   enum
*/
struct SwitchList {
  const char* pName;
  CGateCommand::Switches eValue;
} SwitchTable[] = {
  { "-new"   , CGateCommand::newgate },
  { "-delete", CGateCommand::deletegate },
  { "-list"  , CGateCommand::listgates },
  { "-id"    , CGateCommand::id } ,
  { "-byid"  , CGateCommand::byid },
  { "-trace" , CGateCommand::trace}
};

static const UInt_t nSwitches = sizeof(SwitchTable) / 
                                sizeof(SwitchList);

/*
   Mapping between gate type character strings and the type of gate
   to create according to the Gate Factory's enum.
*/
CGateCommand::GateFactoryTable GateTable[] = {
  { "*"     , CGateFactory::And,         kfTRUE,  0, kfFALSE},
  { "b"     , CGateFactory::band,        kfFALSE, 2, kfFALSE},
  { "c2band", CGateFactory::bandcontour, kfTRUE,  0, kfFALSE},
  { "c"     , CGateFactory::contour,     kfFALSE, 2, kfFALSE},
  { "s"     , CGateFactory::cut,         kfFALSE, 1, kfFALSE},
  { "-deleted-", CGateFactory::deleted,  kfTRUE,  0, kfFALSE},
  { "F",      CGateFactory::falseg,       kfTRUE,  0, kfFALSE},
  { "T",      CGateFactory::trueg,        kfTRUE,  0, kfFALSE},
  { "-",      CGateFactory::Not,         kfTRUE,  0, kfFALSE},
  { "+",      CGateFactory::Or,          kfTRUE,  0, kfFALSE},
  { "gs",     CGateFactory::gammacut,     kfFALSE,  0, kfTRUE},
  { "gb",     CGateFactory::gammaband,    kfFALSE,  0, kfTRUE},
  { "gc",     CGateFactory::gammacontour, kfFALSE,  0, kfTRUE},
  { "em",     CGateFactory::em,           kfFALSE,   1, kfFALSE},
  { "am",     CGateFactory::am,           kfFALSE,   1, kfFALSE},
  { "nm",     CGateFactory::nm,           kfFALSE,   1, kfFALSE}
};
static const UInt_t nGateTypes =
                      sizeof(GateTable)/sizeof(CGateCommand::GateFactoryTable);

static const  char* pUsage[] = {
  "Usage:\n"
  "     gate [-new] name type { description }\n",
  "     gate -delete [-id] Gate1 [Gate2 ... }\n",
  "     gate -list [-byid] [pattern]\n",
  "     gate -trace add|delete|change ?script?\n"
};
static const UInt_t nUsageLines = (sizeof(pUsage) / sizeof(char*));

// Functions for class CGateCommand

/*!
  Construct the object.
  \param pInterp   - Points to the interpreter object on which the command
                     will be registered
  The package will be registered by the package itself when the command is added.
*/
CGateCommand::CGateCommand(CTCLInterpreter*      pInterp) :
  CTCLPackagedObjectProcessor(*pInterp, "gate", true),
  m_pAddScript(0),
  m_pDeleteScript(0),
  m_pChangeScript(0),
  m_pObserver(0)
{
  m_pObserver = new CMyGateObserver(this);
  SpecTcl* pApi = SpecTcl::getInstance();
  pApi->addGateDictionaryObserver(m_pObserver);
#ifdef WITH_MPI
  m_id = Tcl_GetCurrentThread();
  if (isMpiApp()) {
    startTracePump();
  }
#endif  
}

/*!
  Destroy the item.. release all dynamic storage.
*/
CGateCommand::~CGateCommand()
{
  SpecTcl* pApi = SpecTcl::getInstance();
  pApi->removeGateDictionaryObserver(m_pObserver);
  delete m_pAddScript;
  delete m_pDeleteScript;
  delete m_pChangeScript;
  delete m_pObserver;

}
//////////////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//          operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                     int nArgs, char* pArgs[])
//  Operation Type: 
//          evaulation
//
int 
CGateCommand::operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv) {
  // Called to evaluate the "gate" command.
  // This command creates, lists or deletes a gate.
  //
  // Formal Parameters:
  //    CTCLInterpreter& rInterp:
  //        Refers to the interpreter which is invoking us.
  //    objv - the command words.
  //
  // Return int - TCL_OK on success, TCL_ERROR otherwise.
  //
  
  // Manufactor, nArgs, and pArgs from objv:

  TCLPLUS::Int_t nArgs = objv.size();
  std::vector<std::string> words;
  std::vector<const char*> pWords;


  // Due to lifetime issues with const char* have to
  // do it in two loops but there are usually few words so...
  for (auto& word: objv) {
    words.push_back(std::string(word));
  }

  for (int i =0; i < words.size(); i++) {
    pWords.push_back(words[i].c_str());
  }
  auto pArgs = pWords.data();
  
  nArgs--; 
  pArgs++;			// Skip the command name.
  if(nArgs  < 1) {		// Must be at least one parameter:
    rInterp.setResult(Usage());
    return TCL_ERROR;
  }
  // Now decode the switch and figure out what we got:

  switch(MatchSwitches(*pArgs)) {
  case newgate:
    nArgs--;
    pArgs++;
  case notswitch:		// Make a new gate:
    return NewGate(rInterp, nArgs, pArgs);
  case deletegate:		// Delete gates:
    nArgs--;
    pArgs++;
    return DeleteGates(rInterp, nArgs, pArgs);
  case listgates:
    nArgs--;
    pArgs++;
    return ListGates(rInterp, nArgs, pArgs);
  case trace:
    nArgs--;
    pArgs++;
    return traceGates(rInterp,nArgs, pArgs);
  default:
    rInterp.setResult(Usage());
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

/*!
  \para Functionality:
  Performs the  gate -new subcommand.
  This command creates a new gate by invoking the
  gate factory class to create an appropriate gate.
  The gate is then added by CGatePackage::AddGate()
   
  \para Formal Parameters:
  \param  <TT>rInterp (CTCLInterpreter& [in]):</TT>
              Reference to the interpreter running us.
              interp.setResult is used to set the result.
  \param <TT>nArgs (UInt_t [in])</TT>
        Number of parameters remaining on the command line.
  \param] <TT>pArgs[] (char** [in]):</TT>
        Pointer to the remaining command parameters 
        (pArgs[0] should be the gate name).
  
  
  \return Int_t

  \retval  TCL_OK    - the command worked and a new gate was created.
   \retval  TCL_ERROR - The command failed.  The reason for the failure is in
      the result


 */
Int_t 
CGateCommand::NewGate(CTCLInterpreter& rInterp, UInt_t nArgs, const char* pArgs[])
{


  SpecTcl& api(*(SpecTcl::getInstance()));

  if(nArgs != 3) {		// must be exactly 3 parameters....
    rInterp.setResult(Usage());
    return TCL_ERROR;
  }
 // need to figure out how to parse the gate description... the first two items
  // are fixed, name and type, the last is a list with gate type 
  // dependent contents.
  //

  const char* pName = *pArgs;
  pArgs++;
  const char* pType = *pArgs;
  pArgs++;
  const char* pList = *pArgs;
  std::string rResult;         // Notationally convenient for the port
  // the gate table is used to drive the rest of the parse:

  CGate*  pGate;
  CGatePackage& rPackage(*(CGatePackage*)getPackage());
  CGateFactory Factory(rPackage.getHistogrammer());
  vector<FPoint> PointValues;	// filled in below.
  vector<string> paramValues;  // Filled in further below
  

  GateFactoryTable* pItem = MatchGateType(pType);
  if(!pItem) {
    rResult = Usage();
    rResult += "\n  invalid gate type";
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  GateFactoryTable& Item(*pItem);

  if(Item.fGateList) {		// the list is just a list of gates.
    CTCLList GateList(&rInterp, pList);
    StringArray Gates;
    if(GateList.Split(Gates) != TCL_OK) {
      rResult =  Usage();
      rResult += "list of gates had incorrect format\n";
      rInterp.setResult(rResult);
      return TCL_ERROR;
    }
    try {
      pGate = api.CreateGate(Item.eGateType,
				 Gates);
    }
    catch(CException& rExcept) {
      rResult = Usage();
      rResult += rExcept.ReasonText();
      rInterp.setResult(rResult);
      return TCL_ERROR;
    }
  }

  else if(!Item.fNoParams) {	// list is parameters followed by point list.
    CTCLList List(&rInterp, pList);
    StringArray Description;
    List.Split(Description);	// bust the list apart.
    if(Description.size() != (Item.nParameters+1)) {
      rResult = Usage();
      rResult += "Incorrect description list format";
      rInterp.setResult(rResult);
      return TCL_ERROR;
    }

    vector<string> Parameters;
    // pull out the parameters
    for(UInt_t nPar = 0; nPar < Item.nParameters; nPar++) {
      Parameters.push_back(Description[nPar]);
    }
    // the last element of the description is a point list:
    CTCLList Points(&rInterp, Description[Item.nParameters]);
    vector <string> PointString;
    Points.Split(PointString);	// point string contains textualized Points.
    
    // if the gate is 1-d, then each point just contains an x-coordinate.

    if(Item.nParameters == 1) {
      if ((string(pType) == string("em")) || 
	      (string(pType) == string("am")) || 
	      (string(pType) == string("nm"))) {
      unsigned long Compare;
      char* endPtr(nullptr);

      // Issue 90:  was using sscanf which forced interpretation 
      //  of the string to hex regardless of how it was passed.
      // strtoul is better since with base =0 it uses any radix
      // elements to decide if the string is hex, or even octal.
      //
      Compare = strtoul(PointString[0].c_str(), &endPtr, 0);
      if (endPtr == PointString[0].c_str()) {
        // Bad conversion:

        rInterp.setResult("Invalid mask value");
        return TCL_ERROR;
      }
      pGate = api.CreateGate(Item.eGateType, Parameters, Compare);
      if(rPackage.AddGate(rInterp, string(pName), pGate)) {
        return TCL_OK;
      }
      else {
        return TCL_ERROR;
      }
      assert(0);
	  }
      else {
	for(UInt_t npoint = 0; npoint < PointString.size(); npoint++) {
	  Float_t x;
	  if(sscanf(PointString[npoint].c_str(), "%f", &x) == 0) {
	    rResult = Usage();
	    rResult += "\ninvalid point string in description";
	    rResult += PointString[npoint];
      rInterp.setResult(rResult);
	    return TCL_ERROR;
	  }
	  PointValues.push_back(FPoint(x,0));
	}
      }
    }
    else {
      // otherwise a point is a list containing x,y...
      for(UInt_t npoint = 0; npoint < PointString.size(); npoint++) {
	Float_t x,y;
	CTCLList Point(&rInterp, PointString[npoint]);
	vector<string> Coords;
	Point.Split(Coords);
	if(Coords.size() != 2) {
	  rResult = Usage();
	  rResult += "\nInvalid point string in description  ";
	  rResult += Point.getList();
    rInterp.setResult(rResult);
	  return TCL_ERROR;
	}
	Float_t s1 = sscanf(Coords[0].c_str(), "%f", &x);
	Float_t s2 = sscanf(Coords[1].c_str(), "%f", &y);
	
	if((s1 != 1)  || (s2 != 1) ) {
	  rResult = Usage();
	  rResult += "\nInvalid point string in description  ";
	  rResult += Point.getList();
    rInterp.setResult(rResult);
	  return TCL_ERROR;
	}
	FPoint pt(x,y);
	PointValues.push_back(pt);
      }
    }
    try {
      pGate = api.CreateGate(Item.eGateType, Parameters, PointValues);
    }
    catch(CException& rExcept) {
      rResult = Usage();
      rResult += rExcept.ReasonText();
      rInterp.setResult(rResult);
      return TCL_ERROR;
    }
  }
  else {        // List is just a point list, possibly followed by a now
                // mandatory spectrum list.
    CTCLList List(&rInterp, pList);
    StringArray Description;
    List.Split(Description);
    
    // gamma cuts have a point and parameters.

    UInt_t nPoint = 0;
    if(Item.eGateType == CGateFactory::gammacut) { 
      Float_t x1, x2;
      Int_t i = sscanf(Description[nPoint].c_str(), "%f %f", &x1, &x2);
      if(i != 2) {
        rResult = Usage();
        rResult += "\nInvalid point string in description";
        rResult += Description[nPoint];
        rInterp.setResult(rResult);
        return TCL_ERROR;
      }
      PointValues.push_back(FPoint(x1,0));
      PointValues.push_back(FPoint(x2,0));
      if(PointValues.size() != 2) {
        rResult = Usage();
        rResult += "\nInvalid point string in description";
        rResult += Description[nPoint];
        rInterp.setResult(rResult);
        return TCL_ERROR;
      }

      if(Description.size() == 2) { // Here are the parameters (used to be spectra).
	CTCLList params(&rInterp, Description[1]);
	vector<string> paramString;
	params.Split(paramString);
	for(UInt_t k = 0; k < paramString.size(); k++) {
	  paramValues.push_back(paramString[k]);
	}
      }
      else {
        rResult = "Gamma gates now require a non-empty parameter list\n";
        rResult +=Usage();
        rInterp.setResult(rResult);
        return TCL_ERROR;
      }
    }
    
    else {     // Otherwise a point is a list containing several x/y pairs..
      CTCLList Points(&rInterp, Description[nPoint]);
      vector<string> PointString;
      Points.Split(PointString);
      for(UInt_t i = 0; i < PointString.size(); i++) {
	CTCLList Point(&rInterp, PointString[i]);
	vector<string> Coords;
	Point.Split(Coords);
	if(Coords.size() != 2) {
	  rResult = Usage();
	  rResult += "\nInvalid point string in description  ";
	  rResult += Point.getList();
    rInterp.setResult(rResult);
	  return TCL_ERROR;
	}
	Float_t x,y;
	Int_t s1 = sscanf(Coords[0].c_str(), "%f", &x);
	Int_t s2 = sscanf(Coords[1].c_str(), "%f", &y);
	if( (s1 != 1) || (s2 != 1) ) {
	  rResult = Usage();
	  rResult += "\nInvalid point string in description  ";
	  rResult += Point.getList();
    rInterp.setResult(rResult);
	  return TCL_ERROR;
	}
	FPoint pt(x, y);
	PointValues.push_back(pt);
      }
      
      if(Description.size() == 2) { // means there are parameters
        CTCLList params(&rInterp, Description[1]);
        vector<string> paramString;
        params.Split(paramString);
        for(UInt_t k = 0; k < paramString.size(); k++) {
          paramValues.push_back(paramString[k]);
        }
      }
      else {
        rResult = "Gamma gates now require a non-empty parameter list\n";
        rResult += Usage();
        return TCL_ERROR;
      }
    }
    
    try {
      // Gamma gates require that we convert parameters to parameter ids:
      //
      vector<UInt_t> paramIds;
      for(int i =0; i < paramValues.size(); i++) {
      CParameter* pParam = api.FindParameter(paramValues[i]);
        if(!pParam) {
          rResult = "Gamma gate creation attempted with nonexisting parameter: ";
          rResult += paramValues[i];
          rResult += "\n";
          rResult += Usage();
          rInterp.setResult(rResult);
          return TCL_ERROR;
        }
	      paramIds.push_back(pParam->getNumber());

      }
      pGate = api.CreateGate(Item.eGateType, PointValues, 
				 paramIds);
    }
    catch(CException& rExcept) {
      rResult = Usage();
      rResult += rExcept.ReasonText();
      rInterp.setResult(rResult);
      return TCL_ERROR;
    }
  }
  
  // Now try to enter the gate in the dictionary:
  
  if(rPackage.AddGate(rInterp, string(pName), pGate)) {
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
//      ListGates(CTCLInterpreter& rInterp, 
//                UInt_t  nArgs, char* pArgs[])
//  Operation Type: 
//      SubFunction
//
Int_t CGateCommand::ListGates(CTCLInterpreter& rInterp, UInt_t  nArgs, const char* pArgs[])  
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
  CGatePackage& Package(*(CGatePackage*)getPackage());
  const char* pattern;
  std::string rResult;

  switch(nArgs) {
  case 0:
    pattern = "*";
    ResultString = Package.ListGates(pattern);
    break;
  case 1:
    if(MatchSwitches(*pArgs) != byid) {
      pattern = *pArgs;
      ResultString = Package.ListGates(pattern);
      break;
    }
    pattern = "*";
    ResultString = Package.ListGatesById(pattern);
    break;
  case 2:
    if (MatchSwitches(*pArgs) == byid) {
      *pArgs++;
      pattern = *pArgs;
      ResultString = Package.ListGatesById(pattern);
    } else {
        rResult = Usage();
        rResult += "\nIncorrect number or wrong order of parameters";
        rInterp.setResult(rResult);
        return TCL_ERROR;
      }
    break;
  default:
    rResult = Usage();
    rResult += "\nIncorrect number of parameters";
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  rInterp.setResult((const char*)(ResultString));
  return TCL_OK;
  
}
/////////////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//       DeleteGates(CTCLInterpreter& rInterp, UInt_t nArgs, char* pArgs[])
//  Operation Type: 
//       Subfunction
//
Int_t 
CGateCommand::DeleteGates(CTCLInterpreter& rInterp, UInt_t nArgs, const char* pArgs[])  
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
  //         Either Points to a list of gates, or -id followed by a list of gates.
  //
  std::string rResult;
  if(nArgs == 0) {		// Not allowed.
    rResult = Usage();
    rResult += "\nMust at least be a gate to delete.";
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  // What we do depends on whether or not the next Item is a -id switch:

  CGatePackage& Package(*(CGatePackage*)getPackage());
  if(MatchSwitches(*pArgs) == id) { // Remaining list is a set of ids...
    nArgs--;
    pArgs++;
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
      } else if(n < 0) {
        CTCLString error;
        error.StartSublist();
        error.AppendElement(*pArgs);
        error.AppendElement(" Gate ID cannot be negative");
        error.EndSublist();
        rResult += (const char*)error;
        ConvertFailed = kfFALSE;
      } else {			// n  is a good gate id:
	      Ids.push_back((UInt_t)n);
      }
      pArgs++;
    }
    if(Package.DeleteGates(rInterp, Ids) && (!ConvertFailed)) {   // Sets the result.
      return TCL_OK;
    } else {
      return TCL_ERROR;
    }
  }
  else {			// List is all gate names.
    vector<string> Names;
    for(UInt_t i = 0; i < nArgs; i++) {
      Names.push_back(*pArgs); // All strings are legal names.
      pArgs++;
    }
    if(Package.DeleteGates(rInterp, Names)) {    // Sets the result.
      return TCL_OK;
    } else {
      return TCL_ERROR;
    }
  }
  assert(0);                   // Should never get here.
}
/// Process the gate -trace command and its subcommands.
///
// Parameters:
//    rInterp - Reference to the interpreter object that is running the command.
//    nArgs   - Numberof remaining parameters on the line.
//              should be one  or two
//    pArgs   - The arguments themselves.. should be at most one,
//              and that would be what to trace and the script.
// Returns:
//   TCL_OK    - Everything ok and the previous script in the result.
//   TCL_ERROR - some failure, with result an error message.
Int_t 
CGateCommand::traceGates(CTCLInterpreter& rInterp, UInt_t nArgs,const char* args[])
{
  // if we are mpiParallel, the only ranks in which tracing will work is
  // are the root and the event sink threads. Since we are wrapped in an MPI command,
  // however we'll just make other ranks returrn TCL_OK but as a no-op.

  if (isMpiApp() && 
    (myRank() != MPI_ROOT_RANK) && 
    (myRank() != MPI_EVENT_SINK_RANK)
  ) {
    return TCL_OK;
  }

  std::string rResult;
  // Must be no more than 2 but at least one parameter:
  if ((nArgs) > 2  || (nArgs < 1)){
    rResult = "Too many or too few command line parameters\n";
    rResult += Usage();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
 
  // The first parameter will fetch determine which pointer we're
  // playing with.

  CTCLObject** ppScript(0)
;
  string       scriptSelector(args[0]);
  if (scriptSelector == string("add")) {
    ppScript = &m_pAddScript;
  }
  if (scriptSelector == string("delete")) {
    ppScript = &m_pDeleteScript;
  }
  if (scriptSelector == string("change")) {
    ppScript = &m_pChangeScript;
  }
  if (!ppScript) {
    rResult = "Incorrect trace type selector must be 'add' 'delete' or 'change'\n";
    rResult += Usage();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  // Create the old script string; empty if there is no object else the contents
  // of the script string.
  string oldScript;
  if (*ppScript) {
    oldScript = string(**ppScript);
  } 
  else {
    oldScript =  "";
  }

  // If there is a new script replace the old one:

  if (nArgs == 2) {
    string newScript(args[1]);
    delete *ppScript;
    *ppScript = (CTCLObject*)NULL;

    if (newScript != string("")) {
      CTCLObject* pNewObject = new CTCLObject();
      pNewObject->Bind(rInterp);
      (*pNewObject) = newScript;    // The script is one list element.
      *ppScript = pNewObject;
    }
  }

  // Return the old script and OK:

  rResult = oldScript;
  rInterp.setResult(rResult);
  return TCL_OK;

}

/*!
   Invoke the add script if it's defined.
   Parameters:
     Name of the added gate.
*/
void
CGateCommand::invokeAddScript(std::string name)
{
  invokeAScript(m_pAddScript, name);
}

/*!
   Invoke the delete script if it's defined.
   Parameters:
      Name of the deleted gate.
*/
void
CGateCommand::invokeDeleteScript(std::string name)
{
  invokeAScript(m_pDeleteScript, name);
}
/*!
  Invoke the gate changed script if it's defined.
  Parameter:
     Name of the modified gate.
*/
void
CGateCommand::invokeChangedScript(std::string name)
{
  invokeAScript(m_pChangeScript, name);
}
/*!
  Invoke some script object
  Parameters:
    Pointer to the script object.
    Parameter to append to the script.

*/
void
CGateCommand::invokeAScript(CTCLObject* pScript,
			    string      parameter)
{
  // Do nothing if there's no script defined:

  if (pScript) {
    CTCLObject fullScript(*pScript);
    fullScript.Bind(getInterpreter());
    fullScript += parameter;

    // Only invoke traces in root rank:

    if (!isMpiApp() || (myRank() == MPI_ROOT_RANK)) {

      try {
        fullScript();
      }
      catch (CException& e) {
        cerr << "Gate trace script failed: " << e.ReasonText() << endl;
      }
      catch (string msg) {
        cerr << "Gate trace script faield: " << msg << endl;
      }
      catch (const char* msg) {
        cerr << "Gate trace script failed: " << msg << endl;
      }
      catch (...) {
        cerr << "Gate trace script failed with an un-anticipated exception type\n";
      }

    }
  }
}

//////////////////////////////////////////////////////////////////////////////////
//
// Function:
//    Switches MatchSwitches(char* pKey)
// Operation type:
//    Protected utility.
//
CGateCommand::Switches
CGateCommand::MatchSwitches(const char* pKey)
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

//  This section of code represents the receiver part of the gate trace
// pump (the sender part is in Histogrammer.{h,cpp}.
// what we do is forward gate traces for any scripts that have
// done a gate -trace operation.
//
// We have three methods:
//   -  mpiTraceRelayCatchThread - runs in MPI_ROOT_RANK  it accepts messgaes 
//      tagged as MPI_GATE_TRACE_RELAY_TAG and queues them to the main thread via
//      Tcl_ThreadQueueEvent
//   -  traceRelayHandler - handles the events queued in the main thread and just
//      invokes the appropriate invoke...Script method.
//  - startTracePump - starts the  pump thread.
//
#ifdef WITH_MPI

// We need an event struct that contains the object as well as the trace information in the
// message:

struct GateTraceEvent {
  Tcl_Event     s_event;
  CGateCommand* s_commandObject;
  TraceRelay   s_trace;
};

// This implements the thread that receives MPI trace forward messages from the Event sink
// process We don't need to check for the right rank or even that we are in an MPI app.  That's
// already been done.  Our ClientData are just a pointer to the CGateCommand so we can establish
// object to fill in s_commandObject.
//
Tcl_ThreadCreateType
CGateCommand::mpiTraceRelayCatchThread(ClientData command) {
  CGateCommand* pCommand = reinterpret_cast<CGateCommand*>(command);

  while (true) {
    TraceRelay message;
    MPI_Status stat;
    if(MPI_Recv(
      &message, 1, getTraceRelayType(), MPI_EVENT_SINK_RANK, MPI_GATE_TRACE_RELAY_TAG, MPI_COMM_WORLD, &stat) 
      != MPI_SUCCESS) {
        throw std::runtime_error("Failed to read a trace notifiation");
    }
    if (message.s_traceType == TRACE_EXIT_THREAD) {
      break;
    }
    auto pEvent = reinterpret_cast<GateTraceEvent*>(Tcl_Alloc(sizeof(GateTraceEvent)));
    if (!pEvent) {
      throw std::runtime_error("Failed to allocate a gate trace Tcl event");
    }
    pEvent->s_event.proc = CGateCommand::traceRelayEventHandler;
    pEvent->s_event.nextPtr = nullptr;
    pEvent->s_commandObject = pCommand;
    memcpy(&(pEvent->s_trace), &message, sizeof(TraceRelay));

    Tcl_ThreadQueueEvent(pCommand->m_id, &pEvent->s_event, TCL_QUEUE_TAIL);
    Tcl_ThreadAlert(pCommand->m_id);
  }
  TCL_THREAD_CREATE_RETURN;
}
// The event handler - we just invoke the appropriate trace script method.
//

int 
CGateCommand::traceRelayEventHandler(Tcl_Event* pEvent, int flags) {
  GateTraceEvent* pTraceEvent = reinterpret_cast<GateTraceEvent*>(pEvent);
  std::string gateName(pTraceEvent->s_trace.s_gateName);
  switch (pTraceEvent->s_trace.s_traceType) {
    case TRACE_ADD_GATE:
      pTraceEvent->s_commandObject->invokeAddScript(gateName);
      break;
    case TRACE_REMOVE_GATE:
      pTraceEvent->s_commandObject->invokeDeleteScript(gateName);
      break;
    case TRACE_MODIFY_GATE:
      pTraceEvent->s_commandObject->invokeChangedScript(gateName);
      break;
    default:
      throw std::runtime_error("Unexpected gate trace type!");
  }

  return 1;
}
// For the root rank, start the event pump...other wise we are a no-op.
// It's already been determined that we are an MPI app at this time.
void
CGateCommand::startTracePump() {
  if (myRank() == MPI_ROOT_RANK) {
    Tcl_ThreadId tid;
    Tcl_CreateThread(
      &tid, CGateCommand::mpiTraceRelayCatchThread, this,
      TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS
    );
  }
}
// Called by the event sink rank to stop the gate pump by sending it a TRACE_EXIT_THREAD
// type message.  This is static so it can be called without reference to the object.

void
CGateCommand::stopTracePump() {
#ifdef WITH_MPI
  if (myRank() == MPI_EVENT_SINK_RANK) {
    // Only we can send it:

    TraceRelay message;
    message.s_traceType = TRACE_EXIT_THREAD;
    message.s_gateName[0] = '\0';

    MPI_Send(
      &message, 1, getTraceRelayType(),
       MPI_ROOT_RANK, MPI_GATE_TRACE_RELAY_TAG, MPI_COMM_WORLD
    );
  }
#endif
}


#endif