//  CSpectrumCommand.cpp
// Implements the spectrum TCL command.
// This command has the following formats:
//
//   spectrum [-new] name stype {parameterlist} {reslist} [dtype]
//                   Creates a new spectrum.
//   spectrum -list [-byid]
//                    List all spectra [sorted by id]
//   spectrum -list name
//                    Lists a spectrum given the name.
//   spectrum -list -id nId
//                     Lists a spectrum given it's id.
//   spectrum -delete namelist
//   spectrum -delete -id idlist
//                     Deletes sets of spectra by id or name.
//    spectrum -delete -all
//                      Deletes all spectra.
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
// Change log:
//   July 3, 1999        - Use CreateSpectrum, rather than
//                         New1d, New2D etc.
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//


#include "SpectrumCommand.h"
#include "SpectrumPackage.h"
#include "TCLInterpreter.h"
#include "TCLResult.h"
#include "TCLCommandPackage.h"
#include "TCLList.h"
#include "TCLString.h"

#include <tcl.h>
#include <string.h>
#include <malloc.h>
#include <algorithm>
#include <vector>


static const char* Copyright = 
"CSpectrumCommand.cpp: Copyright 1999 NSCL, All rights reserved\n";

typedef std::vector<std::string>::iterator StrVectorIterator;

struct SwitchInfo {
  char*   pSwitch;
  CSpectrumCommand::Switch  Value;
};
static const SwitchInfo SwitchTable[] = {
  { "-new",    CSpectrumCommand::keNew},
  { "-list",   CSpectrumCommand::keList},
  { "-id",     CSpectrumCommand::keId},
  { "-delete", CSpectrumCommand::keDelete},
  { "-all",    CSpectrumCommand::keAll},
  { "-byid",   CSpectrumCommand::keById}
};

static const UInt_t SwitchTableSize = sizeof(SwitchTable)/sizeof(SwitchInfo);

//  Local helper clases:
//
class CompareIds		// Function object.. compares ids of spectra.
{				// given property strings.
  CSpectrumCommand* pCommand;
public:
  CompareIds(CSpectrumCommand* p) : pCommand(p) {}
  int operator()(const std::string& i,const std::string& j) {
    return (pCommand->ExtractId(i) < pCommand->ExtractId(j));
  }
};

class CompareNames		// Function object.. compares names of spectra
{				// given property strings.
  CSpectrumCommand* pCommand;
public:
  CompareNames(CSpectrumCommand* p) : pCommand(p) {}
  int operator()(const std::string& i,const std::string& j) {
    std::string s1 = pCommand->ExtractName(i);
    std::string s2 = pCommand->ExtractName(j);
    return (strcmp(s1.c_str(), s2.c_str()) < 0);
  }
};
// Functions for class CSpectrumCommand

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterpreter, CTCLResult& rResult,
//                      int  nArgs, char* pArgs[] )
//  Operation Type:
//     Functional operator
//
int 
CSpectrumCommand::operator()(CTCLInterpreter& rInterpreter,
			     CTCLResult& rResult, int  nArgs, char* pArgs[]) 
{
// Called by the TCL subsystem when the
// parameter command is being executed.
//
// Formal Parameters:
//    CTCLInterpreter& rInterpreter:
//       The interpreter executing the command.
//    CTCLResult& rResult:
//        Represents the result string.
//    UInt_t nArgs:
//         Number of command parameters.
//    char* pArgs[]:
//         Array of arguments.
// Returns:
//      TCL_OK        - If the command executed correctly.
//      TCL_ERROR     - If not.
//
  nArgs--;
  pArgs++;

  if(nArgs <= 0) {
    Usage(rResult);
    return TCL_ERROR;
  }
  //  The first parameter is either a switch or the name of a spectrum to
  //  create.  Parse it out and based on the result, break out into the
  //  appropriate function.

  switch(MatchSwitch(pArgs[0])) {
  case keList:			// List spectra.
    nArgs--;
    pArgs++;
    return List(rInterpreter, rResult, nArgs, pArgs);
  case keDelete:		// Delete spectra.
    nArgs--;
    pArgs++;
    if(nArgs <= 0) {		// Delete requires at least one more param..
      Usage(rResult);
      return TCL_ERROR;
    }
    return Delete(rInterpreter, rResult, nArgs, pArgs);
  case keNew:			// Explicit new spectrum.
    nArgs--;
    pArgs++;
  case keNotSwitch:		// Implicit new spectrum.
    if((nArgs != 4) && (nArgs != 5)) { // Optional 5'th param is data type.
      Usage(rResult);
      return TCL_ERROR;
    }
    return New(rInterpreter, rResult, nArgs, pArgs);
  default:			// Invalid switch in context.
    Usage(rResult);
    return TCL_ERROR;
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t New ( TCLInterpreter& rInterpreter, TCLResult& rResult,
//                int nArgs, char* pArgs[] )
//  Operation Type:
//     Utility
//
Int_t 
CSpectrumCommand::New(CTCLInterpreter& rInterpreter, CTCLResult& rResult,
		      int nArgs, char* pArgs[]) 
{
// Handles subparsing and dispatching
// associated with the spectrum -new command.
// 
// Formal Parameters:
//     CTCLInterpreter& rInterpreter:
//             Refers to the interpreter running the
//             command.
//      CTCLResult&  rResult:
//            Refers to the Result string.
//      int   nArgs:
//           # remaining command line arguments, should be 4
//      char* pArgs[]
//           pointers to the remaining command line arguments.
// Returns:
//      TCL_OK     - if the parameter was created.
//      TCL_ERROR  - Otherwise.

  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();
  char* pName = pArgs[0];
  char* pType = pArgs[1];

  // Figure out if there's a data type parameter on the line.

  char* pDataType = (char*)kpNULL;
  if(nArgs == 5) pDataType = pArgs[4];
  
  // The only parameters which need pre-processing before calling the package's
  // create are the parameter list and the resolution list.
  //
  // The parameter list is split, then made into a vector of names.
  //

  int                      nListElements;
  char**                   ppListElements;
  std::vector<std::string> vParameters;

  CTCLList lParameters(&rInterpreter, pArgs[2]);
  lParameters.Split(nListElements, &ppListElements);
  rPack.GetNameList(vParameters, nListElements, ppListElements);
  free(ppListElements);

  //  The Resolution list is split into a vector of resolution numbers.
  //
  std::vector<UInt_t> vResolutions;

  CTCLList lResolutionList(&rInterpreter, pArgs[3]);
  lResolutionList.Split(nListElements, &ppListElements);
  if(rPack.GetNumberList(rResult, vResolutions, 
			  nListElements, ppListElements)) {
     delete []ppListElements;
     return TCL_ERROR;
   }
   delete []ppListElements;
   return rPack.CreateSpectrum(rResult,  pName, pType, vParameters, vResolutions,
                               pDataType);
   
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t List ( CTCLInterpeter& rInterp, CTCLResult& rResult, int nArgs, char* Args[] )
//  Operation Type:
//     Utility
//
Int_t 
CSpectrumCommand::List(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		       int nArgs, char* pArgs[]) 
{
// Handles the subparsing and dispatching
// for the parameter -list command.
//
//  Formal parameters:
//      CTCLInterpreter& rInterp:
//              Refers to the interpreter.
//      CTCLResult&       rResult:
//               Refers to the command result string.
//      int nArgs:
//                Number of command parameters past the -list switch.
//      char* pArgs[]:
//                  Pointers to the arguments.
//

  CSpectrumPackage& rPack = (CSpectrumPackage& )getMyPackage();

  // If there are no parameters, then we just list them all.

  if(nArgs == 0) {		// List with sort by name.
    std::vector<std::string> vDescriptions;
    rPack.ListSpectra(vDescriptions);
    SortSpectraByName(vDescriptions);
    VectorToResult(rResult, vDescriptions);
    return TCL_OK;
  }
  // If there are parameters, they must be switches or a parameter name:
  // The legal switchs are:
  //    -id      - List properties of following ident.
  //    -byid    - List all spectra sorted by id.
  // 
  std::vector<std::string> vDescriptions;
  Int_t nId;
 
  switch(MatchSwitch(pArgs[0])) {
  case keId:			// List 1 spectrum given ident.
    nArgs--;
    pArgs++;
    if((ParseInt(pArgs[0], &nId) != TCL_OK) || (nId < 0)) {
      rResult = "Invalid spectrum id ";
      rResult += pArgs[0];
      rResult += "\n";
      return TCL_ERROR;
    }
    return rPack.ListSpectrum(rResult, nId);

  case keById:			// List all spectra sorted by ident.
    if(nArgs > 1) {		// Too many command parameters...
      rResult = "Too many command line parameters\n";
      Usage(rResult);
      return TCL_ERROR;
    }
    rPack.ListSpectra(vDescriptions);
    SortSpectraById(vDescriptions);
    VectorToResult(rResult, vDescriptions);
    return TCL_OK;

  case keNotSwitch:		// List one spectrum given the name.
    return rPack.ListSpectrum(rResult, pArgs[0]);

  default:			// Invalid switch in context.
    Usage(rResult);
    return TCL_ERROR;
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t Delete ( TCLInterpreter& rInterp, TCLResult& rResult, 
//                   int nArgs, char* pArgs[] )
//  Operation Type:
//     Utillity
//
Int_t 
CSpectrumCommand::Delete(CTCLInterpreter& rInterp, CTCLResult& rResult,
			 int nArgs, char* pArgs[]) 
{
// Parses and dispatches the spectrum -delete
//  subcommand.  This command can delete
//   lists of spectra, by id or name, and can also
//   delete all spectra.
// 
// Formal Parameters:
//      TCLInterpreter&  rInterp:
//          References the interpreter executing
//          this command.
//      TCLResult&   rResult:
//          References the result string of the
//          interpreter.
//       int nArgs:
//          number of command arguments following the -delete
//          switch.
//       char* pArgs[]:
//          Array of pointers to the arguments.
//

  CSpectrumPackage& rPack = (CSpectrumPackage&) getMyPackage();

  if(nArgs <= 0) {		// Must be at least one parameter.
    Usage(rResult);
    return TCL_ERROR;
  }
  // The next parameter is either the beginning of a name list or one of the
  // following swiches:
  //   -id        - Indicates the beginning of a list of id's to delete.
  //   -all       - Indicating all spectra are to be deleted.
  //


  std::vector<UInt_t> vIds;
  std::vector<std::string> vNames;

  switch(MatchSwitch(pArgs[0])) {
  case keId:			// Delete given id list.
    nArgs--;
    pArgs++;
    if(rPack.GetNumberList(rResult, vIds, nArgs, pArgs)) {
      Usage(rResult);
      return TCL_ERROR;
    }
    return rPack.DeleteList(rResult, vIds);
  case keAll:			// Delete all spectra.
    if(nArgs != 1) {
      rResult = "Too many parmaeters\n";
      Usage(rResult);
      return TCL_ERROR;
    }
    rPack.DeleteAll();
    return TCL_OK;

  case keNotSwitch:		// Delete name list.
    CSpectrumPackage::GetNameList(vNames, nArgs, pArgs);
    return rPack.DeleteList(rResult, vNames);

  default:			// Invalid switch in context.
    Usage(rResult);
    return TCL_ERROR;
  }

}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    void   Usage(CTCLResult& rResult)
// Operation Type:
//    Protected Utility
//
void 
CSpectrumCommand::Usage(CTCLResult& rResult)
{
  //  Adds a description of the command set implemented by this
  //  processor to the result string.
  //

  rResult += "Usage: \n";
  rResult += "  spectrum [-new] name type { parameters... } { resolutions ...} [datatype]y\n";
  rResult += "  spectrum -list [-byid]\n";
  rResult += "  spectrum -list name\n";
  rResult += "  spectrum -list -id id\n";
  rResult += "  spectrum -delete name1 [name2...]\n";
  rResult += "  spectrum -delete -id id1 [id2...]\n";
  rResult += "  spectrum -delete -all\n";
  rResult += "\n  The spectrum command creates and deletes spectra as well\n";
  rResult += "  as listing their properties.";

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     Switch MatchSwitch(const char* pSwitch)
//  Operation Type:
//     Protected utility, parsing.
CSpectrumCommand::Switch
CSpectrumCommand::MatchSwitch(const char* pSwitch)
{
  // Match pSwitch agaist the legal set of switches.
  // Returns the value of the Switch enum corresponding to pSwitch.
  // note that keNotSwitch is returned if there's no match.

  for(UInt_t i = 0; i < SwitchTableSize; i++) {
    if(strcmp(pSwitch, SwitchTable[i].pSwitch) == 0) {
      return SwitchTable[i].Value;
    }
  }
  return keNotSwitch;
}
////////////////////////////////////////////////////////////////////////
//
//  Function:
//     void   SortSpectraById(std::vector<std::string>& rvProperties)
//  Operation Type:
//     Utility.
//
void
CSpectrumCommand::SortSpectraById(std::vector<std::string>& rvProperties)
{
  // Sorts a vector of spectrum property strings by spectrum Id.  The
  // property strings are lists of the form:
  //    id name { parameters... } { resolutions...}
  //
  sort(rvProperties.begin(), rvProperties.end(), CompareIds(this));
}
////////////////////////////////////////////////////////////////////////
//
//  Function:
//     void   SortSpectraByName(std::vector<std::string>& rvProperties)
//  Operation type:
//     protected utility.
//
void 
CSpectrumCommand::SortSpectraByName(std::vector<std::string>& rvProperties)
{
  //  Sorts a vector of spectrum property strings by spectrum name.
  //  the property strings are lists of the form:
  //      id name { parameters... } { resolutions... }
  //
 
  sort(rvProperties.begin(), rvProperties.end(), CompareNames(this));
}
////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void   VectorToResult(CTCLResult& rResult, 
//                          std::vector<std::string>& rvStrings)
//  Operation Type:
//     protected Utility function.
//
void
CSpectrumCommand::VectorToResult(CTCLResult& rResult, 
				 std::vector<std::string>& rvStrings)
{
  // Appends a vector of strings as a list to rResult.
  //

  CTCLString List;
  for(UInt_t i = 0; i < rvStrings.size(); i++) {
    List.AppendElement(rvStrings[i]);
    List.Append("\n");
  }
  rResult += (const char*)List;
}
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//     UInt_t      ExtractId(const std::string& rProperties)
//  Operation Type:
//     Public Utility.
//
UInt_t
CSpectrumCommand::ExtractId(const std::string& rProperties)
{
  // Takes a spectrum properties string of the form:
  //     id name {parameters..} {resolutions...}
  // and returns the id of the spectrum.
  // Assumes:  The list is properly formatted.
  //
  CTCLList PropertyList(getInterpreter(), rProperties);
  int nElements;
  char **pElements;
  PropertyList.Split(nElements, &pElements);

  Int_t nId;
  ParseInt(pElements[0], &nId);
  free((char*)pElements);
  return (UInt_t)nId;
}
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//      std::string ExtractName(const std::string& rProperties)
//  Operation type:
//      Public utility
//
std::string
CSpectrumCommand::ExtractName(const std::string& rProperties)
{
  // Returns the spectrum name from a spectrum property list of the form:
  //
  //     id name {parameters..} {resolutions...}
  // Assumes:  The list is properly formatted.
  //
 
  CTCLList PropertyList(getInterpreter(), rProperties);
  int nElements;
  char** pElements;
  PropertyList.Split(nElements, &pElements);
  
  std::string Result(pElements[1]);
  free((char*)pElements);
  return Result;

}
