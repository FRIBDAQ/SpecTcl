//
// AppInit.cpp:
//    This file contains the application inintialization for the
//    histogramming application TclGrammer.  In this file,
//    we describe the TCLApplication derived object.  It's initialization
//    function will be responsible for setting up the initail valuse for 
//    appropriate global variables.
//
// Author:
//    Ron FOx
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

static char* pCopyright =
"AppInit.cpp - (c) Copyright 1999 NSCL, All rights reserved";

static char* pProgramCopyright =
"SpecTcl NSCL Data Analyzer (c) Copyright 1999 NSCL, All rights reserved";

#include "Histogrammer.h"
#include "TestFile.h"
#include "TCLApplication.h"
#include "TCLVariable.h"
#include "TCLProcessor.h"
#include "TKRunControl.h"
#include "GaussianDistribution.h"

#include "Globals.h"
#include "RunControlPackage.h"
#include "ParameterPackage.h"
#include "SpectrumPackage.h"
#include "DataSourcePackage.h"

#include <histotypes.h>
#include <string>
#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/sysinfo.h>
#ifdef OSF1
#include <machine/hal_sysinfo.h>
#include <sys/proc.h>
#endif

// External references:

extern "C" {
  char* tcl_RcFileName;
}

// Local constants which are defaults for various sizing parameters:
//

static const UInt_t knParameterCount   = 256;
static const UInt_t knEventListSize    = 256;

// The following are used to locate our initialization and RC files:

static const char*  kpInstalledBase    = INSTALLED_IN; // Base of installation dir.
static const char*  kpAppInitSubDir    = "/Etc/";  // This subdir has app init.
static const char*  kpAppInitFile      = "SpecTclInit.tcl";
static const char*  kpHomeEnvName      = "HOME";
static  char*  kpUserInitFile          = "/SpecTclRC.tcl";

// This class defines our TCL application:
//

class CTclGrammerApp : public CTCLApplication
{
  UInt_t m_nDisplaySize;
  UInt_t m_nParams;
  UInt_t m_nListSize;
  CAnalyzer*     m_pAnalyzer;
  CHistogrammer* m_pHistogrammer;
  CTKRunControl*  m_pRunControl;

  // Command Packages:
  //

  CRunControlPackage* m_pRunControlPackage;
  CParameterPackage*  m_pParameterPackage;
  CSpectrumPackage*   m_pSpectrumPackage;
  CDataSourcePackage* m_pDataSourcePackage;
public:
  CTclGrammerApp() :
    m_nDisplaySize(0),
    m_nParams(0),
    m_nListSize(0),
    m_pAnalyzer(0),
    m_pHistogrammer(0),
    m_pRunControl(0),
    m_pRunControlPackage(0),
    m_pParameterPackage(0),
    m_pSpectrumPackage(0)

  {
    // Turn off UAC printing.
#ifdef OSF1
    int buf[4];
    buf[0] = SSIN_UACPROC;
    buf[1] = UAC_NOPRINT;
    buf[2] = SSIN_IECPROC;
    buf[3] = IEC_NOPRINT;
    setsysinfo(SSI_NVPAIRS, buf, 2, 0, 0);
#endif
  }
  ~CTclGrammerApp() {
    delete m_pAnalyzer;
    delete m_pHistogrammer;
    delete m_pRunControl;
    delete m_pRunControlPackage;
    delete m_pParameterPackage;
    delete gpEventSource;
  }
  virtual int operator()();

  
protected:
  void SetupTestSource();
  void UpdateLimits();
  void RegisterPackages();
};

//
// The definitions below cause the TCL initializattion code to 
// discover and start our TCL application.
//
CTclGrammerApp   app;
CTCLApplication* gpTCLApplication = &app;

CTCLVariable Rcfile(std::string("tcl_rcFileName"),
		    kfFALSE);
//
//  This code initializes the TclGrammer Tcl interpreter and its environment.
//
CTclGrammerApp::operator()()
{

  CTCLInterpreter* pInterp = getInterpreter();
  CTCLVariable     RcFile(std::string("tcl_rcFileName")  , kfFALSE);
  Rcfile.Bind(pInterp);


  // Set default values for the variables which contain sizing information
  //

  CTCLVariable DisplaySize(std::string("DisplayMegabytes"),  kfFALSE);
  CTCLVariable ParameterCount(std::string("ParameterCount"), kfFALSE);
  CTCLVariable EventListSize(std::string("EventListSize"),   kfFALSE);


  DisplaySize.Bind(pInterp);
  ParameterCount.Bind(pInterp);
  EventListSize.Bind(pInterp);

  char value[100];		// Holds ascii encoded numberics.
  sprintf(value, "%d", knDefaultSpectrumSize/kn1M);
  DisplaySize.Set(value, TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);

  sprintf(value, "%d", knParameterCount);
  ParameterCount.Set(value, TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);

  sprintf(value, "%d", knEventListSize);
  EventListSize.Set(value, TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);

  // Set up a variable called  SpecTclHome which contains the base of
  // the installation tree.

  CTCLVariable HomeDir(std::string("SpecTclHome"), kfFALSE);
  HomeDir.Bind(pInterp);
  HomeDir.Set(kpInstalledBase);

  
  // If the Application initialization file exists, then
  // attempt to run it at this time.  This file is supposed to set
  // TCL variables describing sizes etc.
  
  try {
    std::string AppFilename(kpInstalledBase);
    AppFilename += kpAppInitSubDir;
    AppFilename += kpAppInitFile;
    pInterp->EvalFile(AppFilename);
  }
  catch (...) {
    cerr << "TclGrammer - Unable to run Application initialization file ";
    cerr << kpAppInitFile << endl;
    cerr << "Starting up with Built in default values" << endl;
  }
  // Do this again for a home directory file...however don't complain on 
  // failure:

  assert(getenv(kpHomeEnvName)); // this env name must exist!!
  try {
    std::string AppFilename(getenv(kpHomeEnvName));
    AppFilename += "/";
    AppFilename += kpAppInitFile;
    pInterp->EvalFile(AppFilename);
  }
  catch (...) {
  }


  // Now set the rc file for the user  This is run to allow user initialization
  // of the TCL/TclGrammer application environment.
  //

  std::string RCFile;
  RCFile = getenv(kpHomeEnvName);
  RCFile += kpUserInitFile;

  Rcfile.Set(RCFile.c_str(), TCL_GLOBAL_ONLY);

  // Set up the Global variables which need initial values in the 
  // Globals.cpp file.
  //

  UpdateLimits();

  // Interpreter and source are straightforward for now...
  //

  gpInterpreter = pInterp;


  // Histogrammer is produced based on limits gotten from UpdateLimits();

  m_pHistogrammer = new CHistogrammer(m_nDisplaySize*kn1M);
  gpEventSink          = m_pHistogrammer;

  // Analyzer initial sizing is based on m_pAnalyzer.

  m_pAnalyzer     = new CAnalyzer(m_nParams, m_nListSize);
  gpAnalyzer      = m_pAnalyzer; 
  //
  // Hook the analyzer together with its sink, unpacker and decoder.
  //
  gpAnalyzer->AttachSink(*gpEventSink);
  gpAnalyzer->AttachUnpacker(*gpUnpacker);
  gpAnalyzer->AttachDecoder(*gpBufferDecoder);

  // Hook together the run control.  Note that normally this can't be done
  // until the event source is selected, but for tests we can:
  //

  SetupTestSource();		// Sets up the test data source and shoves it
				// into gpEventSource.
  m_pRunControl = new CTKRunControl(pInterp, *gpAnalyzer, *gpEventSource);
  gpRunControl  = m_pRunControl;


  // Register the command packages:
  //
  RegisterPackages();
  //

  //
  // Now just put out at the cout the parameterization etc."
  //

  cout << pProgramCopyright << endl;
  cout << "Parameters:"     << endl;
  cout << "Display storage:        " << m_nDisplaySize << "(MBytes)" << endl;
  cout << "Initial Parameter count " << m_nParams      << endl;
  cout << "Initial Event List size " << m_nListSize    << endl;

  return TCL_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
// This is only used by test versions.  The test data source is set up.
//
CGaussianDistribution d1(512.0, 128.0, 1024.0);
CGaussianDistribution d2(256.0, 64.0,  1024.0);
CGaussianDistribution d3(128.0, 32.0,  1024.0);
CGaussianDistribution d4( 64.0, 16.0,  1024.0);
CGaussianDistribution d5( 32.0,  8.0,  1024.0);

void
CTclGrammerApp::SetupTestSource()
{

  CTestFile* pTestSource = new CTestFile;
  pTestSource->AddDistribution(d1);
  pTestSource->AddDistribution(d2);
  pTestSource->AddDistribution(d3);
  pTestSource->AddDistribution(d4);
  pTestSource->AddDistribution(d5);
 
  pTestSource->Open("Testing", kacRead);

  gpEventSource = pTestSource;

}

////////////////////////////////////////////////////////////////////////
//
//  UpdateLimits:
//     Reads in the current values of the TCL Variables associated with
//     limits on the histogrammer.  These are gotten from the application
//     initialization file.
//
void
CTclGrammerApp::UpdateLimits()
{
  UInt_t Result;

  CTCLVariable DisplaySize(std::string("DisplayMegabytes"),    kfFALSE);
  CTCLVariable nParams    (std::string("ParameterCount"), kfFALSE);
  CTCLVariable nListSize  (std::string("EventListSize"),  kfFALSE);



  DisplaySize.Bind(getInterpreter());



  if(sscanf(DisplaySize.Get(), "%d", &Result) > 0) {
    m_nDisplaySize = Result;
  }

  nParams.Bind(getInterpreter());
  if(sscanf(nParams.Get(), "%d", &Result) > 0) {
    m_nParams = Result;
  }
  nListSize.Bind(getInterpreter());
  if(sscanf(nListSize.Get(), "%d", &Result)) {
    m_nListSize = Result;
  }
}
//////////////////////////////////////////////////////////////////////////
//
// RegisterPackages
//   Registers the command pacakges  which provide application
//   specific TCL commands.  As each package is registered,
//   it's signon message is printed to cerr.
//
void 
CTclGrammerApp::RegisterPackages()
{
  // Note that at construction time, the interpreter may not be bound, so for
  // each package:
  //   The Package dynamically created bound to getInterpreter()
  //   The Package's register() member is called.
  //   The Package's signon message is output to the cerr stream.

  // Run Control:

  m_pRunControlPackage = new CRunControlPackage(getInterpreter());
  m_pRunControlPackage->Register();
  m_pRunControlPackage->InitializeRunState();
  cerr << m_pRunControlPackage->getSignon().c_str() << endl;

  // Parameter definition and listing:

  m_pParameterPackage = new CParameterPackage(getInterpreter(),
					      m_pHistogrammer);
  m_pParameterPackage->Register();
  cerr << m_pParameterPackage->getSignon().c_str() << endl;

  // Spectrum definition, deletion and listing:

  m_pSpectrumPackage = new CSpectrumPackage(getInterpreter(), m_pHistogrammer);
  m_pSpectrumPackage->Register();
  cerr << m_pSpectrumPackage->getSignon().c_str() << endl;

  // Data source management:

  m_pDataSourcePackage = new CDataSourcePackage(getInterpreter());
  m_pDataSourcePackage->Register();
  cerr << m_pDataSourcePackage->getSignon().c_str() << endl;

}


