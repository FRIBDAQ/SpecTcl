/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Kanayo Orji
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";

#include <config.h>
#include "FilterCommand.h"
#include <GatedEventFilter.h>
#include <FilterDictionary.h>
#include "CFilterOutputStageFactory.h"

#include <SpecTcl.h>

#include <string>
#include <vector>
#include <map>

#include <TCLList.h>
#include <TCLString.h>
#include <TCLResult.h>
#include <TclGrammerApp.h>

#include "Globals.h"
#include "Histogrammer.h"
#include "GateCommand.h"
#include "EventSinkPipeline.h"

#include <assert.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Static Data:
struct SwitchTableEntry {
  const char* pSwitchText;
  CFilterCommand::eSwitches Switch;
};

static const SwitchTableEntry Switches[] = {
  {"-new", CFilterCommand::keNew},
  {"-delete", CFilterCommand::keDelete},
  {"-enable", CFilterCommand::keEnable},
  {"-disable", CFilterCommand::keDisable},
  {"-regate", CFilterCommand::keRegate},
  {"-file", CFilterCommand::keFile},
  {"-list", CFilterCommand::keList},
  {"-format", CFilterCommand::keFormat}
};

static const string defaultOutputFormat("xdr");

static const UInt_t nSwitches = sizeof(Switches)/sizeof(SwitchTableEntry);



// Constructors.

/*!
   Construct the filter.  The command is registered
   as the command "filter".  The client constructing us must
   register the command on the interpreter.

   \param rInterp (CTCLInterpreter& [in]):
      The interpreter the command will be registered on.
     
*/
CFilterCommand::CFilterCommand(CTCLInterpreter& rInterp) : 
  CTCLProcessor("filter", &rInterp) 
{
}


CFilterCommand::~CFilterCommand() {
}

// Operators.

/*!
    Executes the filter command.  This function just dispatches  the 
    command as follows:
    -  -new calls Create
    -  -enable calls Enable
    -  -disable calls Disable
    -  -regate calls Regate
    -  -file   calls File
    -  -list   calls List
    -  (noswitch) calls Create too.
    
    \param rInterp (in):
        The interpreter that's executing the command.
    \param rResult (out):
        The result string the command returns.
    \param nArgs (in):
        The number of command parameters (counting the keyword).
    \param pArgs (in):
        The list of command keywords.

    \return  one of:
    - TCL_OK - everything worked fine.
    - TCL_ERROR - The command failed.
*/
int CFilterCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			       int nArgs, char* pArgs[]) {
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

  case keFile:
    nArgs--;
    pArgs++;
    return File(rInterp, rResult, nArgs, pArgs);

  case keList:
    nArgs--;
    pArgs++;
    return List(rInterp, rResult, nArgs, pArgs);

    // Both --new and no switch create a filter:
  case keNew:
    nArgs--;
    pArgs++;
  case keNotSwitch:		//  Create a new filter.
    return Create(rInterp, rResult, nArgs, pArgs);
  case keFormat:
    return Format(rInterp, rResult, nArgs, pArgs);

  default:                     // Bug to get here:
    assert(0);

  }
  // Bug to get here too:
  assert(0);
}
/*!
   Matches a character string against the set of recognized
   command switches.  The switch value is returned as an enumerated
   value of type eSwitches.  If the string is not a recognizable switch,
   keNotSwitch is returned.

   \param pSwitch (in):
      The string to recognize.
    
   \return (CFilterCommand::eSwitches):
      The encoded value of the recognized switch or keNotSwitch instead.
*/
CFilterCommand::eSwitches
CFilterCommand::MatchSwitch(const char* pSwitch) {
  for(UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(pSwitch, Switches[i].pSwitchText) == 0) {
      return Switches[i].Switch;
    }
  }
  return keNotSwitch;
}
/*!
    Returns a string that describes how to use the filter command.
    Typically this will be put in to the Result of the command in case
    there was an error in the command syntax.
    \return (string):
       Mini-help on the filter command.
*/

std::string CFilterCommand::Usage() {
  std::string Use;
  Use  = "Usage:\n";
  Use += " filter [-new] filtername gatename {par1 par2 ...}\n";
  Use += " filter -delete filtername\n"; 
  Use += " filter -enable filtername\n";
  Use += " filter -disable filtername\n";
  Use += " filter -regate filtername gatename\n";
  Use += " filter -file filename filtername\n";
  Use += " filter -list ?glob-pattern?\n";
  Use += " filter -format filtername format\n";
  Use += "\nfilter creates pre-sorted event files\n";
  Use += "\nfilter formats are: \n";
  CFilterOutputStageFactory& fact(CFilterOutputStageFactory::getInstance());
  Use += fact.document();
  return Use;
}

/*!
    Creates a new filter.  When this is called, the remainder of the command
    line should be a filtername, a gatename and a list of parameters.
    The filter is created and added to the filter dictionary.
    \param rInterp (in):
       The interpreter that's executing this command.
    \param rResult (out):
       The command result string.  If the filter was succesfully created
       a TCL list will be returned whose elements are:
       - The name of the filter.
       - The name of the gate that defines the filter criterion.
       - The list of parameter names that are output by the filter.
       - The name of the file to which the filtered data is 
          sent (empty initially)
       - The state of the filter (initially disabled).
    \param nArgs (in):
       Number of parameters remaning on the command line.  This must be 3.
    \param pArgs (in):
       The remaining command line parameters.
     \return an int status (see operator()).
 */

Int_t 
CFilterCommand::Create(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		       int nArgs, char* pArgs[]) 
{
  SpecTcl& Api(*(SpecTcl::getInstance()));

  if(nArgs != 3) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Parse the filter description.
  const char* pFilterName = *pArgs;    // Name of the filter.
  pArgs++;
  const char* pGateName = *pArgs;      // Name of the gate.
  pArgs++;
  const char* pParameterList = *pArgs;  // Parameter list.

  // Bust the parameter list into a TCL list.

  CTCLList ParameterList(&rInterp, pParameterList);
  StringArray Description; 
  vector<string> Parameters; 

  if(ParameterList.Split(Description) != TCL_OK) { // Bust the list apart.
    rResult = Usage();
    rResult += "Incorrect parameter description list format.";
    return TCL_ERROR;
  }
  //  Transform  the TCL list into a vector of names:


  for(UInt_t nPar = 0; nPar < Description.size(); nPar++) {
    Parameters.push_back(Description[nPar]);
  }

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator FilterDictionaryIterator = 
    pFilterDictionary->Lookup(pFilterName);
  CGatedEventFilter* pGatedEventFilter;

  if(FilterDictionaryIterator == pFilterDictionary->end()) { 

    // Filter not already present in FilterDictionary... we can make it.

    CGateContainer* pGateContainer = Api.FindGate(pGateName);

    if(pGateContainer) { 
      // Make sure Parameters exist, and retrieve their IDs.

      for(UInt_t i=0; i<Parameters.size(); i++) {
	CParameter* pParameter = Api.FindParameter(Parameters[i]);

	if(pParameter == (CParameter*)kpNULL) { 
	  rResult += "Error: Invalid parameter (" + Parameters[i] + ").";
	  return TCL_ERROR; 
	}
      }
      // At this point nothing can stop us from making the filter:


      pGatedEventFilter = new CGatedEventFilter;
      pGatedEventFilter->setGateContainer(*pGateContainer);
      pGatedEventFilter->setParameterNames(Parameters); 

      // Hard wire the formatter in this version:
      // TODO: provide mechanism to select formatter.
      //
      CFilterOutputStageFactory& factory(CFilterOutputStageFactory::getInstance());
      pGatedEventFilter->setOutputFormat(factory.create(defaultOutputFormat));

      pFilterDictionary->Enter(pFilterName, pGatedEventFilter);

      // Add the filter to the event sink.

      Api.AddEventSink(*pGatedEventFilter, SinkName(pFilterName).c_str());
    } else {
      rResult += "Error: Invalid gate (" + std::string(pGateName) + ").";
      return TCL_ERROR;
    }
  } else {
    rResult += "Error: Filter (" + 
      std::string(pFilterName) + ") already present.";
    return TCL_ERROR;
  }

  // Output result. 

  rResult +=  ListFilter(string(pFilterName), 
			 pGatedEventFilter); // Return filter name string in
				// std format.

  return TCL_OK;
}

/*!
  Command processor to delete a Filter.  If successful, the result is empty,
  otherwise it is an informative error message.
  \param rInterp (in):
     The interpreter running this command.
  \param rResult (in):
     The command result string (what is captured by e.g. [filter -delete xx].
  \param nArgs (in):
     Number of remaining command line parameters (must be 1 [name of the
     filter to delete]).
  \param pArgs (in):
     List of the parameters.

  \return an integer status (see operator()).

*/
Int_t CFilterCommand::Delete(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			     int nArgs, char* pArgs[]) {
  if(nArgs != 1) {
    rResult = Usage();
    return TCL_ERROR;
  }

  // Parse the filter description.
  const char* pFilterName = *pArgs;

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  if(pFilterDictionary->Lookup(pFilterName) == pFilterDictionary->end()) {
    rResult = "No such filter (" + std::string(pFilterName) + ").";
    return TCL_ERROR;
  }
  // Remove from event sink pipeline, dictionary and destroy the filter.

  SpecTcl& Api(*(SpecTcl::getInstance()));
  CGatedEventFilter* pFilter = 
    dynamic_cast<CGatedEventFilter*>(Api.RemoveEventSink(SinkName(pFilterName)));
  pFilterDictionary->Remove(pFilterName);
  
  // If the filter is enabled, disable it so that it flushes
  // etc.
  if (!pFilter) {
    rResult = "Name is not a filter";
    return TCL_ERROR;
  }
  if (pFilter->CheckEnabled()) {
    pFilter->Disable();
  }
  delete pFilter;

  return TCL_OK;
}
/*
  Enables a filter.  For a filter to function (to create output),
   it must:
   - Have been created.
   - Have an output file associated with it.
   - Have been enabled.
   
   \param rInterp (in):
      The interpreter that is running this command.
   \param rResult (out):
      The result string that is produced by this command.
   \param nArgs   (in):
      Number of remaining parameters (must be 1 the name of the filter).
   \param pArgs   (in)
      The remaining parameters.

   \return an integer TCL status (see operator()).
*/
Int_t CFilterCommand::Enable(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) {
  if(nArgs != 1) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Parse the filter description.
  const char* pFilterName = *pArgs;

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();

  CFilterDictionaryIterator p = pFilterDictionary->Lookup(pFilterName);
  if(p != pFilterDictionary->end()) {
    CGatedEventFilter* pEventFilter = p->second;
    pEventFilter->Enable();
    if(pEventFilter->CheckEnabled()) {
      rResult += ListFilter(p->first,pEventFilter);
      return TCL_OK;
    } else {
      rResult = "Error: Filter (" 
	      + std::string(pFilterName) + ") could not be enabled.";
      return TCL_ERROR;
    }
  } else {

    rResult = "Error: Invalid filter (" + std::string(pFilterName) + ").";
    return TCL_ERROR;
  }
  // It's a bug to land here:

  assert(0);
}
/*!
  Disable an existing filter.
For a filter to function (to create output),
   it must:
   - Have been created.
   - Have an output file associated with it.
   - Have been enabled.
   
   \param rInterp (in):
      The interpreter that is running this command.
   \param rResult (out):
      The result string that is produced by this command.
   \param nArgs   (in):
      Number of remaining parameters (must be 1 the name of the filter).
   \param pArgs   (in)
      The remaining parameters.

   \return an integer TCL status (see operator()).
 */
Int_t CFilterCommand::Disable(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			      int nArgs, char* pArgs[]) {
  if(nArgs != 1) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Parse the filter description.

  const char* pFilterName = *pArgs;

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator   p        = 
    pFilterDictionary->Lookup(pFilterName);
  if(p != pFilterDictionary->end()) {
    CGatedEventFilter* pEventFilter = p->second;
    pEventFilter->Disable();
    if(!(pEventFilter->CheckEnabled())) {
      rResult += ListFilter(p->first,pEventFilter);
      return TCL_OK;
    } else {
      rResult = "Error: Filter (" + std::string(pFilterName) + ") could not be disabled.";
      return TCL_ERROR;
    }
  } else {
    rResult = "Error: Invalid filter (" + std::string(pFilterName) + ").";
    return TCL_ERROR;
  }
  // Should not wind up here:

  assert(0);
}
/*!
    Regates a filter:  Given a filter, the current gate is
    replaced with a new one.
    \param rInterp (in):
       The interpreter that is running this command.
    \param rResult (out):
       On return will contain the text string the command returns.
    \param nArgs   (in):
       Number of parameters in the remaining command.  Sould be two:
       - Filter name
       - gate.
    \param pArgs   (in):
       The remaining command line parameters.

    \result integer status (see operator()).
*/
Int_t CFilterCommand::Regate(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			     int nArgs, char* pArgs[]) {
  if(nArgs != 2) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Parse the filter description.

  const char* pFilterName = *pArgs;
  pArgs++;
  const char* pGateName = *pArgs;

  // Locate the filter in the dictionary:

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator FilterDictionaryIterator = 
    pFilterDictionary->Lookup(pFilterName);

  // If the filter was found it can be regated.

  if(FilterDictionaryIterator != pFilterDictionary->end()) { 
    CGatedEventFilter* pGatedEventFilter = 
      FilterDictionaryIterator->second; 
    CGateContainer* pGateContainer = 
      ((CHistogrammer*)gpEventSink)->FindGate(pGateName); 

    // If the gate is found, nothing can fail:

    if(pGateContainer) { 
      pGatedEventFilter->setGateContainer(*pGateContainer);
      rResult += ListFilter(FilterDictionaryIterator->first, 
			    pGatedEventFilter);
      return TCL_OK;
    } else {
      rResult += "Error: Invalid gate (" + std::string(pGateName) + ").";
      return TCL_ERROR;
    }
  } else {
    rResult += "Error: Invalid filter (" + std::string(pFilterName) + ").";
    return TCL_ERROR;
  }
  // it's a bug to get here:

  assert(0);
}
/*!
   Associates an output file with a filter.  The output file will contain
   the filtered event data in filterdata format. 
   \param rInterp (in):
      The interpreter on which this command is running.
   \param rResult (out):
      The result string for the command.  On success this will be the output
      of ListFilter... otherwise an error message.
   \param nArgs   (in):
      The number of remaining command line parameters.  This should be 2:
      - The filter name
      - The gate name.
   \param pArgs   (in):
      The list of remaining command line parameters.

   \result an integer status value (See operator()).

   \note a side effect of changing the filename is that the filter is
         disabled...why is this???
*/
Int_t CFilterCommand::File(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			   int nArgs, char* pArgs[]) {
  if(nArgs != 2) {
    rResult = Usage();
    return TCL_ERROR;
  }

  // Parse the filter description.
  string Filename(*pArgs);
  Filename = rInterp.TildeSubst(Filename);
  const char* pFileName = Filename.c_str();
  pArgs++;
  const char* pFilterName = *pArgs;


  // Find the filter and gate in the dictionary.  We can only procede
  // if all of these are found.


  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator FilterDictionaryIterator = 
    pFilterDictionary->Lookup(pFilterName);

  if(FilterDictionaryIterator != pFilterDictionary->end()) { 
    CGatedEventFilter* pGatedEventFilter = 
      FilterDictionaryIterator->second; 
    string sFileName(pFileName);
    pGatedEventFilter->setFileName(sFileName); 

    rResult += ListFilter(FilterDictionaryIterator->first,
			  pGatedEventFilter);
    return TCL_OK;
  } else {
    rResult += "Error: Invalid filter (" + std::string(pFilterName) + ").";
    return TCL_ERROR;
  }
  assert(0);
}
/*!
   Lists the set of filters that matcha glob pattern.  If the
   glob pattern is not supplied,then all filters are listed.
   This is done as follows:  The filter dictionary is iterated over.
   For each filter that matches the pattern (supplied or defaulted), the
   filter is listed using ListFilter.
   The filters are listed in a TCL list where each element of the list is
   the output of ListFilter for that filter.

   \param rInterp (in):
       The interpreter the command is running on.
   \param rResult (out):
       The string the command returns.  This will be the set of filters
       that matches the pattern.
   \param nArgs   (in):
       The number of remaining command line parameters. (should be 0 or 1).
   \param pArgs   (in):
       The parameters themselves.

   \return  An integer status (see operator()).
*/
Int_t CFilterCommand::List(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			   int nArgs, char* pArgs[]) 
{
  const char* pPattern = "*";		// Default pattern is just *.
  if(nArgs == 1) {
    pPattern = *pArgs;
  }
  if(nArgs > 1) {
    rResult = Usage();
    return TCL_ERROR;
  }

  // Iterate thorugh  the dictionary lsting the filter results into 
  // a string that is later set to be the result:

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  CTCLString         OutputList; 
  CFilterDictionaryIterator i = pFilterDictionary->begin();
  while(i != pFilterDictionary->end()) {
    CTCLString          FilterName(i->first);
    CGatedEventFilter*  pFilter   = i->second;

    if(FilterName.Match(pPattern)) {
      OutputList.AppendElement(ListFilter(i->first, pFilter));
      OutputList.Append("\n");
    }
    i++;
  }
  rResult = string((const char*)OutputList);

  return TCL_OK;
}
/*!
    Set the output format of a filter.  The format of the command is:
    filter -format filtername format

    Where format is a format selector that is recognized by the filter format
    factory.

    \param rInterp - Reference to the interpreter running this command.
    \param rResult - Reference to the interpreter result object.
    \param nArgs   - Number of command words, should be exactly 3.
                     as the caller has sliced off the command verb.
    \param pArgs   - Pointers to the command words.

    \return int
    \retval TCL_OK - The command worked. The result will be the filter name.
    \retval TCL_ERROR - The command failed, the result will be an error message.
*/
Int_t
CFilterCommand::Format(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[])
{
  if (nArgs != 3) {
    rResult = "In correct number of parameters in filter -format: \n";
    rResult += Usage();
    return TCL_ERROR;
  }

  // Pull out the filter name and the output type and validate them:

  char*  name    = pArgs[1];
  string format  = pArgs[2];

  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator fIterator  = pFilterDictionary->Lookup(name);


  // Filter does not exist.

  if (fIterator == pFilterDictionary->end()) {
    rResult  = "Filter: ";
    rResult += name;
    rResult += " does not exist in filter -format command\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  // It's an error to do any of this if the filter is enabled:

  CEventFilter* pFilter = fIterator->second;
  if (pFilter->CheckEnabled()) {
    rResult  = "Filter: ";
    rResult += name;
    rResult += " is enabled, and filter -format can only be used on disabled filters\n";
    rResult += Usage();
    return TCL_ERROR;
  }

  // Try to get the formater:

  CFilterOutputStageFactory& outFactory(CFilterOutputStageFactory::getInstance());
  CFilterOutputStage*        pOutputStage = outFactory.create(format);
  if (!pOutputStage) {
    rResult  = "Filter format type: ";
    rResult += format;
    rResult += " does not exist in filter -format command";
    rResult += Usage();
    return TCL_ERROR;
  }



  pFilter->setOutputFormat(pOutputStage);
  rResult = name;
  return TCL_OK;
}

/*!
   Lists a filter given a pointer to the filter itself.
   See as well the overloaded version that first looks up a filter
   by name in the dictionary and then calls  us to do the dirty work.

   \param rName  (in):
      The name of the filter (this only has relevance in the filter
      dictionary.
   \param pFilter (in):
      A pointer to the filter to list.
   \return string - a description of the filter that consists of:
     - The filter name.
     - The gate set on the filter.
     - The name of the file to which the filter is writing (may be empty).
     - the names of the parameters  that are written by the filter.
     - The enable status of the filter.
*/
string CFilterCommand::ListFilter(const string& rName,
				  CGatedEventFilter* pFilter) 
{
  CTCLString List;
  
  List.AppendElement(rName);     // Filter name.
  List.AppendElement(pFilter->getGateName()); // GateContainer has Gate name.
  List.AppendElement(pFilter->getFileName()); // File name.
  vector<string> params = pFilter->getParameterNames();
  List.StartSublist();
  for(int i =0; i < params.size(); i++) {
    List.AppendElement(params[i]);
  }
  List.EndSublist();
  if(pFilter->CheckEnabled()) {
    List.AppendElement(std::string("enabled"));
  } else {
    List.AppendElement(std::string("disabled"));
  }

  List.AppendElement(pFilter->outputFormat());

  return string((const char*) List);
}
/*!
  Lists a filter given its name: Delegates to ListFilter(CGatedEventFilter*).
  \param name (in):
     The name of the filter.
  \return string description of the filter or empty if the filter does not
     exist.
*/
string
CFilterCommand::ListFilter(const string& name)
{
  CFilterDictionary* pFilterDictionary = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator i          = pFilterDictionary->Lookup(name);
  if(i == pFilterDictionary->end()) {
    return string("");
  }
  else {
    return ListFilter(name, i->second);
  }
  assert(0);
}
/*!
   Create the event sink name given the filter name:
*/
string
CFilterCommand::SinkName(string filterName)
{
  string result("Filter::");
  result += filterName;

  return result;
}
