   
///////////////////////// FILE_NAME.cpp /////////////////////////////////////////////////////
#include <config.h>
#include "CStringConfigParam.h"    	
#include <TCLInterpreter.h>			
#include <TCLResult.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
	Construct a string valued configuration parameter.
	\param key (const string& [in]) The keyword that identifies
		this parameter.
*/
CStringConfigParam::CStringConfigParam (const string& rKey) :
   CConfigurationParameter(rKey) 
{   
    
} 
/*!
	Destructor:  In this case it's a no-op.
*/
 CStringConfigParam::~CStringConfigParam ( )
{
}
/*!
	Copy constructor. All we need to do is invoke the base class copy constructor.
*/
CStringConfigParam::CStringConfigParam (const CStringConfigParam& rhs)
  : CConfigurationParameter (rhs) 
{
} 
/*!
	Assignment. The base class assignment operator does all the work.
*/
CStringConfigParam& 
CStringConfigParam::operator= (const CStringConfigParam& rhs)
{ 

    if (&rhs == this) {
	CConfigurationParameter::operator=(rhs);
    }
    return *this;
}
/*!
    Equality comparison.   Base class does all the work.
*/
int 
CStringConfigParam::operator== (const CStringConfigParam& rhs) const
{ 
    return CConfigurationParameter::operator==(rhs);
}

// Functions for class CStringConfigParam

/*! 

Returns the current value of the string.
For a string valued parameter, we just get the value
stored by the base class.

*/
string 
CStringConfigParam::getOptionValue()  
{ 
    return getValue();
}  

/*!  

Sets the value of the actual parameter from the 
stringified param (no-op here since we use the base
class's string value.

*/
int 
CStringConfigParam::SetValue(CTCLInterpreter& rInterp, 
			     CTCLResult& rResult, 
			     const char* value)  
{ 
    return TCL_OK;
}
/*!
    Returns the format of the configuration parameter.
    In this case it's the text "string"
*/
string
CStringConfigParam::GetParameterFormat()
{
    return string("string");
}
