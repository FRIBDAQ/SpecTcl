
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

#include <config.h>
#include "CFitCommand.h"
#include <CFitDictionary.h>
#include <CFitFactory.h>
#include <CFit.h>
#include <CSpectrumFit.h>
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>
#include <assert.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Static class data:

int CFitCommand::m_fitId(0);	// Unique fit id (not really used but no cost).

/*!
  Create/register the fit command with the interpreter.
  The fit dictionary reference is initialized and that, if necessary instantiates
  the fit dictionary which, in turn registers the standard fit creators.
  This must be done prior to creating the first fit or else none of the standard
  fit types will be known to the fit factory.
  \param interp : CTCLInterpreter&
     References the interpreter on which this command is being registered.
  \param name : std::string [fit]
      Command name (argv[0]).  This defaults to "fit" if not provided.
*/
CFitCommand::CFitCommand(CTCLInterpreter& interp,
			 string           name) : 
  CTCLObjectProcessor(interp, name),
  m_dictionary(CFitDictionary::getInstance())
{}
/*!
  Nothing really to destruction, but this is provdied to chain tot he base class
  which deletes us from the interpreter.
*/
CFitCommand::~CFitCommand()
{
}
//////////////////////////////////////////////////////////////////////
//////////////////// Command dispatching/processing //////////////////
//////////////////////////////////////////////////////////////////////

/*!
  Called when the fit command is being executed. This top level
  just dispatches to the appropriate subcommand handler.
  \param interp : CTCLInterpreter&
     Reference to the interpreter that is executing this command.
  \param objv : std::vector<CTCLObject>& 
     Reference to an array of Tcl objects that are the command line words
     (e.g. objv[0] is the command keyword).
   \return int
   \retval TCL_OK  - Command was successful.
   \retval TCL_ERROR - command had an error.

    In most cases where TCL_ERROR is returned, usable error and usage
    information is returned in the command result as well.
*/

int
CFitCommand::operator()(CTCLInterpreter& interp,
			vector<CTCLObject>& objv) 
{
  // We must have at least a subcommand.

  if (objv.size() < 2) {
    string result  = objv[0];
    result        += " - must have at least a subcommand: \n";
    result        += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  // dispatch or error

  string subcommand = objv[1];
  if (subcommand == string("create") ){
    return Create(interp, objv);
  }
  else if (subcommand == string("update")) {
    return Update(interp,objv);
  }
  else if (subcommand == string("delete")) {
    return Delete(interp,objv);
  }
  else if (subcommand == string("list")) {
    return List(interp, objv);
  }
  else if (subcommand == string("proc")) {
    return Proc(interp, objv);
  }
  else {
    string result = objv[0];
    result       += " invalid subcommand keyword: ";
    result       += string(objv[1]);
    result       += ":\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  // Control should not bounce here.

  assert(0);
}

/*!
   Allocate a fit id. (used for external fit creators).
*/
int 
CFitCommand::id()
{
  return m_fitId++;
}

/*!
   Create a  fit.
   The full syntax of this is:

   fit create spectrum-name low-chan high-chan fittype

   - spectrum-name - the name of a 1-d spectrum on which the fit will be done.
   - low-chan      - the low limit of the region of interest over which the fit
                     will be done.
   - high-chan     - the high limit of the region of interest over which the fit
                     will be done.
   - fittype       - the type of fit to create (e.g. "gaussian").


  
  \param interp : CTCLInterpreter&
     Reference to the interpreter that is executing this command.
  \param objv : std::vector<CTCLObject>& 
     Reference to an array of Tcl objects that are the command line words
     (e.g. objv[0] is the command keyword).
  \return int
   \retval TCL_OK  - Command was successful.
   \retval TCL_ERROR - command had an error.

*/
int
CFitCommand::Create(CTCLInterpreter& interp,
		    vector<CTCLObject>& objv)
{
  // ensure we have all the stuff we need:

  if (objv.size() != 7) {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " - does not have the right number of command line parameters.\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  objv[4].Bind(interp);
  objv[5].Bind(interp);

  string name = objv[2];
  string spectrumName = objv[3];
  int  low;
  int  high;
  try {
    low  = objv[4];
    high = objv[5];
  }
  catch (CException& failure) {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " - ";
    result       += failure.ReasonText();
    result       += "\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  string type = objv[6];

  // Make sure that low < high...

  if (low >= high) {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " - low limit must be less than high limit\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  // Now we are ready to rock:

  CFit* pFit = CFitFactory::Create(type, name, m_fitId++);
  if (!pFit) {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " - Unable to create a fit of type ";
    result       += type;
    result       += " probably this fit type is not supported\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;

  }
  // Wrap the fit in a spectrum fit and insert it into the dictionary.
  //

  CSpectrumFit* pSpectrumFit;
  try {
    pSpectrumFit = new CSpectrumFit(spectrumName, pFit->getNumber(),
				    low, high, *pFit);
    m_dictionary.add(*pSpectrumFit);
				    
  }
  catch(CException& error) {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " - unable to wrap the fit and insert it in the dictionary\n";
    result       += error.ReasonText();
    result       += '\n';
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  
  interp.setResult(name);
  return TCL_OK;
}

/*!
  Update - Requests that one or more fits in the dictionary be updated.
  The syntax of this command is:
\verbatim
   fit update ?pattern?
\endverbatim

   If the pattern is not supplied, it defaults to *.  All fits that
   are a 'glob' match to the pattern are updated.  This means that
   the data in the spectra are upated into the fit, the fit is
   re-performed, and therefore, the observers attached to the fit
   are informed of this update.  From the user's point of view what they
   should see is that the fit should correspond to the current data in
   Xamine or whatever displayer is in use.

  \param interp : CTCLInterpreter&
     Reference to the interpreter that is executing this command.
  \param objv : std::vector<CTCLObject>& 
     Reference to an array of Tcl objects that are the command line words
     (e.g. objv[0] is the command keyword).
  \return int
   \retval TCL_OK  - Command was successful.
   \retval TCL_ERROR - command had an error.
*/
int 
CFitCommand::Update(CTCLInterpreter& interp,
		    vector<CTCLObject>& objv)
{
  // We need either 2 or three parameters in the command line:


  size_t nWords = objv.size();
  if ((nWords != 2) && (nWords != 3) ) {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " - Incorrect number of command line parameters\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  // Now figure out if there's a pattern.. but defaut it to "*".

  string pattern = "*";
  if (nWords == 3) {
    pattern = string(objv[2]);
  }

  // Do the deed (can't fail from here on out):

  m_dictionary.updateFits(pattern);

  // Nothing to put in the result...

  return TCL_OK;
}
/*!
   Deletes a fit from the dictionary, and deletes the underlying fit.
   If the fit does not exist this is reported as a scripting error.
   The syntax of the command is:
\verbatim
   fit delete fitname
\endverbatim
 
    Only a single fit can be provided.  Naturally, however constructions like
    the one shown below can work perfectly well.
\verbatim
   foreach fit [fit list $pattern] {
     set name [lindex $fit 0]
     fit delete $name
    }
\endverbatim


  \param interp : CTCLInterpreter&
     Reference to the interpreter that is executing this command.
  \param objv : std::vector<CTCLObject>& 
     Reference to an array of Tcl objects that are the command line words
     (e.g. objv[0] is the command keyword).
  \return int
   \retval TCL_OK  - Command was successful.
   \retval TCL_ERROR - command had an error
*/
int
CFitCommand::Delete(CTCLInterpreter& interp,
		    vector<CTCLObject>& objv)
{
  // There must be exactly 3 parameters on the command line:

  if (objv.size() != 3) {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " - Incorrect number of command line parameters.\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  // Now we need to see if the fit really exists, and if so get an
  // iterator and a pointer to it.

  string fitName = objv[2];
  CFitDictionary::iterator p = m_dictionary.find(fitName);
  if (p == m_dictionary.end()) {
    string result = fitName;
    result       += " - fit not found so it can't be deleted";
    interp.setResult(result);
    return TCL_ERROR;
  }

  CSpectrumFit* pFit = p->second;
  m_dictionary.erase(p);
  delete pFit;

  return TCL_OK;		// Nothing really to return for a result.
}
/*!
   List the set of fits whose name matches the specified pattern.
   The full format of this command is:

\verbatim
   fit list ?pattern?
\verbatim

   If the optional pattern parameter is not provided, it defaults to "*".
   Note that the match can contain any glob wildcard characters.
   The result of the fit is a list of 5 element sublists.  The following
   are the elements of each sublist:
   - fitname - The name of the fit.
   - spectrumname - the name of the spectrum the fit was created on.
   - type    - the type of the fit (e.g. "gaussian").
   - limits  - A two element list  of the fit area of interest in [list $low $high]
               form.
   - parameters - A property list that contains the fit parameters.   Fit parameters
                  may vary wildly from fit type to fit type, however all fits
                  should return a fit parameter named chisquare that is a measure
                  of the goodness of the fit.  A property list is a list of
		  2-element sublists.  Each sublist is in the form
		  [list $keyword $value] where the keyword is the name of the
		  property (e.g. "chisquare") and the value is its (in this case
		  numeric) value.


  \param interp : CTCLInterpreter&
     Reference to the interpreter that is executing this command.
  \param objv : std::vector<CTCLObject>& 
     Reference to an array of Tcl objects that are the command line words
     (e.g. objv[0] is the command keyword).
  \return int
   \retval TCL_OK  - Command was successful.
   \retval TCL_ERROR - command had an error
*/
int
CFitCommand:: List(CTCLInterpreter& interp,
		   vector<CTCLObject>& objv)
{
  // the command must have either 2 or 3 words:

  size_t nWords = objv.size();
  if ((nWords != 2) && (nWords != 3)) {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " - Incorrect number of command line parameters\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  // Figure out the pattern:

  string pattern = "*";
  if (nWords == 3) {
    pattern = string(objv[2]);
  }

  // We need an object in which to store the result:

  CTCLObject result;
  result.Bind(interp);

  // Now iterate through the fits, creating descriptions for each fit
  // and adding them to the result as a list element.
  //
  CFitDictionary::iterator p = m_dictionary.begin();
  while (p != m_dictionary.end()) {
    CTCLObject element;
    element.Bind(interp);
    string         name   = p->first;
    CSpectrumFit*  pFit   = p->second;

    if (Tcl_StringMatch(name.c_str(), pattern.c_str())) {
      element = describeFit(interp, pFit);
      result += element;
    }

    p++;
  }
  interp.setResult(result);
  return TCL_OK;
}
/*!
   Returns a Tcl proc definition for a proc named fitline that
   when passed a single parameter evaluates the height of the fit at that
   parameter.  Syntax is:
\verbatim

   fit proc name

\endverbatim
   A typical usage might be to define a proc in the current interpreter
   to evaluate the fit e.g.:

\verbatim 
   set proc [fit proc somefit]
   eval $proc;                    # defines proc fitline
   rename fitline somefit;        # renames it after the fit.

\endverbatim

  \param interp : CTCLInterpreter&
     Reference to the interpreter that is executing this command.
  \param objv : std::vector<CTCLObject>& 
     Reference to an array of Tcl objects that are the command line words
     (e.g. objv[0] is the command keyword).
  \return int
   \retval TCL_OK  - Command was successful.
   \retval TCL_ERROR - command had an error
*/
int
CFitCommand::Proc(CTCLInterpreter& interp,
		  vector<CTCLObject>& objv)
{
  // The command requires exactly 3 parameters:

  if (objv.size() != 3) {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " - incorrect number of command line parameters\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  string fitName = objv[2];

  // Locate the fit.  It is an error for the fit to not exist:

  CFitDictionary::iterator p = m_dictionary.find(fitName);
  if (p == m_dictionary.end()) {
    string result = fitName;
    result       += " - could not be found to describe its proc";
    interp.setResult(result);
    return TCL_ERROR;
  }
  // Get the description

  CSpectrumFit* pFit = p->second;
  interp.setResult(pFit->makeTclFitScript());
  return TCL_OK;
}

////////////////////////////////////////////////////////////////////
/////////////////////// Private utility procs.  ////////////////////
////////////////////////////////////////////////////////////////////

/*
   Describe a fit into a CTCLObject.
   The fit description is returned as list element for the
   result of List.  See the comments to List for the format
   of this return list.

*/
CTCLObject
CFitCommand::describeFit(CTCLInterpreter& interp, CSpectrumFit* pFit)
{
  // Pull out the elements of the fit description:
  //
  string fitName       = pFit->fitName();
  string spectrumName  = pFit->getName();
  string fitType       = pFit->fitType();
  int    low           = pFit->low();
  int    high          = pFit->high();
  CFit::FitParameterList params = pFit->getParameters();

  // Build the sublists
  // Limits:

  CTCLObject limits;
  limits.Bind(interp);
  limits     += low;
  limits     += high;

  // Fit parameters:

  CTCLObject parameters;
  CTCLObject parameter;
  parameters.Bind(interp);
  parameter.Bind(interp);
  CFit::FitParameterIterator p = params.begin();
  while (p != params.end()) {
    string paramName = p->first;
    double value     = p->second;
    parameter   = paramName;
    parameter  += value;
    parameters += parameter;

    p++;
  }
  // Now we're ready to build pu and return the result:

  CTCLObject result;
  result.Bind(interp);
  result += fitName;
  result += spectrumName;
  result += fitType;
  result += limits;
  result += parameters;

  return result;
  
}
/*
   Return a string that describes the proper command syntax.
*/
string
CFitCommand::Usage()
{
  string usage;
  usage       = "Usage:\n";
  usage      += "   fit create name spectrum low high type\n";
  usage      += "   fit update ?pattern?\n";
  usage      += "   fit delete name\n";
  usage      += "   fit list ?pattern?\n";
  usage      += "   fit proc name\n";
  usage      += "Where\n";
  usage      += "    name      - is the name of a fit\n";
  usage      += "    spectrum  - is the name of an existing 1-d spectrum\n";
  usage      += "    low,high  - define the fit area of interest in channels\n";
  usage      += "    type      - is the fit type e.g. gaussian\n";
  usage      += "    pattern   - is a pattern that can contain wildcards\n";

  return usage;
  
}
