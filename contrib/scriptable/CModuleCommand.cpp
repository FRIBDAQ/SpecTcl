#include "CModule.h"
#include "CModuleCommand.h"    				
#include "CModuleDictionary.h"
#include "CUnpacker.h"
#include "CModuleCreator.h"
#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <string>
#include <algorithm>
#include <TCLString.h>
// Local classes:

// Predicate to visit modules in the module dictionary and create the 
// list of modules.
class ListVisitor {
  string         m_Pattern;
  CTCLResult& m_rResult;
public:
	ListVisitor(const string& rPattern, CTCLResult& rResult) :
		m_Pattern(rPattern), 
		m_rResult(rResult) {}
	void operator()(pair<string,CModule*> p) {
	  CTCLString entry;
	  if(Tcl_StringMatch(p.first.c_str(), m_Pattern.c_str())) {
	    CModule* pModule = p.second;
	    entry.AppendElement(pModule->getName());
	    entry.AppendElement(pModule->getType());
	    m_rResult.AppendElement((const char*)entry);
	    
	  }
	}
};


// Predicate to visit module creators and create the types list.

class TypesVisitor {
    CTCLResult& m_rResult;
public:
    TypesVisitor(CTCLResult& rResult) :
	m_rResult(rResult) {}
    void operator()(pair<string, CModuleCreator*> item)
    {
	string info(item.first);
	info   += "\t - ";
	info   += item.second->TypeInfo();
	m_rResult.AppendElement((const string)info);
    }
};
// Class implementation.

/*!
  Constructs the module command object.  This involves;
  - Constructing ourselves as a CTCLProcessor,
  - Initializing the references for the unpacker and module dictionary
     from the parameters.
  - Registering our command with the interpreter.
  
  \param rInterp (CTCLInterpreter& [in]):  The interpreter on which this command
		  should be registered.
  \param rCommand (const string& [in]):  The command keyword that will be registered.
  \param rUnpacker (CUnpacker& [in]):    Reference to the unpacker.
  \param rDict (CModuleDictionary& [in]): Reference to the module dictionary.
*/
CModuleCommand::CModuleCommand (CTCLInterpreter& rInterp,
						   const string&      rCommand,
						   CUnpacker&        rUnpacker,
						   CModuleDictionary& rDict) :
    CTCLProcessor(rCommand, &rInterp),
    m_rUnpacker(rUnpacker),
    m_rModules(rDict)
 
{
  Bind(&rInterp);
  Register();
} 
/*!
   Desctruction is only  requires the invocation of the base class
   destructor to unregister our command.  This is done automatically
   via virtual contructor chaining.
*/
CModuleCommand::~CModuleCommand ( ) 
{
}

// Functions for class CModuleCommand

/*!  Function: 	

Invoked to execute the module command.
This function decodes the first parameter,
which is assumed to be a subcommand.
The subcommands dispatch as follows
- create  - invokes the Create() member to 
   create a new module.
- delete - Invokes the Destroy() member to 
   remove the module from the unpacker,
   the module dictionary and to destroy the
   module object.
- list - Invokes the List member to create a list
  of modules whose names match a glob pattern.
- types - invokes the ListTypes member function.

\param rInterp (CTCLInterpreter& [in]): The interpreter that is running this command.
	Note that it is theoretically possible for a command to be registered on multiple 
	interpreters.
\param rResult (CTCLResult& [out]): Will contain the string result of the command.
	This will either be something useful to other commands or an error message.
\param argc (int [in]): Number of parameters on the command line.  Note that
	the command keyword counts as a parameter.
\param pArgv (char** [in]): The parameters.

\return One of:
	- TCL_OK if the command worked.
	- TCL_ERROR if an error was detected.
*/
int 
CModuleCommand::operator()(CTCLInterpreter&  rInterp, 
					  CTCLResult& rResult, 
					  int argc, char** pArgv)  
{ 
	int nStatus(TCL_OK);
	argc--; 
	pArgv++; 
	
	if(argc) {                           // There must be a subcommand keyword
		string Command(*pArgv);  // Extract the command.
		argc--;
		pArgv++;
		
		// Dispatch on the command keyword:
		
		if       (Command == string("-create")) {  // Make a module.
			nStatus = Create(rInterp, rResult, argc, pArgv);
		}
		else if (Command == string("-list")) {    // List existing modules.
			nStatus = List(rInterp, rResult, argc, pArgv);
		}
		else if (Command == string("-types")) { // List module types:
			nStatus = ListTypes(rInterp, rResult, argc, pArgv);
		}
		else if (Command == string("-delete")) {
			nStatus = Destroy(rInterp, rResult, argc, pArgv);
		}
		else {		// Asssume create:
		  argc++;
		  pArgv--;
		  nStatus = Create(rInterp, rResult, argc, pArgv);
		}
	} 
	else {                               // No subcommand keyword
		rResult = Usage();
		nStatus = TCL_ERROR;
	}
	
	return nStatus;
}  

/*!  Function: 	
   int Create(CTCLInterpreter& rInterp, CTCLResult& rResult, intq argc, char** pArgv) 
 Operation Type:
    
Purpose: 	

Invoked to create a new module:
- The Set of creators is iterated through
   until one claims to match.
- The matching creator's create member is
   invoked to create a new instance of the
   module.
- The new module's Configure member is called
  to eat up the remaining command parameters and
  configure the module for use.
- The module is entered in the module dictionary.

\param rInterp (CTCLInterpreter& [in]): The interpreter that is running this command.
	Note that it is theoretically possible for a command to be registered on multiple 
	interpreters.
\param rResult (CTCLResult& [out]): Will contain the string result of the command.
	This will either be something useful to other commands or an error message.
\param argc (int [in]): Number of parameters on the command line.  This and pArgv
	are positioned to the first parameter after the create keyword.
\param pArgv (char** [in]): The parameters.

\return One of:
	- TCL_OK if the command worked.
	- TCL_ERROR if an error was detected.

*/
int 
CModuleCommand::Create(CTCLInterpreter& rInterp, CTCLResult& rResult,
				    int argc, char** pArgv)  
{ 
	int nStatus(TCL_OK);
	
	// We need at least a module name and module type.
	
	if(argc >= 2) {
	  string Name(*pArgv);
	  string Type(pArgv[1]);
	  argc    -= 2;
	  pArgv   += 2;
	  
	  // Locate the creator:
	  
	  CreatorIterator iCreator = m_Creators.find(Type);
	  if(iCreator != m_Creators.end()) {
	    CModuleCreator* pCreator = iCreator->second;
	    
	    // Create and configure the module:
	    
	    CModule* pModule = pCreator->Create(rInterp, Name);
	    if(pModule) {
	      if(argc) {
		nStatus = pModule->Configure(rInterp,
					     rResult,
					     argc, pArgv);
	      }
	      // add the module to the module dictionary:
	      
	      m_rModules.Add(pModule);
	    }
	    else {          // Module creation failed.
	      rResult += "Failed to create a module named ";
	      rResult += Name;
	      rResult += " of type ";
	      rResult += Type;
	      rResult += "\n";
	      nStatus = TCL_ERROR;
	    }
	  }
	  else {                   // No match for module.
	    rResult += "Unrecoginzed module type: ";
	    rResult += Type;
	    rResult +=  "\n";
	    rResult += Usage();
	    nStatus = TCL_ERROR;
	  }
	}
	else {                            // Insufficient parameters on command line.
	  rResult += "module creation requires at least a ";
	  rResult += " module name and type\n";
	  rResult += Usage();
	  nStatus = TCL_ERROR;
	}
	return nStatus;
}  

/*!  

Destroys a single module:
- If the module is in the unpacker,
  it is removed from the unpacker.
- The module is removed from the
  module dictionary.
- The module object is deleted.

\param rInterp (CTCLInterpreter& [in]): The interpreter that is running this command.
	Note that it is theoretically possible for a command to be registered on multiple 
	interpreters.
\param rResult (CTCLResult& [out]): Will contain the string result of the command.
	This will either be something useful to other commands or an error message.
\param argc (int [in]): Number of parameters on the command line.  Note that
	both argc and pArgv are positioned past the delete keyword.
\param pArgv (char** [in]): The parameters.

\return One of:
	- TCL_OK if the command worked.
	- TCL_ERROR if an error was detected.

*/
int 
CModuleCommand::Destroy(CTCLInterpreter& rInterp, CTCLResult& rResult, 
				     int argc, char** pArgv)  
{ 
  int nStatus(TCL_OK);
  
  // I need exactly one parameter: The name of the module to destroy.
  
  if(argc == 1) {
    
    string Name(*pArgv);
    // Locate and remove the module from the dictionary:
    
    CModuleDictionary::ModuleIterator iModule(m_rModules.Find(Name));
    if(iModule != m_rModules.end()) {
      // Remove the module from the dictionary.
      
      CModule* pModule= iModule->second;
      m_rModules.Delete(iModule);
      
      // If the module is in the unpacker, remove it.
       
      CUnpacker::UnpackerIterator iUnpack = m_rUnpacker.Find(Name);
      if(iUnpack != m_rUnpacker.end()) {
	m_rUnpacker.Delete(iUnpack);
      }
     
      // Delete the module.
      
      delete pModule;
    }
    else {
      rResult += "Module deletion failed to find module ";
      rResult += Name;
      rResult += " in module dictionary\n";
      rResult += Usage();
      nStatus = TCL_ERROR;
    }
  }
  else {				// Incorrect no of parameters.
    rResult += "module delete parameter count invalid\n";
    rResult += Usage();
    nStatus = TCL_ERROR;
  }
  return nStatus;
}  

/*! 

Lists the set of modules in the module dictionary
whose names match the optional glob pattern.
If the pattern is omittted, it defaults to * which causes
the entire module dictionary to be listed.
\param rInterp (CTCLInterpreter& [in]): The interpreter that is running this command.
	Note that it is theoretically possible for a command to be registered on multiple 
	interpreters.
\param rResult (CTCLResult& [out]): Will contain the string result of the command.
	This will either be something useful to other commands or an error message.
\param argc (int [in]): Number of parameters on the command line.  Both argc and
	pArgv are pointed after the list keyword.
\param pArgv (char** [in]): The parameters.

\return One of:
	- TCL_OK if the command worked.
	- TCL_ERROR if an error was detected.

*/
int 
CModuleCommand::List(CTCLInterpreter& rInterp, CTCLResult& rResult, 
				int argc, char** pArgv)  
{ 
	int nStatus(TCL_OK);
	string SearchPattern("*");
	
	// We need 0 or 1 parameters... 1 parameter will be  a pattern to look for,
	// 0 will imply a search pattern of *
	
	if((argc == 0) || (argc == 1)) {
		if(argc == 1) SearchPattern   = *pArgv;
		for_each(m_rModules.begin(), m_rModules.end(),
			     ListVisitor(SearchPattern, rResult));
	}
	else {
		rResult += "Invalid number of parameters for the ";
		rResult += "module list subcommand\n";
		rResult += Usage();
		nStatus = TCL_ERROR;
	}
	return nStatus;
}  

/*!  Function: 	
   
Returns the command usage.

*/
string 
CModuleCommand::Usage()  
{ 
	string Result("Usage:\n");
	       Result += "    ";
	       Result +=      getCommandName();
	       Result +=      " [-create] name type ?config-pairs?\n";
	
	       Result += "   ";
	       Result +=      getCommandName();
	       Result +=      " -list ?pattern?\n";
               Result += "   ";
	       Result +=      getCommandName();
               Result +=      " -delete module\n";
	       Result += "   ";
               Result +=      getCommandName();
	       Result +=      " -types\n"; 
	
	return Result;
}  

/*! 

	Lists the types of modules that can be instantiated.  This is done by
	iterating through the creators and asking them to give some type information.
	Iteration is done via for_each with a TypesIterator.
\param rInterp (CTCLInterpreter& [in]): Interpreter that was running the command.
\param rResult (CTCLResult& [out]):  The result string that will be loaded with the
	output from the visitor (the set of types that are allowed).
\param argc (int [in]): The number of parameters onthe command line.  this is
	supposed to be zero now because the parameter pointers etc. have been advanced
	past the command and subcommand keyword.
\param argv (char** [in]):  The array of parameter strings.  In this case it should be
	an empty list.

\return On of:
	- TCL_OK 	- Everything worked ok.
	- TCL_ERROR - if there was an error.
*/
int 
CModuleCommand::ListTypes(CTCLInterpreter& rInterp, 
					CTCLResult& rResult, 
					int argc, char** argv)  
{ 
    int nStatus(TCL_OK);
    if(argc == 0) {
	for_each(m_Creators.begin(), m_Creators.end(),
		    TypesVisitor(rResult));
    }
    else {
	rResult += "Too many paramers to module types \n";
	rResult += Usage();
	nStatus = TCL_ERROR;
    }
	
	return nStatus;
}
/*!
   Adds a creator to the set of recognixed modules that can be 
   created by the module command.
   \param rType (const string [in]) : string that will be recognized
        as the keyword to create a module of this type.
   \param rCreator (CModuleCreator& [in]) : reference to the module
         creator to add.
    
   \throw string exception if a recognizer has already been registered with
      this name.
*/
void
CModuleCommand::RegisterCreator(const string& rType, CModuleCreator& rCreator)
{
  CreatorIterator r = m_Creators.find(rType);
  if(r != m_Creators.end()) {
    string exception(" Attempt to register a recognizer with the keyword ");
    exception  += rType;
    exception  += " but this type already exists.";
    throw exception;
  }
  else {
    m_Creators[rType] = &rCreator;
  }
}
