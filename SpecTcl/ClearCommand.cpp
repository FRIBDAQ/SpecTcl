/*
    This software is Copyright by the Board of Trustees of Michigan
                         State University (c) Copyright 2014.
                     
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

//  CClearCommand.cpp
// Parses and dispatches the TCL clear
// command.  This command clears selected
// spectra.  The form of this command is:
// 
//    clear -all
//          All spectra are cleared.
//    clear namelist
//         The named spectra are cleared.
//    clear -id idlist
//         The spectra given by ID are cleared.
//
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
#include <config.h>
#include "ClearCommand.h"                               
#include "SpectrumPackage.h"
#include "TCLInterpreter.h"
#include "TCLResult.h"
#include "SpecTcl.h"

#include <stdio.h>
#include <string.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

struct SwitchEntry {
  const char*                  pSwitchText;
  CClearCommand::Switch  eSwitchValue;
};

static const SwitchEntry SwitchTable[] = {
  { "-all", CClearCommand::keAll },
  { "-channels", CClearCommand::keChannels},
  { "-stats",    CClearCommand::keStatistics},
  { "-id" , CClearCommand::keId }
};
static const UInt_t SwitchTableSize = sizeof(SwitchTable)/sizeof(SwitchEntry);


// Functions for class ClearCommand

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                     int nArgs, char* pArgs[] )
//  Operation Type:
//     Command Processor.
//
int 
CClearCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult,
			  int nArgs, char* pArgs[]) 
{
// Called by the TCL application framework
// when the clear command is encountered.
//  The remainder of the command is parsed
//  and dispatched to the appropriate function(s)
//   in the CSpectrumPackage interface class.
//
// Formal parameters:
//     CTCLInterpreter& rInterp:
//          Reference to the interpreter executing
//          this command.
//     CTCLResult&  rResult:
//           References the command's result string.
//     int nArgs:
//           Number of command parameters..
//    char* pArgs[]:
//           Pointers to the command parameters.
// Returns:
//    TCL_OK         - if all spectra are cleared.
//    TCL_ERROR - if one or more spectra could not be cleared.
//

  nArgs--;			// Don't pay attention to command name.
  pArgs++;

  // Clear has two sorts of parameters switches and spectrum identities.
  // We're going to:
  // - Build a set of switches.
  // - Build a vector of spectrum identities
  
  std::vector<std::string> spectrumIdentifiers;    // Names or ids.
  std::set<CClearCommand::Switch> switches;
  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();

  for(int i = 0; i < nArgs; i++) {
    CClearCommand::Switch sw = MatchSwitch(pArgs[i]);
    if (sw != keNotSwitch) {
        switches.insert(sw);
    } else {
        spectrumIdentifiers.push_back(std::string(pArgs[i]));
    }
  }
  
  // -all and a non-zero CSpectrum* vector is not allowed.
  
  if ((switches.count(keAll) > 0) && (spectrumIdentifiers.size() > 0)) {
    Usage(rResult);
    return TCL_ERROR;
  }
  
  // If the -id was supplied then ClearIdList, otherwise, ClearNameList.
  
  if (switches.count(keAll) > 0) {
    clearAll(switches);
    return TCL_OK;
  } else if (switches.count(keId)) {
    return clearIdList(rResult, switches, spectrumIdentifiers);  
  } else {
    return clearNameList(rResult, switches, spectrumIdentifiers);
  }
  
}

/////////////////////////////////////////////////////////////////////
//
//  Function:
//     void   Usage(CTLResult& rResult)
//  Operation type:
//     Protected Utility.
//
void
CClearCommand::Usage(CTCLResult& rResult)
{
  rResult += "Usage: \n";
  rResult += "   clear ?-stats? ?-channels? -all\n";
  rResult += "   clear ?-stats? ?-channels? name1 [name2 ...]\n";
  rResult += "   clear ?-stats? ?-channels? -id id1 [id2...]\n";
  rResult += "\n  Clears all or a selected set of spectra\n";
  rResult += "\n  Using -stats or -channels allows you to specify ";
  rResult += "clearing the statistics or channels or both.";
}
////////////////////////////////////////////////////////////////////
//
//  Function:
//    Switch MatchSwitch(const char* pSwitchText)
//  Operation type:
//    Protected utility (parsing).
//
CClearCommand::Switch
CClearCommand::MatchSwitch(const char* pSwitchText)
{
  for(int i = 0; i < SwitchTableSize; i++) {
    if(strcmp(pSwitchText, SwitchTable[i].pSwitchText) == 0) {
      return SwitchTable[i].eSwitchValue;
    }
  }
  return CClearCommand::keNotSwitch;
}


/**
 * clearAll
 *   Clears all spectra.  The switches determine what is actually
 *   cleared. See clearWhat for information about how that works.
 *
 *   @param switches - the set of switches.
 *   
 */
void CClearCommand::clearAll(std::set<Switch> switches)
{
    SpecTcl* pApi = SpecTcl::getInstance();
    SpectrumDictionaryIterator p = pApi->SpectrumBegin();
    std::pair<bool, bool> clearFlags = clearWhat(switches);
    while (p != pApi->SpectrumEnd()) {
        clearSpectrum(clearFlags, p->second);
        p++;
    }
}
/** clearIdList
 *     Clear spectra given a list of strings that are supposed to be spectrum ids.
 *     Note that this returns an error and does nothing if:
 *     - one of the ids is not an integer.
 *     - one of the ids does not identify an existing spectrum.
 *
 * @param  rResult - Interpreter result - set to an error message on bad return.
 * @param  switches - Switches that were present in the command.
 * @param  idStrings - Vector of strings that are supposed to be ids.
 * @return int TCL_OK - succes - no result, TCL_ERROR-  failed, result is error msg.
 */
int
CClearCommand::clearIdList(
    CTCLResult& rResult, std::set<Switch> switches,
    std::vector<std::string> idStrings
  )
  {
    SpecTcl* pApi = SpecTcl::getInstance();
    std::vector<CSpectrum*> spectra;            // Built up by looking up spectra.
    
    for (int i = 0; i < idStrings.size(); i++) {
        unsigned id;
        if (sscanf(idStrings[i].c_str(), "%u", &id) != 1) {
            rResult = "Invalid spectrum id: ";
            rResult += idStrings[i];
            return TCL_ERROR;
        } else {
            CSpectrum* pSpec = pApi->FindSpectrum(id);
            if (!pSpec) {
                rResult = "There is no spectrum with the id: ";
                rResult += idStrings[i];
                return TCL_ERROR;
            }
            spectra.push_back(pSpec);
        }
    }
    
    // All spectra found, clear them:
    
    std::pair<bool, bool> clearFlags = clearWhat(switches);
    for (int i = 0; i < spectra.size(); i++) {
        clearSpectrum(clearFlags, spectra[i]);
    }
    
    return TCL_OK;
  }

/**
 *   clearNameList
 *      Same as above, but the vector is a vector of spectrum names.
 * @param  rResult - Interpreter result - set to an error message on bad return.
 * @param  switches - Switches that were present in the command.
 * @param  names    - Vector of strings that are supposed to be spectrum names.
 * @return int TCL_OK - succes - no result, TCL_ERROR-  failed, result is error msg.
 */ 
int CClearCommand::clearNameList(
    CTCLResult& rResult, std::set<CClearCommand::Switch> switches,
    std::vector<std::string> names
  )
{
    SpecTcl* pApi = SpecTcl::getInstance();
    std::vector<CSpectrum*> spectra;            // Built up by looking up spectra.
    
    for (int i = 0; i < names.size(); i++) {
        
        CSpectrum* pSpec = pApi->FindSpectrum(names[i]);
        if (!pSpec) {
            rResult = "There is no spectrum with the name ";
            rResult += names[i];
            return TCL_ERROR;
        }
        spectra.push_back(pSpec);
    }
    
    // All spectra found, clear them:
    
    std::pair<bool,bool> clearFlags = clearWhat(switches);
    for (int i = 0; i < spectra.size(); i++) {
        clearSpectrum(clearFlags, spectra[i]);
    }
    
    return TCL_OK;

}
/**
 *  Given command switches, determines which things need clearing.
 *
 *  @param switches - command switches.
 *  @return std::pair<bool>  first - clear channels, second - clear stats.
 */
std::pair<bool, bool>
CClearCommand::clearWhat(std::set<Switch> switches)
{
    std::pair<bool, bool> result(false, false);
 
    if (switches.count(keChannels)) {
        result.first = true;
    }
 
    if (switches.count(keStatistics)) {
        result.second = true;
    }
    // If neither are present it's like both are
    
    if (! (result.first || result.second)) {
        result = std::pair<bool, bool>(true, true);
    }
    
    
    return result;
}

/**
 *  clearSpectrum
 *    Like the name says
 *
 *  @param clearFlags .first - clear channels, .second - clear stats.
 *  @param pSpec - Pointre to the CSpectrum object that makes up the spectrum.
 */
void
CClearCommand::clearSpectrum(std::pair<bool, bool> clearFlags, CSpectrum* pSpec)
{
    if (clearFlags.first) pSpec->Clear();
    if (clearFlags.second) pSpec->clearStatArrays();
}