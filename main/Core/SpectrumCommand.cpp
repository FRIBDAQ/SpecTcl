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


//
// Header Files:
//
#include <config.h>

#include "SpectrumCommand.h"
#include "SpectrumPackage.h"
#include "TCLInterpreter.h"
#include "TCLResult.h"
#include "TCLCommandPackage.h"
#include "TCLList.h"
#include "TCLString.h"
#include "TCLObject.h"
#include <RangeError.h>
#include <Exception.h>
#include "SpecTcl.h"
#include <Histogrammer.h>
#include <iostream>
#include "GateContainer.h"
#include "CompoundGate.h"

#include <tcl.h>
#include <string.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <algorithm>
#include <vector>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


static const int MAXRESBITS(32); // Maximum resolution value.

typedef vector<string>::iterator StrVectorIterator;

struct SwitchInfo {
  const char*   pSwitch;
  CSpectrumCommand::Switch  Value;
};
static const SwitchInfo SwitchTable[] = {
  { "-new",    CSpectrumCommand::keNew},
  { "-list",   CSpectrumCommand::keList},
  { "-id",     CSpectrumCommand::keId},
  { "-delete", CSpectrumCommand::keDelete},
  { "-all",    CSpectrumCommand::keAll},
  { "-byid",   CSpectrumCommand::keById},
  { "-showgate",   CSpectrumCommand::keShowGate},
  { "-trace",      CSpectrumCommand::keTrace}
};

static const string defaultTrace("proc __defaultSpectrumTrace name {}; __defaultSpectrumTrace");

static const UInt_t SwitchTableSize = sizeof(SwitchTable)/sizeof(SwitchInfo);

//  Local helper clases:
//
class CompareIds		// Function object.. compares ids of spectra.
{				// given property strings.
  CSpectrumCommand* pCommand;
public:
  CompareIds(CSpectrumCommand* p) : pCommand(p) {}
  int operator()(const string& i,const string& j) {
    return (pCommand->ExtractId(i) < pCommand->ExtractId(j));
  }
};

class CompareNames		// Function object.. compares names of spectra
{				// given property strings.
  CSpectrumCommand* pCommand;
public:
  CompareNames(CSpectrumCommand* p) : pCommand(p) {}
  int operator()(const string& i,const string& j) {
    string s1 = pCommand->ExtractName(i);
    string s2 = pCommand->ExtractName(j);
    return (strcmp(s1.c_str(), s2.c_str()) < 0);
  }
};



// The spectrum observer:

typedef CSpectrum* SpectrumPointer;

class SpectrumTraceObserver : public SpectrumDictionaryObserver
{

private:
  CSpectrumCommand*    m_pCommand;
public:
  SpectrumTraceObserver(CSpectrumCommand* commandObject) :
    m_pCommand(commandObject) {}
  
  virtual void onAdd(string name, CSpectrum*&  pSpectrum) {
    m_pCommand->traceAdd(name, pSpectrum);
  }
  virtual void onRemove(string name, CSpectrum*&  pSpectrum) {
    m_pCommand->traceRemove(name, pSpectrum);
  }
};

// Functions for class CSpectrumCommand

/*!
   Construct the command.  We must intialize the base class
   and set up our member data.  We will also
   set up a trace observer on the histogrammer.
*/
CSpectrumCommand::CSpectrumCommand (CTCLInterpreter* pInterp) :

  CTCLPackagedObjectProcessor(*pInterp, "spectrum", true),
  m_fTracing(false),
  m_pObserver(new SpectrumTraceObserver(this))
  
{ 
  m_createTrace = defaultTrace;
  m_createTrace.Bind(pInterp);

  m_removeTrace = defaultTrace;
  m_removeTrace.Bind(pInterp);
  
  SpecTcl* pApi = SpecTcl::getInstance();
  pApi->addSpectrumDictionaryObserver(m_pObserver);

}

/*
   Destroy the command.
*/
CSpectrumCommand::~CSpectrumCommand()
{
  SpecTcl* pApi = SpecTcl::getInstance();
  pApi->removeSpectrumDictionaryObserver(m_pObserver);
  delete m_pObserver;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterpreter, CTCLResult& rResult,
//                      int  nArgs, char* pArgs[] )
//  Operation Type:
//     Functional operator
//
int 
CSpectrumCommand::operator()(CTCLInterpreter& rInterpreter, std::vector<CTCLObject>& objv)
{
// Called by the TCL subsystem when the
// parameter command is being executed.
//
// Formal Parameters:
//    CTCLInterpreter& rInterpreter:
//       The interpreter executing the command.
//    std::vector<CTCLOjbect>& objv:
//       Object encapsulated comamnd words.
// Returns:
//      TCL_OK        - If the command executed correctly.
//      TCL_ERROR     - If not.
//
  // Create nArgs and pArgs in order to make the port -> object processor simpler:

  std::vector<std::string> words;
  std::vector<const char*> pWords;

  // Due to lifetimes and how c_str behaves we need two loops not one:

  for (auto& word : objv) {
    words.push_back(std::string(word));
  }
  for (int i =0; i < words.size(); i++) {
    pWords.push_back(words[i].c_str());
  }
  int nArgs = words.size();
  auto pArgs = pWords.data();

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
    return List(rInterpreter, nArgs, pArgs);
  case keDelete:		// Delete spectra.
    nArgs--;
    pArgs++;
    if(nArgs <= 0) {		// Delete requires at least one more param..
      Usage(rResult);
      return TCL_ERROR;
    }
    return Delete(rInterpreter, nArgs, pArgs);
  case keNew:			// Explicit new spectrum.
    nArgs--;
    pArgs++;
  case keNotSwitch:		// Implicit new spectrum.
    if((nArgs != 4) && (nArgs != 5)) { // Optional 5'th param is data type.
      Usage(rInterp);
      return TCL_ERROR;
    }
    return New(rInterpreter, nArgs, pArgs);
  case keTrace:
    nArgs--; pArgs++;		// advance past the -trace switch.
    return Trace(rInterpreter, nArgs, pArgs);
  default:			// Invalid switch in context.
    Usage(rResult);
    return TCL_ERROR;
  }
}
/*!
  Parses the spectrum -new command.  This command attempts to 
  create a new spectrum.   The command line is parsed and
  passed to the spectrum package that actually interacts with
  the histogramming kernel to create a spectrum.  The format of
  the spectrum -new command is:
  \verbatim
     spectrum ?-new? name type {parameters...} {axes...}
  \endverbatim
 
  Where:
  - name is the name of the spectrum.
  - type is the type of the spectrum.
  - {parameters...} is the list of parameters required to increment
     the spectrum (the number of these depends on the type of spectrum
     being created.
  - {axes...} is a list of axis definitions.  The number of these required
    depends on the type of spectrum.

    An axis definition can one of the following two forms:

    \verbatim
    nbits
    
    {low high nchannels}
    
    \endverbatim

    where all of these can be expressions and:
    - nbits - evaulates to the number of bits of resolution in 
              the spectrum, the  number of channels is 1 << nbits.
    - nchannels - evaluates to an integer number of channels on 
              this axis.
    - low   - evaluates to a floating point that represents the
              the parameter value that corresponds to channel 0
              of this axis.
    - high  - evaluates to a floating point that represents the
              parameter value that corresponds to the last channel
	      on this axis.

    If the parameters have been specified as mapped parameters,
    and the secon form of an axis definintion is used, the mapped
    value of the parameter is them mapped to the range specified
    by the axis. 

    \param <tt> rInterpreter (CTCLInterpreter& [in])</tt>
       The TCL interpreter that is running this command.
    
    \param <tt> nArgs        </tt>
       Number of parameter remaining on the command line.
    \param <tt> pArgs        </tt>
       Pointer to the number of parameters remaining on the command
        line.

    nArgs and pArgs have been setup so that the first remaining 
parameter is the name of the spectrum.

    \retval Int_t
       Status of the command.  This can be one of:
       - TCL_OK    - The command generated a spectrum.
       - TCL_ERROR - The command was not able to generate a
          spectrum.  In this case rResult will contain a textual
	  errror message.


*/
Int_t 
CSpectrumCommand::New(CTCLInterpreter& rInterpreter, 
		      int nArgs, const char* pArgs[]) 
{

  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  std::string rResult;

  char* pName = pArgs[0];
  char* pType = pArgs[1];

  // Figure out if there's a data type parameter on the line.

  const char* pDataType = (char*)kpNULL;
  if(nArgs == 5) pDataType = pArgs[4];
  
  // The only parameters which need pre-processing before calling the package's
  // create are the parameter list and the resolution list.
  //
  // The parameter list is split, then made into a vector of names.
  //

  int                      nListElements;
  char**                   ppListElements;
  vector<string> vParameters;
  vector<string> vyParameters;
  vector<vector<string> > vGSParameters; // gamma summary parameters.
  vector<CGateContainer*> m2projGates;   // Gates used to maintain the ROI for m2 proj.
  bool                    xproj;         // true for m2 proj is x.

  CTCLList lParameters(&rInterpreter, pArgs[2]);
  lParameters.Split(nListElements, &ppListElements);

  // Get the parameter name list
  
  // If the spectrum is a Gamma 2d Deluxe there must be
  // exactly 2 elements in the list..each of them themselves
  // a list of x/y parameters.
  // Otherwise, there's just a single parameter list:

  if (string(pType) == string("gd")) {
    if (nListElements == 2) {
      int nxParams;
      int nyParams;
      char** pxParams;
      char** pyParams;
      CTCLList lxParameters(&rInterpreter, ppListElements[0]);
      CTCLList lyParameters(&rInterpreter, ppListElements[1]);

      lxParameters.Split(nxParams, &pxParams);
      lyParameters.Split(nyParams, &pyParams);

      rPack.GetNameList(vParameters, nxParams, pxParams);
      rPack.GetNameList(vyParameters, nyParams, pyParams);

      Tcl_Free((char*)pxParams);
      Tcl_Free((char*)pyParams);
      

    }
    else {
      Usage(rInterp, "Gamma 2d spectra must have a pair of parameter lists");
      return TCL_ERROR;
    }
  }
  else if (string(pType) == string("gs")) {
    // Gamma summary spectrum is just wierd.  Expect a list
    // of lists of parameters:

    for (int i=0; i < nListElements; i++) {
      StringArray column;
      CTCLList cparams(&rInterpreter, ppListElements[i]);
      cparams.Split(column);
      vGSParameters.push_back(column);
      
    }
  } else if (string(pType) == "2dmproj") {
    /*
     * Well 2dmproj spectra are even stranger.  The spectrum 'parameters'
     * element must consist of three elements.  The first of these is a list
     * of parameters that increment the specctrum as x1 y1 x2 y2 -- must be
     * an even number of them.  The second parameter must be the text "x" or
     * "y" indicating the projection direction.  We're going to be nice and
     * make this case insensitive.   The third parameter must be either a single
     * gate name...of an OR gate of contours or a list of contours.
     */
    
    if (nListElements == 3) {   // Parameters, direction and gate(s)
        std::string parameters = ppListElements[0];
        std::string direction  = ppListElements[1];
        std::string projectionGate = ppListElements[2];
        
        if (!pDataType) {
            pDataType = "long";          // Default data type.
        }
        
        // Unpack the parameters into vParameters
        
        CTCLObject params;
        params.Bind(rInterpreter);
        params = parameters;
        for (int i =0; i < params.llength(); i++) {
            vParameters.push_back(params.lindex(i));
        }
        // Get the projection direction.
        
        xproj = (direction== "x") || (direction == "X");
        if (!xproj && ((direction != "Y") && (direction != "y"))) {
            interp.setResult(
                std::string("Invalid projection direction must be 'x' or 'y'")
            );
              
            return TCL_ERROR;
        }
        // Get the Gate(s).  Assume that gate names can't have spaces or it's just
        // far too ugly a proposition.  If the gate is a single OR gate,
        // It's decomposed into its constituents.
        
        CTCLObject gates;
        gates.Bind(rInterpreter);
        gates = projectionGate;
        if (gates.llength() == 1) {
            m2projGates = getConstituents(rInterp, projectionGate);
        } else {
            m2projGates = getGates(rResult, gates);
        }
        // Errors leave a message in result and return an empty vector.
        
        if (m2projGates.empty()) {
            return TCL_ERROR;    
        }
        
        
    }  else{
        rInterp.setResult(
            std::string("M2 projection spectra must have parameters, direction and ROI gate in their definition");
        );
        return TCL_ERROR;
    }
  }
  else {
    rPack.GetNameList(vParameters, nListElements, ppListElements);
  }


  

  Tcl_Free((char*)ppListElements);
  
  // The axis definitions are decoded into
  // the vectors below:
  //
  vector<UInt_t>  vChannels;        //  # channels on axis.
  vector<Float_t> vLows;	         // Low limit
  vector<Float_t>  vHighs;           // Hi limit.


  // Split the axis list....

  CTCLList lResolutionList(&rInterpreter, pArgs[3]);
  lResolutionList.Split(nListElements, &ppListElements);

  // Each list element has one of the following forms:
  //   2^( n   -  1) << is the channelcount and 
  //                lowlim = 0, hilim = channelcount-1
  //  {lo hi n} - n is the channel count and lo/hi are
  //              given by the other list elements.

  // Iterate over the list to fill the vectors above:
  // This loop can bounce us out if:
  //    - one of the axis definitions does not have the right number
  //      of elements (1 or 3).
  //    - one of the elements of an axis definition does
  //      not decode to the proper data type.

  for(int i =0; i < nListElements; i++) {
    // Split the axis definition list into its subelements:
    int     nAxisElements;
    char**  ppAxisElements;

    CTCLList AxisList(&rInterpreter, ppListElements[i]);
    AxisList.Split(nAxisElements, &ppAxisElements);
    if(nAxisElements == 1) {	   // Just size (in bits) given.
      Int_t   nBits;		   // Axis element is bits. 
      UInt_t  nChannels;	   // which are converted to chans.
      try {
        nBits = (Int_t)rInterpreter.ExprLong(ppAxisElements[0]);
        //
        // Just because the bits are an integer doesn't mean they are a good
        // integer.  Let's limit spectra to MAXRESBITS channels and positive
        // resolutions!!
        //
        if((nBits < 0) || (nBits > MAXRESBITS)) {
          throw CRangeError(0, MAXRESBITS, nBits, 
                    "Selecting channel resolution");
        }
        nChannels = 1 << nBits;
    
        // Let's fix this right here and now.
        // rather than having a special case later in the code
        // The axis range represented by this is (while the integerists hold sway)
        // [0, nChannels-1] with nChannels bins:
    
        
        vChannels.push_back((UInt_t)nChannels);
        vLows.push_back(0.0);	// Indicate there is no high/low for this axis.
        vHighs.push_back(static_cast<float>(nChannels-1));
       }
      catch (CException& rExcept) { // Spectrum size invalid.
        rResult = "Spectrum size "; // Note that TCL Exceptions set Reason!!
        rResult += ppAxisElements[0];
        rResult += " must evaluate to an integer\n";
        rResult += rExcept.ReasonText();
        rResult += "\n";
        Usage(rInterp, rResult.c_str());
	
        if (*ppAxisElements) Tcl_Free((char*)ppAxisElements);
        if (*ppListElements) Tcl_Free((char*)ppListElements);
	        return TCL_ERROR;		       
	
      }

    }
    else if (nAxisElements == 3) { // Full description given.
      Int_t   nChannels;
      Float_t fLow;
      Float_t fHigh;
      Int_t   element;		// Element of list being processed.
      try {
        element = 0;
        fLow       = (Float_t)
                        rInterpreter.ExprDouble(ppAxisElements[0]);
        element++;
        fHigh      = (Float_t)
                        rInterpreter.ExprDouble(ppAxisElements[1]);
        element++;
        nChannels  = (Int_t)rInterpreter.ExprLong(ppAxisElements[2]);
    
        vChannels.push_back((UInt_t)nChannels);
        vLows.push_back(fLow);
        vHighs.push_back(fHigh);
        

      }
      catch (CException& rExcept) {
        rResult += " Axis definition: ";
        rResult += ppListElements[i];
        rResult += " contains an invalid element: ";
        rResult += ppAxisElements[element];
        rResult += "\n";
        Usage(rInterp, rResult.c_str());
    
        Tcl_Free((char*)ppAxisElements);
        Tcl_Free((char*)ppListElements);
	      return TCL_ERROR;
      }
    }
    else {			   // Invalid description. 
      rResult += "Axis definition: ";
      rResult += ppListElements[i];
      rResult += " is not a valid axis definition\n";
      Usage(rInterp, rResult.c_str());
      return TCL_ERROR;
	
    }

    Tcl_Free((char*)ppAxisElements);
  }
  Tcl_Free((char*)ppListElements);


  if (string(pType) == string("gs")) {

    return rPack.CreateSpectrum(rInterp, pName, pType, vGSParameters,
			 vChannels, vLows, vHighs, pDataType);
  } else if (string(pType) == "2dmproj") {
    return rPack.CreateSpectrum(
            rInterp, pName, pType, vParameters, xproj, m2projGates,
            vChannels, vLows, vHighs, pDataType                                        
        );
  }
  else if (vyParameters.size() == 0) {
    
    return rPack.CreateSpectrum(rInterp,  pName, pType, vParameters, 
				vChannels, vLows, vHighs,
				pDataType);
  }
  else {
    return rPack.CreateSpectrum(rInterp, pName, pType, 
				vParameters,
				vyParameters,
				vChannels,
				vLows,
				vHighs,
				pDataType);
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t List ( CTCLInterpeter& rInterp, int nArgs, char* Args[] )
//  Operation Type:
//     Utility
//
Int_t 
CSpectrumCommand::List(CTCLInterpreter& rInterp,
		       int nArgs, const char* pArgs[]) 
{
// Handles the subparsing and dispatching
// for the parameter -list command.
//
//  Formal parameters:
//      CTCLInterpreter& rInterp:
//              Refers to the interpreter.
//      
//      int nArgs:
//                Number of command parameters past the -list switch.
//      char* pArgs[]:
//                  Pointers to the arguments.
//

  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  std::string rResult;


  // First process the switches that are legal:

  bool idswitch   = false;
  bool byidswitch = false;
  bool showgates  = false;
  Switch result;

  while(nArgs && (result = MatchSwitch(pArgs[0])) != keNotSwitch) {
    switch(result) {
    case keId:
      idswitch = true;
      break;
    case keById:
      byidswitch = true;
      break;
    case keShowGate:
      showgates  = true;
      break;
    default: 
      rResult = "Switch ";
      rResult += pArgs[0];
      rResult += " not recognized in this context\n";
      Usage(rInterp, rResult.c_str());
      return TCL_ERROR;
    }

    pArgs++;
    nArgs--;
  }
  // Any additional switches are parameters of the listing.
  // The bools:
  //    idswitch, byidswitch, showgates reflect the presence
  //    of these switches.

  // idswitch and byidswitch are not both allowed:


  if (idswitch && byidswitch) {
    rResult = "-id and -byid cannot both be specified on a -list operation\n";
    Usage(rInterp, rResult.c_str());
    return TCL_ERROR;
  }
  
  if (idswitch) {

    // List a single Id.

    Int_t           nId;

    if (nArgs != 1) {
      rResult  = "Need an id, and only an id to do  a -list -id\n";
      Usage(rResult);
      return TCL_ERROR;
    }
    if ((ParseInt(pArgs[0], &nId) != TCL_OK) || (nId < 0)) {
      rResult = "Invalid spectrum id ";
      rResult += pArgs[0];
      rResult += "\n";
      Usage(rInterp, rResult.c_str());
      return TCL_ERROR;      
    }
    return rPack.ListSpectrum(rInterp, nId, showgates);
  }
  else {  
    // List possibly matching a pattern.
    // Figure out the pattern or that there are too many parametesr.

    vector<string> vDescriptions;
    string pattern = "*";


    if (nArgs > 1) {
      rResult = "Too many parameters.  Expecting at most a spectrum name pattern only\n";
      Usage(rInterpreter, rResult.c_str());
      return TCL_ERROR;
    }
    if(nArgs) {
      pattern = string(pArgs[0]); 
    }
    rPack.ListSpectra(vDescriptions, pattern.c_str(), showgates);

    if (byidswitch) {
      SortSpectraById(vDescriptions);
    }
    VectorToResult(rInterp, vDescriptions);
  }
  return TCL_OK;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t Delete ( TCLInterpreter& rInterp, 
//                   int nArgs, char* pArgs[] )
//  Operation Type:
//     Utillity
//
Int_t 
CSpectrumCommand::Delete(CTCLInterpreter& rInterp, 
			 int nArgs, const char* pArgs[]) 
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
//       int nArgs:
//          number of command arguments following the -delete
//          switch.
//       char* pArgs[]:
//          Array of pointers to the arguments.
//

  CSpectrumPackage& rPack(*(CSpectrumPackage*) getPackage());

  if(nArgs <= 0) {		// Must be at least one parameter.
    Usage(rResult);
    return TCL_ERROR;
  }
  // The next parameter is either the beginning of a name list or one of the
  // following swiches:
  //   -id        - Indicates the beginning of a list of id's to delete.
  //   -all       - Indicating all spectra are to be deleted.
  //


  vector<UInt_t> vIds;
  vector<string> vNames;

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
  rResult ="BUG - report this: Control reached end of CSpectrumCommand::Delete";
  return TCL_ERROR;

}
/*!
  Establish traces for either the add or delete of spectra in the
  spectrum dictionary.

  Implements:

\verbatim
   spectrum -trace add ?script?
   spectrum -trace delete ?script?
\endverbatim

   \param rInterp : CTCLInterpreter&
       Reference to the interpreter that is executing this command.
   \param rResult : CTCLResult&
       Reference to the result object for the interpreter.
   \param argc : int
       Number of remaining command line parameters (after spectrum -trace).
   \param argv : char**
       The remaining command line parameters (after spectrum -trace).

   \return Int_t
   \retval TCL_OK    - Correct return.
   \retval TCL_ERROR - Some error occured executing the command which return
           be the contents of the error variable.



*/
Int_t 
CSpectrumCommand::Trace(CTCLInterpreter& rInterp, 
			CTCLResult&      rResult,
			int argc, const char** argv)
{
  // Need at least one extra argument, the trace operation
  // which  must be add or delete and just selects the trace object:

  CTCLObject* pTraceScript(0);
  if (argc < 1) {
    rResult = "Need a trace op add | delete\n";
    Usage(rResult);
    return TCL_ERROR;
  }
  if (string(argv[0])  == string("add") ) { 
    pTraceScript = &m_createTrace;
  }
  if (string(argv[0]) == string("delete")) {
    pTraceScript = &m_removeTrace;
  }

  if (pTraceScript == 0) {
    rResult = "Invalid trace operation.  Must be add or delete\n";
    Usage(rResult);
    return TCL_ERROR;
  }

  // pTraceScript now represents the trace object we will be doing stuff
  // to:
  // If there's no script, then we just list the script as our result.

  argc--;
  argv++;
 
  // Get the current trace script and map it to ""
  // in the event it's the default.

  rResult = (string)(*pTraceScript);
  if ((string)(*pTraceScript) == defaultTrace) {
    rResult = string("");
  }

  if (argc < 1) {
    return TCL_OK;
  }

  // If there's a script we need to 
  // map the scrip to the default if it's empty.

  string script(argv[0]);
  if (script == "") {
    script = defaultTrace;
  }
  *pTraceScript = script;

  return TCL_OK;
  
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
  rResult += "  spectrum [-new] name type { parameters... } {axisdefs... [datatype]y\n";
  rResult += "  spectrum -list ?-byid? ?-showgate? [pattern]\n";
  rResult += "  spectrum -list pattern\n";
  rResult += "  spectrum -list -id ?-showgate? id\n";
  rResult += "  spectrum -delete name1 [name2...]\n";
  rResult += "  spectrum -delete -id id1 [id2...]\n";
  rResult += "  spectrum -delete -all\n";
  rResult += "  spectrum -trace add ?script?\n";
  rResult += "  spectrum -trace delete ?script?\n";
  rResult += "    In the above, an axsidef has one of the following formats:\n";
  rResult += "         n           - n is the Log(2) the number of channels\n";
  rResult += "         {low hi n}  - Full definition where:\n";
  rResult += "                       low  - Parameter value represented by channel 0\n";
  rResult += "                       hi   - Parameter value represented by channel n-1\n";
  rResult += "                       n    - Number of channels on the axis.\n";
  rResult += "\n  The spectrum command creates and deletes spectra as well\n";
  rResult += "  as listing their properties.";
  rResult += " The -trace switch allows the creation, inspection and removal\n";
  rResult += " of traces on adding and deleting spectra\n";

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
//     void   SortSpectraById(vector<string>& rvProperties)
//  Operation Type:
//     Utility.
//
void
CSpectrumCommand::SortSpectraById(vector<string>& rvProperties)
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
//     void   SortSpectraByName(vector<string>& rvProperties)
//  Operation type:
//     protected utility.
//
void 
CSpectrumCommand::SortSpectraByName(vector<string>& rvProperties)
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
//    void   VectorToResult(CTCLInterpreter& rInterp,
//                          vector<string>& rvStrings)
//  Operation Type:
//     protected Utility function.
//
void
CSpectrumCommand::VectorToResult(CTCLInterpreter& rInterp, 
				 vector<string>& rvStrings)
{
  // Appends a vector of strings as a list to rResult.
  //
  
  CTCLString List;
  for(UInt_t i = 0; i < rvStrings.size(); i++) {
    List.AppendElement(rvStrings[i]);
    List.Append("\n");
  }
  rInterp.setResult((const char*)List);
}
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//     UInt_t      ExtractId(const string& rProperties)
//  Operation Type:
//     Public Utility.
//
UInt_t
CSpectrumCommand::ExtractId(const string& rProperties)
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
  Tcl_Free((char*)pElements);
  return (UInt_t)nId;
}
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//      string ExtractName(const string& rProperties)
//  Operation type:
//      Public utility
//
string
CSpectrumCommand::ExtractName(const string& rProperties)
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
  
  string Result(pElements[1]);
  Tcl_Free((char*)pElements);
  return Result;

}


/*!
   Process spectrum add traces.
*/
void
CSpectrumCommand::traceAdd(const string& name,
			   const CSpectrum* pSpectrum)
{
  if (!m_fTracing) {
    if (string(m_createTrace) != defaultTrace) {
      m_fTracing = true;
      m_createTrace.Bind(getInterpreter());
      CTCLObject script(m_createTrace);
      script.Bind(getInterpreter());
      script += name;
      CTCLObject result;
      result.Bind(getInterpreter());
      int status = TCL_OK;
      try {
	result = script();
      }
      catch (...) {
	status = TCL_ERROR;
      }
      if (status == TCL_ERROR) {
	cerr << "Error executing spectrum add trace on " << (string)(script) << endl;
      }
      m_fTracing = false;
    }
  }
}

/*!
   Process spectrum removed traces:
*/
void 
CSpectrumCommand::traceRemove(const string& name,
			      const CSpectrum* pSpectrum)
{
  if (!m_fTracing) {
    if (string(m_removeTrace) != defaultTrace) {
      m_fTracing = true;
      m_createTrace.Bind(getInterpreter());
      CTCLObject script(m_removeTrace);
      script.Bind(getInterpreter());
      script += name;
      CTCLObject result;
      result.Bind(getInterpreter());
      int status = TCL_OK;
      try {
	result = script();
      }
      catch (...) {
	status = TCL_ERROR;
      }
      if (status == TCL_ERROR) {
	cerr << "Error executing spectrum remove trace on " << (string)(script) << endl;
      }
      m_fTracing = false;
    }
  }
}

/**
 * getConstituents
 *    Given a gate name, returns the gate constituents.
 *    - It is an error for the gate not to exist.
 *    - It is an error for the gate not to be an OR gate.
 *    
 * @param res - Interpreter running the  command..
 * @param gateName - the parent gate name.
 * @return std::vector<CGateContainer*> - the containers for the constituents.
 * @retval - empty vector if there was an error.  The interpreter result is set on error.
 */
std::vector<CGateContainer*>
CSpectrumCommand::getConstituents(CTCLInterpreter& rInterp, std::string gateName)
{

    SpecTcl& api(*SpecTcl::getInstance());
    std::vector<CGateContainer*> result;
    // Find the parent's gate container.
    
    CGateContainer* pParent = api.FindGate(gateName);
    if (!pParent) {
        std::string message = "No such gate: ";
        message += gateName;
        rInterp.setResult(message);
        return result;
    }
    // Must be an or gate:
    
    if((*pParent)->Type() != "+") {
        std::string message = "For m2dproj - single gate must be a + gate ";
        message += gateName;
        message += "'s gate type is ";
        message += (*pParent)->Type();
        rInterp.setResult(message);
        return result;
    }
    // Get the constituent gates; We'll let the creation determine that these are
    // all contours of the appropriate type:
    
    CCompoundGate* pParentGate = reinterpret_cast<CCompoundGate*>(pParent->getGate());
    std::list<CGateContainer*>& contours(pParentGate->GetConstituents());
    result.insert(result.begin(), contours.begin(), contours.end());
    return result;
}
/**
 *  getGates
 *      Given a string that is a valid Tcl List of gates, returns a vector
 *      of the gate containers that make up the gate.
 *      - It is an error for any gate not to exist.
 *      - It is an error for any gate not to be a "c" gate.
 *      There are other restrictions, but we'll let the gate construction
 *      throw errors for those.
 *
 * @param rInterp  - Tcl interpreter running the command.
 * @param gates - CTCLObject reference for a Tcl list of gate names.
 * @return std::vector<CGateContainer*> - vector of pointers to gate containers for
 *       the gates.
 * @retval empty - error and the interpreter result has the error message.
 * @note it is an error for the gate list to be empty as well.
 */
std::vector<CGateContainer*>
CSpectrumCommand::getGates(CTCLInterpreter& rInterp, CTCLObject& gates)
{
    SpecTcl& api(*SpecTcl::getInstance());
    std::vector<CGateContainer*> result;
    std::vector<CGateContainer*> errorResult;   // for reporting errors as result is accumulating
    // Get the gate list as separate objercts:
    
    std::vector<CTCLObject> gateNames = gates.getListElements();
    if (gateNames.size() == 0) {
        res = "Region of interest gate list cannot be empty";
        return result;
    }
    
    for (int i =0; i < gateNames.size(); i++) {
        gateNames[i].Bind(gates.getInterpreter());
        std::string aName = std::string(gateNames[i]);
        CGateContainer* pGate = api.FindGate(aName);
        if (!pGate) {
            std::string message = "Gate: ";
            message += aName;
            message += " does not exist";
            rInterp.setResult(message);
            return errorResult;
        }
        if ((*pGate)->Type() != "c") {
            std::string message("Gate: ");
            message += aName;
            message += " is not a contour but all ROI gates must be";
            rInterp.setResult(message);
            return errorResult;
        }
        result.push_back(pGate);
    }
    return result;
    
}