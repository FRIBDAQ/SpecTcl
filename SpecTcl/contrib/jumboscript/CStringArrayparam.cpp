////////////////////////// FILE_NAME.cpp /////////////////////////////////////////////////////
#include <config.h>
#include "CStringArrayparam.h"    				
#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <TCLList.h>
#include <vector>
#include <RangeError.h>
#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/*!
	Construct a string array parameter. 
	\param rKey (const string& [in]): The name of the parameter.
	\param nSize (int [in]): Size of the array.
*/
CStringArrayparam::CStringArrayparam (const string& rKey, int nSize)
   : CConfigurationParameter(rKey), 
   m_aStrings(0),
   m_nSize(nSize)
{   
    m_aStrings = new string[m_nSize];
} 
/*!
    Destroy a string array parameter.
    The array needs to be deleted.
*/
 CStringArrayparam::~CStringArrayparam ( )  //Destructor - Delete dynamic objects
{
    delete []m_aStrings;
}
/*!
    Copy construction.  The big work is copying the 
    string array.
*/
CStringArrayparam::CStringArrayparam (const CStringArrayparam& rhs ) 
  : CConfigurationParameter (rhs),
  m_nSize(rhs.m_nSize)
 
{
    m_aStrings = new string[m_nSize];
    for(int i =0; i < m_nSize; i++) {
	m_aStrings[i] = rhs.m_aStrings[i];
    }
} 
/*!
    Assignment operator. The main work is deleting any existing string array,
    allocating a new one and copying the rhs's string array in.
    
*/
CStringArrayparam& 
CStringArrayparam::operator= (const CStringArrayparam& rhs)
{ 
    if (this == &rhs) {
	delete []m_aStrings;
	m_nSize     = rhs.m_nSize;
	m_aStrings = new string[m_nSize]; 
    }
    return *this;
}
/*!
    Equality comparison.  Two parameters are equal if and only if:
    - The keywords are the same (base class compares).
    - The size of the two string arrays is equal.
    - The string arrays are element for element equal.
    
*/
int 
CStringArrayparam::operator== (const CStringArrayparam& rhs) const
{ 

    if(!CConfigurationParameter::operator==(rhs)) return 0;
    if(m_nSize != rhs.m_nSize) return 0;
    for(int i =0; i < m_nSize; i++) {
	if(m_aStrings[i] != rhs.m_aStrings[i]) return 0;
    }
    return 1;                      // Equality!.
}

// Functions for class CStringArrayparam

/*!  

Returns the contents of the
string indexed by the parameter or throws
a CRangeError exception if the index is invalid.

*/
string& 
CStringArrayparam::operator[](int n)  
{
     if((n < 0) || (n >= m_nSize)) {
       CRangeError r(0, m_nSize-1, n,
		     string("CStringArrayParam::operator[] bad index"));
       throw r;
     }
     return m_aStrings[n]; 
}  

/*!  
    
Purpose: 	

Sets the value as parsed from the command argument.
The argument is assumed to be a list of strings to be put into the
array elements
\param rInterp (CTCLInterpreter&  [in]) The interpreter running the config
    command
\param rResult (CTCLResult& [out]) The result string that will be filled in.
\param pValue (char* [in]) A C-string that is a correctly formatted Tcl list.

*/
int 
CStringArrayparam::SetValue(CTCLInterpreter& rInterp, 
			    CTCLResult& rResult, 
			    const char* pValue)
{ 
    // First throw things up into a list and then split the list into a string vector:
    
    StringArray Values;
    try {
	CTCLList ValueList(&rInterp, pValue);
	ValueList.Split(Values);
    }
    catch(...) {
	rResult += "Configuration string array: ";
	rResult += getSwitch();
	rResult += " configuration list was badly formatted.";
	rResult += pValue;
	return TCL_ERROR;
    }
    
    // The size of the vector should be the same as the size of the
    // m_aStrings array:
    
    if(Values.size() != m_nSize) {
	rResult += "Configuration string array: ";
	rResult += " Size mismatch between the parameter list and \n";
	rResult +=  "the list. ";
	rResult += getSwitch();
	return TCL_ERROR;
    }
	
    // Copy the string vector into the string array:
    
    for(int i =0; i < m_nSize; i++) {
	m_aStrings[i] = Values[i];
    }
    return TCL_OK;
}
/*!
  Get the format string for the parameter which is :
  string[m_nSize]
  */
string
CStringArrayparam::GetParameterFormat()
{
    char result[100];
    sprintf(result, "string[%d]", m_nSize);
    return string(result);
}
