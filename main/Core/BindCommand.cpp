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

//  CBindCommand.cpp
//  This class implements the bind command.
//   The bind command manipulates and examines
//   the display bindings associated with spectra.
//   The command takes the following forms:
//
//        bind namelist
//                 binds the named spectra to the display.  Spectra
//                 already bound are not effected.
//        bind -all
//                  binds all spectra to the display.  Already bound spectra
//                  are not affected.
//        bind -list
//                   List all bindings.
//        bind -list namelist
//                   List bindings for named spectra.
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
#include "BindCommand.h"
#include "TCLInterpreter.h"
#include "TCLString.h"
#include "TCLObject.h"
#include "SpectrumPackage.h"

#include <histotypes.h>                               
#include <string>
#include <vector>
#include <string.h>

#include "BindTraceSingleton.h"
#include <TCLObject.h>
#include <stdexcept>


using namespace std;

// Static Data:


struct SwitchTableEntry {
  const char*      pSwitchText;
  CBindCommand::eSwitches Switch;
};
static const SwitchTableEntry Switches[] = {
  { "-new",  CBindCommand::keNew },
  { "-id",   CBindCommand::keId },
  { "-all",  CBindCommand::keAll },
    { "-list", CBindCommand::keList },
    { "-trace", CBindCommand::keTrace},
    {"-untrace", CBindCommand::keUntrace}
};

static const TCLPLUS::UInt_t nSwitches = sizeof(Switches)/sizeof(SwitchTableEntry);

// Functions for class CBindCommand

/**
 *  Constructor.
 *    @param pInterp - pointer to the interpreter on which this command will be
 *      registered
*/
CBindCommand::CBindCommand(CTCLInterpreter* pInterp) :
  CTCLPackagedObjectProcessor(*pInterp, "sbind", true)
{}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv )
//  Operation Type:
//     Command Processor
//
int 
CBindCommand::operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv)
{
// Called whenever TCL executes the bind
// command.  The command is parsed and 
// dispatched to the appropriate handler.
// 
// Formal Parameters:
//     CTCLInterpreter&  rInterp:
//         The interpreter on which this command is running.
//      std::vector<CTCLObject>& objv - the command words.
// Returns:
//      TCL_OK           - If the command worked.
//      TCL_ERROR   - if not.
//

  // This function must mostly distinguish between actual binding requests
  // and list requests:

  // marshall into arg , argv form to make the porting minimal work:

  int nArgs = objv.size();
  std::vector<std::string> words;
  std::vector<const char*> pWords;

  // Two loops due to lifetime issues and c_str().

  for (auto& word: objv) {
    words.push_back(std::string(word));
  }
  for (auto& word: words) {
    pWords.push_back(word.c_str());
  }
  auto pArgs = pWords.data();

  nArgs--; pArgs++;		// Don't care about command name.
  
  if(nArgs <= 0) {		// Need at least on parameter.
    Usage(rInterp);
    return TCL_ERROR;
  }
  // Parse out the switch:

  switch(MatchSwitch(pArgs[0]) ){
  case keList:			// List bindings.
    nArgs--;
    pArgs++;
    return ListBindings(rInterp,  nArgs, pArgs);

  case keAll:			// New bindings with all.
    nArgs--;
    pArgs++;
    if(nArgs) {			// No arguments permitted on -all switch.
      Usage(rInterp);
      return TCL_ERROR;
    }
    return BindAll(rInterp);
    
  case keNew:			// New by name (explicit).
    nArgs--;			// Skip over the -new switch
    pArgs++;			// and fall through.
  case keNotSwitch:		// New by name (implied).
    if(nArgs <= 0) {		// Must be at least on binding:
      Usage(rInterp);
      return TCL_ERROR;
    }
    return BindByName(rInterp, nArgs, pArgs);
  case keTrace:
    return Trace(rInterp, nArgs, pArgs);
  case keUntrace:
    return Untrace(rInterp, nArgs, pArgs);
  default:			// Switch not allowed or unrecognized.
    Usage(rInterp);
    return TCL_ERROR;
  }

}
//////////////////////////////////////////////////////////////////////////
// 
//  Function:
//    Int_t BindAll(CTCLInterpreter& rInterp)
// Operation type:
//    Utility
//
TCLPLUS::Int_t 
CBindCommand::BindAll(CTCLInterpreter& rInterp)
{
  try {
    CSpectrumPackage &rPack(*(CSpectrumPackage*)(getPackage()));
  
    return rPack.BindAll(rInterp);
  }
  catch (std::exception& e) {
    rInterp.setResult(e.what());
    return TCL_ERROR;
  }
}



TCLPLUS::Int_t
CBindCommand::BindByName(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[])
{
  // Binds a list of spectrum names to Displayer slots. 
  //
  // Formal Parameters:
  //     CTCLInterpreter& rInterp:
  //        TCL Interpreter executing the command.
  //     int nArgs, char* p Args[]:
  //        Command line parameters.
  // Returns:
  //    TCL_OK      if bound.
  //    TCL_ERROR   if some could not be bound.
  //

  try {
    std::vector<std::string> vNames;
    CSpectrumPackage::GetNameList(vNames, nArgs, pArgs);
  
    CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  
    return rPack.BindList(rInterp, vNames);
  }
  catch (std::exception & e) {
    rInterp.setResult(e.what());
    return TCL_ERROR;
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ListBindings ( CTCLInterpreter& rInterp,  int nArgs, char* pArgs[] )
//  Operation Type:
//     Utility
//
TCLPLUS::Int_t 
CBindCommand::ListBindings(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]) 
{
// Processes the bind commands which
//  list bindings.
//   
// Formal parameters:
//     CTCLInterpreter&  rInterp:
//            TCL Interpreter.
//     int nArgs:
//           Number of parameters past the
//           -list swtich.
//     char* pArgs[]:
//          pointers to those parameters.

  if(nArgs) {			// If there are more params; partial list.
    //
    //  The next parameter determines what the lookup list is:
    //
    switch(MatchSwitch(pArgs[0])) {

    case keNotSwitch:		// List given names.
      //return ListByName(rInterp, rResult, nArgs, pArgs);
      return ListAll(rInterp, pArgs[0]);

    default:			// Invalid switch in this context...
      return ListAll(rInterp, pArgs[0]);

    }
  }
  else {
    return ListAll(rInterp, "*");
  }

}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Int_t ListAll(CTCLInterpreter& rInterp, const char* pattern)
// Operation Type:
//   Utility:
//
TCLPLUS::Int_t
CBindCommand::ListAll(CTCLInterpreter& rInterp, const char* pattern)
{
  // List all spectrum bindings.
  //

  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  rPack.ListAllBindings(rInterp, pattern);
  return TCL_OK;

}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Int_t ListByName(CTCLINterpreter& rInterp, int nArgs, char* pArgs[])
// Operation Type:
//   Utility
//
TCLPLUS::Int_t
CBindCommand::ListByName(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[])
{
  // List the bindings of a set of spectra given their names.
  // The set of parameters in nArgs/pArgs is assumed to be a set of
  // spectrum names.
  //
  // Formal Parameters:
  //    CTCLInterpreter& rInterp:
  //       TCL Interpreter running the command.
  //    CTCLResult& rResult:
  //       Result string.
  //    int nArgs, char* pArgs[]:
  //       Number of parameters and the parameter string pointers.
  //
  // Returns:
  //   TCL_ERROR - if error.
  //   TCL_OK    - if all spectrum bindings fetched.
  //

  vector<string> vNames;
  CSpectrumPackage::GetNameList(vNames, nArgs, pArgs);

  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  return rPack.ListBindings(rInterp, vNames);
}
/**
 * Trace
 *    Add a new trace to the sbind traces.
 *    - There must be exactly 3 arguments, sbind, -trace, and the script stem.
 *    - Locate the bind trace singleton.
 *    - Encapsulate the script stem in a CTCLOBject and
 *    - Add it as a sbinding trace.
 *  @return Int_t - TCL_OK On success, TCL_ERROR on failure.
 */
TCLPLUS::Int_t
CBindCommand::Trace(
  CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]
)
{
  // Validate the argument count:
  
  if (nArgs != 2) {
    Usage(rInterp);
    return TCL_ERROR;
  }
  CTCLObject scriptStem;
  scriptStem.Bind(rInterp);
  scriptStem = pArgs[1];
  
  // Get the trace singleton and add the script stem:
  
  BindTraceSingleton& traceContainer(BindTraceSingleton::getInstance());
  traceContainer.addSbindTrace(rInterp, scriptStem);
  
  // Return success:
  
  return TCL_OK;
}
/**
 * Untrace
 *    Remove an sbindings trace:
 *    - Ensure there are three parameters: sbind, -untrace, script-stem.
 *    - Convert the script-stem into a CTCLObject.
 *    - Get the trace container singleton and remove the script object.
 *  @return  int TCL_OK on success, TCL_ERROR on failure
 *  @note The untrace operation in the singleton will report errors via
 *        an std::exception...we'll catch that and convert it to an
 *        interpreter result and TCL_ERROR return.
 */
TCLPLUS::Int_t
CBindCommand::Untrace(
  CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]
)
{
  // Validate the argument count.
  
  if (nArgs != 2) {
    Usage(rInterp);
    return TCL_ERROR;
  }
  // Pull the script stem into an object:
  
  CTCLObject scriptStem;
  scriptStem.Bind(rInterp);
  scriptStem = pArgs[1];
  
  // Get the singleton and try to unregister this script:
  
  try {
    BindTraceSingleton& traceContainer(BindTraceSingleton::getInstance());
    traceContainer.removeSbindTrace(scriptStem);
  } catch (std::exception& e) {
    rInterp.setResult(e.what());
    return TCL_ERROR;
  }
  return TCL_OK;
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//    static eSwitches MatchSwitch(const char* pSwitch)
// Operation type:
//    protected utility.
//
CBindCommand::eSwitches
CBindCommand::MatchSwitch(const char* pSwitch)
{
  for(TCLPLUS::UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(pSwitch, Switches[i].pSwitchText) == 0) {
      return Switches[i].Switch;
    }
  }
  return keNotSwitch;
}
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void      Usage(CTCLResult& rResult)
//  Operation Type:
//    Protected Utility
//
void 
CBindCommand::Usage(CTCLInterpreter& rInterp)
{
  // Fills rResult with the correct command usage for the
  // bind Tcl Command:

  std::string rResult;
  rResult  = "Usage: \n";
  rResult += "   sbind [-new] name1 [name2...]\n";
  rResult += "   sbind -all\n";
  rResult += "   sbind -list\n";
  rResult += "   sbind -list name1 [name2 ...]\n";
  rResult += "   sbind -trace script-stem\n";
  rResult += "   sbind -untrace script-stem\n";
  rResult += "\n sbind adds a spectrum or a list of spectra \n";
  rResult += " to the display. It also can be used to list bound\n";
  rResult += " spectra by name.";
  rResult += "  With SpecTcl 5.5, the ability to add and remove traces to sbind\n";
  rResult += "  has been added with the -trace and -untrace options.\n";
  rResult += "NOTE: The bind command is a Tk command that binds gui events\n";
  rResult += "      to tcl procedures\n";

  rInterp.setResult(rResult);
}

// In the  MPI environment we need to forward traces from the event sink
// rank -> main rank.  The code below implements that as a combination of static
// and member functions.

#ifdef WITH_MPI
#include "BindTraceSingleton.h"
#include "Globals.h"
#include <mpi.h>
#include <TclPump.h>
#include <stdexcept>
#include <TCLInterpreter.h>
#include <TCLObject.h>


/**
 *   The struct that is sent to forward an event: (MPI_Send, MPI_Recv)
 * 
 */

#define MAX_SPECTRUM_NAME 256
typedef struct _BindTraceMessage {
  char s_spectrumName[MAX_SPECTRUM_NAME];
  int  s_bindIndex;
  int  s_unbind;                 // nonzero if the message is for an unbind else 0.
} BindTraceMessage, *pBindTraceMessage;

/**
 *   The event we'll post to the event loop of the main thread
 *   when we get a trace:
 */
typedef struct _BindTraceEvent {
  Tcl_Event      s_base;                 // Base event struct.
  BindTraceMessage s_msg;                 // Message sent via MPI
  CTCLInterpreter* s_pInterp;            // Interpreter to execute the trace.
}  BindTraceEvent, *pBindTraceEvent;

/**
 *   we need to be able to produce a data type for the BindTraceMessage.
 */
static MPI_Datatype 
messageType() {
  static bool created = false;
  MPI_Datatype dataType;

  if (!created) {
    MPI_Aint offsets[3] = {
      offsetof(BindTraceMessage, s_spectrumName),
      offsetof(BindTraceMessage, s_bindIndex),
      offsetof(BindTraceMessage, s_unbind)
    };
    int sizes[3] = {MAX_SPECTRUM_NAME, 1, 1};
    MPI_Datatype types[3] = {
      MPI_CHAR, MPI_INT, MPI_INT
    };


    if (MPI_Type_create_struct(
      3, sizes, offsets, types, &dataType
    ) != MPI_SUCCESS) {
      throw std::runtime_error("Unable to create the bind trace data type");
    }
    if (MPI_Type_commit(&dataType) != MPI_SUCCESS) {
      throw std::runtime_error("Failed to commit the bind trace data type");
    }
    created = true;
  }
  return dataType;
}


static void 
fillMessage(
  BindTraceMessage& msg, const std::string& spName, UInt_t bindIndex, int remove
  ) {
  if (spName.size() >= MAX_SPECTRUM_NAME) {
     throw std::length_error("Spectrum name too long");
  }
  strncpy(msg.s_spectrumName, spName.c_str(), MAX_SPECTRUM_NAME);
  msg.s_bindIndex = bindIndex;
  msg.s_unbind = remove;

}

/** 
 *   forwardNewBinding (API):
 *     Sned a message to the ROOT rank with the tag MPI_BIND_TRACE_RELAY_TAG describing
 * a new trace.
 *    @param spName - name of the spectrum being bound.
 *    @param bindIndex - The binding index assigned to the spectrum.
*/
void
CBindCommand::forwardNewBinding(const std::string& spName, UInt_t bindIndex) {
  BindTraceMessage msg;
  fillMessage(msg, spName, bindIndex, 0);

  if (MPI_Send(
    &msg, 1, messageType(), MPI_ROOT_RANK, MPI_BIND_TRACE_RELAY_TAG, MPI_COMM_WORLD
  ) != MPI_SUCCESS) {
    throw std::runtime_error("Failed to send bindings tracde create message");
  }
}
/**
 *  forwardUnbind
 *    Same as above but the trace message indicates an ubinding:
 *
 *    @param spName - name of the spectrum being bound.
 *    @param bindIndex - The binding index assigned to the spectrum.
* 
 */

void
CBindCommand::forwardUnbind(const std::string& spName, UInt_t bindIndex) {
  BindTraceMessage msg;
  fillMessage(msg, spName, bindIndex, 1);

  if (MPI_Send(
    &msg, 1, messageType(), MPI_ROOT_RANK, MPI_BIND_TRACE_RELAY_TAG, MPI_COMM_WORLD
  ) != MPI_SUCCESS) {
    throw std::runtime_error("Failed to send bindings tracde create message");
  }
}

/**
 * BindTraceEventHandler
 *    Scheduled from the event loop when a trace must be dispatched.
 * @param pEvent  - Pointer to the event struct -- actually a pBindTraceEvent.
 * @param flags   - Event processing flags (Ignored).
 * @return int : 1 indicating we're dont handling the event and Tcl can release its storage.
 * 
 */
int
CBindCommand::BindTraceEventHandler(Tcl_Event* pEvent, int flags) {
  pBindTraceEvent pFullEvent = reinterpret_cast<pBindTraceEvent>(pEvent);

  // Marshall the spectrum name an dbinding index into objects:

  CTCLObject name;
  name.Bind(pFullEvent->s_pInterp);
  name = pFullEvent->s_msg.s_spectrumName;

  CTCLObject index;
  index.Bind(pFullEvent->s_pInterp);
  index = pFullEvent->s_msg.s_bindIndex;

  // Get the binding trace singleton and invoke the correct tracer depending 
  // on pFullEvent->s_msg.s_unbind

  BindTraceSingleton& rBinder = BindTraceSingleton::getInstance();
  if (pFullEvent->s_msg.s_unbind) {
    rBinder.invokeUnbind(*pFullEvent->s_pInterp, name, index);

  } else {
    rBinder.invokeSbind(*pFullEvent->s_pInterp, name, index);
  }

  return 1;
}


/**
 * THe pump thread.  It needs the interpreter AND the thread of of the caller:
 * 
 */
typedef struct  _PumpClientData {
  CTCLInterpreter* s_pInterp;
  Tcl_ThreadId     s_target;
} PumpClientData, *pPumpClientData;


/**
 *  pumpThread
 *     This is the thread started for the pump.  We accept messages
 *  until given one with a zero length spectrum name, which is our signal
 *  to exit.
 *   @param cd - Actually a pPumpClientData struct pointer.,
 */
Tcl_ThreadCreateType
CBindCommand::pumpThread(ClientData cd) {
  auto pInfo = reinterpret_cast<pPumpClientData>(cd);

  while(1) {
    pBindTraceEvent pEvent = reinterpret_cast<pBindTraceEvent>(Tcl_Alloc(sizeof(BindTraceEvent)));
    pEvent->s_base.proc = BindTraceEventHandler;
    pEvent->s_base.nextPtr = nullptr;
    pEvent->s_pInterp = pInfo->s_pInterp;

    MPI_Status stat;
    if(MPI_Recv(
      &pEvent->s_msg, 1, messageType(), 
      MPI_EVENT_SINK_RANK, MPI_BIND_TRACE_RELAY_TAG, MPI_COMM_WORLD, 
    &stat) != MPI_SUCCESS) {
      throw std::runtime_error("Failed to receive a bind trace msg");
    }
    if (strlen(pEvent->s_msg.s_spectrumName) == 0) break;    // Special end message.
    Tcl_ThreadQueueEvent(pInfo->s_target, &(pEvent->s_base), TCL_QUEUE_TAIL);
    Tcl_ThreadAlert(pInfo->s_target);

  }

  delete pInfo;
  TCL_THREAD_CREATE_RETURN;
}

/**
 * startPump - must be called in the root rank.
 *   start the pumpTHread
 * 
 * @param rInterp - Reference to the interpreter on which dispatched commands should run.
 */
void
CBindCommand::startPump(CTCLInterpreter& interp) {
  pPumpClientData pData  = new PumpClientData;
  pData->s_pInterp = &interp;
  pData->s_target = Tcl_GetCurrentThread();

  Tcl_ThreadId id;
  Tcl_CreateThread(
    &id, pumpThread, reinterpret_cast<ClientData>(pData), 
    TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS);
}

/**
 *  stopPump
 *     Should be called from MPI_EVENT_SINK_RANK _ sends a stop message to the pump thread.
 * 
 */
void
CBindCommand::stopPump() {
  if (isMpiApp() && (myRank() == MPI_EVENT_SINK_RANK)) {
    std::string emptyName;
    forwardNewBinding(emptyName, 0);
  }
}

#endif