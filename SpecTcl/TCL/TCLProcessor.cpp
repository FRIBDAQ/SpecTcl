//  CTCLProcessor.cpp
// Encapsulates TCL Command processing
// as an object per command.
// This is especially intended for
// cases where tcl commands create
// objects which can then be manipulated
// by name.
//   NOTE:  the ability to bind the same command
//                object to multiple interpreters is explicitly
//                supported as one mechanism to share state
//                between multiple interpreters, on the whole,
//                however it's probably much simpler to just
//                instantiate an object per interpreter if that
//                particular feature is not needed.
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


#include "TCLProcessor.h"                               
#include "TCLException.h"
#include "TCLResult.h"
#include <tcl.h>
#include <assert.h>
#include <vector>
#include <string>
#include <string.h>

static const char* Copyright = 
"CTCLProcessor.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CTCLProcessor

CTCLProcessor::CTCLProcessor(const std::string& sCommand, 
			     CTCLInterpreter* pInterp) :
  CTCLInterpreterObject(pInterp)
{
  m_sCommandName.resize(strlen(sCommand.c_str())+2, 0);
  m_sCommandName = sCommand;
}
CTCLProcessor::CTCLProcessor(const char* pCommand, CTCLInterpreter* pInterp) :
  CTCLInterpreterObject(pInterp)
{
  m_sCommandName.resize(strlen(pCommand)+2, 0); // Ensure clear since stl has
  m_sCommandName = pCommand;	// construction/destruction problems?
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string ConcatenateParameters ( int nArguments, char* pArguments[] )
//  Operation Type:
//     Utility:
//
std::string 
CTCLProcessor::ConcatenateParameters(int nArguments, char* pArguments[]) 
{
// Concatenates a parameter set into a single string.
// This can be  used by command interpreters which
//  will need to further parse their input (e.g an
//  epxression evaluator may want to act globally on the
//  expression, rather than a token at a time).  Spaces are put
//  between each parameter.
//
//  Formal parametesrs:
//        int nArguments:
//            Number of parameters to concatenate.
//        char* pArguments[]
//             Pointer to the set of parameters to concatenate.
// Returns:
//        String containing the concatenation of the parameters.
//
//   NOTE:
//     Command invocations set argv[0] to be the command text.  In order
//     to be policy free, we act as if that's not the case, therefore if the
//     client really only wants the parameters, they should pass a pointer
//     to argv[1]

  std::string Result;

  for(int i = 0; i < nArguments; i ++) {
    Result += pArguments[i];
    if(i < nArguments - 1) Result += ' '; // Since there are more params.
  }
  return Result;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int EvalRelay (ClientData pData,
//                   TclInterp* pInterp,
//                   int ARgc, char* argv[]  )
//  Operation Type:
//     contextualizer
//
int 
CTCLProcessor::EvalRelay(ClientData pObject,
			 Tcl_Interp* pInterpreter,
			 int Argc, char* Argv[]) 
{
// This member:
//     1. Establishes object context for the callback.
//     2. Determines which interpreter is associated with
//         the callback and binds the object to it termporarily
//     3. Creates and binds a result protocol for the associated
//         interpreter and 
//     4. Calls the object's operator()
//     5. destroys the result protocol object and
//     6. rebinds the object back to its original 
//         interpreter.
// Formal Parameters:
//       ClientData   pObject:
//           Pointer to the object which is being evaluated.
//        Tcl_Interp* pInterpreter:
//            Pointer to C API Interpreter which is evaluating the object.
//         int nArguments:
//            Number of parameters to evaulate.
//         char* pArguments[]
//            Array of pointers to the command arguments.
//
// Returns:
//     What operator() returns.

  CTCLProcessor*    pThat = (CTCLProcessor*)pObject;
  CTCLInterpreter*  pInt((CTCLInterpreter*)kpNULL);


  // The most likely case is that the interpreter bound is the one invoking
  // us since most software has a single interpreter.

  if(pInterpreter == pThat->getInterpreter()->getInterpreter()) {
    pInt = pThat->getInterpreter();
  }
  else {
    for(TCLInterpreterIterator i = pThat->begin(); i != pThat->end(); i++) {
      if((*i)->getInterpreter() == pInterpreter) {
	pInt = (*i);
	break;
      }
    }
  }
  // It's a really bad bug if there is no matching interpreter object:

  assert(pInt != (CTCLInterpreter*)kpNULL);
  
  //  Create a corresponding result object, and then call our operator().

  CTCLResult       Result(pInt);
  CTCLInterpreter* pPrior = pThat->getInterpreter();
  pThat->Bind(*pInt);

  try {
    int nResult = pThat->operator()(*pInt, Result, Argc, Argv);
    pThat->Bind(*pPrior);
    return nResult;
  }
  catch(CTCLException& rexcept) { // map TCL exceptions to return codes.
    pThat->Bind(*pPrior);
    return rexcept.ReasonCode();
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void OnDelete (  )
//  Operation Type:
//     override interface
//
void 
CTCLProcessor::OnDelete() 
{
// Called by the DeleteRelay member function when
//  the command is being removed from a TCL interpreter.
//  Any cleanup required for this removal should be done
//  at this time.  Remember that this object could be
//  registered on more than one command interpreter.
//  Because of the way in which it's done, if the deletion
//  is through the Unregister interface, the current binding
//  will be the one from which this command is being deleted.
//
//   Note: if the deletion is being done through our Unregister or
//         our UnregisterAll function, this is bound to the interpreter
//         from which we're being deleted.  This is the *ONLY* good
//         way to do a delete.
//

  // Typically this function is overridden if needed.  It does nothing in 
  // this implementation so that commands without need for OnDelete actions
  // don't need to supply one.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void DeleteRelay ( ClientData pObject )
//  Operation Type:
//     contextualizer.
//
void 
CTCLProcessor::DeleteRelay(ClientData pObject)
{
//   Establishes object context for the
//   OnDelete call.  After OnDelete exits,
//   the current binding is removed from the
//   bindings list m_vRegisteredOn and the
//   object is bound to the first entry in the list, or to
//   kpNULL if the list has become empty.
//
// Formal Parameters:
//     ClientData  pObject:
//          Pointer to the object which is being unregistered.
// 

  // Call the OnDelete member:

  CTCLProcessor* pThat = (CTCLProcessor*)pObject;

  pThat->OnDelete();

  // Remove the current binding from the bindings list:

  for(TCLInterpreterIterator i = pThat->begin(); i != pThat->end(); i++) {
    if((*i)->getInterpreter() == pThat->getInterpreter()->getInterpreter()) {
      (*i) = pThat->m_vRegisteredOn.back();
      pThat->m_vRegisteredOn.pop_back();
      break;
    }
  }
  // Rebind us to either the front of the registration array or to kpNULL
  // if we're completely unbound:

  pThat->Bind( (pThat->m_vRegisteredOn.empty() ? (CTCLInterpreter*)kpNULL : 
		pThat->m_vRegisteredOn[0]));

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int ParseInt ( const char* pString, int* pInteger )
//  Operation Type:
//     Utiilty
//
int 
CTCLProcessor::ParseInt(const char* pString, int* pInteger) 
{
// Uses the currently bound interpreter to parse
// a parameter as an integer.
// 
// Formal Parameters;
//      char* pString:
//            Pointer to the string to parse as an integer.
//      int* pInteger:
//           Pointer to the integer to fill in.
// Returns:
//     TCL_OK if ok, o r TCL_ERROR If not.. client should instantiate a 
//     CTCLResult object on the bound interpreter to get detailed error
//     information.
//

  // Ensure that we're bound to an interpreter:
  // If not return TCL_ERROR, but we can't set a result code since that 
  // depends on an interpreter.
  //

  CTCLInterpreter* pInterpObj = getInterpreter();
  if(pInterpObj == (CTCLInterpreter*)kpNULL)
    return TCL_ERROR;		// Not bound...
  Tcl_Interp*      pInterp    = pInterpObj->getInterpreter();
  if(pInterp == (Tcl_Interp*)kpNULL)
    return TCL_ERROR;

  return Tcl_GetInt(pInterp, (char*)pString, pInteger);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int ParseDouble ( const char* pString, double* pDouble )
//  Operation Type:
//     utility
//
int 
CTCLProcessor::ParseDouble(const char* pString, double* pDouble) 
{
// Parses a string as a double using the currently
//  bound interpreter.
//  Formal Parameters:
//       char* pString:
//             String to parse.
//       double* pDouble:
//           Points to where to put value to parse.
// Returns:
//    TCL_OK - If ok, else TCL_ERROR and a CTCLResult
//     can be instantiated on the bound interpreter to 
//     retrieve a textual reason for the parse failure.
//
  CTCLInterpreter* pInterpObj = getInterpreter();
  if(pInterpObj == (CTCLInterpreter*)kpNULL)
    return TCL_ERROR;		// Not bound...
  Tcl_Interp*      pInterp    = pInterpObj->getInterpreter();
  if(pInterp == (Tcl_Interp*)kpNULL)
    return TCL_ERROR;

  return Tcl_GetDouble(pInterp, (char*)pString, pDouble);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int ParseBoolean ( const char* pString, Bool_t* pBoolean )
//  Operation Type:
//     utility
//
int 
CTCLProcessor::ParseBoolean(const char* pString, Bool_t* pBoolean) 
{
// Parses a string as a boolean.
//  True values are any of: 1,true,on, or yes
//   False values are any of 0, false, off, or no.
//
// Formal Parameters:
//     const char* pString:
//           Pointer to the string to parse.
//     Bool_t* pBool:
//           Pointer to the result boolean.
// Returns:
//     TCL_OK if parse worked, else TCL_ERROR
//     and instantiating a CTCLResult object on the
//     currently bound interpreter will enable a textual
//     reason for the parse failure to be retrieved.
//

  CTCLInterpreter* pInterpObj = getInterpreter();
  if(pInterpObj == (CTCLInterpreter*)kpNULL)
    return TCL_ERROR;		// Not bound...
  Tcl_Interp*      pInterp    = pInterpObj->getInterpreter();
  if(pInterp == (Tcl_Interp*)kpNULL)
    return TCL_ERROR;

  int flag;
  int nStatus = Tcl_GetBoolean(pInterp, (char*)pString, &flag);
  *pBoolean = flag;
  return nStatus;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Register (  )
//  Operation Type:
//     Connector
//
void 
CTCLProcessor::Register() 
{
// Registers the command on the currently bound interpreter.
//
// The current binding is added to the bindings list.

  CTCLInterpreter* pInterp = AssertIfNotBound();

  pInterp->AddCommand(m_sCommandName, EvalRelay, (ClientData)this, 
		      DeleteRelay);
  m_vRegisteredOn.push_back(pInterp);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int Unregister (  )
//  Operation Type:
//     connector
//
int 
CTCLProcessor::Unregister() 
{
// Unregisters the command from the currently bound interpreter.
// Note that the Unregister operation causes the DeleteRelay to be
// called which in turn will call the appropriate OnDelete and
// remove the currently bound interpreter from the bindings list.
// the bindings are re-adjusted.

  CTCLInterpreter* pInterp = AssertIfNotBound();

  pInterp->UnregisterCommand(m_sCommandName);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void UnregisterAll (  )
//  Operation Type:
//     
//
void 
CTCLProcessor::UnregisterAll() 
{
// Unregisters the command from all interpreters in the 
// current bindings list.

  while(!m_vRegisteredOn.empty()) {
    if(getInterpreter() == (CTCLInterpreter*)kpNULL)
      Bind(m_vRegisteredOn[0]);	// Ensure we start out bound.
    Unregister();		// This will eventually rebind.
  }

}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   static int MatchKeyword(const vector<string>& MatchTable, 
//			     const string& rValue, 
//			     int NoMatch = -1)
// Operation type:
//    Utility.
//
int
CTCLProcessor::MatchKeyword(vector<string>& MatchTable, 
			    const string& rValue, 
			    int NoMatch)
{
  // Match a keyword, rValue, against the set of keywords in a MatchTable.
  // If no match can be found, the NoMatch value is returned.
  // Otherwise the index into the table is returned.
  //
  int index = 0;
  vector<string>::iterator i;
  for(i = MatchTable.begin(); i != MatchTable.end(); i++) {
    if(*i == rValue) return index;
    index++;
  }
  return NoMatch;
}
