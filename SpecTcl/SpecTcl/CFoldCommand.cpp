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

///////////////////////////////////////////////////////////
//  CFoldCommand.cpp
//  Implementation of the Class CFoldCommand
//  Created on:      22-Apr-2005 12:54:15 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
#include <config.h>
#include "CFoldCommand.h"
#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <GateContainer.h>
#include <Spectrum.h>
#include <SpecTcl.h>
#include <TrueGate.h>
#include <CGammaSpectrum.h>
#include <CFold.h>
#include <vector>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Static storage:

static CTrueGate truegate;

//
// Beast of a gate container used to remove a folding from a spectrum.
//
CGateContainer CFoldCommand::Unfolded("unfolded", 666, truegate);

/**
 *  The base class destructor will unregister the command.  We don't need to 
 *  do anything.
 */
CFoldCommand::~CFoldCommand()
{
}


/**
 * Constructors with arguments
 *     @param pInterp
 *           The interpreter on which we register this command.
 * 
 */
CFoldCommand::CFoldCommand(CTCLInterpreter* pInterp) :
  CTCLProcessor("fold", pInterp)
{
  Register();
}


/**
 * Top level dispatching of the 'fold' command.  The fold command is an
 * ensemble commands that includes:
 *   - fold -apply foldname spectrum ...
 *   - fold -list  ?pattern?
 *   - fold -remove spectrum
 *      
 * @param rInterp
 *    The interpreter that is running this command.
 * @param rResult
 *    The result object for rInterp.
 * @param argc
 *    Number of command line parameters.  Note that this includes the
 *    command keyword.
 * @param argv
 *    List of command line parameters.  Note that argv[0] -> "fold"
 * 
 */
int 
CFoldCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			 int argc, char** argv)
{
  
  // Skip the command keyword:
  
  argc--;
  argv++;

  // Pull out the switch keyword for the ensemble:
  //
  if(!argc) {
     rResult   = "Missing subcommand\n";
     rResult  +=  Usage();
     return TCL_ERROR;
  }
  //
  string subcommand(argv[0]);
  argc--;
  argv++;
  //
  // Dispatch to the appropriate function depending on the  command keyword... 
  // or error if there is no match:
  //
  if(subcommand == "-apply") {
     return applyFold(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-remove") {
     return removeFold(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-list") {
     return listFolds(rInterp, rResult, argc, argv);
  }
  else {
    rResult   = "Unrecognized subcommand: ";
    rResult += subcommand;
    rResult += "\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  

}


/**
 * Applies a fold to the list of gamma spectra.. In the event of an error gives a
 * reason for this.  All validation is done prior to any changes so the command is
 * either totally successful or a total failure.
 * @param rInterp
 *    The interpreter that is running this command.
 * @param rResult
 *    The result object for rInterp.
 * @param argc
 *    Number of command line parameters; the fold command and the -
 *    apply switch have been eaten up. The remaining items should be a fold 
 *    name and a list of spectra.
 * @param argv
 *    List of command line parameters.
 * 
 */
int CFoldCommand::applyFold(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			    int argc, char** argv)
{

  SpecTcl& api(*(SpecTcl::getInstance()));
  //
  // We need to have a fold and at least one spectrum to apply it to.
  //
  string               gateName;
  vector<string>       spectrumNames;
  
  // Validate the parameters:

  if(!argc) {
     rResult   = "fold -apply  requires a gate name\n";
     rResult += Usage();
     return TCL_ERROR;
  }
  gateName = argv[0];
  argv++;
  argc--;
  
  if(!argc) {
     rResult = "fold -apply requires at least one spectrum name\n";
     rResult += Usage();
     return TCL_ERROR;
  }
  // Save the spectrum names to which we will be applying this gate as a fold
  // for validation:

  for(int i =0; i < argc; i++) {
     spectrumNames.push_back(argv[i]);
  }
  
  
  //
  // In order to apply this fold, the gate must exist and be a gamma gate.
  //
  CGateContainer* pGateC = api.FindGate(gateName);
  if(!pGateC) {
    rResult   = "fold -apply : Gate : ";
    rResult += gateName;
    rResult += " does not exist.\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  if(!isGammagate(pGateC)) {
    rResult   = "fold -apply : Gate: ";
    rResult +=  gateName;
    rResult += "  is not a gamma gate\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  

  //
  // All the spectra must exist and be  gamma spectra .. the names of the
  //   spectra that don't meet these  criteria are put in the badSpectra
  //  vector, while the others are turned into CGammaSpectrum* and
  //  put in the goodSpectra vector.
  //
  vector<string>           badSpectra;
  vector<CGammaSpectrum*>  goodSpectra;

  for(int i =0; i < spectrumNames.size(); i++) {
    CSpectrum* pSpectrum = api.FindSpectrum(spectrumNames[i]);
    if (!pSpectrum) {
      badSpectra.push_back(spectrumNames[i]);
    } 
    else if (!isGammaSpectrum(pSpectrum)) {
      badSpectra.push_back(spectrumNames[i]);
    } 
    else {
      goodSpectra.push_back(dynamic_cast<CGammaSpectrum*>(pSpectrum));
    }
  }
  
  if (badSpectra.size()) {
    rResult = "fold -apply : the spectra in the list below either don't exist ";
    rResult += " or  are not gamma spectra: \n";
    for(int i =0; i < badSpectra.size(); i++) {
      rResult += badSpectra[i];
      rResult += "\n";
    }
    rResult += Usage();
    return TCL_ERROR;
  }
  // Now we know the folding will work so we can loop over the spectrum list
  // and apply the folds.
  //
  for(int i =0; i < goodSpectra.size(); i++) {
    goodSpectra[i]->Fold(pGateC);
  }
  
  return TCL_OK;
}


/**
 * Produces a list of the spectra with folds and the folds applied to them.  The
 * TCL list returned has pairs as elements where each pair contains the spectrum
 * name, and the name of the gate involved in the fold.
 * @param rInterp
 *    Refers to the interpreter that is running this command.
 * @param rResult
 *    Refers to the result object for the interpreter that is
 *    running this command.
 * @param argc
 *    Number of parameters remaining in the tail of the command.  This
 *    should be 0 or 1.
 * @param argv
 *    List of command parameters in the tail of the command.  This
 *    should either be empty or a single string that is a glob pattern that must be
 *    matched in order to list the folds on a spectrum.
 * 
 */
int 
CFoldCommand::listFolds(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			    int argc, char** argv)
{

  // Figure out the search pattern.
  
  string pattern("*");             // Default match pattern.
  if (argc) {
    pattern = argv[0];          // User supplied pattern.
    argc--;
    argv++;
  }
  
  //// Any more parameters is bad:
  //
  if (argc) {
    rResult   = "Too many command line parameters fold -list ";
    rResult += pattern;
    rResult += " ";
    rResult += argv[0];
    rResult += " ...\n";
    rResult += Usage();
    return TCL_ERROR;
  }

  // Now loop through the spectra looking for those that match the pattern and
  // have folds:

  SpecTcl& api(*(SpecTcl::getInstance()));
  
  CTCLString  answer;                  //  Build up the answer string here.
  SpectrumDictionaryIterator i = api.SpectrumBegin();

  while (i != api.SpectrumEnd()) {
    string name = i->first;
    if(Tcl_StringMatch(name.c_str(), pattern.c_str())) {
      CSpectrum* pSpectrum = i->second;
      
      // Must be a gamma spectrum and have a fold to list:
      
      if( isGammaSpectrum(pSpectrum)) {
	CGammaSpectrum *pG = dynamic_cast<CGammaSpectrum*>(pSpectrum);
	if (pG) {	 // Since snapshot spectra will be gamma but not cast!.
	  if(pG->haveFold()) {
	    answer.StartSublist();
	    answer.AppendElement(name);
	    answer.AppendElement((pG->getFold())->getFoldName());
	    answer.EndSublist();
	  }
	}
      }
    }
    i++;
  }
    
  rResult = (const char*)(answer);
  return TCL_OK;
  
  
}


/**
 * Removes folding from the specified spectrum.  If the spectrum is not a gamma
 * spectrum or the spectrum has no fold in the first place this is an error... no
 * fold includes the case where the underlying gate has been replaced by a non
 * folding gate.
 * @param rInterp
 *     Interpreter running this command.
 * @param rResult
 *     Result string  for the intepreter running this command.
 * @param argc
 *     Count of the arguments remaining on the command line.  Should be
 *      a spectrum name only.
 * @param argv
 *     List of pointers to remaining command line parameters.
 * 
 */
int 
CFoldCommand::removeFold(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			 int argc, char** argv)
{

  // Get the spectrum and ensure there are no extra
  // params nor too few params.
  //
  if (!argc) {
    rResult += "fold -remove ... too few command parameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  string spectrumName(argv[0]);
  argc--;
  argv++;
  
  
  if (argc) {
    rResult += "fold -remove ";
    rResult += spectrumName;
    rResult += " ... too many command parameters";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  // Locate the spectrum and ensure it's a gamma spectrum.
  
  SpecTcl& api(*(SpecTcl::getInstance()));
  
  CSpectrum* pSpectrum = api.FindSpectrum(spectrumName);
  if (pSpectrum == (CSpectrum*)kpNULL) {
    rResult += "fold -remove ";
    rResult += spectrumName;
    rResult += " : Spectrum not found";
    return TCL_ERROR;
  }  
  
  if(!isGammaSpectrum(pSpectrum)) {
    rResult += "fold -remove ";
    rResult += spectrumName;
    rResult += " : Spectrum is not a gamma spectrum.";
    return TCL_ERROR;
  }
  
  // Now if it has a fold, remove it by replacing it with
  // a TRUE gate container... since that's not a gamma gate,
  // The spectrum will revert to unfolded behavior.
  //
  CGammaSpectrum* pG = dynamic_cast<CGammaSpectrum*>(pSpectrum);
  pG->Fold(&CFoldCommand::Unfolded);
  
  return TCL_OK;
  

}


/**
 * Provide usage information in the event of an error.
 */
std::string 
CFoldCommand::Usage()
{

  string info    = "Usage:\n";
  info   += "     fold -apply gate spectrum ...\n";
  info   += "     fold -list ?pattern?\n";
  info   += "     fold -remove spectrum\n";
  info   += "Where:\n\n";
  info   += " gate     - is a gamma gate.\n";
  info   += " spectrum - is a gamma spectrum\n";
  return info;
  
  

}


/**
 * Returns true if the spectrum given is a gamma spectrum.
 * @param pSpectrum
 *    Pointer to the spectrum to check.
 * 
 */
Bool_t 
CFoldCommand::isGammaSpectrum(CSpectrum* pSpectrum)
{

  SpectrumType_t  t = pSpectrum->getSpectrumType();
  return ((t == keG1D) || (t == keG2D));
  

}


/**
 * Returns true if the specified gate container points at a gamma gate.
 * @param pGateContainer
 *    Pointer to the gate's container.
 * 
 */
Bool_t 
CFoldCommand::isGammagate(CGateContainer* pGateContainer)
{

  string type = (*pGateContainer)->Type();
  
  return ((type == "gb") || (type =="gc") || (type == "gs"));
  
  
}


