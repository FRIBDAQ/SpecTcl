/*
	Implementation file for CFitCommand for a description of the
	class see CFitCommand.h
*/



// Include files required:

#include <config.h>
#include "CFitCommand.h"    				
#include "CFitFactory.h"
#include "CFit.h"
#include "CFitCreator.h"

#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <TCLList.h>
#include <TCLString.h>
#include <TCLException.h>
#include <DesignByContract.h>

#include <Point.h>
#include <vector>
#include <string>
#include <algorithm>

#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


using namespace DesignByContract;

// Static attribute storage and initialization for CFitCommand

static vector<string> SwitchTable;

enum Switches {
  swCreate,
  swList,
  swDelete,
  swPerform,
  swAdd,
  swEvaluate
};

// Local helper classes used by STL algorithms.
//


/*!
   Create a list of fits.  The list is created in a
   TCLList but returnable as a string.
*/
class CreateListResult
{
private:
  CTCLString& m_listListing;
  string     m_sPattern;
public:
  CreateListResult(string sPattern, CTCLString& result);

  bool operator()(pair<string, CFit*> Item);
  string GetResult();
  static string FormatFit(string sName, CFit* pFit);
  static string FormatPoints(CFit* pFit);
  static string FormatParameters(CFit* pFit);
  
};
/*!
   Construct a fit lister:
*/
CreateListResult::CreateListResult(string sPattern,
				   CTCLString& result) :
  m_listListing(result),
  m_sPattern(sPattern)
{
  
}
/*! 
  If pattern is matched, add the fit to the listing:
*/

bool CreateListResult::operator()(pair<string, CFit*> Item)
{
  CTCLString name(Item.first);
  if(name.Match(m_sPattern)) {
    m_listListing.AppendElement(FormatFit(Item.first, Item.second));
  }
}
/*!
   Return the current list result string.
*/
string 
CreateListResult::GetResult()
{
  return m_listListing;
}
/*!
    Format the listing of a fit.  Each fit is listed as a TCL list
    containing:
   {name state {points} {parameters} }
   
  - name is the name of the fit.
  - state is the state of the fit: accepting | performed 
  - points is a list of lists of {x y} points.
  - parameters is empty ({}) if the fit state is not performed. Otherwise
    it is a list of the fit parameters in the form {paramname value}.

    \param sName (string)
       Name of the fit
    \param pFit (CFit*)
       Pointer to fit.

*/
string 
CreateListResult::FormatFit(string sName, CFit* pFit)
{
  CTCLString Info;
  Info.AppendElement(sName);
  Info.AppendElement(pFit->Type());
  Info.AppendElement((pFit->GetState() == CFit::Accepting) ?
		                   "accepting" : "performed");
  // The points.
  Info.AppendElement(FormatPoints(pFit));


  // The fit parameters:

  Info.AppendElement(FormatParameters(pFit));


  // Done.
  
  return string((const char*)Info);
  
}
/*!
   Return a TCL formatted list of fit points.
   \param pFit (CFit*)
    Pointer to the fit object.
*/
string
CreateListResult::FormatPoints(CFit* pFit)
{
  CTCLString Info;
  CFit::PointIterator i = pFit->begin();
  while(i != pFit->end()) {
    char aPoint[100];
    sprintf(aPoint, "%g %g ", i->x, i->y);
    Info.AppendElement(aPoint);
    i++;
  }    
  return string((const char*)Info);
}
/*!
   Format the Fit parameters into a TCL proper list.
   \param pFit (CFit*)
    Pointer to the fit object.
*/
string
CreateListResult::FormatParameters(CFit* pFit)
{
  CTCLString Info;

  if(pFit->GetState() == CFit::Performed) { // Otherwise no params.
    CFit::FitParameterList params = pFit->GetParameters();
    CFit::FitParameterIterator i = params.begin();
    while(i != params.end()) {
      Info.StartSublist();
      {
	char Value[100];
	sprintf(Value, "%g", i->second);
	Info.AppendElement(i->first);
	Info.AppendElement(Value);
      }
      Info.EndSublist();
      i++;
    }
    
  } 
  return string((const char*)Info);
}


//////////////////////////////// CFitCommand Implementation

/*!
    Create an object of type CFitCommand
    
    \param pInterp (CTCLInterpreter*)
     The interpreter on which this command is registsered.
     The command itself is unconditionally registered as 'fit'.
*/
CFitCommand::CFitCommand (CTCLInterpreter* pInterp) : 
  CTCLProcessor("fit", pInterp)
{ 
  // Stock the switch lookup table:

  if(SwitchTable.empty()) {
    SwitchTable.push_back("-create");
    SwitchTable.push_back("-list");
    SwitchTable.push_back("-delete");
    SwitchTable.push_back("-perform");
    SwitchTable.push_back("-add");
    SwitchTable.push_back("-evaluate");
  }

} 

/*!
    Called to destroy an instance of CFitCommand
*/
 CFitCommand::~CFitCommand ( )
{

}

/*! 

Description:

Gains control when the fit command is to be executed.
This function simply decodes the first switch and 
dispatches to the appropriate _parse function:
- -type   -> Create_parse
- -list     -> List_parse
- -delete -> Delete_parse
- -perform -> Perform_parse
- -add      -> AddPoints_parse
- -evaluate -> Evaluate_parse



Parameters:

\param rInterp (CTCLInterpreter& )
    TCL Interpreter object.
\param rResult (CTCLResult&)
    The result string
\param argc (int)
    The number of command line parameters.  Note that 
    the first parameter is the command name itself.
\param argv (char**)
    The parameters.

\return int
\retval TCL_OK - The command was successful, the result
    string contains subcommand specific stuff.
\retval TC_ERROR - The command failed and the result string
    contains the error message.


*/
int 
CFitCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult,
			int argc, char** argv)  
{
  // The first parameter is the command name:

  argc--;
  argv++;
 
  // The second parameter msut exist. It should be either a
  // valid switch or alternatively the type of a fit to be 
  // created:

  if(!argc) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Now let's figure out what the switch is:

  char* pSubCommand = *argv;

  switch(MatchKeyword(SwitchTable,
		      string(pSubCommand),
		      (int)swCreate)) {
  case swCreate:
    if(pSubCommand[0] == '-') {	// An actual switch:
      argc--;
      argv++;
    }
    return Create_parse(rInterp, rResult, argc, argv);
  case swList:
    argc--;
    argv++;
    return List_parse(rInterp, rResult, argc, argv);
  case swDelete:
    argc--;
    argv++;
    return Delete_parse(rInterp, rResult, argc, argv);
  case swPerform:
    argc--;
    argv++;
    return Perform_parse(rInterp, rResult, argc, argv);
  case swAdd:
    argc--;
    argv++;
    return AddPoints_parse(rInterp, rResult, argc, argv);
  case swEvaluate:
    argc--;
    argv++;
    return Evaluate_parse(rInterp, rResult, argc, argv);
  default:			// Illegal:
    argc--;
    argv++;
    rResult = Usage();
    return TCL_ERROR;
  }
  // Control should not fall here:

  REQUIRE(0, "Defect in program logic");
}  

/*! 

Description:

Parses the command arguments and, if all goes well, calls Create to create a new
fit object. This division of labor supports testing at low and high levels.
The form of the command is:

\verbatim
fit ?-create? fittype fitname
  o fittype a fit type e.g. linear
  o fitname the name of a fit.
\endverbatim

Parameters:

\param rInterp  (CTCLInterpreter&)
    The interpreter object.
\parm rResult (CTCLResult&)
    The result object.
\param argc (int)
    number of arguments this has been adjusted to
   eat up both the fit command and the -create switch.
\param argv (char**)
   Command line arguments.  This has been adjusted to contain only the fit type and name
(assuming the user's call is correct).



\return int
\retval TCK_OK
\retval TCL_ERROR

Requirements:
- There are two command line parameters.
- The fit must not yet exist.



*/
int 
CFitCommand::Create_parse(CTCLInterpreter& rInterp,
                          CTCLResult& rResult, 
			  int argc, char** argv)  
{
  // The requirements are like preconditions with a bit less
  // force behind them.  Instead of an exception, violating one
  // leads to an error return.

  // Check the number of parameters:

  if(argc != 2) {
    rResult = Usage();
    return TCL_ERROR;
  }
  
  // Check the name does not yet exist:

  char* pType = argv[0];
  char* pName = argv[1];

  if(CFitFactory::FindFit(string(pName)) != CFitFactory::end()) {
    rResult  = "Fit: ";
    rResult += pName;
    rResult += " already exists";
    return TCL_ERROR;
  }

  // Try to create the fit.  Any failure is assumed to be because
  // the fit type was invalid at this point:

  CFit* pFit = CFitFactory::Create(string(pType), string(pName));
  if(pFit) {
    rResult = pName;
    return TCL_OK;
  } 
  else {
    rResult = "Failed to create fit: ";
    rResult += pName;
    rResult += " probably not a valid type\n";
    rResult += Usage();
    return TCL_ERROR;
  }

  REQUIRE(0, "Program logic error");

}  

/*! 

Description:

Parses the -list command and passes control to the List function.
In fact the list function produces a vector describing all fits.  This vector is
filtered by the pattern passed into or defaulted by this command.



\pre 

\post 

Parameters:

\param rInterp (CTCLInterpreter&)
   Intepreter object.
\param rResult (CTCLResult&)
    Refers to the result string.
\param argc (int)
    count of command line parameters.  This has been
adjusted so that the only remaining parameter is the optional
pattern.  If the pattern exists, it is used to filter the fit names.
If not, it defaults to "*" which matches all fit names.
\param argv (char**)
    Pointers to the parameters.  Please look at argc to get an 
idea of what this points to.

\return int
\retval TCL_OK
\retval TCL_ERROR



*/
int 
CFitCommand::List_parse(CTCLInterpreter& rInterp, 
			CTCLResult& rResult, 
			int argc, char** argv)  
{ 
  // There may be at most one other parameter:  The pattern:

  if(argc > 1) {
    rResult = Usage();
    return TCL_ERROR;
  }
  // Figure out the pattern:

  string Pattern;

  if(argc) {
    Pattern = *argv;
  } 
  else {
    Pattern = "*";
  }

  // Build up the result list via a for_each loop:

  CTCLString  listing;
  CreateListResult listPredicate(Pattern, listing);

  for_each(CFitFactory::begin(), CFitFactory::end(), listPredicate);

  rResult = listPredicate.GetResult();
  return TCL_OK;
}  

/*! 

Parses the -delete switch and dispatches to the Delete function.

Parameters:

\param rInterp (CTCLInterpreter&)
    The interpreter executing us.
\param rResult (CTCLResult&)
    The result string.
\param argc (int)
    Remaining number of command line parameters (after -delete has been eaten)
\param argv (char**)
    Remaining command line parameters.

\return int
\retval TCL_OK
\retval TCL_ERROR


*/
int 
CFitCommand::Delete_parse(CTCLInterpreter& rInterp, 
			  CTCLResult& rResult, 
			  int argc , char** argv)  
{

  // There must be a single additional parameter, the
  // name of the fit:

  if(argc != 1) {
    rResult = Usage();
    return TCL_ERROR;
  }

  // To delete a gate requires that it first exist:
  //
  string sFitName(*argv);
  CFitFactory::FitIterator pFit = CFitFactory::FindFit(sFitName);
  if(pFit != CFitFactory::end()) {
    if(CFitFactory::Delete(sFitName)) {	// Deleted ok.
      rResult = sFitName;
      return TCL_OK;
    }
    else {			// Failed but don't know why.
      rResult = "Unable to delete fit: ";
      rResult += sFitName;
      return TCL_ERROR;
    }
  }
  else {			// No such fit:
    rResult  = "Fit: ";
    rResult += sFitName;
    rResult += " does not exist!";
    return TCL_ERROR;
  }

}  

/*! 


Parses the -perform switch.  Then calls Perform which must locate the
appropriate fit and perform it.



\param rInterp (CTCLInterpreter&)
   The interpreter 
\param rResult (CTCLResult&)
    The result string.
\param argc (int)
     The number of parameters after -delete.
\param argv (char**)
    Pointer to these parameters

*/
int
CFitCommand::Perform_parse(CTCLInterpreter& rInterp,
			   CTCLResult&      rResult,
			   int argc, char** argv)  
{
  // There must be a parameter; the name of the fit:

  if(argc != 1) {
    rResult = Usage();
    return TCL_ERROR;
  }
  string sFitName(*argv);

  // The fit must exist:

  CFitFactory::FitIterator pFit = CFitFactory::FindFit(sFitName);
  if(pFit !=  CFitFactory::end()) { // Fit exists.
    // The fit must perform with no error:

    if(CFitFactory::Perform(sFitName)) {
      CFit* p = pFit->second;
      rResult = CreateListResult::FormatParameters(p);
      return TCL_OK;
    }
    else {			// Fit failed, likely isufficient pts
      rResult = "Fit: ";
      rResult += sFitName;
      rResult += " could not be performed. Probably needs more points";
      return TCL_ERROR;
    }
       
  }
  else {			// Fit does not exist.

    rResult  = "Fit: ";
    rResult += sFitName;
    rResult += " does not exist";
    return TCL_ERROR;
  }
}  

/*! 

Description:

Parses the fit -add command.  This command adds points
to the fit, by parsing the points on the command line and repeatedly
calling AddPoint to add each point to the fit.



\param rInterp (CTCLInterpreter&)
    Interpreter object.
\param rResult (CTCLResult&)
   The TCL interpreter result string.
\param argc (int)
   number of command line parameters after the -delete switch
\param argv (char**)
    The parameters after the -delete switch.


\return int
\retval TCL_OK
\retval TCL_ERROR


*/
int 
CFitCommand::AddPoints_parse(CTCLInterpreter& rInterp, 
			     CTCLResult& rResult, 
			     int argc, char** argv)  
{
  // There must be at least two additional parameters... 
  // A fit name and a point to add
  //
  if(argc < 2) {
    rResult = Usage();
    return TCL_ERROR;
  }

  // The fit must exist:

  string sFitName(*argv);
  CFitFactory::FitIterator p = CFitFactory::FindFit(sFitName);
  if(p == CFitFactory::end()) {
    rResult  = "Fit: ";
    rResult += sFitName;
    rResult += " does not exist";
    return TCL_ERROR;
  }
  argc--;
  argv++;

  // All the remaining parameters must be 2 element lists
  // containing pairs of values that parse as doubles.
  //

  vector<FPoint> vPoints;
  try {
    vPoints = ParsePoints(rInterp, argc, argv); 
  }
  catch (string failed) {	// Points parse failed.
    rResult  = "Fit points must be pairs of doubles and: ";
    rResult += failed;
    rResult += " is not.";
    return TCL_ERROR;
  }
  // Now we have enough stuff to ask for the points to be
  // added:

  if(CFitFactory::AddPoints(sFitName, vPoints)) { // OK!
    rResult = sFitName;
    return TCL_OK;
  }
  else {			// Failed!
    rResult  = "Unable to add points to ";
    rResult += sFitName;
    return TCL_ERROR;
  }
}  

/*! 

Description:

Parses the -evaluate subcommand.
This command accpts a fit name and an x coordinate
and invokes the Evaluate function to evaluate the
fit at that x coordinate.  Note that this can fail if
the fit is in the adding points state.
 
Parameters:

\param rInterp (CTCLInterpreter&)
   The interpreter object executing this command.
\param rResult (CTCLResult&)
    The interpreter result string.
\param argc (int)
    The number of parameters after the -delete command.
\param argv (char**)
    The parameters after the delete command.

*/
int 
CFitCommand::Evaluate_parse(CTCLInterpreter& rInterp, 
			    CTCLResult& rResult, int argc, 
			    char** argv)  
{ 
  // There must be exactly two parameters, a fit name and
  // a point:

  if(argc != 2) {
    rResult = Usage();
    return TCL_ERROR;
  }
  string sFitName(*argv);
  argc--;
  argv++;
  string sPoint(*argv);

  // The point must evaluate to a double precision value:

  DFloat_t dPoint;
  try {
    dPoint = rInterp.ExprDouble(sPoint);
  }
  catch (CTCLException& rExcept) {
    rResult  = "Point value: ";
    rResult += sPoint;
    rResult += " does not evaluate to a double expression";
    return TCL_ERROR;
  }
  // Now evaulate the fit:

  
  try {
    DFloat_t dResult = CFitFactory::Evaluate(sFitName, dPoint);
    char     sResult[100];
    sprintf(sResult, "%g", dResult);
    rResult = sResult;
    return TCL_OK;
  }
  catch(...) {			// Failed.
    rResult  = "Evaluation of fit: ";
    rResult += sFitName;
    rResult += " @ ";
    rResult += sPoint;
    rResult += " failed.  Fit may not have been performed yet.";
    return TCL_ERROR;
  }
  
}  

/*! 

Description:

Return the usage of the fit command as a string.

\pre 

\post 

Parameters:

\return string
\return string
\retval the reason the fit failed.

\throw  

Pseudo code:

\verbatim

\endverbatim

*/
string 
CFitCommand::Usage()  
{ 
  string sUsage("Usage:\n");
  sUsage     += "   fit ?-create? type name\n";
  sUsage     += "   fit  -list ?pattern?\n";
  sUsage     += "   fit  -delete name\n";
  sUsage     += "   fit  -perform name\n";
  sUsage     += "   fit  -add name point1 ?point2...?\n";
  sUsage     += "   fit  -evaluate name x\n";
  sUsage     += "\n";
  sUsage     += "Where:\n";
  sUsage     += "  `type' is the type of fit to create (see below)\n";
  sUsage     += "   `name' is the name of the fit to create or manipulate\n";
  sUsage     += "   `pattern' Is a pattern to match in listing fits.\n";
  sUsage     += "   `pointn' are pairs of floating point numbers that\n";
  sUsage     += "         are independent and dependent values that\n";
  sUsage     += "         are the data points to be fit.\n";
  sUsage     += "   `x'   Is a value to evaluate with the fit function\n";
  sUsage     += "\n";
  sUsage     += "And the valid fit types are:\n";
  CFitFactory::FitCreatorIterator i = CFitFactory::beginCreators();
  while(i != CFitFactory::endCreators()) {
    CFitCreator* pCreator = i->second;
    sUsage   += "   ";
    sUsage   += i->first;
    sUsage   += " - ";
    sUsage   += pCreator->DescribeFit();
    sUsage   += "\n";
    i++;
  }
  return sUsage;
}


// Private utilities:

/*!
   Parse a set of parameters where each parameters is supposed to
   contain a two element list containing two doubles.   If the
   parse fails, a string exception is thrown where the value of the
   string is the parameter the parse failed on.

   \param rInterp (CTCLInterpreter& rInterp)
      Refers to a TCL interpreter that can be used to assist the 
      parse.
   \param argc    (int)
      Number of parameters to parse.
   \param argv    (char**)
      List of parameters to parse.
   

   \return vector<FPoint>
       Each element of the vector is a parsed pair.

*/
vector<FPoint>
CFitCommand::ParsePoints(CTCLInterpreter& rInterp,
			 int argc, char** argv)
{
  FPoint p;
  vector<FPoint> ParsedList;

  // Each point is processed in the loop below:

  for(int i =0; i < argc; i++) {
    CTCLList lPoint(&rInterp, argv[i]);
    try {
      StringArray s;
      int i = lPoint.Split(s);
      if((i != TCL_OK) ||
	 (s.size() != 2)) {	// Each parameter mb. a 2 element list
	throw string(argv[i]);
      }
      else {
	p = FPoint(rInterp.ExprDouble(s[0]),
		   rInterp.ExprDouble(s[1]));
	ParsedList.push_back(p);
      }
    }
    catch(...) {		// Turn any exceptions into a string...
      string FailedParam(argv[i]);
      throw FailedParam;
    }
  }
  return ParsedList;
}
