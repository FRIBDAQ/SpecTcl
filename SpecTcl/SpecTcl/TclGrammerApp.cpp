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
#include "NSCLBufferDecoder.h"

#include "Globals.h"
#include "RunControlPackage.h"
#include "ParameterPackage.h"
#include "SpectrumPackage.h"
#include "DataSourcePackage.h"
#include "GatePackage.h"
#include "FilterCommand.h"
#include "EventSinkPipeline.h"


#include "TCLAnalyzer.h"

#include "XamineEventHandler.h"

#include "SpecTcl.h"

#include <CTreeParameterCommand.h>
#include <CTreeVariableCommand.h>
#include <CTreeParameter.h>
#include <CTreeVariable.h>
#include "CFoldCommand.h"
#include "CFitCommand.h"

#include <CProjectionCommand.h>


#include <histotypes.h>
#include <buftypes.h>
#include <string>
#include <Iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <histotypes.h>
#include <buftypes.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

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



// File scoped unbound variables:
static const UInt_t knParameterCount = 256;
static const UInt_t knEventListSize  = 256;
static const UInt_t knDisplaySize    = 8;

static const char* kpInstalledBase = INSTALLED_IN; // Preprocessor def.
static const char* kpAppInitSubDir = "/etc";
static const char* kpAppInitFile   = "/SpecTclInit.tcl";
static const char* kpUserInitFile  = "/SpecTclRC.tcl";

// Static attribute storage and initialization for CTclGrammerApp

// Constructors, destructors and other replacements for compiler cannonicals:
/*!
   Constructing a CTclGrammerApp is what glues the library called SpecTcl
   into the program called SpecTcl.
*/
CTclGrammerApp::CTclGrammerApp() :
  CTCLApplication(),
  m_nDisplaySize(knDisplaySize),
  m_nParams(knParameterCount),
  m_nListSize(knEventListSize),
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
  m_RCFile(string("tcl_rcFielname"),            kfFALSE),
  m_TclDisplaySize(string("DisplayMegabytes"),  kfFALSE),
  m_TclParameterCount(string("ParameterCount"), kfFALSE),
  m_TclEventListSize(string("EventListSize"),   kfFALSE),
  m_pMultiTestSource((CMultiTestSource*)kpNULL)
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
  api->AddEventProcessor(rEventProcessor, name);

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
  try {				// Try from SpecTcl library dir.
    string  AppFilename(kpInstalledBase);
    AppFilename += kpAppInitSubDir;
    AppFilename += kpAppInitFile;
    rInterpreter.EvalFile(AppFilename);
  }
  catch(...) {			// Errors are silently ignored.
  }

  if(getenv("HOME")) {		// Need home env variable.
    try {				// Try from user's home dir:
      string AppFilename(getenv("HOME"));
      AppFilename += kpAppInitFile;
      rInterpreter.EvalFile(AppFilename);
    }
    catch(...) {
    }
  }

  try {				// Try the cwd.
    string AppFilename(".");
    AppFilename += kpAppInitFile;
    rInterpreter.EvalFile(AppFilename);
  }
  catch(...) {
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
  m_pHistogrammer     = new CTCLHistogrammer(gpInterpreter, 
					     m_nDisplaySize*kn1M);
  gpEventSink = m_pHistogrammer;
  gpEventSinkPipeline->AddEventSink(*m_pHistogrammer, "::Histogrammer");
}

//  Function:
//    void SelectDisplayer(UInt_t nDisplaysize, CHistogrammer& rHistogrammer)
//  Operation Type:
//     Behavior
/*!

  Selects the displayer and hooks it into the histogrammer.
  Note:  In the current architecture, this selection is hardwired, however
  in the future, we will support turning on and off displayers, null
  displayers and so on and this member function will be useful
  at that time.
  \param nDisplaySize (in)
     The size of the display shared memory in mbytes.
  \param rHistogrammer (in)
     A reference to the histogrammer object.
*/
void CTclGrammerApp::SelectDisplayer(UInt_t nDisplaysize, 
				     CHistogrammer& rHistogrammer) 
{
  // We need to set up the Xamine event handler however:
  m_pXamineEvents = new CXamineEventHandler(gpInterpreter, 
					    (CHistogrammer*)gpEventSink);
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

  // The histogrammer is hooked to the analyzer as an event sink:
  //m_pAnalyzer->AttachSink(*gpEventSink);


  m_pAnalyzer->AttachSink(*gpEventSinkPipeline);
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
  m_pDecoder      = new CNSCLBufferDecoder;
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
  m_pRunControlPackage = new CRunControlPackage(&rInterp);
  m_pRunControlPackage->Register();
  m_pRunControlPackage->InitializeRunState();
  cerr << m_pRunControlPackage->getSignon() << endl;

  // Tacit assumption that the event sink is a histogrammer
  m_pParameterPackage = new CParameterPackage(&rInterp, 
					      (CTCLHistogrammer*)gpEventSink);
  m_pParameterPackage->Register();
  cerr << m_pParameterPackage->getSignon() << endl;

  m_pSpectrumPackage  = new CSpectrumPackage(&rInterp, 
					     (CHistogrammer*)gpEventSink);
  m_pSpectrumPackage->Register();
  cerr << m_pSpectrumPackage->getSignon() << endl;

  m_pDataSourcePackage = new CDataSourcePackage(&rInterp);
  m_pDataSourcePackage->Register();
  cerr << m_pDataSourcePackage->getSignon() << endl;

  m_pGatePackage = new CGatePackage(&rInterp, 
				    (CHistogrammer*)gpEventSink);
  m_pGatePackage->Register();
  cerr << m_pGatePackage->getSignon() << endl;

  // For Filter command.
  CFilterCommand* pFilterCommand = new CFilterCommand(rInterp);
  pFilterCommand->Bind(rInterp);
  pFilterCommand->Register();
  cerr << "Filter command (c) 2003 NSCL written by  Kanayo Orji\n";

  // Create the tree parameter package commands and bind any variables
  // that have been defined:


  CTreeParameterCommand* pTreeParamCommand = new CTreeParameterCommand;
  CTreeVariableCommand*  pTreeVariableCommand = new CTreeVariableCommand;
  CTreeVariable::BindVariables(*(getInterpreter()));

  cerr << "Tree parameter/variable  command " << CTreeParameter::TreeParameterVersion;
  cerr << " (c) Copyright 2005 NSCL written by Daniel Bazin, Ron Fox\n";

  CFoldCommand* pFold = new CFoldCommand(&rInterp);

  cerr << "fold command (c) 2005 NSCL Written by Ron Fox\n";

  CProjectionCommand* pProjection = new CProjectionCommand(rInterp);

  cerr << "project command (c) 2005 NSCL Written by Ron Fox\n";

  CFitCommand *Fit  = new CFitCommand(rInterp);
  cerr << "fit command (c) 2006 NSCL Written by Ron Fox\n";

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
  try {				// First run the ~ script:
    if(getenv("HOME")) {
      string RCFilename(getenv("HOME"));
      RCFilename += kpUserInitFile;
      rInterp.EvalFile(RCFilename);
    }
  }
  catch (...) {
  }

  try {
    string RCFilename(".");
    RCFilename += kpUserInitFile;
    rInterp.EvalFile(RCFilename);
  }
  catch (...) {
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


  // Fetch and setup the interpreter member/global pointer.
  gpInterpreter = getInterpreter();

  // Bind any variables to Tcl:
  BindTCLVariables(*gpInterpreter);

  // Source limit setting scripts:
  SourceLimitScripts(*gpInterpreter);

  // Based on all of this set the final startup limits/values:
  SetLimits();

  // Create the histogrammer event sink:
  CreateHistogrammer();

  // Setup the histogram displayer:
  SelectDisplayer(m_nDisplaySize, *((CHistogrammer*)gpEventSink));

  // Setup the test data source:
  SetupTestDataSource(); // No longer done. By default, no source is to be set so that users aren't mistakenly fooled by test data.

  // Create an analyzer and hook the histogrammer to it.
  //CreateAnalyzer(gpEventSink);
  CreateAnalyzer(gpEventSinkPipeline);

  //  Setup the buffer decoder:
  SelectDecoder(*gpAnalyzer);

  // Setup the command packages:
  AddCommands(*gpInterpreter);

  // the run control objects.
  SetupRunControl();

  //  Setup the user's analysis pipeline:


  CreateAnalysisPipeline(*gpAnalyzer);
  CTreeParameter::BindParameters();           // Needed by treeparameter.

  // Finally the user may have some functional setup scripts they want
  // to run.  By the time these are run, SpecTcl is essentially completely
  // set up.
  SourceFunctionalScripts(*gpInterpreter);

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
  cerr << " If your name should be on this list and is not, my apologies, please contact\n";
  cerr << "   fox@nscl.msu.edu and let me know what your contribution was and I will add you to\n";
  cerr << "   the list of credits.\n";

  // Finally run the version script:

  try {
    gpInterpreter->GlobalEval(printVersionScript);
  }
  catch (...) {
    cerr << "SpecTcl Version: " << gpVersion << endl;
  }

  return TCL_OK;
}

// Function:
//   static void UpdateUInt(CTCLVariable& rVar, UInt_t& rValue)
// Operation Type:
//   Utility.
void CTclGrammerApp::UpdateUInt(CTCLVariable& rVar, UInt_t& rValue) {
  int nResult;

  const char* pValue(rVar.Get(TCL_LEAVE_ERR_MSG|TCL_GLOBAL_ONLY));
  if(pValue) {
    if(sscanf(pValue, "%ud", &nResult) > 0) {
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
