static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";

#include <string>
#include <vector>
#include <string>
#include <map>

#include <TCLList.h>
#include <TCLString.h>
#include <TCLResult.h>
#include <Exception.h>
#include <histotypes.h>
#include <TclGrammerApp.h>

#include "FilterCommand.h"
#include "Globals.h"
#include "Histogrammer.h"
#include "GateCommand.h"

// Constructors.
CFilterCommand::CFilterCommand(CTCLInterpreter& rInterp) : CTCLProcessor("filter", &rInterp) {
}

CFilterCommand::~CFilterCommand() {
}

// Operators.
/*
  int CFilterCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) {
  rResult = "Echoing the NEW Filter command: ";
  for(int i=0; i<nArgs; i++) {
  rResult.AppendElement(pArgs[i]);
  // Alternatively, ...
  //rResult.AppendElement(*argv);
  //argv++;
  }
  return TCL_OK;
  }
*/

int CFilterCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) {
  nArgs--; pArgs++; // Don't care about command name.

  if(nArgs <= 0) { // Need at least one parameter.
    rResult = Usage();
    return TCL_ERROR;
  }

  // Parse out the switch:
  switch(MatchSwitch(pArgs[0])) {
  case keDelete:
    nArgs--;
    pArgs++;
    return Delete(rInterp, rResult, nArgs, pArgs);

  case keEnable:
    nArgs--;
    pArgs++;
    return Enable(rInterp, rResult, nArgs, pArgs);

  case keDisable:
    nArgs--;
    pArgs++;
    return Disable(rInterp, rResult, nArgs, pArgs);

  case keRegate:
    nArgs--;
    pArgs++;
    return Regate(rInterp, rResult, nArgs, pArgs);

  case keList:
    nArgs--;
    pArgs++;
    return List(rInterp, rResult, nArgs, pArgs);

  default: // Create a new filter.
    //nArgs--;
    //pArgs++;
    return Create(rInterp, rResult, nArgs, pArgs);
  }
}

// Static Data:
struct SwitchTableEntry {
  char* pSwitchText;
  CFilterCommand::eSwitches Switch;
};

static const SwitchTableEntry Switches[] = {
  {"-delete", CFilterCommand::keDelete},
  {"-enable", CFilterCommand::keEnable},
  {"-disable", CFilterCommand::keDisable},
  {"-regate", CFilterCommand::keRegate},
  {"-list", CFilterCommand::keList}
};

static const UInt_t nSwitches = sizeof(Switches)/sizeof(SwitchTableEntry);

CFilterCommand::eSwitches
CFilterCommand::MatchSwitch(const char* pSwitch) {
  for(UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(pSwitch, Switches[i].pSwitchText) == 0) {
      return Switches[i].Switch;
    }
  }
  return keNotSwitch;
}

std::string CFilterCommand::Usage() {
  std::string Use;
  Use  = "Usage:\n";
  Use += "   filter name gate {par1 par2 ...}\n";
  Use += "   filter -delete name\n"; 
  Use += "   filter -enable name\n";
  Use += "   filter -disable name\n";
  Use += "   filter -regate name gate\n";
  Use += "   filter -list ?glob-pattern?\n";
  Use += "\n filter allows the gating upon a parameter in an event.\n";
  return Use;
}

Int_t CFilterCommand::Create(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) {
  if(nArgs != 3) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Parse the filter description.
  const char* pFilterName = *pArgs;
  pArgs++;
  const char* pGateName = *pArgs;
  pArgs++;
  const char* pParameterList = *pArgs;

  CTCLList ParameterList(&rInterp, pParameterList);
  StringArray Description; // May be removed from use in future, leaving just Parameters.
  if(ParameterList.Split(Description) != TCL_OK) { // Bust the list apart.
    rResult = Usage();
    rResult += "Incorrect parameter description list format.";
    return TCL_ERROR;
  }
  vector<string> Parameters;
  // Pull out the parameters.
  for(UInt_t nPar = 0; nPar < Description.size(); nPar++) {
    Parameters.push_back(Description[nPar]);
  }
  // Put Gate in GatedEventFilter.
  //CGatePackage& Package((CGatePackage&)getMyPackage());
  //CHistogrammer* pHist = Package.getHistogrammer();
  //FilterDictionaryIterator p = pHist->GateBegin();
  /*
    These would be nice.
    CGateContainer* pGateContainer = CGateDictionary[pGateName]; // This would be nice. *********************
    CGateContainer* pGateContainer = CHistogrammer::FindGate(&(*pGateName));
  */
  CGateContainer* pGateContainer = ((CHistogrammer*)gpEventSink)->FindGate(&(*pGateName));
  if(pGateContainer) { // There IS a Gate with this name in the Histogrammer's GateDictionary.
    CGatedEventFilter* pGatedEventFilter = new CGatedEventFilter;
    pGatedEventFilter->setGateContainer(*pGateContainer);
    // Put GatedEventFilter in FilterDictionary.
    CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
    pFilterDictionary->Enter(pFilterName, &(*pGatedEventFilter)); // CHECK THIS! **********************
  } else {
    // ERROR. Gate not present.
    rResult += "Error: Gate (" + std::string(pGateName) + ") not present in dictionary.";
    return TCL_ERROR;
  }

  // Output result.
  rResult += "Filter created.\n";
  rResult += " Filter: " + std::string(pFilterName) + "\n";
  rResult += " Gate: " + std::string(pGateName) + "\n";
  rResult += " Parameters:";
  for(UInt_t i = 0; i < Parameters.size(); i++) {
    rResult += " " + Parameters[i];
  }
  return TCL_OK;
}

Int_t CFilterCommand::Delete(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) {
  if(nArgs != 1) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Parse the filter description.
  const char* pFilterName = *pArgs;

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  if(pFilterDictionary->Lookup(pFilterName) != pFilterDictionary->end()) {
    (*pFilterDictionary).Remove(pFilterName);
    rResult = "Filter (" + std::string(pFilterName) + ") deleted.";
  } else {
    rResult = "Filter (" + std::string(pFilterName) + ") not in dictionary.";
  }
  return TCL_OK;
}

Int_t CFilterCommand::Enable(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) {
  if(nArgs != 1) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Parse the filter description.
  const char* pFilterName = *pArgs;

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  if((*pFilterDictionary).Lookup(&(*pFilterName))!=(*pFilterDictionary).end()) {
    CEventFilter* pEventFilter = (pFilterDictionary->Lookup(pFilterName))->second;
    (*pEventFilter).Enable();
    //(&(*((*pFilterDictionary).Lookup(&(*pFilterName))))).Enable();
    rResult = "Filter (" + std::string(pFilterName) + ") enabled.";
  } else {
    rResult = "Filter (" + std::string(pFilterName) + ") not in dictionary.";
  }
  return TCL_OK;
}

Int_t CFilterCommand::Disable(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) {
  if(nArgs != 1) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Parse the filter description.
  const char* pFilterName = *pArgs;

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  if((*pFilterDictionary).Lookup(&(*pFilterName))!=(*pFilterDictionary).end()) {
    CEventFilter* pEventFilter = (pFilterDictionary->Lookup(pFilterName))->second;
    (*pEventFilter).Disable();
    //(**((*pFilterDictionary).Lookup(&(*pFilterName)))).Disable();
    // &(*((*pFilterDictionary).Lookup(&(*pFilterName)))).Disable(); // (*(*pFilterDictionary).Lookup(pFilterName)).Disable();
    rResult = "Filter (" + std::string(pFilterName) + ") disabled.";
  } else {
    rResult = "Filter (" + std::string(pFilterName) + ") not in dictionary.";
  }
  return TCL_OK;
}

Int_t CFilterCommand::Regate(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) {
  if(nArgs != 2) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Parse the filter description.
  const char* pFilterName = *pArgs;
  pArgs++;
  const char* pGateName = *pArgs;

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  if(pFilterDictionary->Lookup(pFilterName) != pFilterDictionary->end()) {
    // IMPLEMENT *************************************************************************
    rResult = "Filter (" + std::string(pFilterName) + ") regated (" + std::string(pGateName) + ").";
  } else {
    rResult = "Filter (" + std::string(pFilterName) + ") not in dictionary.";
  }
  return TCL_OK;
}

Int_t CFilterCommand::List(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) {
  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  if(pFilterDictionary->size() > 0) {
    CDictionary<CGatedEventFilter*>::DictionaryIterator i = (*pFilterDictionary).begin();
    rResult = "Listing filters:";
    while(i != (*pFilterDictionary).end()) {
      rResult += "\n " + i->first;
      i++;
    }
  } else {
    rResult = "No filters.";
  }
  return TCL_OK;
}

/*
  //******************************************************************************************************************
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

*/
