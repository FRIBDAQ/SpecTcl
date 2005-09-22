///////////////////////////////////////////////////////////
//  CTreeParameterCommand.cpp
//  Implementation of the Class CTreeParameterCommand
//  Created on:      30-Mar-2005 11:03:51 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
#include <config.h>
#include "CTreeParameterCommand.h"
#include "CTreeParameter.h"

#include "CMatchingVisitor.h"
#include "SetLimitsVisitor.h"
#include "SetUnitsVisitor.h"
#include "SetChannelsVisitor.h"
#include "SetWidthVisitor.h"
#include "SetAllVisitor.h"
#include "ListVisitor.h"
#include "UncheckVisitor.h"
#include <SpecTcl.h>
#include <histotypes.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Constants:

// The constant below defines the tolerance in width/channels mismatch
// allowed as a fraction of the channel width computed from the limits
// and the channel count.  This is important because the low/high/channel-count
// channel-width form an overdetermined system that, if all can be simultaneously 
// specified must be checked for consistency.
//
static const DFloat_t tolerance(0.01);


//!  Destructor: Nothing really to do.

CTreeParameterCommand::~CTreeParameterCommand()
{

}


/**
 * Constructor for the "treeparameter" command.
 * @param pInterp
 *        Pointer to he interpreter in which this command will be
 *        registered.
 * 
 */
CTreeParameterCommand::CTreeParameterCommand(CTCLInterpreter* pInterp) :
  CTCLProcessor("treeparameter", pInterp)
{
  // If the interpreter is not supplied (NULL), then locate it using
  // the SpecTcl API.
  //
  if(!pInterp) {
    SpecTcl& api(*(SpecTcl::getInstance()));
    Bind(api.getInterpreter());
  
  }

  // Register the command.
  
  Register();
  
  
}


/**
 * Dispatches to the appropriate subcommand processor.
 * @param rInterp
 *        Reference to the interpreter that is executing this command.
 * @param rResult
 *        The result of the command.
 * @param argc
 *        Number of command line 'words' .
 * @param argv
 *        Array of pointers to the words. Note that argv[0] points to the
 *        command that got us here.
 * 
 */
int CTreeParameterCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
				      int argc, char** argv)
{

  argc--; argv++;		// Skip our command verb.
  //
  // We require a subcommand parameter...after all this does implement an ensemble.
  //
  if(!argc)  {
    rResult = "Insufficient parameters\n";
    
    rResult += Usage();
    return TCL_ERROR;
  }
  //
  //  stringify the subcommand and dispatch to the appropriate function:
  //
  int status;
  string subcommand(argv[0]);
  argv++;
  argc--;
  //
  if(subcommand == "-list") {
    status = List(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-set") {
    status = SetDefinition(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-setinc") {
    status = SetIncrement(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-setbins") {
    status = SetChannelCount(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-setunit") {
    status = SetUnit(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-setlimits") {
    status = SetLimits(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-check") {
    status = Check(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-uncheck") {
     status = UnCheck(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-version") {
    status = Version(rInterp, rResult, argc, argv);
  }
  else if (subcommand == "-create") {
    status = Create(rInterp, rResult, argc, argv);
  }
  else {
    // Invalid ensemble subcommand:
    //
    rResult = "Invalid subcommand";
    rResult += subcommand;
    rResult += "\n";
    rResult += Usage();
    status = TCL_ERROR;
  }
  //
  //
  return status;

  
}


/**
 * Returns the usage string information for the command.
 */
string 
CTreeParameterCommand::Usage()
{

  string usage;
  //
  usage   = "Usage:\n";
  usage += "     treeparameter -list ?pattern?\n";
  usage += "     treeparameter -set name bins low high inc units\n";
  usage += "     treeparameter -setinc name inc\n";
  usage += "     treeparameter -setbins name bins\n";
  usage += "     treeparameter -setunit name units\n";
  usage += "     treeparameter -setlimits name low high\n";
  usage += "     treeparameter -check name\n";
  usage += "     treeparameter -uncheck name\n";
  usage += "     treeparameter -create  name low high bins units\n";
  usage += "     treeparameter -version";
  //
  return usage;

}


/**
 * Lists the characteristics of the matching tree parameters  Returns either
 * TCL_OK or TCL_ERROR
 * @param rInterp
 *        Reference to the interpreter object on which this command is
 *        executing.
 * @param rResult
 *        Reference to the object wrapped TCL result string that we are
 *        supposed to create and return to the caller.
 * @param argc
 *        Number of remaining un-processed command parameters (should be 0
 *        or 1 only).
 * @param argv
 *        Should be an array of pointers to the command parameters.
 * 
 */
int CTreeParameterCommand::List(CTCLInterpreter& rInterp, CTCLResult& rResult, 
				char argc, char** argv)
{

  string pattern ="*";		// Default matching pattern matches all of them.
  if(argc) {
    pattern = argv[0];		// Override if the user supplied it
    argc--; argv++;
  }
  if (argc) {			// Should not be any more command line parameters...
    rResult   = "Extra command line parameters after treeparameter -list ";
    rResult += pattern;
    rResult += " ";
    rResult += argv[0];
    rResult += " ...\n";
    rResult += Usage();
    return TCL_ERROR;
  }

  // We use the for_each generic algorithm with a ListVisitor to construct the
  // listing directly into Result:
  
  ListVisitor visitor(pattern, rResult);
  
  for_each (CTreeParameter::begin(), CTreeParameter::end(), visitor);
  
  return TCL_OK;
  
  
}


/**
 * Sets the definition of the matching tree parameter(s).  Returns either TCL_OK
 * or TCL_ERROR.  Note that in order to match the legacy definition of this
 * function, while hitting acceptable quality targets, we need to deal with the
 * fact that low, high channels, and channel width overspecifies the axis.
 * 
 * TreeParameter internally, allows width to imply a change in the upper axis
 * limit while holding the channel count and the lower limit fixed.  What we will
 * do here is
 * - Always compute the width from low/high/channels.
 * - enforce a consistency between the width computed from the low,high,channels
 * and the width supplied by the user.   If this consistency isn't present to
 * within a
 *  tolerance that is some fraction of the 'exact' width, the command is
 *  considered to be in error and nothing is done to the underlying parameter.]
 * 
 * @param rInterp
 *        The interpreter on which this command is executing.
 * @param rResult
 *        The result string the command returns that is the return
 *        value of the command at script level.
 * @param argc
 *        Number of remaining words on the command line.  This should
 *        consist of
 *        - The name of the parameter to modify.
 *        - The new bin count
 *        - The new low limit.
 *        - The new high limit
 *        - The channel width
 *        - The new units of measure.
 * @param argv
 *        List of parameters.  See the discussion of argc for information
 *        about what's expected.
 * 
 */
int 
CTreeParameterCommand::SetDefinition(CTCLInterpreter& rInterp, CTCLResult& rResult, 
				     int argc, char** argv)
{
  //
  // Require the correct number of parameters:
  //
  if(argc != 6) {
    rResult   = "Incorrect number of command parameters";
    rResult += Usage();
    return TCL_ERROR;
  }  
  //
  // Pull out the parameters, and check our ability to parse them properly.
  // I should have:
  //     name bins start stop width units
  //
  // nothing can go wrong with the string parameters.
  //
  string name(argv[0]);
  string units(argv[5]);
  //
  // The channels must parse as an integer.
  //
  int Index;		       // used by the exception handler if parses fail.
  Long_t   channels;
  DFloat_t start;
  DFloat_t stop;
  DFloat_t width;
  
  try {
    Index = 1;
    channels = rInterp.ExprLong(argv[Index]);
    
    Index++;
    start = rInterp.ExprDouble(argv[Index]);
    
    Index++;
    stop = rInterp.ExprDouble(argv[Index]);
    
    Index++;
    width = rInterp.ExprDouble(argv[Index]);
    
  }
  catch (...) {
    return TypeSafeParseFailed(rResult, argv[Index], "correct data type");
    
  }

  // Channels must be > 0:

  if (channels <= 0) {
    rResult  = "Channel value < 0 : ";
    rResult += argv[1];
    rResult += "\n";
    rResult += Usage();
    return TCL_ERROR;
  }

  // Low > high is an error (= is ok I suppose).

  if (start > stop) {
    rResult  = "Start must be <= stop.  Start = ";
    rResult += argv[2];
    rResult += " Stop = ";
    rResult += argv[3];
    rResult += "\n";
    rResult += Usage();
    return TCL_ERROR;
  }

  // Width must be >= 0:

  if (width < 0) {
    rResult  = "Width must be >= 0 : ";
    rResult += argv[4];
    rResult += "\n";
    rResult += Usage();
    return TCL_ERROR;
  }

  //  Ensure the low/high/bins is  compatible with the width.
    
  DFloat_t correctWidth = (stop - start)/(channels);
  if (fabs(correctWidth - width) >  tolerance*correctWidth) {
    rResult = "Width supplied is not consistent with other parameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  // Now we know everything and can construct the visitor and proceess
  // the set.
  
  SetAllVisitor visit(name,  channels, start, stop, units);
  for_each(CTreeParameter::begin(), CTreeParameter::end(), visit);
  return TCL_OK;
}


/**
 * Sets the channel width of the inhdicated parameter.  Returns either TCL_OK or
 * TCL_ERROR
 * @param rInterpreter
 *        Reference to the interpreter executing this command.
 * @param rResult
 *        Reference to the result string that this function returns
 *        (The command result).
 * @param argc
 *        Number of command line parameters remaining.  There shoulid be two:
 *        - Name pattern.
 *        - New increment to apply to the name pattern.
 * @param argv
 *        An array of pointers to the parameters.
 * 
 */
int 
CTreeParameterCommand::SetIncrement(CTCLInterpreter& rInterpreter, 
				    CTCLResult& rResult, int argc, char** argv)
{

  // Require 2 parameters:
  //
  if(argc !=2) {
    rResult   = "Invalid number of #endifparameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  // First is a string, but the second must parse as a double:
  
  string   pattern(argv[0]);
  double width;
  try {
    width = rInterpreter.ExprDouble(argv[1]);
  }
  catch (...) {
    return TypeSafeParseFailed(rResult, argv[1], "double");
  }

  // Validate the width:

  if(width <= 0.0) {
    rResult = "Width must be greater than zero and was: ";
    rResult += argv[1];
    rResult += "\n";
    rResult += Usage();
    return TCL_ERROR;
  }

  // Now visit all the parameters with a SetWidthVisitor:
  
  SetWidthVisitor  visitor(pattern, width);
  for_each(CTreeParameter::begin(),CTreeParameter::end(), visitor);
  return TCL_OK;
}


/**
 * Sets the channel count for the default axis of a spectrum produced by the GUI.
 * Returns either TCL_OK or TCL_ERROR
 * @param rInterp
 *        The interpreter that is executing this command.
 * @param rResult
 *        The result string the command returns as the command value.
 * @param argc
 *        Number of remaining unprocessed command parameters.  This should
 *        consist of:
 *        - pattern - the pattern of treeparameters that get modified by this command.
 *        - channels - the new number of channels the matching parameters should have by
 *          default.
 * @param argv
 *        Array of pointers to the string parameters.
 * 
 */
int 
CTreeParameterCommand::SetChannelCount(CTCLInterpreter& rInterp, 
					   CTCLResult& rResult, 
					   int argc, char** argv)
{

  // Require 2 parameters:
  
  if(argc !=2) {
    rResult   = "Invalid number of parameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  // First is a string, but the second must parse as a double:
  
  string    pattern(argv[0]);
  Long_t channels;
  try {
    channels = rInterp.ExprLong(argv[1]);
  }
  catch (...) {
    return TypeSafeParseFailed(rResult, argv[1], "integer");
  }
  if (channels <= 0) {
    rResult = "Channel count must be > 0  was : ";
    rResult += argv[1];
    rResult += "\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  // Now visit all the parameters to modify the matches:
  
  SetChannelsVisitor visitor(pattern, channels);
  for_each(CTreeParameter::begin(), CTreeParameter::end(), visitor);
  return TCL_OK;
  
  
}


/**
 * Sets the units of measure of the parameter.  At present this is a successful no-
 * op.
 * @param rInterp
 *        The interpreter on which this command is executing.
 * @param rResult
 *        The result string that is returned by this object.
 * @param argc
 *        count of remaining command line parameters.  Should be 2:
 *        - pattern - specifies the set of parameters that are affected.
 *        - units   - New units to apply to the parameters.
 * @param argv
 *        Array of pointers to the command line parameters.
 * 
 */
int 
CTreeParameterCommand::SetUnit(CTCLInterpreter& rInterp, CTCLResult& rResult, 
				   int argc, char** argv)
{
  
  // Validate the parameters:
  
  if(argc != 2) {
    rResult = "Incorrect number of parameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  string pattern(argv[0]);
  string units(argv[1]);
  
  // Create a SetUnits visitor and do the deed:
  
  SetUnitsVisitor visitor(pattern, units);
  for_each(CTreeParameter::begin(), CTreeParameter::end(), visitor);
  
  return TCL_OK;
  
  
}


/**
 * sets the default axis limits for an axes on the matching parameters for spectra
 * created by the GUI.
 * @param rInterp
 *        Interpreter on which the command is running.
 * @param rResult
 *        Result string the command returns as its value.
 * @param argc
 *        Count of command line parameters.  Should be 3:
 *        - pattern  - Set of paramters that should be modified.
 *        - low       - New low limit.
 *        - high      - New high limit.
 * @param argv
 *        Array of pointers to the parameter strings.
 * 
 */
int 
CTreeParameterCommand::SetLimits(CTCLInterpreter& rInterp, CTCLResult& rResult, 
				     int argc, char** argv)
{

  if(argc != 3) {
     rResult = "Incorrect number of parameters on command line\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  string    pattern(argv[0]);
  DFloat_t  low;
  DFloat_t  high;
  
  int index;
  try {
      index = 1;
      low = rInterp.ExprDouble(argv[index]);
      index = 2;
      high = rInterp.ExprDouble(argv[index]);
  }
  catch (...) {
  
     return TypeSafeParseFailed(rResult, argv[index], "double");
  }

  if (low > high) {
    rResult = " Low limit cannot be > high limit.  Low = ";
    rResult += argv[1];
    rResult += " high = ";
    rResult += argv[2];
    rResult += "\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  SetLimitsVisitor visitor(pattern, low, high);
  for_each(CTreeParameter::begin(), CTreeParameter::end(), visitor);

  return TCL_OK;

}


/**
 * Returns true if the first matching parameter has been  modified
 * @param rInterp
 *        Interpreter that's running this command.
 * @param rResult
 *        The result string that is returned by the command as the
 *        command value.
 * @param argc
 *        The count of remaining command line parameters.  This must be 1
 *        and is expected to be the name of a parameter to check.  The assumption is that
 *        all tree parameters with the same name are coherently managed, since they point
 *        to the same underlying parameter.
 * @param argv
 *        array of pointers to the command parameter(s).
 * 
 */
int 
CTreeParameterCommand::Check(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			     int argc, char** argv)
{

  //// Validate parameter count
  //
  if(argc != 1) {
    rResult    = "Incorrect number of command line parameters\n";
    rResult   += Usage();
    return TCL_ERROR;
  }
  //
  // Get the parameter that matches the name or fail..
  
  string name(argv[0]);
  multimap<string, CTreeParameter*>::iterator p = CTreeParameter::find(name);
  
  if(p == CTreeParameter::end()) {
    rResult = "Could not find parameter ";
    
    rResult += name;
    return TCL_ERROR;
  }
  
  // Figure out the return  value.
  
  CTreeParameter* pParam = p->second;
  rResult = pParam->hasChanged() ? "1" : "0";
  return TCL_OK;


}


/**
 * Resets the modified flag of all matching parameters.
 * @param rInterp   
 *        Interpreter that's executing this command.
 * @param rResult
 *        Result string.
 * @param argc
 *        Count of remaining command psrameters.
 * @param argv
 *        Parameter arguments.
 * 
 */
int 
CTreeParameterCommand::UnCheck(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			       int argc, char** argv)
{
  
  if (argc != 1) {
    rResult   = "Incorrect number of parameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  UncheckVisitor visitor(argv[0]);
  for_each(CTreeParameter::begin(), CTreeParameter::end(), visitor);
  return TCL_OK;
}


/**
 * Sets the result string to the tree parameter version string, at present
 * @param rInterp
 * @param rResult
 *        Result string that this command returns.
 * @param argc
 * @param argv
 * 
 */
int 
CTreeParameterCommand::Version(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			       int argc, char** argv)
{
  if(argc != 0) {
    rResult = "Incorrect number of command paramters";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  rResult = CTreeParameter::TreeParameterVersion;
  return TCL_OK;
}
/**
 *   Creates a new tree parameter.  The tree parameter is created
 *   and bound to the true parameter space.  This will make the 
 *   tree parameter either correspond to an existing parameter or 
 *   create a new one.
 *   The remaining command line parameters must be:
 *   - name    - The name of the new tree parameter.
 *   - low     - The default low limit of the parameter on spectrum axes.
 *   - high    - The default high limit of the parameter on spectrum axes.
 *   - bins    - The default number of channesl for the parameter on spectrum axes.
 *   - units   - The units of measure of the parameter. 
 *
 * @param rInterp  - The interprereter on which the command is executing
 * @param rResult  - The result to be returned; on success, this is the
 *                   name of the new parameter.  On failure, an error  message.
 * @param argc     - Number of parameters remaining on the command line
 *                   following the -create switch.
 * @param argv     - The remaining command parameters (see above).
 * \return int
 * \retval TCL_OK  - If the command succeeded in creating and binding a new
 *                   tree parameter.
 * \retval TCL_ERROR - if the command failed.
 */
int
CTreeParameterCommand::Create(CTCLInterpreter& rInterp, CTCLResult& rResult,
			      int argc, char** argv)
{
  if (argc != 5) {
    rResult = "Insufficient command parameters for -create\n";
    rResult += Usage();
    return TCL_ERROR;
  }

  double low, high;
  long   bins;
  string name, units;
  int index = 1;

  // Refuse to allow a duplicate name:

  name = argv[0];
  if (CTreeParameter::find(name) != CTreeParameter::end()) {
    rResult = name;
    rResult += " is already a treeparameter.  Duplicates are not allowed";
    return TCL_ERROR;
  }

  // Parse the floats (low, high)

  try {
    low  = rInterp.ExprDouble(argv[index]);
    index++;
    high = rInterp.ExprDouble(argv[index]); 
  }
  catch (...) {
    return TypeSafeParseFailed(rResult, argv[index], "double");
  }
  // Parse the long (channel count).

  try {
    bins = rInterp.ExprLong(argv[3]);
  } 
  catch (...) {
    return TypeSafeParseFailed(rResult, argv[3], "long");
  }
  // get the unts:

  units = argv[4];

  // Now create the tree parameter:
  
  CTreeParameter* pParam = new CTreeParameter;
  pParam->Initialize(name, bins, low, high, units);
  pParam->Bind();
  pParam->setUnit(units);	// Since bind may unset them.
  
  rResult = name;
  return TCL_OK;

}
/**
 * Called when a type safe parse failed
 * @param result
 *        TCL result string we will fill in.
 * @param parameter
 *        The command parameter we were trying to parse.
 * @param expectedType
 *        The parameter type we expected to get.
 * 
 */
int 
CTreeParameterCommand::TypeSafeParseFailed(CTCLResult& rResult,
					   std::string parameter, 
					   std::string expectedType)
{

  rResult += "Could not parse ";
  rResult +=   parameter;
  rResult += " as type ";
  rResult += expectedType;
  rResult += "\n";
  rResult += Usage();
  return TCL_ERROR;
  
}



