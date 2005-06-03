   

/*
	Implementation file for CCalibratedParameterCommand for a description of the
	class see CCalibratedParameterCommand.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////


// Include files required:

#include <config.h>
#include "CCalibratedParameterCommand.h"  
#include "CCalibratedParameterManager.h"
#include "CCalibratedParameter.h"
#include "CFit.h"
#include "CFitFactory.h"

#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <TCLException.h>
#include <TCLString.h>
#include <Histogrammer.h>
#include <Parameter.h>
#include <Globals.h>

#include <DesignByContract.h>

#include <vector>
#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


using namespace DesignByContract;
  				

// Static attribute storage and initialization for CCalibratedParameterCommand

static vector<string> SwitchTable;
enum Switches {
		swCreate,
		swList,
		swDelete,
		swUpdate };
	
// Helper class to build up a list of formatted calibrated parameters.

class Format {
private:
	CTCLString& 	m_rResult;
	string          m_Pattern;
public:
	Format(CTCLString& rResult, string pattern) :
		m_rResult(rResult),
		m_Pattern(pattern)
	{}
	void operator()(CCalibratedParameterManager::MapEntry entry)
	{
			REQUIRE(entry.second, "Null Parameter in map entry");
			if(Tcl_StringMatch(entry.first.c_str(), m_Pattern.c_str())) {
			string name                  = entry.first;
			CCalibratedParameter* pParam = entry.second;

			string item(CCalibratedParameterCommand::FormatParameter(name,
						  										   *pParam));
			m_rResult.AppendElement(item);

		}
	}
};
// Helper class to update calibrated parameters with new fits.

class Updater {
private:
	string m_Pattern;
public:
	Updater(string Pattern) :
		m_Pattern(Pattern) {}
	void operator()(CCalibratedParameterManager::MapEntry entry)
	{
		REQUIRE(entry.second, "Null parameter in map entry");
		if(Tcl_StringMatch(entry.first.c_str(), m_Pattern.c_str())) {
			CCalibratedParameter*      pParam = entry.second;
			CFitFactory::FitIterator   
							 pF   = CFitFactory::FindFit(pParam->getFitName());
			if(pF != CFitFactory::end()) {
				CFit* pFit = 		 pF->second;
				if(pFit->GetState() == CFit::Performed) {
					CFit* p = pParam->ReplaceFit(*pFit);
					delete p;
				}
			}
			
		}
	}
};
/*!
    Create an object of type CCalibratedParameterCommand
    \post m_pHistogrammer is not null.
*/
CCalibratedParameterCommand::CCalibratedParameterCommand
												   (CTCLInterpreter* pInterp) :
	CTCLProcessor("calibparam", pInterp),
	m_pHistogrammer(0)
 
{ 
		// Set up the switch table vector.
		
		if (SwitchTable.empty()) {
			SwitchTable.push_back("-create");
			SwitchTable.push_back("-list");
			SwitchTable.push_back("-delete");
			SwitchTable.push_back("-refresh");			
		}
		
		// Locate the histogrammer and save it in the
		// member variable.
		
		m_pHistogrammer = (CHistogrammer*)gpEventSink;
		ENSURE(m_pHistogrammer, "NULL Histogrammer pointer.");
} 

/*!
    Called to destroy an instance of CCalibratedParameterCommand
*/
 CCalibratedParameterCommand::~CCalibratedParameterCommand ( )
{
}

// Functions for class CCalibratedParameterCommand

/*! 

Description:

Does a top level parse and dispatch of 
commands.  The command is parsed
ito its function and dispatched.


Parameters:

\param rInterp (CTCLInterpreter&)
   Reference to the interpreter that's executing
   this command.
\param rResult (CTCLResult&)
   Reference to the interpreter result.
\param argc (int)
   Number of command line parameters.
   Note that the command itself is
   included in this.
\param argv (char**)
  The command line parameter.


\return int
\retval TCL_OK
   Function completed successfully.
   The contents of rResult will depend on the 
   function.
\retval TCL_ERROR
   Function failed. rResult will be an error message.
Pseudo code:

\verbatim
Operation = argv[1]
Opcode    = MatchKeyword(Operation)

if (Opcode == Create) {
    return Create_Parse(rInterp, rResult, remaining argc, remaining argv)
}
if (Opcode == List) {
    return List_Parse(rInterp, rResult, remainng argc, remaining argv)
}
if(Opcode == Delete) {
    return List_Parse(rInterp, rResult, remaining argc, remaining argv)
}
if(Opcode == Update) {
    return Update_Parse(rInterp, RResult, remaining argc, remaining argv)
}
if(Opcode == Create) {
    return Create_Parse(rInterp, rResult, remaining argc, remaining argv)
}
if(OpCode == unrecognized)
   return Create_Parse(rInterp, rResult, argc, argv)
}
FAIL
\endverbatim

*/
int
CCalibratedParameterCommand::operator()(CTCLInterpreter& rInterp, 
									    CTCLResult& rResult, 
									    int argc, char** argv)  
{ 
	// The first parameter is the command itself and that can be ignored:
	
	NextParam(argc,argv);
	
	// There must be at least one parameter, either a command keyword,
	// or in the case of a create, the name of the target:
	
	if(!argc) {
		rResult = "Insufficient parameters: \n";
		rResult += Usage();
		return TCL_ERROR;
	}

	// Pull out the  the dispatch switch. Note that the lack of a 
	// recognized switch is equivalent to create.
	
	string Switch(argv[0]);

	switch (MatchKeyword(SwitchTable, Switch, (int) swCreate)) {
	case swCreate:
		if (Switch[0] == '-') {     // The switch, not a default case.
			NextParam(argc,argv);
		}
		return Create_parse(rInterp, rResult, argc, argv);
	case swList:
		NextParam(argc,argv);
		return List_parse(rInterp, rResult, argc, argv);
	case swDelete:
		NextParam(argc,argv);
		return Delete_parse(rInterp, rResult, argc, argv);
	case swUpdate:
		NextParam(argc,argv);
		return Update_parse(rInterp, rResult, argc, argv);	
	default:			
		ENSURE(0, "Invalid switch: impossible condition");
	}
	ENSURE(0, "Fell past switch, impossible condition");
	rResult = "Defect in CCalibratedParameterCommand::operator()";
	return TCL_ERROR;
}  

/*! 

Description:

Parses the Create command and reports any
parsing errors to the caller.  If the parse is
successful, Create is called to do the actual
creation of a new calibrated parameter.



Parameters:

\param rInterp (CTCLInterpreter&)
   Reference to the interpreter that's executing
   this command.
\param rResult (CTCLResult&)
   Reference to the interpreter result.
\param argc (int)
   Number of command line parameters.
   Note that the command itself is
   included in this.
\param argv (char**)
  The command line parameter.

\return int
\retval TCL_OK
    - Parse was correct.
    - Create returned TCL_OK.
\retval TCL_ERROR
    - Parse failed OR
    - Create returned an error.

Pseudo code:

\verbatim
targetname = argv[0]
targetid   = argv[1]
rawname    = argv[2]
fitname    = argv[3]

If (argc == 5) {
   units = argv[4]
} else
   units = ""
}

status = Create(message, targetname, targetid, rawname, fitname, units)
rResult = message
return status
\endverbatim

*/
int 
CCalibratedParameterCommand::Create_parse(CTCLInterpreter& rInterp, 
										  CTCLResult& rResult, 
										  int argc, char** argv)  
{ 
	// There must be 4 or 5 parameters on the command line:
	
	if ((argc != 4) && (argc != 5)) {
	  rResult  = "Improper number of parmeters on command line";
	  rResult += Usage();
	  return TCL_ERROR;
	}
	// The first 4 are clear:
	
	string targetname = argv[0];
	string targetid   = argv[1];
	string rawname    = argv[2];
	string fitname    = argv[3];
	
	// The last one is there only if the user supplies units:
	
	string units="";
	if (argc == 5) {
		units = argv[4];
	}
	// The target id must parse to an integer (long):
	Long_t nTargetId;
	try {
		nTargetId = rInterp.ExprLong(targetid);
	} catch (CTCLException& e) {
		rResult  = "Target Id is not an integer and must be: ";
		rResult += e.ReasonText();
		rResult += "\n";
		rResult += Usage();
		return TCL_ERROR;
	}
	// Now that we have a good parse we let Create do the rest:
	
	string Result;
	int status = Create(Result, targetname, (int)nTargetId, rawname, fitname, units);
	rResult    = Result;
	return status;
	
}  

/*! 

Description:

Parses the list command.  If the parse is succesful,
invokes List to do the listing.  Parse failures
will result in an error return and will also leave
an error message and usage in the rResult parameter


Parameters:

\param rInterp (CTCLInterpreter&)
   Reference to the interpreter that's executing
   this command.
\param rResult (CTCLResult&)
   Reference to the interpreter result.
\param argc (int)
   Number of command line parameters.
   Note that the command itself is
   included in this.
\param argv (char**)
  The command line parameter.


\return int
\retval   TCL_OK
     The function and parse succeeded 
\retval TCL_ERROR
    The function or the parse failed.

Pseudo code:

\verbatim
pattern = "*"
if(argc) pattern = argv[0]

rResult = List(pattern)
return TCL_OK

\endverbatim

*/
int 
CCalibratedParameterCommand::List_parse(CTCLInterpreter& rInterp, 
										CTCLResult& rResult, 
										int  argc, char** argv)  
{ 
	// Ensure that the parameter count is 0 or 1.
	
	if(argc > 1) {
		rResult = "Improper Parameter count";
		rResult += Usage();
		return TCL_ERROR;
	}
	// Now we can't fail.. just need to figure out the pattern:
	
	string pattern("*");
	string listing;
	if(argc) {
		pattern = argv[0];
	}
	listing = List(pattern);
	rResult = listing;
	return TCL_OK;
}  

/*! 

Description:

Parses the delete subcommand.  If the parse is successful,
control is passed to Delete to do the actual delete.


Parameters:

\param rInterp (CTCLInterpreter&)
   Reference to the interpreter that's executing
   this command.
\param rResult (CTCLResult&)
   Reference to the interpreter result.
\param argc (int)
   Number of command line parameters.
   Note that the command itself is
   included in this.
\param argv (char**)
  The command line parameter.

\return int
\retval TCL_OK
  The parse and the command succeeded.
\retval TCL_ERROR
    The function or parse failed.


Pseudo code:

\verbatim
name = argv[0]
status = Delete(msg, name)
rResult = msg
return status



\endverbatim

*/
int 
CCalibratedParameterCommand::Delete_parse(CTCLInterpreter& rInterp, 
										  CTCLResult& rResult, 
										  int argc, char** argv)  
{ 
	// we are required to have a parameter:
	
	if (argc != 1) {
		rResult = "Incorrect number of command line parameters";
		rResult += Usage();
		return TCL_ERROR;
	}
	// Now it's in the hands of Delete():
	
	string Result;
	int status = Delete(Result, string(argv[0]));
	rResult = Result;
	return status;
}  

/*! 

Description:

Parses the update command.  The update 
command parse will then invoke the update member if
the parse is successful.


Parameters:

\param rInterp (CTCLInterpreter&)
   Reference to the interpreter that's executing
   this command.
\param rResult (CTCLResult&)
   Reference to the interpreter result.
\param argc (int)
   Number of command line parameters.
   Note that the command itself is
   included in this.
\param argv (char**)
  The command line parameter.


\return int
\retval TCL_OK
   The parse and call of Update succeeded.
\retval TCL_ERROR
   The parse or call of Update failed.

Pseudo code:

\verbatim
pattern = "*"
if (argc) pattern = argv[0]
Update(pattern)
return TCL_OK
\endverbatim

*/
int 
CCalibratedParameterCommand::Update_parse(CTCLInterpreter& rInterp, 
										  CTCLResult& rResult, 
										  int argc, char** argv)  
{ 
	// This can have either 0 or 1 parameters:
	
	if (argc > 1) {
		rResult = "Improper number of command line parameters";
		rResult += Usage();
		return TCL_ERROR;
	}
	// Now figure out the pattern (defaults to *)
	
	string pattern("*");
	if(argc) {
		pattern = argv[0];
	}
	
	// Leave it in the hands of Update:

	Update(pattern);
	return TCL_OK;
	
}  

/*! 

Description:

Creates a new calibrated parameter.
The parameter is instantiated here and entered
in the CCalibratedParameterManager via a call
to CCalibratedParameterManager::AddParameter

\post pCalibrated must be non null.
\post Created parameter must be findable in parameter manager.

Parameters:

\param Result (string)
    A status string from the creation.
\param name (string)
  Name of the parameter being created.
\param id (int)
  Number of the parameter being created.
  This number must not correspond to an existing
   parameter.
\param raw (string)
   Name of the raw parameter being calibrated.
\param fitname (string)
   Name of the fit that is invoked to calibrate the parameter
\param units (string)
   Parameter units (blank means no units).

\return int
\retval TCL_OK
   The calibrated parameter was made and successfully
   inserted into the calibration manager.\
  Result will be a string containing the parameter description
  list
\retval TCL_ERROR
   The calibrated parameter specified could not be
   created.
   the Result will be a string that contains an error
   message.

Pseudo code:

\verbatim
if exists name  {
   Result = "$name exists"
   return TCL_ERROR
}

if exists number  {
   Result = "Parameter $number already is defined"
   return TCL_ERROR
}

if !exists raw {
   Result = "Raw parameter $raw does not exist"
   return TCL_ERROR
}

if !exists fit {
    Result = "Fit $fit does not exist"
    return TCL_ERROR
}

pParameter = CreateParameter(name, number, units)
pCalibParam = new CCalibratedParameter(number, pRaw->getNumber(),
                                       pFit, fit)
CCalibratedParameterManager::AddParameter(name, pCalibParam)
return TCL_OK

\endverbatim

*/
int 
CCalibratedParameterCommand::Create(string& Result, string name, 
									int id, string raw, string fitname, 
									string units)  
{ 
  // The calibrated parameter must not exist
  //  this could happen if the user deleted the target param
  // with parameter -delete not calibparam -delete.
  // or just overwriting an existing calibparam.
  
	if(CCalibratedParameterManager::FindParameter(name) != 
	   CCalibratedParameterManager::end()) {
	  Result  = "The calibrated parameter: " ;
	  Result += name;
	  Result += " already exists.";
	  return TCL_ERROR;
	    
	}
	// The name and Id of the target must be unique and new so both the lookups
	// below  must fail:
		
	CParameter* pParam = m_pHistogrammer->FindParameter(name);
	if(pParam) {
		Result  = "A parameter named ";
		Result += name;
		Result += " already exists.";
		return TCL_ERROR;
	}
	pParam = m_pHistogrammer->FindParameter(id);
	if (pParam) {
		char sId[100];
		Result = "A parameter with the id: ";
		sprintf(sId, "%d", id);
		Result += sId;
		Result += " already exists";
		return TCL_ERROR;
	}


	// The raw parameter name must exist.
	
	pParam = m_pHistogrammer->FindParameter(raw);
	if(!pParam) {
		Result  = "The Raw Parameter ";
		Result += raw;
		Result += " must exist and does not";
		return TCL_ERROR;
	}
	// The fit must also exists.
	
	CFit* pFit = FindFit(fitname);
	if(!pFit) {
		Result  = "The fit ";
		Result += fitname;
		Result += " must already exist and does not";
		return TCL_ERROR;
	}
	
	// Now everything must work:
	
	try {
	  CParameter* pTarget = m_pHistogrammer->AddParameter(name, id, 
							      units.c_str());
	  CCalibratedParameter* pCalibrated = new CCalibratedParameter(id,
								       pParam->getNumber(),
								       fitname,
								       pFit);
	  CCalibratedParameterManager::AddParameter(name, pCalibrated);
	  Result = name;
	  ENSURE(pCalibrated, "Created parameter pointer is null");
	  ENSURE(CCalibratedParameterManager::FindParameter(name) != 
		 CCalibratedParameterManager::end(),
		 "Added parameter could not be found");
	  return TCL_OK;									
	  
	} 
	catch (CException& e) {
	  Result += "Unanticipated CException in CCalibratedParameter::Create() : ";
	  Result += e.ReasonText();
	  return TCL_ERROR;
	}
	catch (string& e) {
	  Result += "Unanticipated string exception in CCalibratedParameter::Create() : ";
	  Result += e;
	  return TCL_ERROR;
	}
	catch (char* e) {
	  Result += "Unanticipated cstring exception in CCalibratedParameter::Create() : ";
	  Result += e;
	  return TCL_ERROR;
	}
	catch (...) {
	  Result += "Unrecognized exception type in CCalibratedParameter::Create() : ";
	  return TCL_ERROR;
	  
	}
	
}  

/*! 

Description:

Produces a listing of the parameters that match the
pattern.  The pattern can contain wild card characters
from the glob set.


Parameters:

\param pattern (string)
   Pattern to match when doing the list.
 
\return string
\retval a list, containing one entry per matched parameter
  each element of the list is of the form:
   {name, number, rawname, fitname, units}

Pseudo code:

\verbatim
result = <empty>
for_each CalibratedParameter {
   if  CalibratedParameter Name matches pattern {
       result.AddElement(FormatFit(name, CalibratedParameter)
}
return $result

\endverbatim

*/
string 
CCalibratedParameterCommand::List(string pattern)  
{ 
	CTCLString Result;
	Format Formatter(Result, pattern);
	for_each(CCalibratedParameterManager::begin(),
		     CCalibratedParameterManager::end(), Formatter);
	return string((const char*)Result);
		
}  

/*! 


Deletes a parameter given a name.

Parameters:

\param Result string
   On success empty on failure an error message
\param name string
   Name of the parameter to destroy.

\return int
\retval TCL_OK
   The named parameter was deleted.
\retval TCL_ERROR
   The named parameter could not be
   deleted and the reason is textually given by
   Result.

Pseudo code:

\verbatim
pParameter = CCalibratedParameterManager::DeleteParmeter(name)
if ! pParameter{
 
   Result = "No such parameter $name"
    return TCL_ERROR
} 
else {
    delete spectcl parameter.
    delete pParameter
    return TCL_OK
}
\endverbatim

*/
int 
CCalibratedParameterCommand::Delete(string& Result, string parameter)  
{
  CCalibratedParameter* pParameter = 
    CCalibratedParameterManager::DeleteParameter(parameter); 
  if(pParameter) {
    //   Locate, and delete the parameter from the histogrammer too.
    
    CParameter* pHParameter = 
      m_pHistogrammer->RemoveParameter(parameter); // dictionary remove
    if(pHParameter) {
      delete pHParameter;	// My responsibility to delete the object.
    }
    delete pParameter;
    return TCL_OK;
  } 
  else {
    Result  = "No Such Parameter: ";
    Result += parameter;
    return TCL_ERROR;
  }
}  

/*! 

Description:

Updates the fit for all parameters that correspond
to the pattern.  The pattern may contain
glob based wild card characters.


Parameters:

\param pattern (string)
   Only the parameters that match this pattern
  will be calibrated.

\return void
Pseudo code:

\verbatim
for_each name,pParameter {
   if name matches pattern {
       pFit = CFitFactory::FindFit(pParameter.getFitname())
       if pFit = CFitFactory::end()  {
             // Last known fit is used.
       }
       else {
          if pFit.State() is Performed then
             pParameter.ReplaceFit(pFit)   // Update with new fit.
          else {
               // fit unchanged, exists but is accepting.
          }
       }
   }
}
\endverbatim

*/
void 
CCalibratedParameterCommand::Update(string pattern)  
{ 
	Updater Update(pattern);
	for_each(CCalibratedParameterManager::begin(),
			 CCalibratedParameterManager::end(), 
			 Update);
}  

/*! 

Description:

Local function to locate a fit in the CFitFactory.


\pre 

\post 

Parameters:

\param sName (string)
    The name of the fit to look for.


\return CFitFactory::FitIterator
\return CFitFactory::FitIterator
  \retval CFitFactory::end()  - Fit not found.
  \retval other                      - Fit found and this an iterator that 'points' to the 
                                             name, CFit* pair.


\throw  

Pseudo code:

\verbatim

\endverbatim

*/
CFit*
CCalibratedParameterCommand::FindFit(string sName)  
{ 
	CFitFactory::FitIterator i = CFitFactory::FindFit(sName);
	if(i != CFitFactory::end()) {
		return i->second;
	}
	else {
		return (CFit*)NULL;
	}
}  

/*! 

Description:

Returns a string that describes the command usage.

Parameters:

\return string

Pseudo code:

\verbatim
message = "Usage:
              calibparam ?-create? name number raw fitname ?units?
              calibparam -list ?pattern?
              calibparam -delete name
              calibparam -refresh ?pattern?
           Where:
               name   - the name of a calibrated parameter.
               number - The parameter id of a calibrated parameter.
               raw    - The name of a raw parameter
               fitname- the name of a fit.
               units  - Units of measure  of the parameter.
               pattern- A match pattern that canuse glob wild card
                        characters"
return message
\endverbatim

*/
string 
CCalibratedParameterCommand::Usage()  
{ 
	string message = "Usage:\n";
	message       += "   calibparam ?-create? name number raw fitname ?units?\n";
	message       += "   calibparam -list ?patern?\n";
	message       += "   calibparam -delete name\n";
	message       += "   calibparam -refresh ?pattern?\n";
	message       += "Where:\n";
	message       += "   name   - The name of a calibrated parameter\n";
	message       += "   number - The parameter id of the calibrated parameter\n";
	message       += "   raw    - The name of a raw parameter\n";
	message       += "   fitname- The name of a fit.\n";
	message       += "   units  - Units of measure of the parameter.\n";
	message       += "   pattern - A match pattern that can use glob wild cards\n";
	message       += "   ?param? - Means the parameter may be omitted\n";
	
	return message;
}  

/*! 

Description:

Formats a listing of parameter.
The listing is of the form:
{name number rawname fitname units}

- If the parameter has been deleted, the name will be -deleted-
  and units will be blank.
- If the raw parameter has been deleted, its name will be -deleted-
- If the fit has been deleted its name will be "orignal name (deleted)"
- If the parameter has no units assigned to it, the units field will be ""

\pre 

\post 

Parameters:

\param sName (string)
    Name of the parameter.
\param pParameter (CCalibratedParameter& rParam)
    Pointer to the calibrated Parameter.


\return string
\return string
   \retval the listing of the parameter as described above.

\throw  

Pseudo code:

\verbatim
pCalibrated = FindParameter(sName)
pFit        = FindFit(pParam->getFitName())
pRaw        = FindParameter(pParam->getParameterId())
number      = pParam->getTargetParameterId()

if(!pCalibrated) {
   name = -deleted-
   units= "" 
} else {
   name = rCalibrated->getName()
   units= rCalibrated->getUnits()
}
if(pRaw) {
   rawname = pRaw->GetName()
} 
else {
   rawname = -deleted-
}
if (pFit) {
   fitname = rParam->getFitName()
}
else {
   fitname = rParam->getFitName() + "(deleted)"
}
return List(name, number,rawname, fitname, units)

\endverbatim

*/
string 
CCalibratedParameterCommand::FormatParameter(string  sParamName, 
					     CCalibratedParameter& rParam)  
{ 
	CTCLString result;
	string     name;
	string     units;
	string     number;
	string     rawname;
	string     fitname;
	char       FormatBuffer[100];


	CHistogrammer* pHistogrammer = (CHistogrammer*)gpEventSink;
	CParameter* pHParameter = pHistogrammer->FindParameter(sParamName);
	
	sprintf(FormatBuffer, "%d", rParam.getTargetId());
	if(pHParameter) {    // calibrated Parameter exists
		name = sParamName;
		number = FormatBuffer;
		units = pHParameter->getUnits();
		
	}
	else {
		name = "-deleted-";
		number= FormatBuffer;
		units = "";
	}
	
	pHParameter = pHistogrammer->FindParameter(rParam.getRawId());
	if(pHParameter) {
		rawname = pHParameter->getName();
	}
	else {
		rawname = "-deleted-";
	}
	fitname = rParam.getFitName();
	if(CFitFactory::FindFit(fitname) == CFitFactory::end()) {
		fitname += " (deleted)";
	}
	result.AppendElement(name);
	result.AppendElement(number);
	result.AppendElement(rawname);
	result.AppendElement(fitname);
	result.AppendElement(units);	
	return string((const char*) result);
	
}
