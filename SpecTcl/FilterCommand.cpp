static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";

#include <string>
#include <sstream> // Solely for converting from integers to strings.
#include <vector>
#include <map>

#include <TCLList.h>
#include <TCLString.h>
#include <TCLResult.h>
#include <TclGrammerApp.h>

#include "Globals.h"
#include "Histogrammer.h"
#include "GateCommand.h"
#include "FilterCommand.h"
#include "EventSinkPipeline.h"

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
  case keNew:
    nArgs--;
    pArgs++;
    return Create(rInterp, rResult, nArgs, pArgs);

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

  case keFile:
    nArgs--;
    pArgs++;
    return File(rInterp, rResult, nArgs, pArgs); // Allowable???*****************************

  case keList:
    nArgs--;
    pArgs++;
    return List(rInterp, rResult, nArgs, pArgs);

  default: // Create a new filter.
    // -new option is unnecessary.
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
  {"-new", CFilterCommand::keNew},
  {"-delete", CFilterCommand::keDelete},
  {"-enable", CFilterCommand::keEnable},
  {"-disable", CFilterCommand::keDisable},
  {"-regate", CFilterCommand::keRegate},
  {"-file", CFilterCommand::keFile},
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
  Use += " filter [-new] filtername gatename {par1 par2 ...}\n";
  Use += " filter -delete filtername\n"; 
  Use += " filter -enable filtername\n";
  Use += " filter -disable filtername\n";
  Use += " filter -regate filtername gatename\n";
  Use += " filter -file filename filtername\n";
  Use += " filter -list ?glob-pattern?\n\n";
  Use += " filter allows the gating upon a parameter in an event.";
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

  // Pull out the parameters.
  vector<string> Parameters; // Referred to as ParameterNames within GatedEventFilter.
  vector<UInt_t> ParameterIds;
  for(UInt_t nPar = 0; nPar < Description.size(); nPar++) {
    Parameters.push_back(Description[nPar]);
  }
  // Put Gate in GatedEventFilter.
  //CGatePackage& Package((CGatePackage&)getMyPackage());
  //CHistogrammer* pHist = Package.getHistogrammer();
  //FilterDictionaryIterator p = pHist->GateBegin();
  /*
    These would be nice...
    CGateContainer* pGateContainer = CGateDictionary[pGateName];
    CGateContainer* pGateContainer = CHistogrammer::FindGate(&(*pGateName));
  */

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator FilterDictionaryIterator = pFilterDictionary->Lookup(pFilterName);
  if(FilterDictionaryIterator == pFilterDictionary->end()) { // Filter not already present in FilterDictionary.
    CGateContainer* pGateContainer = ((CHistogrammer*)gpEventSink)->FindGate(pGateName); // Find the Gate in the Histogrammer's GateDictionary.
    if(pGateContainer) { // Gate present in Histogrammer's GateDictionary.
      // Make sure Parameters exist, and retrieve their IDs.
      for(UInt_t i=0; i<Parameters.size(); i++) {
	CParameter* pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(Parameters[i]);
	if(pParameter != (CParameter*)kpNULL) { // Parameter is valid and present in dictionary.
	  //ParameterIds.push_back((UInt_t)atoi((const char*)(Parameters[i].c_str())));
	  ParameterIds.push_back(pParameter->getNumber());
	} else {
	  rResult += "Error: Invalid parameter (" + Parameters[i] + ").";
	  return TCL_ERROR; // Err out on the first invalid parameter, to be on the safe side of things.
	}
      }

      // We now have the ParameterIds, and all is well. Make the Filter.
      CGatedEventFilter* pGatedEventFilter = new CGatedEventFilter;
      pGatedEventFilter->setGateContainer(*pGateContainer); // Set the Filter's Gate.
      pGatedEventFilter->setParameterNames(Parameters); // Set the Filter's Parameter(Name)s.
      pGatedEventFilter->setParameterIds(ParameterIds); // Set the Filter's Parameter IDs.
      pFilterDictionary->Enter(pFilterName, pGatedEventFilter); // Put GatedEventFilter in FilterDictionary.
      gpEventSinkPipeline->AddEventSink((CGatedEventFilter&)*pGatedEventFilter); // Put GatedEventFilter in EventSinkPipeline.
    } else {
      rResult += "Error: Invalid gate (" + std::string(pGateName) + ").";
      return TCL_ERROR;
    }
  } else {
    rResult += "Error: Filter (" + std::string(pFilterName) + ") already present.";
    return TCL_ERROR;
  }

  // Output result. (Differs from ListFile() only in the use of Lookup() and the fact that it only lists the first found.)
  std::ostringstream oss;
  CTCLString List, ParameterIdList;
  List.StartSublist();
  List.AppendElement(pFilterName); // Filter Name.
  List.AppendElement(pGateName); // Gate Name.
  // Parameters.
  for(UInt_t i=0; i<ParameterIds.size(); i++) {
    oss << ParameterIds[i];
    ParameterIdList.AppendElement(oss.str());
    oss.str(""); // Clear the output string stream.
  }
  List.AppendElement(ParameterIdList); // List of Parameters becomes an element too.
  // Enabled/Disabled.
  FilterDictionaryIterator = pFilterDictionary->Lookup(pFilterName); // Filter should be defined and present in dictionary by now, so lookup should pass.
  List.AppendElement(FilterDictionaryIterator->second->getFileName()); // File name.
  if(FilterDictionaryIterator->second->CheckEnabled()) {
    List.AppendElement("enabled");
  } else {
    List.AppendElement("disabled");
  }
  List.EndSublist();
  rResult += (const char*)List;
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
    pFilterDictionary->Remove(pFilterName);
    rResult = "Filter (" + std::string(pFilterName) + ") deleted.";
  } else {
    rResult = "Error: Invalid filter (" + std::string(pFilterName) + ").";
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
  //if((*pFilterDictionary).Lookup(pFilterName)!=(*pFilterDictionary).end()) {
  if(pFilterDictionary->Lookup(pFilterName)!=pFilterDictionary->end()) {
    CEventFilter* pEventFilter = (pFilterDictionary->Lookup(pFilterName))->second;
    //(*pEventFilter).Enable();
    //(&(*((*pFilterDictionary).Lookup(&(*pFilterName))))).Enable();
    pEventFilter->Enable();
    if(pEventFilter->CheckEnabled()) {
      //rResult = "Filter (" + std::string(pFilterName) + ") enabled.";
      rResult += ListFilter(pFilterName);
      return TCL_OK;
    } else {
      rResult = "Error: Filter (" + std::string(pFilterName) + ") could not be enabled.";
      return TCL_ERROR;
    }
  } else {
    rResult = "Error: Invalid filter (" + std::string(pFilterName) + ").";
    return TCL_ERROR;
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
  //if((*pFilterDictionary).Lookup(pFilterName)!=(*pFilterDictionary).end()) {
  if(pFilterDictionary->Lookup(pFilterName)!=pFilterDictionary->end()) {
    CEventFilter* pEventFilter = (pFilterDictionary->Lookup(pFilterName))->second;
    //(*pEventFilter).Disable();
    //(&(*((*pFilterDictionary).Lookup(&(*pFilterName))))).Disable();
    pEventFilter->Disable();
    if(!(pEventFilter->CheckEnabled())) {
      //rResult = "Filter (" + std::string(pFilterName) + ") disabled.";
      rResult += ListFilter(pFilterName);
      return TCL_OK;
    } else {
      rResult = "Error: Filter (" + std::string(pFilterName) + ") could not be disabled.";
      return TCL_ERROR;
    }
  } else {
    rResult = "Error: Invalid filter (" + std::string(pFilterName) + ").";
    return TCL_ERROR;
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
  CFilterDictionaryIterator FilterDictionaryIterator = pFilterDictionary->Lookup(pFilterName);
  if(FilterDictionaryIterator != pFilterDictionary->end()) { // Filter present in FilterDictionary.
    CGateContainer* pGateContainer = ((CHistogrammer*)gpEventSink)->FindGate(pGateName); // Find the Gate in the Histogrammer's GateDictionary.
    if(pGateContainer) { // Gate present in Histogrammer's GateDictionary.
      CGatedEventFilter* pGatedEventFilter = FilterDictionaryIterator->second; // Get the Filter. //(pFilterDictionary->Lookup(pFilterName))->second;
      pGatedEventFilter->setGateContainer(*pGateContainer); // Set the Filter's Gate (Re-gate).
      //rResult = "Filter (" + std::string(pFilterName) + ") regated (" + std::string(pGateName) + ").";
      rResult += ListFilter(pFilterName);
      return TCL_OK;
    } else {
      rResult += "Error: Invalid gate (" + std::string(pGateName) + ").";
      return TCL_ERROR;
    }
  } else {
    rResult += "Error: Invalid filter (" + std::string(pFilterName) + ").";
    return TCL_ERROR;
  }
}

Int_t CFilterCommand::File(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) {
  if(nArgs != 2) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Parse the filter description.
  const char* pFileName = *pArgs;
  pArgs++;
  const char* pFilterName = *pArgs;

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator FilterDictionaryIterator = pFilterDictionary->Lookup(pFilterName);
  if(FilterDictionaryIterator != pFilterDictionary->end()) { // Filter present in FilterDictionary.
    CGatedEventFilter* pGatedEventFilter = FilterDictionaryIterator->second; // Get the Filter. //(pFilterDictionary->Lookup(pFilterName))->second;
    string sFileName = string(pFileName); // Just to get a string& for setFileName(string&).
    pGatedEventFilter->setFileName(sFileName); // Automatically calls for a file name reset by disabling and enabling so...
    pGatedEventFilter->Disable(); // ... Prevent any funky stuff.
    rResult += ListFilter(pFilterName);
    return TCL_OK;
  } else {
    rResult += "Error: Invalid filter (" + std::string(pFilterName) + ").";
    return TCL_ERROR;
  }
}

Int_t CFilterCommand::List(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) { // List all filters.
  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  if(pFilterDictionary->size() > 0) {
    CFilterDictionaryIterator FilterDictionaryIterator = pFilterDictionary->begin();
    // Output result.
    //std::ostringstream oss;
    CTCLString List, FilterList;
    while(FilterDictionaryIterator != pFilterDictionary->end()) {
      List.StartSublist();
      List.AppendElement(FilterDictionaryIterator->first); // Filter name.
      List.AppendElement(FilterDictionaryIterator->second->getGateName()); // GateContainer has Gate name.
      List.AppendElement(FilterDictionaryIterator->second->getFileName()); // File name.
      /* Parameters not done yet.
	 for(UInt_t i=0; i<ParameterIds.size(); i++) {
	 oss << ParameterIds[i];
	 ParameterIdList.AppendElement(oss.str());
	 oss.str(""); // Clear the output string stream.
	 }
	 List.AppendElement(ParameterIdList); // List of Parameters becomes an element too.
      */
      if(FilterDictionaryIterator->second->CheckEnabled()) {
	List.AppendElement(std::string("enabled"));
      } else {
	List.AppendElement(std::string("disabled"));
      }
      List.EndSublist();
      List.Append("\n");
      FilterDictionaryIterator++;
    } // End while.
    rResult += (const char*)List;
  }
  return TCL_OK;
}

string CFilterCommand::ListFilter(const char* pFilterName) { // List a single filter.
  string FilterName = string(pFilterName);
  string result;
  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  if(pFilterDictionary->size() > 0) {
    CFilterDictionaryIterator FilterDictionaryIterator = pFilterDictionary->begin();
    // Output result.
    //std::ostringstream oss;
    CTCLString List, FilterList;
    while(FilterDictionaryIterator != pFilterDictionary->end()) {
      if(pFilterName == FilterDictionaryIterator->first) {
	List.StartSublist();
	List.AppendElement(FilterDictionaryIterator->first); // Filter name.
	List.AppendElement(FilterDictionaryIterator->second->getGateName()); // GateContainer has Gate name.
	List.AppendElement(FilterDictionaryIterator->second->getFileName()); // File name.
	/* Parameters not done yet.
	   for(UInt_t i=0; i<ParameterIds.size(); i++) {
	   oss << ParameterIds[i];
	   ParameterIdList.AppendElement(oss.str());
	   oss.str(""); // Clear the output string stream.
	   }
	   List.AppendElement(ParameterIdList); // List of Parameters becomes an element too.
	*/
	if(FilterDictionaryIterator->second->CheckEnabled()) {
	  List.AppendElement(std::string("enabled"));
	} else {
	  List.AppendElement(std::string("disabled"));
	}
	List.EndSublist();
	List.Append("\n");
      }
      FilterDictionaryIterator++;
    } // End while.
    result = string((const char*)List);
    return result;
  } else {
    return "";
  }
  return "";
}
