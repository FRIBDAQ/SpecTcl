// Class: CMySpecTclApp

////////////////////////// FILE_NAME.cpp /////////////////////////////////////////////////////
#include "MySpecTclApp.h"    				
#include "EventProcessor.h"
#include "CCAENDigitizerCreator.h"
#include "CModuleCommand.h"
#include "CModuleDictionary.h"
#include "CUnpackerCommand.h"
#include "CCAENV830Creator.h"
#include "TCLAnalyzer.h"
#include "CUnpacker.h"
#include "CPosition.h"
#include "CProfile.h"
#include "CSeeAnalyzer.h"

#include <Event.h>
#include <Globals.h>

#define SEE_PACKETID 0x8000	// Assigned by Daniel.


//
// These event processors produce ppac positions:
//

CPosition ppacx("see.ppac.x",
		"see.ppac.l",
		"see.ppac.r",
		"ppac.x.scale1",
		"ppac.x.offset1",
		"ppac.x.scale2",
		"ppac.x.offset2",
		"ppac.x.minsum",
		"ppac.x.slope",
		"ppac.x.offset");

CPosition ppacy("see.ppac.y",
		"see.ppac.u",
		"see.ppac.d",
		"ppac.y.scale1",
		"ppac.y.offset1",
		"ppac.y.scale2",
		"ppac.y.offset2",
		"ppac.y.minsum",
		"ppac.y.slope",
		"ppac.y.offset");

CProfile xProfile("see.ppac.x_profile",
		  "see.ppac.x",
		  "ppac.x.profilechans");
CProfile yProfile("see.ppac.y_profile",
		  "see.ppac.y",
		  "ppac.y.profilechans");

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
   The sample implementation in this
file produces a two step pipeline.  The first step decodes a fixed length
event into the CEvent array.  The first parameter is put into index 1 and so on.
The second step produces a compiled pseudo parameter by adding event array
elements 1 and 2 and putting the result into element 0.


*/

void 
CMySpecTclApp::CreateAnalysisPipeline(CAnalyzer& rAnalyzer)  
{ 

  RegisterEventProcessor(*m_pUnpacker);
  RegisterEventProcessor(ppacx);
  RegisterEventProcessor(ppacy);
  RegisterEventProcessor(xProfile);
  RegisterEventProcessor(yProfile);

}  

// Constructors, destructors and other replacements for compiler cannonicals:

CMySpecTclApp::CMySpecTclApp () :
  m_pDictionary(0),
  m_pUnpackerCommand(0),
  m_pModuleCommand(0),
  m_pUnpacker(0)
{   
} 

	// Destructor:

 CMySpecTclApp::~CMySpecTclApp ( )
{
} 


// Functions for class CMySpecTclApp



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
CMySpecTclApp::BindTCLVariables(CTCLInterpreter& rInterp)  
{ 
  CTclGrammerApp::BindTCLVariables(rInterp);
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
CMySpecTclApp::SourceLimitScripts(CTCLInterpreter& rInterpreter)  
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
CMySpecTclApp::SetLimits()  
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
CMySpecTclApp::CreateHistogrammer()  
{ CTclGrammerApp::CreateHistogrammer();
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
CMySpecTclApp::SelectDisplayer(UInt_t nDisplaySize, CHistogrammer& rHistogrammer)  
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
CMySpecTclApp::SetupTestDataSource()  
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
CMySpecTclApp::CreateAnalyzer(CEventSink* pSink)  
{ 
  gpAnalyzer = new CSeeAnalyzer(*gpInterpreter,
				getParams(),
				getListSize());
  setAnalyzer(gpAnalyzer);
  gpAnalyzer->AttachSink(*gpEventSink);

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
CMySpecTclApp::SelectDecoder(CAnalyzer& rAnalyzer)  
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
CMySpecTclApp::AddCommands(CTCLInterpreter& rInterp)  
{ 
  CTclGrammerApp::AddCommands(rInterp);
  
  //  Create the low end objects referred to by the commands:

  m_pDictionary = new CModuleDictionary;
  m_pUnpacker   = new CUnpacker(SEE_PACKETID);

  // Create the unpacker command object, linked to the
  // dictionary and the unpacker created above.

  m_pUnpackerCommand = new CUnpackerCommand(string("unpack"),
  					    rInterp,
  					    *m_pDictionary,
					    *m_pUnpacker);
  // Create the Module command and register creators:

  m_pModuleCommand = new CModuleCommand(rInterp,
					string("module"),
					*m_pUnpacker,
					*m_pDictionary);

  // The aliased entries allow for compatibility between
  // SpecTcl and the readout software.
  //
  m_pModuleCommand->RegisterCreator(string("caenvme"), 
				    *(new CCAENDigitizerCreator));
  m_pModuleCommand->RegisterCreator(string("caenv775"),
				    *(new CCAENDigitizerCreator));
  m_pModuleCommand->RegisterCreator(string("caenv785"),
				    *(new CCAENDigitizerCreator));
  m_pModuleCommand->RegisterCreator(string("caenv792"),
				    *(new CCAENDigitizerCreator));

  // The following setup enough of an infrastructure that
  // scaler modules can be generated, configured and listed.
  //
 
  CModuleDictionary* pScalers = new CModuleDictionary;
  CUnpacker*         pDummy   = new CUnpacker(0);
  CModuleCommand*    pScalercmd = new CModuleCommand(rInterp,
						     string("scaler"),
						     *pDummy,
						     *pScalers);
  pScalercmd->RegisterCreator(string("caenv830"),
			      *(new CCAENV830Creator));
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
CMySpecTclApp::SetupRunControl()  
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
CMySpecTclApp::SourceFunctionalScripts(CTCLInterpreter& rInterp)  
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
CMySpecTclApp::operator()()  
{ 
  return CTclGrammerApp::operator()();
}

CMySpecTclApp   myApp;
CTclGrammerApp& app(myApp);	// Create an instance of me.
CTCLApplication* gpTCLApplication=&app;  // Findable by the Tcl/tk framework.

