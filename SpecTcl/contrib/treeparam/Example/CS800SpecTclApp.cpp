#include <Event.h> 
#include <EventProcessor.h>
#include <TCLAnalyzer.h>
#include <vector>
#include <histotypes.h>
#include <string>
#include <stdio.h>
#include <Analyzer.h>
#include <TCLHistogrammer.h>
#include <Spectrum2DW.h>
#include <Spectrum1DW.h>
#include <Spectrum.h>
#include <SpectrumFactory.h>
#include <Parameter.h>

// The next include statements assume that the -I$(INSTDIR)/Include switch is in the Makefile
#include <../contrib/treeparam/TreeParameter.h>
#include <../contrib/treeparam/TreeVariable.h>

#include "CS800SpecTclApp.h"

#include "CS800.h"
#include "CS800Unpacker.h"
#include "CS800Calibrator.h"
#include "CS800Calculator.h"

CEvent*	CTreeParameter::pEvent = NULL;
vector<CTreeParameter*> CTreeParameter::pSelf;
vector<CTreeVariable*> CTreeVariable::pSelf;
CS800 s800("s800");

static CS800Unpacker   S800Unpacker;
static CS800Calibrator S800Calibrator;
static CS800Calculator S800Calculator;

//  Function: 	
//    void CreateAnalysisPipeline(CAnalyzer& rAnalyzer) 
//  Operation Type:
//     Override
/*  
Purpose: 	

  Sets up an analysis pipeline.  This function is required and must
be filled in by the SpecTcl user.  Pipeline elements are objects
which are members of classes derived from the CEventProcessor
class. They should be added to the Analyzer's event processing pipeline
by calling RegisterEventProcessor (non virtual base class member).

*/

void 
CS800SpecTclApp::CreateAnalysisPipeline(CAnalyzer& rAnalyzer)  
{ 
  RegisterEventProcessor(S800Unpacker);
  RegisterEventProcessor(S800Calibrator);
  RegisterEventProcessor(S800Calculator);
  CTreeParameter::BindParameters(rAnalyzer);	// Bind parameters to SpecTcl
}  

// Constructors, destructors and other replacements for compiler cannonicals:

CS800SpecTclApp::CS800SpecTclApp ()
{   
} 

	// Destructor:

 CS800SpecTclApp::~CS800SpecTclApp ( )
{
} 


// Functions for class CS800SpecTclApp



//  Function: 	
//    void BindTCLVariables(CTCLInterpreter& rInterp) 
//  Operation Type:
//     override
/*  
Purpose: 	

 Add code to this function to bind any TCL variable to
 the SpecTcl interpreter.  Note that at this time,
 variables have not yet been necessarily created so you
 can do Set but not necessarily Get operations.



*/
void 
CS800SpecTclApp::BindTCLVariables(CTCLInterpreter& rInterp)  
{ 
  CTclGrammerApp::BindTCLVariables(rInterp);
  CTreeVariable::BindVariables(rInterp);
}  

//  Function: 	
//    void SourceLimitScripts(CTCLInterpreter& rInterpreter) 
//  Operation Type:
//     Override
/*  
Purpose: 	

Add code here to source additional variable setting
scripts.  At this time the entire SpecTcl/Tk infrastructure
is not yet set up.  Scripts run at this stage can only run
basic Tcl/Tk commands, and not SpecTcl extensions.
Typically, this might be used to set a bunch of initial values
for variables which were bound in BindTCLVariables.

*/
void 
CS800SpecTclApp::SourceLimitScripts(CTCLInterpreter& rInterpreter)  
{ CTclGrammerApp::SourceLimitScripts(rInterpreter);
}  

//  Function: 	
//    void SetLimits() 
//  Operation Type:
//     overide
/*  
Purpose: 	

Called after BindVariables and SourceLimitScripts.
This function can be used to fetch values of bound Tcl
variables which were modified/set by the limit scripts to
update program default values. 

*/
void 
CS800SpecTclApp::SetLimits()  
{ CTclGrammerApp::SetLimits();
}  

//  Function: 	
//    void CreateHistogrammer() 
//  Operation Type:
//     Override
/*  
Purpose: 	

Creates the histogramming data sink.  If you want to override
this in general you probably won't make use of the actual
base class function.  You might, however extend this by 
defining a base set of parameters and histograms from within
the program.

*/
void 
CS800SpecTclApp::CreateHistogrammer()  
{ CTclGrammerApp::CreateHistogrammer();

// Here we create our snapshot spectra which are directly fed from the
// unpacker in crdcUnpacker.cpp

	CTCLHistogrammer*	histogrammer = getHistogrammer();
	string			aString;
	vector<string>		parName;
	CSpectrum*		pSpectrum;
	CSpectrumFactory	theFactory;
	vector<UInt_t>	resolution;

	// Define dummy parameters for the snapshot spectra
	aString = "Undefined";
	histogrammer->AddParameter(aString, 0, 9);
	parName.push_back(aString);
	resolution.push_back(9);

	// 1D with selectable pad
	aString = "s800.fp.crdc1.snapshot";
	pSpectrum = theFactory.CreateSpectrum(aString, ke1D, keWord, parName, resolution);
	histogrammer->AddSpectrum(*pSpectrum);

	aString = "s800.fp.crdc2.snapshot";
	pSpectrum = theFactory.CreateSpectrum(aString, ke1D, keWord, parName, resolution);
	histogrammer->AddSpectrum(*pSpectrum);

	resolution.clear();
	resolution.push_back(8);
	// 1D integrated pad snapshots
	aString = "s800.fp.crdc1.padsnap";
	pSpectrum = theFactory.CreateSpectrum(aString, ke1D, keWord, parName, resolution);
	histogrammer->AddSpectrum(*pSpectrum);

	aString = "s800.fp.crdc2.padsnap";
	pSpectrum = theFactory.CreateSpectrum(aString, ke1D, keWord, parName, resolution);
	histogrammer->AddSpectrum(*pSpectrum);

	// 2D for all pads
	aString = "Undefined";
	parName.push_back(aString);
	resolution.push_back(9);

	aString = "s800.fp.crdc1.snapshots";
	pSpectrum = theFactory.CreateSpectrum(aString, ke2D, keWord, parName, resolution);
	histogrammer->AddSpectrum(*pSpectrum);

	aString = "s800.fp.crdc2.snapshots";
	pSpectrum = theFactory.CreateSpectrum(aString, ke2D, keWord, parName, resolution);
	histogrammer->AddSpectrum(*pSpectrum);
}  

//  Function: 	
//    void SelectDisplayer(UInt_t nDisplaySize, CHistogrammer& rHistogrammer) 
//  Operation Type:
//     Override.
/*  
Purpose: 	

Select a displayer object and link it to the
histogrammer.  The default code will link Xamine
to the displayer, and set up the Xamine event handler
to deal with gate objects accepted by Xamine interaction.

*/
void 
CS800SpecTclApp::SelectDisplayer(UInt_t nDisplaySize, CHistogrammer& rHistogrammer)  
{ CTclGrammerApp::SelectDisplayer(nDisplaySize, rHistogrammer);
}  


//  Function: 	
//    void SetupTestDataSource() 
//  Operation Type:
//     Override
/*  
Purpose: 	

 Allows you to set up a test data source.  At

present, SpecTcl must have a data source of some sort
connected to it... The default test data source produces a 
fixed length event where all parameters are selected from
a gaussian distribution.  If you can figure out how to do it,
you can setup your own data source... as long as you don't
start analysis, the default one is harmless.

*/
void 
CS800SpecTclApp::SetupTestDataSource()  
{ CTclGrammerApp::SetupTestDataSource();
}  

//  Function: 	
//    void CreateAnalyzer(CEventSink* pSink) 
//  Operation Type:
//     Override
/*  
Purpose: 	

Creates an analyzer.  The Analyzer is connected to the data
source which supplies buffers.  Connected to the analyzer is a
buffer decoder and an event unpacker.  The event unpacker is 
the main experiment dependent chunk of code, not the analyzer.
The analyzer constructed by the base class is a CTclAnalyzer instance.
This is an analyzer which maintains statistics about itself in Tcl Variables.

*/
void 
CS800SpecTclApp::CreateAnalyzer(CEventSink* pSink)  
{ CTclGrammerApp::CreateAnalyzer(pSink);
}  

//  Function: 	
//    void SelectDecoder(CAnalyzer& rAnalyzer) 
//  Operation Type:
//     Override
/*  
Purpose: 	

  Selects a decoder and attaches it to the analyzer.
A decoder is responsible for knowing the overall structure of
a buffer produced by a data analysis system.  The default code
constructs a CNSCLBufferDecoder object which knows the format
of NSCL buffers.

*/
void 
CS800SpecTclApp::SelectDecoder(CAnalyzer& rAnalyzer)  
{ CTclGrammerApp::SelectDecoder(rAnalyzer);
}  



//  Function: 	
//    void AddCommands(CTCLInterpreter& rInterp) 
//  Operation Type:
//     Override
/*  
Purpose: 	

This function adds commands to extend Tcl/Tk/SpecTcl.
The base class function registers the standard SpecTcl command
packages.  Your commands can be registered at this point.
Do not remove the sample code or the SpecTcl commands will
not get registered.

*/
void 
CS800SpecTclApp::AddCommands(CTCLInterpreter& rInterp)  
{ 
	CTclGrammerApp::AddCommands(rInterp);
	CTreeVariableCommand*	m_treevariable;
	m_treevariable = new CTreeVariableCommand(&rInterp);
	m_treevariable->Register();
	CTreeParameterCommand*	m_treeparameter;
	m_treeparameter = new CTreeParameterCommand(&rInterp);
	m_treeparameter->Register();
	CS800MapCommand* m_s800map;
	m_s800map = new CS800MapCommand(&rInterp, &s800.fp.track.map);
	m_s800map->Register();
}  

//  Function: 	
//    void SetupRunControl() 
//  Operation Type:
//     Override.
/*  
Purpose: 	

  Sets up the Run control object.  The run control object
is responsible for interacting with the underlying operating system
and programming framework to route data from the data source to 
the SpecTcl analyzer.  The base class object instantiates a 
CTKRunControl object.  This object uses fd waiting within the 
Tcl/TK event processing loop framework to dispatch buffers for
processing as they become available.

*/
void 
CS800SpecTclApp::SetupRunControl()  
{ CTclGrammerApp::SetupRunControl();
}  

//  Function: 	
//    void SourceFunctionalScripts(CTCLInterpreter& rInterp) 
//  Operation Type:
//     Override
/*  
Purpose: 	

  This function allows the user to source scripts
which have access to the full Tcl/Tk/SpecTcl
command set along with whatever extensions have been
added by the user in AddCommands.  

*/
void 
CS800SpecTclApp::SourceFunctionalScripts(CTCLInterpreter& rInterp)  
{ CTclGrammerApp::SourceFunctionalScripts(rInterp);
}  

//  Function: 	
//    int operator()() 
//  Operation Type:
//     Override.
/*  
Purpose: 	

  Entered at Tcl/Tk initialization time (think of this
as the entry point of the SpecTcl program).  The base
class default  implementation calls the member functions
of this class in an appropriate order.  It's possible for the user
to extend this functionality by adding code to this function.

*/
int 
CS800SpecTclApp::operator()()  
{ 
  return CTclGrammerApp::operator()();
}

CS800SpecTclApp   myApp;
CTclGrammerApp& app(myApp);	// Create an instance of me.
CTCLApplication* gpTCLApplication=&app;  // Findable by the Tcl/tk framework.
