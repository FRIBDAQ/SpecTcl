#include "CUnpackerCommand.h"    				
#include "CModuleDictionary.h"
#include "CUnpacker.h"
#include "CModule.h"
#include <algorithm>
// Internal classes:

class UnpackerListVisitor {	// for_each visitor to build up module list.
    CTCLResult& m_rResult;
public:
    UnpackerListVisitor(CTCLResult& rResult) :
	m_rResult(rResult) {}
    void operator()(CModule* pModule) {
	m_rResult.AppendElement(pModule->getName());
    }
};

// Implementation functions

/*!
	Construct an unpacker.  This involved:
	- Constructing our base class.
	- Binding the member references to actual objects.
	- Registering our command.
	\param rCommand (const string& [in]): The name of the command that this
		object will be registered as.
	\param  rInterp (CTCLInterpeter&  [in]): The interpreter that this
	    command will be registered on.  It is possible to register a command on more
	    than one interpreter but not necessary.
	\param rDictionary (CModuleDictionary& [in]): reference to the module dictionary.
	    The module dictionary contains the set of modules that have been created,
	    either at startup time or dynamically via Tcl commands.
	\param rUnpacker (CUnpacker& [io]):  Reference to the unpacker.  The unpacker
	    is what this object configures, by adding an ordered set of modules that will
	    be used to decode events being analyzed by SpecTcl.
*/
CUnpackerCommand::CUnpackerCommand (const string&        rCommand,
				    CTCLInterpreter&    rInterp,
				    CModuleDictionary& rDictionary,
				    CUnpacker&           rUnpacker) :
  CTCLProcessor(rCommand, &rInterp),
  m_rModules(rDictionary),
  m_rUnpacker(rUnpacker)
  
{

  Register();
} 

/*!
   Destructor is a no-op.
*/
 CUnpackerCommand::~CUnpackerCommand ( )  
{
}

// Functions for class CUnpackerCommand

/*!  

Gets control when the unpacker command is
recognized by the Tcl interpreter. This function
dispatches control to one of:
- AddUnpacker - if the add subcommand is recognized.
- RemoveUnpacker  - if the remove subcommand is recognized.
- ListUnpackers lists the modules that make up the unpacker
   in unpacking order.
\param rInterp (CTCLInterpreter& [in]):  The interpreter that is executing this command.
\param rResult (CTCLResult& [out]):  The result string that will record any command
    result or error messages.
\param argc (int [in]): The number of command line parameters.  Note that the first
    parameter is the command itself.
\param argv (char** [in]): The array of pointers to the parameters.  The first of these
    (argv[0]) points to the command keyword itself.
\return One of:
    - TCL_OK 		- The command worked.
    - TCL_ERROR	- The command failed.

*/
int 
CUnpackerCommand::operator()(CTCLInterpreter& rInterp, 
					      CTCLResult& rResult, 
					      int  argc, char** argv)  
{ 
    int nStatus(TCL_OK);
    argc--;
    argv++;
    if(argc >=1 ) {                      // Need the command keyword.
	string command(*argv);  	// Extract the subcommand.
	argc--;
	argv++;
	if        (command == string("add")) {
	    nStatus = AddUnpacker(rInterp, rResult, argc, argv);
	}
	else if (command == string("remove")) {
	    nStatus = RemoveUnpacker(rInterp, rResult, argc, argv);
	}
	else if (command == string("list")) {
	    nStatus = ListUnpackers(rInterp, rResult, argc, argv);
	}
	else {                                // Invalid keyword.
	    rResult   = string ("unpack - invalid subcommand keyword ");
	    rResult  += command;
	    rResult  += "\n";
	    rResult  += Usage();

	    nStatus = TCL_ERROR;
	}
    }
    else {
	rResult   =  string("unpack did not hagve enough parameters\n");
	rResult  +=  Usage();
	nStatus = TCL_ERROR;
    }
    return nStatus;
}  

/*!  

Adds an module to the end of the unpacker's module list.
The added module is now the last one looked for in the event
-  There must be an additional command keyword.
- The keyword must be the name of a module in the module dictionary
- The module must not already be in the unpacker.

\param rInterp (CTCLInterpreter& [in]): The interpreter that is running this command.
\param rResult (CTCLResult& [out]):  The result strin ghat will be filled in with 
	an error description if one is detected.
\param argc (int [in]):  Number of parameters remaining on the command line after
	the subcommand keyword.
\param argv(char** [in]): The parameters remaining on the command line.

\return One of:
	- TCL_OK		- Iff the command succeeded.
	- TCL_ERROR	- If the command exits in an error.

*/
int 
CUnpackerCommand::AddUnpacker(CTCLInterpreter& rInterp, 
					       CTCLResult& rResult, 
					       int argc, char** argv)  
{ 
    int nStatus(TCL_OK);
    
    if (argc == 1) {		//Only need a module name.
	string Name(*argv);
	
	// Find the module in the dictionary:
	CModuleDictionary::ModuleIterator i = m_rModules.Find(Name);
	if ( i != m_rModules.end()) {     // Name matches a module.
	    CModule* pModule = i->second;
	    CUnpacker::UnpackerIterator j = m_rUnpacker.Find(Name);
	    if (j == m_rUnpacker.end()) { // notyet in the unpacker:
		m_rUnpacker.Add(*pModule); // Insert module in unpacker.
	    }
	    else {
		rResult   = string("unpacker add : Module is already in the ");
		rResult  +="unpacker object\n";
		rResult  += Usage();
		nStatus = TCL_ERROR;
		
	    }
	}
	else {
	    rResult      = string("module add: the module specified: ");
	    rResult     += Name;
	    rResult     += " does not exist\n";
	    rResult     += Usage();
	    nStatus = TCL_ERROR;
	}
	
    }
    else {
	rResult    = string("unpack add only requires a single module\n");
	rResult   += Usage();
	nStatus = TCL_ERROR;
    }
    
    return nStatus;
}  

/*!  

Removes a module from the list
of unpackers.
    - There must be a single parameter that is the name of the item to remove
    - The parameter must be the name of a module in the module dictionary.
    - If the parameter is in the Readout list, it is removed from there.
    
    \param rInterp (CTCLInterpreter& [in]): The interpreter object on which the
	command is running.
    \param rResult (CTCLResult& [out]): The result string that will be set with any
	error message required.
    \param argc (int [in]): The number of parameter remaining on the command line after
	the subcommand keyword.
    \param argv (char** [in]): The parameters themselves.
    
    \int Status of the command:
    - TCL_OK		- the command succeeded.
    - TCL_ERROR	- The command failed.
    
*/
int 
CUnpackerCommand::RemoveUnpacker(CTCLInterpreter& rInterp, 
						     CTCLResult& rResult, 
						     int argc, char** argv)  
{ 
    int nStatus(TCL_OK);
    
    if(argc == 1) {			// There must be exactly one more param.
	string Name(*argv);		// Extract the module name.
	CUnpacker::UnpackerIterator ui = m_rUnpacker.Find(Name);
	if(ui != m_rUnpacker.end()) {  // Remove from the unpacker if necessary.
	  m_rUnpacker.Delete(ui);
	}
	else {
	  rResult   = string("unpacker remove : module :");
	  rResult  += Name;
	  rResult  += " does not exist\n";
	  nStatus = TCL_ERROR;
	}
    }
    else {
      rResult    = string("Need  a module to delete \n");
      rResult   += Usage();
      nStatus = TCL_ERROR;
    }

    return nStatus;
}  

/*!  

Lists the set of modules that have been
entered in the unpacker .  The modules are
listed in unpack order.
- There must be no command parameters.

\param rInterp (CTCLInterpreter& [in]): The interpreter that is running this command.
\param rResult (CTCLResult& [out]): The result string that will be filled in with
    either:
    - The list of modules in the unpacker or
    - An error message if an error was detected.
\param argc (int [in]): Must be 0: the number of remaining parameters on the line
    after the list keyword.
\param argv (char** [in]): the parameters on the line.

\return One of:
    - TCL_OK		- If the command completed normally.
    - TCL_ERROR	- If an error was detected.

*/
int 
CUnpackerCommand::ListUnpackers(CTCLInterpreter& rInterp, 
						CTCLResult& rResult, 
						int argc, char** argv)  
{ 
    int nStatus(TCL_OK);
    
    if(argc == 0) {
	//  The for_each iteration algorithm along with an UnpackerListVisitor is
	// used to build up the output list:
	
	for_each(m_rUnpacker.begin(), m_rUnpacker.end(),
		    UnpackerListVisitor(rResult));
    }
    else {
	rResult   = string("module delete : Extra parameters after command\n");
	rResult  += Usage();
	nStatus = TCL_ERROR;
    }
    
    return nStatus;
}  

/*!
  Returns a string describing the command usage:

*/
string
CUnpackerCommand::Usage()
{
    string Result("Usage:\n");
    Result    += "      ";
    Result    +=         getCommandName();
    Result    +=         " add modulename\n";
    Result    += "      ";
    Result    +=         getCommandName();
    Result    +=        " remove modulename\n";
    Result    += "      ";
    Result    +=         getCommandName();
    Result    +=         " list\n";
    
    return Result;
}
