#include "CModule.h"    				
#include "CConfigurationParameter.h"
#include "CBoolConfigParam.h"
#include "CIntConfigParam.h"
#include "CIntArrayParam.h"
#include "CStringArrayparam.h"
#include "CStringConfigParam.h"

#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <TCLString.h>
#include <Analyzer.h>
#include <Histogrammer.h>
#include <BufferDecoder.h>
#include <algorithm>


/// Local visitor class to build up the string of parameter name/formats.

class Visitor {
    CTCLString& m_rParams;
public:
    Visitor(CTCLString& rParams) :
	m_rParams(rParams) {}
    void operator()(pair<string, CConfigurationParameter*> p) {
	m_rParams.StartSublist();
	m_rParams.AppendElement(p.first);
	m_rParams.AppendElement(p.second->GetParameterFormat());
	m_rParams.EndSublist();
    }
};

/*!
     Constructs a module.  The module creates a command
     of the same name.
    \param rName (const string [in]): The name of the module/command.
    \param rInterp (CTCLInterpreter& [in]): The interpreter on which the
	command will be registered.

*/
CModule::CModule (const string& rName, 
			  CTCLInterpreter& rInterp) :
    CTCLProcessor(rName, &rInterp),
    m_sName(rName)
{
  Bind(&rInterp);
  Register();
} 

/*!
    Destroys the module.
*/
 CModule::~CModule ( )  //Destructor - Delete dynamic objects
{
}

// Functions for class CModule

/*!  

Executes commands associated with this module
Commands are divided into subcommands based on the
second parameter:
- config - configures the command.  The configuration
  mechanism used is identical to that of the module reader.
  There are int, bool and int array parameters and added
  to them, string parameters and string array parameters.
-list -  lists the current configuration.
- anything else is dispatched to the virtual function:
   OtherSubcommands

   \param rInterp (CTCLInterpreter& [in]): The interpreter that's running the
       command.
    \param rResult (CTCLResult& [out]): The result string:  This is either empty if
	the command worked, or alternatively an error message describing why the
	command failed.
    \param argc (int [in]): Number of parameters in the command.
    \praam argv (char** [in]): The parameters.
    \return One of:
	- TCL_OK      - If the command worked.
	- TCL_ERROR - If the command failed (rResult has a stringified reason for the
			error).
    \note The first parameter is the command verb.

*/
int 
CModule::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			      int argc, char** argv)  
{ 
    int nStatus = TCL_OK;		// Assume the command works.
    argc--; argv++;			// Skip the verb.
    
    // The next parameter, if it exists should be a subcommand keyword:
    
    if(!argc) {                              // No command keyword.
	nStatus = TCL_ERROR;
	rResult += "Insufficient command parameters:\n";
	rResult += Usage();
    }
    else {					//  dispatch based on the keyword:
	string subcommand(*argv);	// Extract the subcommand:
	if(subcommand == string("config")) {
	    argc--; argv++;
	    nStatus = Configure(rInterp, rResult, argc, argv);
	}
	else if (subcommand == string("cget")) {
	    argc--; argv++;
	    nStatus = ListConfig(rInterp, rResult, argc, argv);
	}
	else {
	    nStatus =  OtherSubcommands(rInterp, rResult, argc, argv);
	}
    }
    
    return nStatus;
}  

/*!  

    Configures the module, by taking keyword value pairs, matching the keyword
    against a configuration parameter and setting the parameter according to the
    value. 
    \param rInterp (CTCLInterpreter& [in]): The interpreter that is executing the
	command.
    \param rResult (CTCLResult& [out]): The result string that will be filled with
	any descriptive error text that may be appropriate.
    \param argc (int [in]):  Count of the remaining parameters after the "config" keyword.
    \param argv (char** [in]): Pointers to the remaining parameters after "config" keyword.
    
    \return One of:
	- TCL_OK 	- If the command worked.
	- TCL_ERROR - If the command failed.

*/
int 
CModule::Configure(CTCLInterpreter& rInterp, CTCLResult &rResult, 
			    int argc, char** argv)  
{
    int nStatus = TCL_OK;                // Assume everything works.
    
    // Parameters must come in pairs, and there must be at least one pair.
    
    if((argc % 2) || (argc == 0)) {			//  Nonzero if odd...
	nStatus = TCL_ERROR;
	rResult  = "config requires a set of keyword value pairs\n";
	rResult += Usage();
    } else { 				// At least the count is ok.
	while(argc) {
	    string key(*argv);
	    argc--; argv++;
	    
	    char* value(*argv);
	    argc--; argv++;
	
	    // See if we can find the keyword:
	
	    ConfigurationIterator i = m_Parameters.find(key);
	    if(i != m_Parameters.end()) {
		CConfigurationParameter* pParam(i->second);
		int s = (*pParam)(rInterp, rResult, value);
		if (s != TCL_OK) nStatus = s;
	    }
	    else {                              // Unrecognized configuration keyword.
		rResult += "Unrecognized configuration keyword: ";
		rResult += key;
		rResult += "ignored.\n";
		nStatus = TCL_ERROR;
	    }
	}
    }
    return nStatus;
}  

/*!  

Lists the current configuration of the
object.  The configuration is returned in the result string as a list.
Each list is itself a 2 element sublist consisting of the parameter keyword
and current value (formatted as a valid tcl list element).

\param rInterp (CTCLInterpreter& [in]): The interpreter that is running this command.
\param rResult (CTCLResult& [out]): The result string returned.  If everything works
    this is the configuration as described above.  If there's a failure, this is the error
    string.
\param argc (int [in]): The number of parameters remaining on the command line
    following the "cget' subcommand.
\param argv (char** [in]): List of pointers to the remaining parameters.
\return One of:
    - TCL_OK		- On success.
    - TCL_ERROR 	- on failure.

*/
int 
CModule::ListConfig(CTCLInterpreter& rInterp, CTCLResult& rResult, int argc, char** argv)  
{ 
    int nStatus = TCL_OK;
    
    // There should be no remaining parameters:
    
    if(argc) {
	rResult += "Excessive parameters on the command line\n";
	rResult += Usage();
	nStatus = TCL_ERROR;
    }
    else {
	CTCLString configList;
	ConfigurationIterator i = m_Parameters.begin();
	while(i != end()) {
	  CConfigurationParameter* pParam(i->second);
	  configList.StartSublist();
	  configList.AppendElement(pParam->getSwitch());
	  configList.AppendElement(pParam->getValue());
	  configList.EndSublist();
	  i++;
	}
	rResult = (const char*)configList;
    }
    return nStatus;
}  

/*!  

Called when an unrecognized subcommand
is supplied.  This is a hook to extend the 
command processing of the module to support
user specirfic requirements.

The default action is to return an error... since the keyword is not recognized.
\param rInterp (CTCLInterpreter& [in]): The interpreter that's running the command.
\param rResult (CTCLResult& [out]): The result string -- an error message.
\param argc (int [in]): Count of remaining command line parameters.  The
    next parameter is the subcommand keyword.
\param argv (char** [in]): Pointers to the remaining parameters.
\return TCL_ERROR - since this should not be called except when the command
    keyword is not recognized.
*/
int 
CModule::OtherSubcommands(CTCLInterpreter& rInterp, 
					CTCLResult& rResult, 
					int argc, char** argv)  
{
    rResult += "Unrecognized subcommand keyword: ";
    rResult += *argv;
    rResult += "\n";
    return TCL_ERROR;
}  



/*! 

Add an integer configuration parameter
to the module.  
\param rName (const string& [in]): The name of the configuration parameter.
\param default (int [in] = 0): The default value of the parameter if none is asssigned.
\return CIntConfigParam* - pointer to the new configuration parameter.
\ 
*/
CIntConfigParam* 
CModule::AddIntParam(const string& rName, int def)
{
    CIntConfigParam* pParam = new CIntConfigParam(rName, def);
    AddParameter(pParam);
    return pParam;
}  

/*!  

Adds a boolean config parameter 
to the configuration list.  
\param rName (const string & [in]): The name of the parameter.
\param default (bool [in] = false): Default value of the parameter.
\return CBoolConfigParam* - Pointer to the newly created parame4ter.

*/
CBoolConfigParam*
CModule::AddBoolParam(const string& rName, bool def)
{ 
    CBoolConfigParam* pParam = new CBoolConfigParam(rName, def);
    AddParameter( pParam);
    
    return pParam;
}  

/*!  

Add an integer array parameter to the configuration 
parameter set.
\param rName (const string & [in]): The name of the parameter.
\param nSize  (int [in]):  The size of the array.
\param default (int [in] = 0): The initial value given to all of the array elements.
\return CIntArrayParam*  - pointer to the newly created parameter.
*/
CIntArrayParam*
CModule::AddIntArrayParam(const string& rName, int nSize, int def)  
{ 
    CIntArrayParam* pParam = new CIntArrayParam(rName, nSize, def);
    AddParameter(pParam);
    
    return pParam;
}  

/*!  

Adds a string valued configuration
parameter to the config set.
\param rName (const string& [in]) - name of the parameter.
\return  A pointer to the newly created parameter.

*/
CStringConfigParam*
CModule::AddStringParam(const string& rName)  
{ 
    CStringConfigParam* pParam = new CStringConfigParam(rName);
    AddParameter(pParam);
    
    return pParam;
}  

/*!  

Adds a string array valued configuration
parameter to the module's parameter set.
\param rName (const string& [in]): The name of the parameter.
\param nSize (int [in]): The number of the elements in the array.
\return A pointer to the newly created parameter.
*/
CStringArrayparam*
CModule::AddStringArrayParam(const string& rName, int nSize)
{ 
    CStringArrayparam* pParam = new CStringArrayparam(rName, nSize);
    AddParameter(pParam);
    
    return pParam;
}  

/*!
    Adds a parameter to the parameter list.  While this can be used
    from the outside, it is also used internally to add parameters that have
    been created by the specific parameter add functions.
    \param pParam (CConfigurationParameter* [in]):  Pointer to the parameter to add.
*/
void
CModule::AddParameter(CConfigurationParameter* pParam)
{
    string key(pParam->getSwitch());
    if (m_Parameters.find(key) != m_Parameters.end()) {
	string error("Duplicate parameter name: ");
	error += key;
	throw error;
    }
    else {
	m_Parameters[key] = pParam;
    }
}
/*!  

Returns the command usage.
Note that all of the parameter keywords are listed along with their formats
We use for_each to do this.

*/
string 
CModule::Usage()  
{ 
    string u;
    CTCLString params;
    Visitor      v(params);           // Visits each parameter in the map.
    for_each(m_Parameters.begin(), m_Parameters.end(), v);
    
    u += "Usage: \n";
    u += "    ";
    u +=       getName();
    u += " config ";
    u +=     string((const char*)params);
    u += "\n";
    u += "    ";
    u +=       getName();
    u += " cget\n";
    
    return u;
}
/*!
   Returns a begin iterator for the parameter map:
*/
CModule::Configuration::iterator
CModule::begin()
{
    return m_Parameters.begin();
}
/*!
  Returns an end of iteration iterator for the parameter map:
*/
CModule::Configuration::iterator
CModule::end()
{
    return m_Parameters.end();
}
/*!
   Returns the number of configuration parameters.
*/
int
CModule::size()
{
    return m_Parameters.size();
}
