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
// Class: CTclGrammerApp

////////////////////////// FILE_NAME.cpp /////////////////////////////////////////////////////

#include <config.h>
#ifdef WITH_MPI  
#include <mpi.h>                         // Compiled with MPI support...
#endif
#include "TclGrammerApp.h"    				
#include <limits.h>
#include <assert.h>
#include "TCLHistogrammer.h"
#include "TestFile.h"
#include "MultiTestSource.h"
#include "TCLApplication.h"
#include "TCLVariable.h"
#include "TCLProcessor.h"
#include "TKRunControl.h"
#include "CRingBufferDecoder.h"
#include "RingFormatHelperFactory.h"

#include "Globals.h"
#include "RunControlPackage.h"
#include "ParameterPackage.h"
#include "DataSourcePackage.h"
#include "GatePackage.h"
#include "FilterCommand.h"
#include "EventSinkPipeline.h"
#include "CSpectrumStatsCommand.h"
#include "SpectrumDictionaryFitObserver.h"
#include "GateBinderObserver.h"
#include "GateCommand.h"
#include "GatingDisplayObserver.h"
#include "CHistogrammerFitObserver.h"

#include "TCLAnalyzer.h"

#include "SpecTclDisplayManager.h"
#include "SpectraLocalDisplay.h"
#include "XamineEventHandler.h"
#include <xamineDataTypes.h>
#include "PyQtDisplay.h"
#include "NullDisplay.h"

#include "SpecTcl.h"

#include <CTreeParameterCommand.h>
#include <CTreeVariableCommand.h>
#include <CTreeParameter.h>
#include <CTreeVariable.h>
#include <TCLException.h>
#include <TCLLiveEventLoop.h>
#include <TCLTimer.h>


#include "BindCommand.h"
#include "SpectrumCommand.h"

#include "CFoldCommand.h"
#include "CFitCommand.h"

#include <CProjectionCommand.h>
#include "IntegrateCommand.h"
#include "VersionCommand.h"
#include "SContentsCommand.h"

#include "SharedMemoryKeyCommand.h"
#include "SharedMemorySizeCommand.h"

#include "CPipelineCommand.h"
#include "CUnpackEvbCommand.h"

#include "CRemoteCommand.h"
#include "CMirrorCommand.h"

#include "ProductionXamineShMem.h"
#include "CHttpdServer.h"

#include "AbstractThreadedServer.h"
#include "MirrorServer.h"
#include "EventMessage.h"


#include <histotypes.h>
#include <buftypes.h>
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <histotypes.h>
#include <buftypes.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>
#include <memory>
#include <stdint.h>
#include <tcl.h>



#include <TclPump.h>
#include "RingItemPump.h"
#include "GatePump.h"

#include <TError.h>

// Fix for issue #122: suppress <RootX11ErrorHandler> messages:
#include <TError.h>

#if defined(Darwin)
#include <sys/syslimits.h>
#endif
#if defined(CYGWIN)
extern "C" 
void cygwin_conv_to_full_win32_path(const char *path, char *win32_path);
#endif



#include "TCLAnalyzer.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



// TCL Script to print the program version.

static const char* printVersionScript = 
"if {[file readable [file join $SpecTclHome VERSION]]} { \
   puts [exec cat [file join $SpecTclHome VERSION]]           \
}";

static const char* tclLibScript = "lappend auto_path [file join $SpecTclHome TclLibs]\n";

static const uint32_t DEFAULT_RING_VERSION(11);


// File scoped unbound variables:

int SpecTclArgc;
char** SpecTclArgv;

static const UInt_t knParameterCount = 256;
static const UInt_t knEventListSize  = 256;
static const UInt_t knDisplaySize    = 8;

static const char* kpInstalledBase = INSTALLED_IN; // Preprocessor def.
static const char* kpAppInitSubDir = "/etc";
static const char* kpAppInitFile   = "/SpecTclInit.tcl";
static const char* kpUserInitFile  = "/SpecTclRC.tcl";

static const char* ProtectedVariables[] = {
  "DisplayMegabytes",
  "ParameterCount",
  "EventListSize",
  "ParameterOverwriteAction",
  "SpectrumOverwriteAction",
  "TKConsoleHistory",
  "TKConsoleBufferSize",
  "NoPromptForNewGui",
  "splashImage",
  "HTTPDPort",
  "MirrorPort",
  0
};

// Static attribute storage and initialization for CTclGrammerApp

CTclGrammerApp* CTclGrammerApp::m_pInstance = NULL;
char** CTclGrammerApp::m_pArgV = NULL;
int CTclGrammerApp::m_argc = 0;

// Forward definitions:

static void MpiExitHandler();

/**
 * @class CSpecTclInitVar
 *    Class that handles traces for variables that are set in SpecTclInit.tcl
 *    -  On construction snapshot the variabkle value.
 *    -  If write trace fires and the value is different - restore the value
 *       and return an error.
 *    -  If an unset trace fires, return an error.
 */
class CSpecTclInitVar : public CTCLVariable
{
  std::string m_originalValue;
  std::string m_errorMessage;
public:
  CSpecTclInitVar(CTCLInterpreter* pInterp, const char* pVarName);
private:
  ~CSpecTclInitVar() {}                  // Not allowed to destroy.
public:
  virtual char* operator()(char* pName, char* pSubscript, int flags);
  
};

/**
 * CSpecTclInitVar constructor
 *    - Construct with tracing off.
 *    - Get the current value if defined.
 *    - turn on write and unset tracing (control over tracing flags is why we
 *      don't construct with tracing enabled).
 */
CSpecTclInitVar::CSpecTclInitVar(CTCLInterpreter* pInterp, const char* pName) :
  CTCLVariable(pInterp, pName, false)
{
  const char* pCurrentValue = Get();
  if (pCurrentValue) m_originalValue = pCurrentValue;
  
  Trace(TCL_TRACE_WRITES | TCL_TRACE_UNSETS);
}
/**
 * operator()
 *     Called when the trace fires.
 *     See class docs for action.
 *
 *  @param pName      - name of the variable.
 *  @param pSubscript - subscript of the variable.
 *  @param flags      - Reason the trace fired.
 *  @return char*     - Pointer to error message or NULL if set is allowed.
 */
char*
CSpecTclInitVar::operator()(char* pName, char* pSubscript, int flags)
{
  char msg[1000];
  
  // All unsets are illegal:
  
  if (flags & TCL_TRACE_UNSETS) {
    
    sprintf(
      msg, "%s can only be unset in SpecTclInit.tcl", getVariableName().c_str()
    );
    m_errorMessage = msg;
    Set(m_originalValue.c_str());
    return const_cast<char*>(m_errorMessage.c_str());
  } else if (flags & TCL_TRACE_WRITES) {
    
    // Writes that change the value are illegal.
    
    const char* pNewValue = Get();
    if (m_originalValue != std::string(pNewValue)) {
      Set(m_originalValue.c_str());
      sprintf(
        msg, "%s can only be set in SpecTclInit.tcl", getVariableName().c_str()
      );
      m_errorMessage = msg;
      return const_cast<char*>(m_errorMessage.c_str());
    }
  }
  return nullptr;
}

// Constructors, destructors and other replacements for compiler cannonicals:
/*!
   Constructing a CTclGrammerApp is what glues the library called SpecTcl
   into the program called SpecTcl.
*/
CTclGrammerApp::CTclGrammerApp() :
  m_nDisplaySize(knDisplaySize),
  m_nParams(knParameterCount),
  m_nListSize(knEventListSize),
  m_displayType("xamine"),
  m_pAnalyzer(0),
  m_pHistogrammer(0),
  m_pDecoder(0),
  m_pRunControl(0),
  m_pXamineEvents(0),
  m_pRunControlPackage(0),
  m_pParameterPackage(0),
  m_pSpectrumPackage(0),
  m_pDataSourcePackage(0),
  m_pGatePackage(0),
  m_RCFile(string("tcl_rcFilename"),            kfFALSE),
  m_TclDisplaySize(string("DisplayMegabytes"),  kfFALSE),
  m_TclParameterCount(string("ParameterCount"), kfFALSE),
  m_TclEventListSize(string("EventListSize"),   kfFALSE),
  m_TclDisplayType(string("DisplayType"),       kfFALSE),
  m_pMultiTestSource((CMultiTestSource*)kpNULL),
  m_nUpdateRate(1000),                              // Seconds between periodic events.
  m_pGatingObserver(NULL)
{
  if(gpEventSource != (CFile*)kpNULL) {
    if(gpEventSource->getState() == kfsOpen) {
      gpEventSource->Close();
    }
    delete gpEventSource;
    gpEventSource = (CFile*)kpNULL;
  }
}

// Destructor:
/*!
   Since exit singals exit from the application (SpecTcl), and since
   I've now got some order issues which result in core dumps on cleanup,
   We'll try not deleting any of the members.
*/
CTclGrammerApp::~CTclGrammerApp() {
    delete m_pGatingObserver;
}

// Functions for class CTclGrammerApp

/*!

  Add an event processor to the list of event processors 
  maintained by SpecTcl's analyzer.  This can only be 
  safely called at the time or after the time that 
  CreateAnalysisPipeline is called, since prior to that 
  there's no assurance that the analyzer has been instantiated.

  \param rEventProcessor (in):
     Reference  to the event processor to add to the pipeline

*/
void CTclGrammerApp::RegisterEventProcessor(CEventProcessor& rEventProcessor,
					    const char* name) {
  
  SpecTcl* api = SpecTcl::getInstance();
  api->AddEventProcessor(rEventProcessor, name);  // Auto registers.
  
}  

//  Function:
//    void BindTCLVariables(CTCLInterpreter& rInterp)
//  Operation Type:
//     Operation
/*
  Purpose:
  Called to bind variables to the TCL interpreter used by SpecTcl.
  This member can be overidden, however for SpecTcl to work
  correctly it must be overridden as an extension, that is
  there should be as the first action a call to:

  CTclGrammerApp::BindTCLVariables
*/
void CTclGrammerApp::BindTCLVariables(CTCLInterpreter& rInterp) {
  // The following variables are bound to the interpreter passed as a 
  // parameter:
  //
  // m_RCFile         - Name of early init file.
  // m_TclDisplaySize - Number of megabytes of display storage.
  // m_ParameterCount - Guess at largest parameter number which will be stuffed
  // m_TclEventListSize - Number of event batched for analysis.
  // m_TclDisplayType-  Type of display to use

  CTCLVariable HomeDir(string("SpecTclHome"), kfFALSE);
  HomeDir.Bind(rInterp);
#ifdef HAVE_WINDOWS_H
  // For CYGWIN, we need to modify the name so that it is the NT path
  // since our Tcl/Tk is unaware that Cygwin exists.
  
  char Win32Path[PATH_MAX+1];
  cygwin_conv_to_full_win32_path(kpInstalledBase, Win32Path);
  //
  // Convert the \'s to /'s.
  //
  for(int i = 0; i < strlen(Win32Path); i++) {
    if(Win32Path[i] == '\\') Win32Path[i] = '/';
  }
  HomeDir.Set(Win32Path);
#else
  HomeDir.Set((char*)kpInstalledBase);
#endif
  m_RCFile.Bind(rInterp);
  m_TclDisplaySize.Bind(rInterp);
  m_TclParameterCount.Bind(rInterp);
  m_TclEventListSize.Bind(rInterp);
  m_TclDisplayType.Bind(rInterp);

  // Append TclLibs to the auto_path:
  
  rInterp.GlobalEval(tclLibScript);
}

//  Function:
//    void SourceLimitScripts(CTCLInterpreter& rInterpreter)
//  Operation Type:
//     Behavior
/*!

  Sources initialization scripts.  The scripts
  sourced at this point in time are intended to
  allow additional variables to be defined and set
  as well as limits etc.  It is not safe to do 
  arbitrary SpecTcl commands at this point.
  Additional 'functional' scripts can be sourced
  from an overridden operator()  Note that the
  default implementation can be overridden, however it
  sources scripts which set various limit variables and
  therefore an override should include the code:

  \param rInterpreter (in):
      Reference to the interpreter that will be used to 
      source the scripts.

*/
void CTclGrammerApp::SourceLimitScripts(CTCLInterpreter& rInterpreter) {
  // The script being run is intended to set initial values
  // for all of the variables which are required by later stages
  // of initialization e.g. DisplayMegabytes. The scripts are searched for
  // and run from the following locations (in the order given):
  //   $SpecTclHome/Scripts
  //   ~
  //   .

  string  AppFilename(kpInstalledBase);
  AppFilename += kpAppInitSubDir;
  AppFilename += kpAppInitFile;
  std::string result = SourceOptionalFile(rInterpreter, AppFilename);
  if (result != "") {
    std::cerr << result << std::endl;
    exit(-1);
  }


  if(getenv("HOME")) {		// Need home env variable.

    string AppFilename(getenv("HOME"));
    AppFilename += kpAppInitFile;
    result = SourceOptionalFile(rInterpreter, AppFilename);
    if (result != "") {
      std::cerr << result << std::endl;
      exit(-1);
    }

  }


  AppFilename = ".";
  AppFilename += kpAppInitFile;
  result = SourceOptionalFile(rInterpreter, AppFilename);
  if (result != "") {
    std::cerr << result << std::endl;
    exit(-1);
  }
  /**
     Now that the limit files are source we're going to noisily prevent
     the user from modifying the following variables:
     - DisplayMegabytes - Xamine shared memory data size.
     - ParameterCount   - Initial # of parameters in an event.
     - EventListSize    - Number of events batched before a histogram pass.
     - ParameterOverwriteAtion - obsolete - determines action on read of
                          SpecTcl parameters using an old read in file.
     - TkConsoleHistory - Number of lines in TkCon history
     - TkConsoleBufferSize - Number of bytes in the TkCon scrollback.
     - NoPromptForNewGui - False if prompt for new/old gui.
     - splashImage       - File containg the Tk Splash image displayed during
                           SpecTclRC.tcl execution.  Must be a supported
                           Tk image/photo type.
     - HTTPDPort   - HTTPD Server port.
  */
  for (const char** pVarName = ProtectedVariables; *pVarName != 0; pVarName++) {
    protectVariable(getInterpreter(), *pVarName);
  }
  // We can do the WorkerChunkSize thing even if we are not in MPI
  // It just gets ignored otherwise.  What we do:
  // Create the TCL Variable.
  // If it has a value we're done.
  // IF not set it to "1" the default chunksize.
  //

  CTCLVariable maxChunkVar(&rInterpreter, WORKER_CHUNKSIZE_VAR, TCLPLUS::kfFALSE);
  if (! maxChunkVar.Get()) {
    maxChunkVar.Set(DEFAULT_MAX_CHUNK_SIZE);
  }
}  
  

//  Function:
//    void SetLimits()
//  Operation Type:
//     Behavior
/*!

  Based on script variables etc. set limits and other statically defined variables.
  The default implementation uses this to set the final values for the size of
  the display subsystem shared memory, the parameter array size and the
  size of the event list.  If this method is overridden, you should call the
  base class functionality to ensure that SpecTcl is properly started:


*/
void CTclGrammerApp::SetLimits() {
  UInt_t nResult;

  // The following members can be updated if the corresponding variables
  // have been created/modified:
  //   m_nDisplaySize    - # megabytes display memory (DisplayMegabytes).
  //   m_nParams         - Number of parameters in an event (ParameterCount).
  //   m_nListSize       - # events in a histogramming batch (EventListSize).

  // By this time the initial RC files have been run.
  UpdateUInt(m_TclDisplaySize,   m_nDisplaySize);
  UpdateUInt(m_TclParameterCount, m_nParams);
  UpdateUInt(m_TclEventListSize, m_nListSize);

  UpdateString(m_TclDisplayType, m_displayType);
}

//  Function:
//    void CreateHistogrammer()
//  Operation Type:
//     Behavior
/*!

  Sets up the initial data sink configuration of spectcl.  SpecTcl
  supports an ordered set of processors that take unpacked event lists
  and operate on them to produce the desired analysis.  By default, Spectcl
  always an initial default entry in this <em> event sink pipeline, </em>
  a histogrammer.  This function creates the pipeline, hooks it into the 
  program superstructure, creates the histogrammer and inserts it in the
  pipeline.
*/
void CTclGrammerApp::CreateHistogrammer() {
  gpEventSinkPipeline = new CEventSinkPipeline;
  m_pHistogrammer     = new CTCLHistogrammer(gpInterpreter);
  gpEventSink = m_pHistogrammer;
  gpEventSinkPipeline->AddEventSink(*m_pHistogrammer, "::Histogrammer");

  SpecTcl& api = *(SpecTcl::getInstance());
  api.addSpectrumDictionaryObserver(new SpectrumDictionaryFitObserver);
}

/*! Function:
* void CreateDisplays()
*
*  This creates the various displays that could be used during the session.
*  Prestart configuration of the displays should be done within this
*  method.
*/
void CTclGrammerApp::CreateDisplays()
{
    SpecTcl& api = *(SpecTcl::getInstance());

    m_pDisplayInterface = new CSpecTclDisplayInterface;
    api.SetDisplayInterface(*m_pDisplayInterface);

    std::shared_ptr<CXamineSharedMemory> pShMem(new CProductionXamineShMem(m_nDisplaySize*1024*1024));
    gpDisplayMemory = pShMem->getSharedMemoryPointer();
    
    // Set up the Xamine display to use the appropriate display size
    CDisplayCreator* pCreator = gpDisplayInterface->getFactory().getCreator("xamine");
    CXamineCreator* pXCreator = dynamic_cast<CXamineCreator*>(pCreator);
    if (pXCreator != NULL) {
        pXCreator->setSharedMemory(pShMem);
    } else {
        throw std::runtime_error("Failed to cast to a CXamineCreator");
    }

    // Set up PyQt display to use the appropriate display size
    pCreator = gpDisplayInterface->getFactory().getCreator("qtpy");
    if (pCreator != NULL) {
      std::cout << "pCreator is OK" << std::endl;
    } else {
        throw std::runtime_error("Failed to cast to a pCreator");      
    }
    CPyQtCreator* pPyQtCreator = dynamic_cast<CPyQtCreator*>(pCreator);
    if (pPyQtCreator != NULL) {
        pPyQtCreator->setSharedMemory(pShMem);
    } else {
        throw std::runtime_error("Failed to cast to a CPyQtCreator");
    }    

    
#ifdef USE_SPECTRA
    // Set up the Xamine display to use the appropriate display size
    pCreator = gpDisplayInterface->getFactory().getCreator("spectra");
    Spectra::CSpectraLocalDisplayCreator* pSpectraCreator
            = dynamic_cast<Spectra::CSpectraLocalDisplayCreator*>(pCreator);
    if (pSpectraCreator != NULL) {
        pSpectraCreator->setSharedMemory(pShMem);
    } else {
        throw std::runtime_error("Failed to cast to a CSpectraLocalDisplayCreator");
    }
#endif

    // Null displayer also needs shared memory  to support remote access:
    
    pCreator = gpDisplayInterface->getFactory().getCreator("null");
    CNullDisplayCreator* pNCreator = dynamic_cast<CNullDisplayCreator*>(pCreator);
    if (pNCreator) {
      pNCreator->setSharedMemory(pShMem);
    }
    
    // Create the displays so they can chosen.
    m_pDisplayInterface->createDisplay("xamine",  "xamine");
    m_pDisplayInterface->createDisplay("batch",   "null");
    m_pDisplayInterface->createDisplay("none", "null");
    m_pDisplayInterface->createDisplay("qtpy",   "qtpy");    
#ifdef USE_SPECTRA    
    m_pDisplayInterface->createDisplay("spectra", "spectra");
#endif

  // If SpecTclInit.tcl provided a MirrorPort variable,
  // We start a mirror server on that port, serving out the memory
  // pointed to by gpDisplayMemory.
  // in mpiSpecTcl, this is only started in the event sink as it has the memory to mirror.
  //
  
  CTCLVariable mirrorPort(gpInterpreter, "MirrorPort", false);
  const char* mirrorPortNum = mirrorPort.Get();
  if (mirrorPortNum) {
    MirrorServerFactory* pFactory =
      new MirrorServerFactory(
        reinterpret_cast<Xamine_shared*>(const_cast<void*>(gpDisplayMemory))
      );
    auto mirrorServer = new ServerListener(mirrorPortNum, pFactory);
    mirrorServer->start();
    // Mirror port for QtPy GUI
    int mp = atoi(mirrorPortNum);
    std::string mport = std::to_string(mp);
    ::setenv("MIRRORport", mport.c_str(), 1);
  } else {
    ::setenv("MIRRORport", "0", 1);   /// local no mirror may be needed
  }
  
 

}

/*! \brief CTclGrammerApp::SelectDisplayer()
*   Operation type:
*    Behavior
*
* Selects the displayer to use and starts it if it exists.
*
* \throws std::runtime_error if user specified display that doesn't exist
*/
void CTclGrammerApp::SelectDisplayer()
{
    SpecTcl* pApi = SpecTcl::getInstance();

    std::cout << "Inside SelectDisplayer: m_displayType -> " << m_displayType << std::endl;
    m_pDisplayInterface->setCurrentDisplay(m_displayType);

    CDisplay* pDisplay = m_pDisplayInterface->getCurrentDisplay();
    if (pDisplay) {
        pDisplay->start();
    } else {
        std::string error("User specified display type does not exist.");
        throw std::runtime_error(error);
    }
    
}
/**
 *  SelectDisplayer
 *     Provided for compatibility with 4.0 and earlier.  Warns that this
 *     is obsolete and calls the above.
 */
void CTclGrammerApp::SelectDisplayer(UInt_t n, CHistogrammer& rh)
{
  std::cerr << "---------------------------------------------------\n";
  std::cerr << "    Warning - Deprecated method used               \n\n";
  std::cerr << "  Your application class calls SelectDisplayer     \n";
  std::cerr << "  Providing two parameters (size and histogrammer  \n";
  std::cerr << "  This method is obsolete and may be removed later \n";
  std::cerr << "  replace it with 'SelectDisplayer();'              \n";
  std::cerr << "  This most likely is in CMySpecTclApp::SelectDisplayer \n";
  std::cerr << "----------------------------------------------------\n";
}

/*!
 * \brief CTclGrammerApp::SetUpDisplay
 *
 *  This is primarily useful for adding observers to the displays. For example,
 *  the GateBinderObserver and the CXamineEventHandler need to be registered here.
 *  These are responsible for monitoring the gate dict and also an application of
 *  a gate to a spectrum. These cause changes in the display.
 */
void CTclGrammerApp::SetUpDisplay()
{
    SpecTcl* pApi = SpecTcl::getInstance();
    pApi->addGateDictionaryObserver(new CGateBinderObserver(*m_pDisplayInterface,
                                                             *m_pHistogrammer));

    m_pGatingObserver = new CGatingDisplayObserver(m_pDisplayInterface);
    pApi->GetHistogrammer()->addGatingObserver(m_pGatingObserver);

    if (m_displayType == "xamine") {
        // We need to set up the Xamine event handler however:
        m_pXamineEvents = new CXamineEventHandler(static_cast<CHistogrammer*>(pApi->GetHistogrammer()),
                                                  dynamic_cast<CXamine*>(m_pDisplayInterface->getDisplay("xamine")));
    }
}

//  Function:
//    void SetupTestDataSource()
//  Operation Type:
//     Behavior
/*!
  Sets up a test (internal) data source.  The default
  behavior is to create  fixed length event with some
  gaussian distributions for parameters.  This source is
  used by SpecTcl developers to test functionality without
  referring to real data.  Note that an eventsource is currently
  necessary to create and setup the run control object.
*/
void CTclGrammerApp::SetupTestDataSource() {
  // Uses a singleton map with at least the default test, 
  // but also possibly named user-defined test distributions.

  m_pMultiTestSource = CMultiTestSource::GetInstance(); 
  m_pMultiTestSource->useDefaultTestSource();
}

//  Function:
//    void CreateAnalyzer(CEventSink* pSink)
//  Operation Type:
//     Behavioral
/*!
  Creates a data analyzer.  The histogrammer
  in gpEventSink is set as the analyzer's event sink.
  Note that by default a CTclAnalyzer is created.
  \param pSink (in)
     The event sink to which unpacked event lists are sent.
*/
void CTclGrammerApp::CreateAnalyzer(CEventSink* pSink) {
  // A TCLAnalyzer is created as the analyzer

  m_pAnalyzer = new CTclAnalyzer(*gpInterpreter,
				 m_nParams,
				 m_nListSize);
  gpAnalyzer  = m_pAnalyzer;

}  

//  Function:
//    void SelectDecoder(CAnalyzer& rAnalyzer)
//  Operation Type:
//     Behavioral
/*!

  Selects the initial Buffer decoder to be associated with the
  data stream.  The default is to instantiate an NSCLBufferDecoder,
  connect it to the global pointer: gpBufferDecoder  and in turn
  to the analyzer.  Now switches on the attach command can select a buffer
  decoder.

  \param rAnalyzer [modified]
     The analyzer object that is being fed raw data.


*/
void CTclGrammerApp::SelectDecoder(CAnalyzer& rAnalyzer) {
  // An NSCL Buffer decoder is produced, saved and hooked to the analyzer:
  // As of Resolving Issue # 95 we default to Ring buffer decoders
  // configured with a default helper for NSCLDAQ-11:

  auto pDecoder =  new CRingBufferDecoder;
  m_pDecoder = pDecoder;
  auto factory = pDecoder->getFormatFactory();
  pDecoder->setDefaultFormatHelper(factory->create(DEFAULT_RING_VERSION,0));
  pDecoder->setFormatHelper(factory->create(DEFAULT_RING_VERSION, 0));
  
  gpBufferDecoder = m_pDecoder;
  rAnalyzer.AttachDecoder(*m_pDecoder);
}

//  Function:
//    void CreateAnalysisPipeline(CAnalyzer& rAnalyzer)
//  Operation Type:
//     Behavioral
/*!

  This must be provided by the subclass.  The experimenter
  must set up the analysis pipeline which manages the
  data received from the data source.  This pipeline
  consists of an ordered set of CEventProcessor
  derived objects.

  Therefore this member function is abstract.

  \param rAnalyzer[modified]
     The analyzer in which the pipeline is being created.

  (NOTE: g++ allows us to 'implement' an abstract member, however the
  member is still treated as abstract externally. We may  need to yank
  this function definition with later compiler versions.


*/
void CTclGrammerApp::CreateAnalysisPipeline(CAnalyzer& rAnalyzer) {}

//  Function:
//    void AddCommands(CTCLInterpreter& rInterp)
//  Operation Type:
//     Behavioral
/*!

  Registers the commands and command packages
  which make up SpecTcl.  Note that the default
  method registers the standard SpecTcl commands.
  If an override is supplied to add user commands, you
  must invoke the base class function at some point:

  \param rInterp [modified]
     The interpreter on which the commands are being registered.
*/
void CTclGrammerApp::AddCommands(CTCLInterpreter& rInterp) {
  // All of the 'standard' SpecTcl commands are organized as packages
  // of related commands.  These packages are not Tcl packages but are just
  // groups of commands which share a common set of services provided
  // by a containing class.

  // Commands get registered everywhere as they are built to run in the
  // correct rank(s) when parallel, but the signons need to be 
  // only output in the root process.
  
  m_pRunControlPackage = new CRunControlPackage(&rInterp);
  m_pRunControlPackage->Register();
  m_pRunControlPackage->InitializeRunState();
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << m_pRunControlPackage->getSignon() << endl;
    cerr << "Tabbed widget (xmTabWidgetClass) used in Xamine thanks to Pralay Dakua \n";
  }

  
  // Tacit assumption that the event sink is a histogrammer
  m_pParameterPackage = new CParameterPackage(&rInterp, 
					      (CTCLHistogrammer*)gpEventSink);

  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << m_pParameterPackage->getSignon() << endl;
  }

  m_pSpectrumPackage  = new CSpectrumPackage(&rInterp, 
                         (CHistogrammer*)gpEventSink,
                         gpDisplayInterface);

  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << m_pSpectrumPackage->getSignon() << endl;
  }
  if (!gMPIParallel || m_mpiRank == MPI_ROOT_RANK) {    // attach/ringformat are only in root.
    m_pDataSourcePackage = new CDataSourcePackage(&rInterp);
    m_pDataSourcePackage->Register();
  }
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << m_pDataSourcePackage->getSignon() << endl;
  }

  m_pGatePackage = new CGatePackage(&rInterp, 
				    (CHistogrammer*)gpEventSink);
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << m_pGatePackage->getSignon() << endl;
  }

  // For Filter command.
  auto pFilterCommand = new CFilterCommand(rInterp); //"filter"
  
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "Filter command (c) 2003 NSCL written by  Kanayo Orji\n";
  }

  // Create the tree parameter package commands and bind any variables
  // that have been defined:


  CTreeParameterCommand* pTreeParamCommand = new CTreeParameterCommand(&rInterp);
  CTreeVariableCommand*  pTreeVariableCommand = new CTreeVariableCommand;
  CTreeVariable::BindVariables(*(getInterpreter()));

  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "Tree parameter/variable  command " << CTreeParameter::TreeParameterVersion;
    cerr << " (c) Copyright 2005 NSCL written by Daniel Bazin, Ron Fox\n";
  }

  CFoldCommand* pFold = new CFoldCommand(&rInterp);

  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "fold command (c) 2005 NSCL Written by Ron Fox\n";
  }

  CProjectionCommand* pProjection = new CProjectionCommand(rInterp);

  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "project command (c) 2005 NSCL Written by Ron Fox\n";
  }

  CFitCommand *Fit  = new CFitCommand(rInterp);
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "fit command (c) 2006 NSCL Written by Ron Fox\n";
  }

  CIntegrateCommand* pIntegrate = new CIntegrateCommand(rInterp);
  
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "integrate command (c) 2007 Written by Ron Fox\n";
  }
  
  CVersionCommand* pVersion = new CVersionCommand(rInterp);
  CSContentsCommand* pContents = new CSContentsCommand(rInterp);
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "version, scontents command (c) 2015 Written by Ron Fox\n";
  }
  
  new CSpectrumStatsCommand(rInterp);
  
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "specstats - spectrum statistics command (c) 2015 Written by Ron Fox\n";
  }
  
  new CSharedMemoryKeyCommand(rInterp, *SpecTcl::getInstance());
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "shmemkey - shared memory key command (c) 2016 Written by Jeromy Tompkins\n";
  }

  new CSharedMemorySizeCommand(rInterp);
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "shmemsize - shared memory size command (c) 2016 Written by Jeromy Tompkins\n";
  }

  new CPipelineCommand(rInterp);
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "pman - analysis pipeline manager (c) 2018 Written by Giordano Cerizza\n";
  }
  new CUnpackEvbCommand(rInterp);
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    cerr << "evbunpack - Event built data unpacking manager (c) 2018 written by Ron Fox\n";
  }
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    new CRemoteCommand(rInterp);
  }
  new CMirrorCommand(rInterp);
  
  cerr.flush();
}

//  Function:
//    void SetupRunControl()
//  Operation Type:
//
/*!
   Setup the run control management.  Run control in this case means
   managing the events that can indicate data readiness, enabling and 
   disabling this.

*/
void CTclGrammerApp::SetupRunControl() {
  // We use a Tk run control.  That's able to make use of the Tk
  // event loop processing software:

  // We use the globals in case some functions have been overridden.

  m_pRunControl = new CTKRunControl(gpInterpreter, *gpAnalyzer, 
				    *gpEventSource);
  gpRunControl = m_pRunControl;
}

//  Function:
//    void SourceFunctionalScripts(CTCLInterpreter& rInterp)
//  Operation Type:
//     Behavioral
/*!

  This function provides an opportunity to
  source scripts which perform functional
  operations either in spectcl or in other (e.g. Tk, Blt)
  packages which are now all hooked together properly.
  We'll try to locate SpecTclRC.tcl first in ~ and then in .

  \note Changed behavior... used to only look in ~
  All found ones will be run in the order found.

  \param rInterp [in]:
    The interpreter that will execute these scripts.
  
*/
void CTclGrammerApp::SourceFunctionalScripts(CTCLInterpreter& rInterp) {
  std::string result;
  if(getenv("HOME")) {
    string RCFilename(getenv("HOME"));
    RCFilename += kpUserInitFile;
    result = SourceOptionalFile(rInterp, RCFilename);
    if (result != "") {
      std::cerr << result << std::endl;
      exit(-1);
    }
  }



  string RCFilename(".");
  RCFilename += kpUserInitFile;
  result = SourceOptionalFile(rInterp, RCFilename);
  if (result != "") {
    std::cerr << result << std::endl;
    exit(-1);
  }
    
}

//  Function:
//    int operator()()
//  Operation Type:
//     Entry point
/*!
  
  Called when the tcl interpreter starts up.  The base
  functionality is to call the action member functions
  (e.g. CreateAnalyzer) in the appropriate order.  This
  can be extended by overriding and delegating the
  operation to the base class or by writing an entirely
  new startup scheme as desired.
*/
int CTclGrammerApp::operator()() {
  try {
  // Fetch and setup the interpreter member/global pointer.
  // All ranks get an interpreter and source these scripts:

  gpInterpreter = getInterpreter();
  
  // Fix for issue #122: suppress <RootX11ErrorHandler> messages:
  gErrorIgnoreLevel = kFatal;
  
  // Bind any variables to Tcl:
  BindTCLVariables(*gpInterpreter);

  // Source limit setting scripts; done everywhere.
  SourceLimitScripts(*gpInterpreter);

  // Based on all of this set the final startup limits/values:
  SetLimits();

  // Create the histogrammer event sink:

  // Histogramer is only created if:
  // ! mpi or mpi but rank1 (the event sink pipeline).  Same for the displays:
  // 

  if (!gMPIParallel || (m_mpiRank == MPI_EVENT_SINK_RANK)) {
    CreateHistogrammer();

    // Create the available displays
    CreateDisplays();

    // If we are mpi parallel we need to start the histogram and state change pump too:

    if (gMPIParallel) {
      startHistogramPump();
      startStateChangePump();
    }
  }
  // All ranks but the event sink must start the pump to get them
  // xamine gates if Xamine is the displayer:

  if (gMPIParallel && (m_mpiRank != MPI_EVENT_SINK_RANK)) {
    startGatePump();
  }

  // The servers run in RANK 0 in the MPI:

  if(!gMPIParallel || ((m_mpiRank == MPI_ROOT_RANK) || (m_mpiRank == MPI_EVENT_SINK_RANK))) { 
    // Setup the histogram displayer.. note here's where we also
    // handle the case where a use has specified
    // HTTDPort in their init file:
    // We'll run this in the 
    CTCLVariable http(gpInterpreter, "HTTPDPort", false);
    const char* httpdPort = http.Get();
    if (httpdPort) {
      // This must be an integer in the non privileged port range:
      
      int nPort = atoi(httpdPort);
      if (nPort < 1024) {
          std::cerr << "The HTTPDPort SpecTclInit.tcl variable must be an integer > 1023"
              << " it was: '" << httpdPort << "'\n";
          exit(EXIT_FAILURE);
      }

      char hostbuffer[256];
      int hostname;
      hostname = gethostname(hostbuffer, sizeof(hostbuffer));
      std::string host(hostbuffer);
      
      if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
        CHttpdServer server(gpInterpreter);
        try {
            if(!server.isRunning()) server.start(nPort);
            
            //host += ".nscl.msu.edu";
          
            std::cout << "hostname: " << host << std::endl;
            std::cout << "port: " << atoi(httpdPort) << std::endl;  
          
            int p = atoi(httpdPort);
            std::string port = std::to_string(p);
            ::setenv("RESThost", host.c_str(), 1);
            ::setenv("RESTport", port.c_str(), 1);
        }
        catch (std::exception& e) {
            std::cerr << "Unable to start the SpecTcl REST server: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
      } else {
        // Event sink rank needs to set the environment vars:
          int p = atoi(httpdPort);
          std::string port = std::to_string(p);
          ::setenv("RESThost", host.c_str(), 1);
          ::setenv("RESTport", port.c_str(), 1);
      }

    } else {
      ::setenv("RESThost", "host", 1);   /// local no mirror may be needed
      ::setenv("RESTport", "0", 1);   /// local no mirror may be needed
    }
  }

  
  // Displayer is  run in the EVENT sink rank.

  if (!gMPIParallel || (m_mpiRank == MPI_EVENT_SINK_RANK)) {
    SelectDisplayer();

    // Set up the display that was picked
    SetUpDisplay();
  }
  if (gMPIParallel) {
    // We need to establish the exit handler here because
    // If the displayer is Xamine, SpecTcl also forks of a copy of itself to monitor
    // if it exits, that copy inherits our exit handler and thinks it's the event sink rank
    // and tries to stop pumps the event sink rank stops; hanging.
    atexit(MpiExitHandler);
  }

  // Setup the test data source: - in the root rank.
  // The analyzer and decoder are done here as when we are analyzing data
  // it's the root process that ships it off to the workers.
  //  These are also needed in the workers.

  if (!gMPIParallel ||(m_mpiRank == MPI_ROOT_RANK)) {
    SetupTestDataSource(); // No longer done. By default, no source is to be set so that users aren't mistakenly fooled by test data.
  }
  if (!gMPIParallel || 
    (gMPIParallel && ((m_mpiRank == MPI_ROOT_RANK) || (m_mpiRank >= MPI_FIRST_WORKER_RANK)))
  ) {
    // Create an analyzer and hook the histogrammer to it.
    //CreateAnalyzer(gpEventSink);
    CreateAnalyzer(gpEventSinkPipeline);
    
      //  Setup the buffer decoder:
    SelectDecoder(*gpAnalyzer);
  
  }
  
  // Setup the command packages: -- all ranks do this.
  AddCommands(*gpInterpreter);

  // the run control objects. -- Root rank for now.

  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    SetupRunControl();
  }
  //  Setup the user's analysis pipeline worker ranks -- and if, mpi start the ring item pumps:
  //
  if (!gMPIParallel || (m_mpiRank >= MPI_FIRST_WORKER_RANK)) {
    CreateAnalysisPipeline(*gpAnalyzer);
    if (gMPIParallel) {
      startRingItemPump();
    }

  }
  // User's code may have created statically defined tree parameters.  
  // These need to bound in all ranks.
  // So that the parameter dicts are consistent
  // TODO - figure out what to do if the user creates them at runtime.
  // CraeteAnalysisPipeline.
  // 
  CTreeParameter::BindParameters();           // Needed by treeparameter in all ranks
  // Finally the user may have some functional setup scripts they want
  // to run.  By the time these are run, SpecTcl is essentially completely
  // set up... done in the root with appropriate e.g. broadcasts
  //
  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {
    SourceFunctionalScripts(*gpInterpreter);
  
    // Now that SpecTcl is essentially set up, we can initialize the analyzer
    
    SpecTcl*      pApi      = SpecTcl::getInstance();
    CTclAnalyzer* pAnalyzer = pApi->GetAnalyzer();
    pAnalyzer->OnInitialize();
  }  
  // Set up the first incantaion of TimedUpdates.  This has to do with display shared memory
  // and only the event sink pipe knows that:

  if (!gMPIParallel || (m_mpiRank == MPI_EVENT_SINK_RANK)) {
  
    Tcl_CreateTimerHandler(m_nUpdateRate, CTclGrammerApp::TimedUpdates, this);
  }


  // Credits only go out in root rank:

  if (!gMPIParallel || (m_mpiRank == MPI_ROOT_RANK)) {

    // Additional credits.

    cerr << "SpecTcl and its GUI would not be possible without the following open source software: \n";
    cerr << "    - Gri  by Dan Kelley and Peter Galbraith.\n";
    cerr << "    - TkCon by Jeff Hobbs\n";
    cerr << "    - BWidgets by Jeff Hobbs\n";
    cerr << "    - BLT by George Howlett\n";
    cerr << "    - Snit by Will Duquette\n";
    cerr << "    - TkTable by Eric Melski, Jeff Hobbs, Joe English and Pat Thoyts\n";
    cerr << "    - IWidgets by David Gravereaux, Don Porter, Jeff Hobbs, Mark Harrison,\n";
    cerr << "                  Marty Backe, Michael McLennan, Chad Smith, and Brent B. Welch\n";
    cerr << "    - Tcl/Tk originally by John K. Ousterhout embellished and extended by the Tcl Core Team\n";
    cerr << "    - Daniel Bazin for the concept of TreeParameter and its original GUI\n";
    cerr << "    - Leilehau Maly and Tony Denault of the NASA IRTF Telescope\n";
    cerr << "      for the  gaussian fit harnesses to the gsl: fitgsl.{c,h}\n";
    cerr << "    - Emmanuel Frecon Swedish Institute of Computer Science for the splash package\n";
    cerr << "    - Kevin Carnes James R. Macdonald Laboratory Kansas State University\n";
    cerr << "      for many good functionality suggestions and for catching some of my stupidities\n";
    cerr << "    - Dirk Weisshaar NSCL for many suggestions for performance and functional improvements\n";
    cerr << "    - Dave Caussyn at Florida State University for comments and defect fixes\n";
    cerr << " If your name should be on this list and is not, my apologies, please contact\n";
    cerr << " fox@nscl.msu.edu and let me know what your contribution was and I will add you to\n";
    cerr << " the list of credits.\n";

    // Finally run the version script:

      try {
        gpInterpreter->GlobalEval(printVersionScript);
      }
      catch (...) {
        cerr << "SpecTcl Version: " << gpVersion << endl;
      }
    }  
    gErrorIgnoreLevel = kFatal;
  }
  // Turn off all those root warnings about x11 crap (I hope).

  

  catch (std::string msg) {
    std::cerr << "Caught string exception in init: " << msg << std::endl;
  }
  catch (const char* msg) {
    std::cerr << "Caught char exception in init: " << msg << std::endl;
  }
  catch (CException& e) {
    std::cerr << "Caught CException in init: " << e.ReasonText() << std::endl;
  }
  catch (std::exception& e) {
    std::cerr << "Caught std:exception in init: " << e.what() << std::endl;
  }
  
  return TCL_OK;                        // If we got here return TCL_OK.
}

CTCLInterpreter* CTclGrammerApp::getInterpreter() {
    return gpInterpreter;
}


// Function:
//   static void UpdateUInt(CTCLVariable& rVar, UInt_t& rValue)
// Operation Type:
//   Utility.
void CTclGrammerApp::UpdateUInt(CTCLVariable& rVar, UInt_t& rValue) {
  int nResult;

  const char* pValue(rVar.Get(TCL_LEAVE_ERR_MSG|TCL_GLOBAL_ONLY));
  if(pValue) {
    if(sscanf(pValue, "%d", &nResult) > 0) {
      rValue = nResult;
    }
    else {			// Value not unsigned complain and no update
      cerr << "The value of the Tcl variable " << rVar.getVariableName();
      cerr << " is " << pValue;
      cerr << " which does not decode to an unsigned int.\n";
      cerr << "SpecTcl will ignore this value and use its internal default.\n";
      cerr.flush();
    }
  }
  // No update.
}

// Function:
//   static void UpdateString(CTCLVariable& rVar, std::string& rValue)
// Operation Type:
//   Utility.
void CTclGrammerApp::UpdateString(CTCLVariable& rVar, std::string& rValue) {

  const char* pValue(rVar.Get(TCL_LEAVE_ERR_MSG|TCL_GLOBAL_ONLY));
  if(pValue) {
      rValue = pValue;
  }
  // No update.
}


/**
 * Utility funtion that sources Tcl optional sript. The script is optional in the sense
 * that file not found errors are not reported.  What is reported are:
 * - File not readable (exists but no read access).
 * - Errors from the script itself.
 * 
 * @param rInterp - Reference to the Tcl interpreter that should source the file.
 * @param filename -name of the script to check.
 * 
 * @return std::string
 * @retval  empty string if no error or file not found.
 * @retval  reason for the error if there was one.
 */
std::string
CTclGrammerApp::SourceOptionalFile(CTCLInterpreter& rInterp, std::string filename) 
{
  // daqdev/SpecTcl#390:  If the script file exists, but is a symlink that's broken,
  //                      Output a warning to stderr but return "" so the caller
  //                      won't exit.
  char targetFile[PATH_MAX+1];
  memset(targetFile, 0, sizeof(PATH_MAX+1));         // So we have null termination.
  ssize_t nBytes = readlink(filename.c_str(), targetFile, PATH_MAX);
  
  // EINVAL Means not a symlink.
  // ENOENT  means the link doesn't exist.
  // Other errors are >bad<.
  
  if(nBytes < 0) {
    if (errno == ENOENT) return "";              // Just like missing file.
    if (errno != EINVAL) {
      std::string reason = string(strerror(errno));
      std::string msg = "Unable to check if ";
      msg += filename;
      msg += " is a symlink and if that symlink is broken: ";
      msg += reason;
      return reason;
    }
    
  } else {
    // Symlink read:
    
    
    
    int stat = access(targetFile, F_OK);
    if (stat && errno == ENOENT) {
      std::cerr << "****WARNING: " << filename << " is a broken symlink\n";
      std::cerr << "****Continuing\n";
      return "";
    }
    filename = targetFile;          // Use the target instead of the link.
    
  }
  
  // Probe existence and return empty if not found.

  int stat = access(filename.c_str(), F_OK);
  if (stat && (errno == ENOENT)) {
    return std::string("");
  }
  if (stat) {
    std::string reason = string(strerror(errno));
    std::string error = "Existence check for ";
    error += filename;
    error += " failed: ";
    error += reason;
    return error;
  }

  // Probe readabilty and return an error if not readable.

  if (access(filename.c_str(), R_OK)) {
    std::string reason = std::string(strerror(errno));
    std::string error = "Readability check failed for ";
    error += filename;
    error += " : ";
    error += reason;
    return error;
  }

  
  // not exist, 
  // Source the script converting an exception in to an error messasge.
  // error message will include the error info too.

  try {
    rInterp.EvalFile(filename);
    return std::string("");    
  }
  catch (CTCLException& e) {
    std::string reason = e.ReasonText();
    std::string error  = "Error processing file: ";
    error += filename;
    error += " ";
    error += reason;
    
    // Get the errorInfo string as well:
    
    CTCLVariable errorInfo(&rInterp, "errorInfo", kfFALSE);
    const char* traceBack = errorInfo.Get();
    if (traceBack) error += traceBack;
    return error;
  }
  
}
/**
 * TimedUpdate  [static]
 *    Called as a timer event
 *    - Update Xamine spectrum statistics
 *    - Reschedule execution
 *
 * @param d - actually a pointer to the object that scheduled us.
 */
void
CTclGrammerApp::TimedUpdates(ClientData d)
{
    CTclGrammerApp*  pObject = reinterpret_cast<CTclGrammerApp*>(d);

    CDisplayInterface* pDisplayInterface = pObject->m_pDisplayInterface;
    pDisplayInterface->getCurrentDisplay()->updateStatistics();

    Tcl_CreateTimerHandler(pObject->m_nUpdateRate, CTclGrammerApp::TimedUpdates, d);
}

/**
 * protectVariable
 *    Called to protect a global variable.
 *    since this is an application class, the code below only _looks_ like it
 *    leaks memory.  What the new's below do is ensure that the
 *    variable remains in scope the lifetime of the application.
 *    This is used to write protect variables that are defined by the SpecTclInit.tcl
 *    file from later, misleading, modification.
 *   
 *    @param pInterp  - pointer to the interpreter the variable lives in.
 *    @param pVarName - pointer to the variable name.
 */
void
CTclGrammerApp::protectVariable(CTCLInterpreter* pInterp, const char* pVarName)
{
  new CSpecTclInitVar(pInterp, pVarName);
}
// Set up Tcl command processing for non rank 0.
// - start the command pump.
// - Enter an event loop suitable for the slave:
//
static void setupSlaveInterpreter(CTCLInterpreter* pInterp) {

  // Close stdin as we're just going to get
  // commands via the pump and event loop.

  int mode;
  auto tclstdin = Tcl_GetChannel(pInterp->getInterpreter(), "stdin", &mode);
  Tcl_UnregisterChannel(pInterp->getInterpreter(), tclstdin);

  // Start the pump/notifier and run a prompt-less event loop:

  startCommandPump(*pInterp);
 
  while (true) {
    Tcl_ReapDetachedProcs();
    struct Tcl_Time timeout;
    timeout.sec = 1000;
    timeout.usec = 0;
    if (Tcl_WaitForEvent(&timeout)) {
        std::cerr << "Event loop exiting\n";
        Tcl_Exit(-1);
    }
    while (Tcl_DoOneEvent(TCL_ALL_EVENTS | TCL_DONT_WAIT))
        ;
}
}

/*!
 * \brief CTclGrammerApp::AppInit
 *
 * The AppInit to use for Tcl_Main. This sets up the global interpreter,
 * runs the start up procedures for CTclGrammerApp (or derived types),
 * and then run the event loop. This eventually sets
 * \param pInterp
 * \return
 */
int CTclGrammerApp::AppInit(Tcl_Interp *pInterp)
{

    // do basic set up stuff for the interpreter
    if (Tcl_Init(pInterp) == TCL_ERROR) {
        return TCL_ERROR;
    }

    // Make two namespaces:
    // ::spectcl - which will hold all spectcl commands in the future.
    // ::spectcl::serial - which will hold the actual wrapped commands:

    Tcl_CreateNamespace(pInterp, "::spectcl", nullptr, nullptr);
    Tcl_CreateNamespace(pInterp, "::spectcl::serial", nullptr, nullptr);

    gpInterpreter = new CTCLInterpreter(pInterp);
    assert(gpInterpreter != NULL);

    CTclGrammerApp* pInstance = CTclGrammerApp::getInstance();

    if (pInstance == NULL) {
        throw std::string("CTclGrammerApp::m_pInstance does not point to object.");
    }

    // This is the virtual method that sets up all of SpecTcl.
    pInstance->operator()();
    if (gMPIParallel && (pInstance->m_mpiRank != 0)) {
      // Slave process in MPI env.

      setupSlaveInterpreter(gpInterpreter);
    } else {
      // Serial or master process in MPI env.
      CTCLLiveEventLoop* pEventLoop = CTCLLiveEventLoop::getInstance();
      pEventLoop->start(gpInterpreter);
    }
    return TCL_OK;
}

/**
 * mpiExitHandler calles MPI_Finalize on Tcl exit:
 *    Tcl has already killed off the stdout/err so we can't really do any error handling.
 */

extern int is_xamine_shm_monitor;  //it's magic... Xamine client is in C.
static void
MpiExitHandler() {
  // This magic is because the Xamine client software forks us in order to
  // clean up shared memory when there are no attaches.  forks inherit exit processors
  // so the forked process sets is_xamine_shm_monitor true so we can 
  // avoid doing things we really don't want to do.
  if (is_xamine_shm_monitor) {
    return;
  }
#ifdef WITH_MPI

  // Note that if we are rank 0 we spray the exit command to all of the
  // other ranks.  Sincde exit won't make a status, we don't get replies.
  if (gMPIParallel) {
    if (myRank() == MPI_ROOT_RANK) {
      MpiTclCommandChunk exitCommand;
      exitCommand.commandLength = strlen("exit") + 1;
      strcpy(exitCommand.commandChunk, "exit");
      exitCommand.commandChunk[exitCommand.commandLength - 1] = '\0';
      MPI_Bcast(&exitCommand, 1, getTclCommandChunkType(), MPI_ROOT_RANK, MPI_COMM_WORLD);
      stopCommandPump();     // Broadcasts the dummy exit thing
      CGateCommand::stopTracePump();
      stopStateChangePump();
      
    } 
    if (myRank() == MPI_EVENT_SINK_RANK) {
      // Stop the histogram pump:
      stopHistogramPump();
      stopGatePump();            // We broadcast the stop message.
      CGateCommand::stopTracePump();   // Send the stop message to the root thread .
      CBindCommand::stopPump();
      CSpectrumCommand::stopTracePump();
    }  else {
      // Root or worker can call this:
      //    - Root will broadcast a dummy event to kill the broadcast recieve thread and
      //    - Workers will send themselves a dummy event to kill the MPI_Recv thread.
      stopRingItemPump(); 
    } 
    sleep(2);         // Let all the thread exit before pulling the MPI  rug out.

    MPI_Finalize();   // Ignore status - might have already been called.
  }
#endif
}

/** 
 * mMPIAppInit 
 *    Start up the application when run in MPI parallel.  
 *    The entire method is a no-op without MPI suport as we should
 *    never be called in that case:
*/
int CTclGrammerApp::MPIAppInit(Tcl_Interp* pInterp) {
#ifdef WITH_MPI
  std::cerr << "Starting parallel SpecTcl\n";
  auto me = CTclGrammerApp::getInstance();
  int actualModel;
  int stat = MPI_Init_thread(&me->m_argc, &me->m_pArgV, MPI_THREAD_MULTIPLE, &actualModel);
  if (stat != MPI_SUCCESS) {
    std::cerr << "BUG : MPI_InitFailed" << std::endl;
    exit(EXIT_FAILURE);
  }
  // Let's make sure that MPI_Finalize is called by setting an atexit handler

  std::cerr << "Setting MpiExitHandler for rank: " << myRank() << std::endl; 
  

  // The app must be at least big enough for one worker:

  int appSize;
  if (MPI_Comm_size(MPI_COMM_WORLD, &appSize) != MPI_SUCCESS) {
    std::cerr << "Could not determine the number of processes in the MPI app" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Save the current rank.

  me->m_mpiRank = myRank();

  // Be sure we have at least one worker:

  if ((appSize <= MPI_FIRST_WORKER_RANK)) {
    if (me->m_mpiRank == 0) {
      std::stringstream msgStream;
      std::cerr << "The -n parameter to mpirun must be at least " 
        << MPI_FIRST_WORKER_RANK + 1 << " but was " << appSize << std::endl;
    }
    exit(EXIT_FAILURE);
  }


  
  // Create a communicator/group for sending around the ring items from root
  // to the workers. 
  // There's only one 'color' ranks will retain the same ordering so the
  // world rank 0 is rank 0 for gRingItemComm.
  // It includes the root rank and all workers:

  // We just need this, realy to distinguish broadcasts intended for the 
  // ring items and for commands.


  int mycolor = ((me->m_mpiRank == 0) || (me->m_mpiRank >= MPI_FIRST_WORKER_RANK)) ? 
     1 :  MPI_UNDEFINED;
  if (MPI_Comm_split(MPI_COMM_WORLD, mycolor, me->m_mpiRank, &gRingItemComm) != MPI_SUCCESS)  {
    std::cerr << "BUG - could not create the ring item group" << std::endl;
    exit(EXIT_FAILURE);
  }
  // We also need a separate communicator to send gate broadcasts around so they don't get
  // with command broadcasts.  Again there's only one color

  mycolor = 1;          // All in the same group.

  if(MPI_Comm_split(MPI_COMM_WORLD, mycolor, me->m_mpiRank, &gXamineGateComm) != MPI_SUCCESS) {
    std::cerr << "BUg - could not create the Xamine gate communicator\n";
    exit(EXIT_FAILURE);
  }
  
  CTclGrammerApp::AppInit(pInterp); 
#else
  // Should not have been called so error out of Tcl:

    Tcl_Obj* result = Tcl_NewStringObj("BUG - MPIAppInit should not have been called in this environmen", -1);
    Tcl_SetObjResult(pInterp, result);
    return TCL_ERROR;
#endif
  return TCL_OK;
}

/*!
 * \brief CTclGrammerApp::run - start the Tcl_Main with our AppInit
 */
void CTclGrammerApp::run()
{
    // Figure out how to set gMPIParallel
    
  std::cerr << "Checking if run under MPIrun\n";
  gMPIParallel = isMpiApp();
  


  if (gMPIParallel) {
    std::cerr << "MPIAppinit starting\n";
    Tcl_Main(m_argc, m_pArgV, &CTclGrammerApp::MPIAppInit);
    
    // Teardown MPI:
#ifdef WITH_MPI
    MPI_Finalize();
#endif
  } else {
    // init start with the Serial
    std::cerr << "Serial Appinit starting\n";
    Tcl_Main(m_argc, m_pArgV, &CTclGrammerApp::AppInit);
    

  }
}



