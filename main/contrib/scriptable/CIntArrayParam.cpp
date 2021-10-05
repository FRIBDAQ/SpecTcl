/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:   CIntArrayParam.cpp 
 *  @brief:  Implement the integer array configurable parameter.
 *
 */
static const char* Copyright = "(C) Copyright Ron Fox 2002, All rights reserved";
/*! \class CIntArrayParam   
           Encapsulates a parameter that is aray valued.  An example of an array
           valued parameter is a set of pedestals one per digitizer channel.  
           The array is passed in as a welll formed TCL list, each of whose elements
           will evailuate to an integer.
*/

////////////////////////// FILE_NAME.cpp /////////////////////////////////////////////////////
#include <config.h>
#include "CIntArrayParam.h"    				
#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <TCLList.h>
#include <RangeError.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <vector>

#include <stdlib.h>
#include <errno.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
   Constructor: Constructs an array that is not range checked.
   The array is filled in from a TCL formatted list.
  \param rName const string& [in]
              The parameter name recognized by the 
              configuration parser.
  \param size unsigned int [in]
            Number of integers in the array.
  \param nDefault int [in]
            Default (initial) value of the array elements.
*/
CIntArrayParam::CIntArrayParam (const string& rName,
                                unsigned int  nSize,
                                int           nDefault) :
  CConfigurationParameter(rName),
  m_nSize(nSize),
  m_aValues(0),
  m_fCheckRange(false)
{   
  SetupArray(nDefault);
  SetupStringValue();
} 

/*!
   Constructor for a range checked array.  Range checking
  causes modification of the array to fail if the value of
  the parameter is not in the interval [m_nLow, m_nHigh].

  \param rName const string [in]
          Name of the parameter.
  \param nSize int [in]
          Number of elements in the array.
  \param nLow,nHigh int [in]
          Range limits.
  \param nDefault int [in] 
        Default value.
*/
CIntArrayParam::CIntArrayParam(const string& rName,
                               unsigned int nSize,
                               int nLow,
                               int nHigh,
                               int nDefault) :
  CConfigurationParameter(rName),
  m_nSize(nSize),
  m_aValues(0),
  m_fCheckRange(true),
  m_nLow(nLow),
  m_nHigh(nHigh)
{
  SetupArray(nDefault);
  SetupStringValue();
}

/*! 
   Destructor
*/
CIntArrayParam::~CIntArrayParam ( )
{
  delete []m_aValues;
}

/*!
   Copy construction:  Used to build temporary objects
  e.g. for pass by value function parameters.
  \param rhs const CIntArrayParam& [in]
      the source for the copy.

*/
CIntArrayParam::CIntArrayParam (const CIntArrayParam& rhs) :
  CConfigurationParameter(rhs),
  m_aValues(0)
{
  // Do a deep copy: first the simple elements are copied.
  m_nSize       = rhs.m_nSize;
  m_fCheckRange = rhs.m_fCheckRange;
  m_nLow        = rhs.m_nLow;
  m_nHigh       = rhs.m_nHigh; 
  
  // Now do a deep copy of the parameter array.
  
  CopyArray(rhs);

} 
/*!
  Assignment operator.  Do a deep copy.
  \param rhs const CIntArrayParam& [in]
        The right hand side of the assignment operator.
  \return *this.
*/
CIntArrayParam& 
CIntArrayParam::operator= (const CIntArrayParam& rhs)
{
  if(this != &rhs) {         // short cut self copy.
    
    m_nSize       = rhs.m_nSize;
    m_fCheckRange = rhs.m_fCheckRange;
    m_nLow        = rhs.m_nLow;
    m_nHigh       = rhs.m_nHigh;
    CopyArray(rhs);         // CopyArray does storage mgmnt.
  }
  return *this;
}

/*!
   Test for functional equality.  Functional equality
  is achieved when all members are equal except the arrays 
  whose contents must be identical.  This function is invoked
  for *this == rhs expressions.
  \param rhs  const CIntArraryParam& [in]
      Right hand side of comparison.
  \return Comparison results:
    - true if *this is functionally equivalent to rhs.
    - false if not.
*/
int 
CIntArrayParam::operator== (const CIntArrayParam& rhs) const
{
  // Check for simple member equality:
  
  if( CConfigurationParameter::operator==(rhs)      &&
     (m_nSize       == rhs.m_nSize)                 &&
     (m_fCheckRange == rhs.m_fCheckRange)           &&
     (m_nLow        == rhs.m_nLow)                  &&
     (m_nHigh       == rhs.m_nHigh) ) {
       
       // Compare the arrays too. that are now know 
       // to have the same size:
    return (memcmp(m_aValues, 
                   rhs.m_aValues, m_nSize*sizeof(int)) == 0);
  }
  else {
    return 0;
  }
}

// Functions for class CIntArrayParam

/*!  	

Returns a reference to the corresponding array elemnt.  If
the index is out of range CRangeError is thrown.
  This function satisfies attempts to do things like
  *this[i].

\param n int [in]  The array index.
\return A reference to the n'th element of m_aValues.
\throw CRangeError if n is out of the range of valid indices.

*/
int&
CIntArrayParam::operator[](int n)  
{ 
  assert(m_aValues); 
  if((n < 0) || (n >= m_nSize)) {
    throw CRangeError(0, m_nSize-1, n,
            "Indexing a CIntArrayParam");
  }
  return m_aValues[n];
}  

/*!   	

Sets the array value from a parameter that is assumed
to be a well formed Tcl list.
This function will fail if:
- Any of the array values does not parse as an integer.
- Any of the array values is outside the range: [m_nLow, m_nHigh] and
  m_fCheckrange is true;.
- The size of the list does not match exactly m_nSize.

\param rInterp CTCLInterpreter& [in] The interpreter that
      is running the module config command.
\param rResult CTCLResult& [in] The result string.
      Will be empty on a clean run and have a descriptive
      error message otherwise.
\param pValues char* [in] The string containing the parameter
        list.
\return Status of the parse:
  - TCL_OK  on normal completion.
  - TCL_ERROR on failure in this case a descriptive error 
    message will be put in rResult.

*/
int 
CIntArrayParam::SetValue(CTCLInterpreter& rInterp, 
                CTCLResult& rResult, 
                const char* pValues)  
{ 
  // Parse the tcl list into a vector of strings.

  StringArray values;
  
  try {
    CTCLList    ValueList(&rInterp, pValues);
    ValueList.Split(values);
  } 
  catch (...) {
    rResult += "Configuration parameter array: ";
    rResult += getSwitch();
    rResult += " Configuration list was badly formatted";
    rResult += pValues;
    return TCL_ERROR;
  }
  // Now parse each element of the vector into an Long
  // The Longs are put into a separate vector so that
  // it's not possible to modify the configuration array
  // until it's known that all the values are ok.
  // At this step, the values are also range checked:
  //
  vector<TCLPLUS::Long_t> Values;
  TCLPLUS::Long_t nLong;
  for(int i = 0; i < values.size(); i++) {
      //      nLong = rInterp.ExprLong(values[i]);
      nLong = strtoll(values[i].c_str(), (char**) NULL, 0);
      if( (nLong == 0) && (errno == EINVAL) ) {
	char value[100];
	sprintf(value, "%ld", Values[i]);
	rResult  += "Configuration parameter array : ";
	rResult += getSwitch();
	rResult += " element: ";
	rResult += value;
	rResult += " failed to parse as a long";
	return TCL_ERROR;
      }
      if(m_fCheckRange) {
        if((nLong < m_nLow) || (nLong > m_nHigh)) {
	  char value[100];
	  sprintf(value, "%ld", Values[i]);
          rResult   += "Array configuration parameter ";
          rResult  += getSwitch();
          rResult  += " Parameter ";
          rResult  += value;
          rResult  += "Failed range check.";
          return TCL_ERROR;
        }
      }
      // The value is correct.
      Values.push_back(nLong);      // Save the elemnt.

  }
  // Now we need to be sure the Values array has the proper
  // size:
  
  if(m_nSize == Values.size()) {
    for(int i =0; i < m_nSize; i++) {
      m_aValues[i] = Values[i];
    }
    return TCL_OK;
  }
  else {
    rResult += " Array configuration parameter: ";
    rResult += getSwitch();
    rResult += " configuration list has incorrect number";
    rResult += " parameters. ",
    rResult += pValues;
    return TCL_ERROR;
  }
  rResult = "BUG Report that: Control reached the end of CIntArrayParam::SetValue";
  return TCL_ERROR;
}
/*!
   \return the format of the configuration parameter in this
   case, int[n]  where n is the stringified  of the 
  array size.
*/
string
CIntArrayParam::GetParameterFormat()
{
  char   result[100];
  sprintf(result, "int[%d]", m_nSize);
  
  return string(result);
}
/*!
    Protected helper functio to create and initialize
    the array:
    \param nDefault int [in]  The value to initialize
            elements of the array to.
*/
void
CIntArrayParam::SetupArray(int nDefault)
{
  if(m_aValues) delete []m_aValues;    // Get rid of any existing array.
  m_aValues = new int[m_nSize];
  for(int i =0; i < m_nSize; i++) {
    m_aValues[i] = nDefault;
  }
}
/*!
   Protected helper function to copy the array from
   some other source.
   \param rhs const CIntArrayParam& [in] The source
       of the array.
*/
void
CIntArrayParam::CopyArray(const CIntArrayParam& rhs) 
{
  m_nSize = rhs.m_nSize;
  SetupArray(0);		// Since all of the 
  for(int i=0; i < m_nSize; i++) { // values get rewritten.
    m_aValues[i] = rhs.m_aValues[i];
  }

}
/*!
    Given the current values of the array, sets the string version of the
    parameter's value.
*/
void
CIntArrayParam::SetupStringValue()
{
  string sValue;
  char   cValue[100];
  for(int i =0; i < m_nSize; i++) {
    sprintf(cValue, "%d ", m_aValues[i]);
    sValue += cValue;
  }
  setValue(sValue);
    
}
