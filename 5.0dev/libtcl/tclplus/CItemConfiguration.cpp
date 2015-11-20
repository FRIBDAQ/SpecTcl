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
#include "CItemConfiguration.h"
#include <set>
#include <stdlib.h>
#include <errno.h>
#include <tcl.h>
#include <math.h>
#include <tcl.h>
#include <string.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////
//////////////////////////    Constants ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////

static const CItemConfiguration::ListSizeConstraint unconstrainedSize = 
  {CItemConfiguration::limit(0),
   CItemConfiguration::limit(0)};

///////////////////////////////////////////////////////////////////////////
//////////////////////// Canonical member functions ///////////////////////
///////////////////////////////////////////////////////////////////////////

/*!
   Contruct the object:
   \param name : std::string
       Name to give to the object.
*/
CItemConfiguration::CItemConfiguration(string name) :
  m_name(name)
{
}

/*!
  Destruction probably will result in some memory leaks since
  it is possible that the typeChecker's will have parameters that
  are dynamically allocated. In a future life we can provide
  cleanup functions.. for now we just assume that destruction will
  be infrequent, and leaks will be small enough to be tolerated.
  
  I think that destruction is not necessary since all the
  pairs will copyconstruct/assign into the map.
  This is a place holder for later code that can handle deletion of the
  typechecker args.

*/
CItemConfiguration::~CItemConfiguration()
{

}

/*!
   Copy construction.  We can just copy the name and map.
   Copy construction should be rare as normally names are unique.
*/
CItemConfiguration::CItemConfiguration(const CItemConfiguration& rhs) :
  m_name(rhs.m_name),
  m_parameters(rhs.m_parameters)
{

}
/*!
   Assignement is similar to copy construction:
*/
CItemConfiguration&
CItemConfiguration::operator=(const CItemConfiguration& rhs)
{
  if (this != &rhs) {
    m_name       = rhs.m_name;
    m_parameters = rhs.m_parameters; 
  }
  return *this;
}

/*!
  Equality is via item comparison.
*/
int
CItemConfiguration::operator==(const CItemConfiguration& rhs) const
{
  return ((m_name == rhs.m_name)   &&
	  (m_parameters == rhs.m_parameters));
}
/*!
  Inequality is the logical inverse of equality:
*/
int
CItemConfiguration::operator!=(const CItemConfiguration& rhs) const
{
  return !(*this == rhs);
}

//////////////////////////////////////////////////////////////////////////
///////////////////////// Selectors //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*!
  \return string
  \retval The name of the object.
 */
string
CItemConfiguration::getName() const
{
  return m_name;
}

/*!
   Get the value of a single configuration item.
   \param name : std::string
      Name of the parameter to retrieve.

   \return std::string
   \retval the value of the configuration parameter.

   \throw string - if name is not a known configuration parameter, afte all,
                   clients should know the configuration parameters.. if they don't
		   see the next cget which retrieves parameters and values.
*/
string
CItemConfiguration::cget(string name) 
{
  ConfigIterator found = findOrThrow(name);

  ConfigData data = found->second;
  return data.first;
}


/*!
   Get the values of all the configuration parameters.
   \return CCOnfigurableObject::ConfigurationArray
   \retval A vector of parameter name/value pairs.
           Given an element, ele of the vector,
	   ele.first is the parameter name, ele.second the value.

   \note While at present, the parameters come out sorted alphabetically,
         you should not count on that fact.
*/
CItemConfiguration::ConfigurationArray
CItemConfiguration::cget() 
{
  ConfigurationArray result;
  ConfigIterator p = m_parameters.begin();
  while(p != m_parameters.end()) {
    
    string name = p->first;
    ConfigData data = p->second;
    string value = data.first;
    result.push_back(pair<string, string>(name, value));
    
    p++;
  }
  return result;
}
/////////////////////////////////////////////////////////////////////////////
///////////////////// convenience functions /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// Convenience functions return the value of a parameter converted to one
// a commonly used form.  The caller is responsible for ensuring that the
// parameter is type checked to that form prior to using these functions,
// as they do not report conversion errors...but instead return some
// value that may or may not make sense.


/*!
    Return the value of an integer parameter.
    \param name : std::string
       Name of the value.

    \return int
    \retval the integer equivalent of the config paramter.

*/
int
CItemConfiguration::getIntegerParameter(string name)
{
  string value = cget(name);

  char* end;
  int iValue = strtol(value.c_str(), &end, 0);
  if (end == value.c_str()) {
    string msg = "Expected an integer parameter value for config. parameter ";
    msg += name;
    msg += "got: ";
    msg += value;
    throw msg;
  }
  return iValue;
}
/*! 
  Same as above but for an integer.  Needed because strtol for something
  bigger than MAXINT returns MAXINT.
*/
unsigned int
CItemConfiguration::getUnsignedParameter(string name)
{
  string value = cget(name);

  char* end;
  int iValue = strtoul(value.c_str(), &end, 0);
  if (end == value.c_str()) {
    string msg = "Expected an integer parameter value for config. parameter ";
    msg += name;
    msg += "got: ";
    msg += value;
    throw msg;
  }
  return iValue;
}

/*!
  Return the value of a bool parameter.
  This uses the same set of true values as the checker.. however
  any other value is assumed to be false.

  \param name : std::string
     Name of the value.
  
  \return bool
  \retval the boolean equivalent of the config param
*/
bool
CItemConfiguration::getBoolParameter(string name)
{
  string value = cget(name);
  set<string> trueValues;
  addTrueValues(trueValues);

  // Is enum does what we want if we pass it the set of true values...
  // it'll give us a true if the value is in the set of legal trues and false otherwise:

  return isEnum(name, value, &trueValues);
}

/*!
   Return a parameter decoded as a double.

   \param name : std::string
     Name of the parameter

   \return double

*/
double
CItemConfiguration::getFloatParameter(string name)
{
  string value = cget(name);
  char* end;
  double result = strtod(value.c_str(), &end);
  if (end == value.c_str()) {
    string error = "Expected a floating point parameter value for config parameter ";
    error       += name;
    error       += " got: ";
    error       += value;
    throw error;
  }
  return result;

}

/*!
  Return a parameter that is a list of integers.
  \param name - name of the parameter.
  \return vector<int>
  \retval Vector containing the integers in the list.

*/
vector<int>
CItemConfiguration::getIntegerList(string name)
{
  string value = cget(name);
  int argc;
  const char** argv;
  vector<int> result;

  if(Tcl_SplitList(NULL, value.c_str(), &argc, &argv) != TCL_OK) {
    string msg  = "Expected a Tcl list for parameter ";
    msg        += name;
    msg        += " got: ";
    msg        += value;
    throw msg;
  }

  for (int i =0; i < argc; i++) {
    char *end;
    result.push_back(static_cast<int>(strtol(argv[i], &end, 0)));
    if (*end != '\0') {
      Tcl_Free((char*) argv);
      string msg = "Expected an integer list element but got ";
      msg       +=  string(argv[i]);
      throw msg;
    }
  }
  Tcl_Free((char*)argv);
  return result;

}
///////////////////////////////////////////////////////////////////////////
/////////////////////// Establishing he configuration /////////////////////
///////////////////////////////////////////////////////////////////////////

/*!
   Adds a configuration parameter to the configuration.
   If there is already a configuration parameter by this name,
   it is silently ovewritten.

   \param name : std::string
      Name of the parameter to add.
   \param checker : typeChecker
      A type checker to validate the values proposed for the parameter,
      if NULL, no validation is performed.
   \param arg   : void
      Parameter passed without interpretation to the typechecker at validation
      time.
   \param defaultValue : string (default = "")
      Initial value for the parameter.
*/
void
CItemConfiguration::addParameter(string      name, 
				      typeChecker checker,
				      void*       arg,
				      string      defaultValue)
{
  TypeCheckInfo checkInfo(checker, arg);
  ConfigData    data(defaultValue, checkInfo);
  m_parameters[name] = data;	// This overwrites any prior.
}


/*!
  Returns true if a proposed configuration is valid.  This will
  still throw if the name is not defined... but will 
@return bool
@retval true - The proposed configuration is acceptable.
@retval false - The proposed configuration is not acceptable.

@param name - of the parameter to validate.
@param value - Proposed new value.
*/
bool
CItemConfiguration::isValid(string name, string value)
{
  ConfigIterator item = findOrThrow(name);

  // If the parameter has a validator get it and validate:

  TypeCheckInfo checker = item->second.second;
  typeChecker   pCheck  = checker.first;
  if (pCheck) {			// No checker means no checkig.
    if (! (*pCheck)(name, value, checker.second)) {
      return false;
    }
  }
  return true;
}

/*!
    Configure the value of a parameter.
    \param name : std::string
       Name of the parameter to configure.
    \param value : std::string
       New value of the parameter

   \throw std::string("No such parameter") if the parameter 'name' is not defined.
   \throw std::string("Validation failed for 'name' <- 'value'") if the value
           does not pass the validator for the parameter.
*/
void
CItemConfiguration::configure(string name, string value)
{
  // Locate the parameter and complain if it isn't in the map:

  ConfigIterator item = findOrThrow(name);


  if (!isValid(name, value)) {

      string msg("Validation failed for ");
      msg += name;
      msg += " <- ";
      msg += value;
      throw msg;
  }

  // Now set the new validated value:

  m_parameters[name].first = value;

}
/*!
  clear the current configuration.  The configuration map m_parameters
  map is emptied.
*/
void
CItemConfiguration::clearConfiguration()
{
  m_parameters.clear();
}

////////////////////////////////////////////////////////////////////////////
/////////////////////  Stock type checkers //////////////////////////////////
////////////////////////////////////////////////////////////////////////////

/*!
   Validate an integer parameter with optional limits.
   \param name : std::string
       Name of the parameter being checked (ignored).
   \param value : std::string
       Proposed new value.
   \param arg : void*
       This is actually a pointer to an Limits structure or NULL.
       - If NULL, no range checking is done.
       - If not null, range checking is done.  Each limit contains a checkme
         flag which allows validation to occur when one or both limits are needed.
         limits are inclusively valid.
    \return bool
    \retval true  - Validation passed.
    \retval false - Validation failed.
*/
bool
CItemConfiguration::isInteger(string name, string value, void* arg)
{
  // first determine the 'integernes' using strtoul.


  char* end;
  long lvalue = strtoul(value.c_str(), &end, 0);	// Base allows e.g. 0x.
  if (*end != '\0') {		               // String is not just an integer.
    return false;
  }
  // If there's no validator by definition it's valid:

  if(!arg) return true;

  // Get the validator in the correct form:

  Limits* pRange = static_cast<Limits*>(arg);
  if (!pRange) {
    string msg("BUG: argument for integer validator for parameter: ");
    msg += name;
    msg += " is not a pointer to a Limits type";
  }
  // check lower limit:

  if((pRange->first.s_checkMe) && (lvalue < pRange->first.s_value)) {
    return false;
  }
  if ((pRange->second.s_checkMe) && (lvalue > pRange->second.s_value)) {
    return false;
  }
  return true;
}

/*!
    Validate a bool parameter.  Bool parameters have any of the values:
    - true: true, yes, 1, on, enabled
    - false: false, no, 0, off, disabled
   We'll just delegate this off to isEnum.
   \param name  : std::string
     Name of the parameter
   \param value : std:: string
     proposed new value.
   \param ignored : void*
     Ignored value parameter.
   
    \return bool
    \retval true  If valid.
    \retval false If invalid.
*/
bool
CItemConfiguration::isBool(string name, string value, void* ignored)
{
  // Build the set required by isEnum:

  set<string> allowedValues;
  addTrueValues(allowedValues);
  addFalseValues(allowedValues);



  return isEnum(name, value, &allowedValues);
}

/*!
  Validate a floating point parameter.  Floating parameters allow the implementor of a device
  to let the user specify values in 'real units' which are then converted transparently to device
  register values.  Floating point values are actually handled as doubles, since C/C++ likes to 
  upconvert floats to doubles in any event.  The conversion fails if the string does not
  convert to a floating point value or the etire string is not used in the conversion.
  NAN's are also considered invali, and isnan is used to determine if the conversion resulted
  in an NAN.  

  The application can place limits on the floating point value as well.  See
  the parameters below:

  \param name : std::string
     Parameter name (ignored).
  \param value : std::string
     The value of the paramter.
  \param values : void*
     Actually a ppointer to a FloatingLimits type which sets the limits
     on the parameter.  If the FloatingLimits type is NULL, all floating
     values are allowed...except for NAN's... which are never allowed.

  \return bool
  \retval true  - if Valid.
  \retval false - if not Valid.

*/
bool
CItemConfiguration::isFloat(string name, string value, void* values)
{
  FloatingLimits* pLimits = static_cast<FloatingLimits*>(values);

  // Do the conversion..and check the basic legality

  const char* start(value.c_str());
  char* endptr;
  double fValue = strtod(start, &endptr);
  if((strlen(start) != (endptr - start))) {
    return false;		// Did not convert entire string e.g. 1.0garbage
  }
  if(isnan(fValue)) {
    return false;
  }

  // If a range was supplied check it:

  if (pLimits) {
    if (pLimits->first.s_checkMe) {
      if (fValue < (pLimits->first.s_value)) {
	return false;
      }
    }
    if (pLimits->second.s_checkMe) {
      if (fValue > (pLimits->second.s_value)) {
	return false;
      }
    }
  }
  
  // All tests passed:

  return true;


}


/*!
   Validate an enum parameter.
   An enumerated parameter is one that can be a string drawn from a limited
   set of keywords. Validation fails of the string is not one of the
   valid keywords.
   \param name : std::string
      Name of the parameter.
   \param value : std::string
      Proposed parameter value.
   \param values : void*
      Actually an std::set<string>* where the set elements are the
      valid keywords.
  \return bool
  \retval true If valid. 
  \retval false If invalid.
*/
bool
CItemConfiguration::isEnum(string name, string value, void* values)
{
  set<string>& validValues(*(static_cast<set<string>*>(values)));

  return ((validValues.find(value) != validValues.end()) ? true : false);

}


/*!
   Validate a list parameter.   To simplify; a list parameter
   is a Tcl formatted list.  List validation can fail for the following
   reasons:
   - The value is not a properly formatted list.
   - The value has too many list elements.
   - The value has too few list elements.
   - One or more of the list elements fails validation.
   \param name : std::string
      Name of the parameter being modified.
   \param value : std::string
      Value proposed for the parameter.
   \param validity : void*
      Actually an optional pointer to an isListParameter:
      - If validity is NULL, no size or element validity checking is done.
      - If validity is non-null, it's s_allowedSize element is used to 
        validate the list size, and if the s_checker.first is not null,
        it is used to validate the list elementss.

   \return bool
   \retval true List is valid.
   \retval fals List is invalid.

*/
bool
CItemConfiguration::isList(string name, string value, void* validity)
{
  // First ensure the list can be split:

  int    listSize;
  const char** list;
  int status = Tcl_SplitList((Tcl_Interp*)NULL, value.c_str(),
			     &listSize, &list);
  if (status != TCL_OK) {
    return false;
  }
  // The list is valid. Validate elements if needed:

  bool result = true;
  if (validity) {
    isListParameter& listValidity(*static_cast<isListParameter*>(validity));
    

    // Check length constraints.

    if (listValidity.s_allowedSize.s_atLeast.s_checkMe && 
	(listSize < listValidity.s_allowedSize.s_atLeast.s_value)) {
      result = false;
    }
    if (listValidity.s_allowedSize.s_atMost.s_checkMe &&
	(listSize > listValidity.s_allowedSize.s_atMost.s_value)) {
      result = false;
    }
    // If checker supplied, check all the list elements for validity.

    if (result && listValidity.s_checker.first) {
      for (int i=0; (i < listSize) && result; i++) {
	result = (*listValidity.s_checker.first)(name, list[i], 
						 listValidity.s_checker.second);
      }
    }

  }

  Tcl_Free((char*)(list));
  return result;

}
/*!
    Check that a list is a valid bool list.
    This is done by constructing the listValidity object with a list element
    checker set to the bool element checker.
    \param name : std::string
      Name of the parameter.
    \param value : std:: string
      Proposed new value.
    \param sizes : void*
      Actually a pointer to ListSizeConstraint if non null:
      - If NULL elements are checked for validity but any list size is ok.
      - If non NULL elements are checked for validity and Limits set the
        limits on size.
    \return bool
    \retval true List validates.
    \retval false List is not valid.
*/
bool
CItemConfiguration::isBoolList(string name, string value, void* size)
{
  // Set up our isListParameter struct initialized so that only
  // elements will be checked:

  isListParameter validator;
  validator.s_checker.first = isBool;
  validator.s_checker.second= NULL;

  if (size) {
    ListSizeConstraint& limits(*static_cast<ListSizeConstraint*>(size));
    validator.s_allowedSize = limits;

  }
  else {
    validator.s_allowedSize = unconstrainedSize;

  }
  return isList(name, value, &validator);
}
/*!
    Check that this is a valid list of integers.
    For string lists in this implementation we are not able to do range
    checking on the values of the list elements.  We do, however enforce
    the integer-ness of each element of the list.
    \param name : std::string
       Name of the parameter.
    \param value : std::string
       Proposed value of the parameter.
    \param sizes : void*
       Actually a pointer to a ListSizeConstraint which, if non null places
       constraints on the number of elements in the list.
    \return bool
    \retval true  - List validated.
    \retval false - List not validated

    \todo Extend the parameter to us to supply optional limit information.
*/
bool
CItemConfiguration::isIntList(string name, string value, void* size)
{
  isListParameter validator;
  validator.s_checker.first  = isInteger; // Require integer but 
  validator.s_checker.second = NULL;      // No range requirements. 

 
  

  if (size) {
    ListSizeConstraint& limits(*static_cast<ListSizeConstraint*>(size));
    validator.s_allowedSize = limits;
  }
  else {
    validator.s_allowedSize = unconstrainedSize;
  }
  return isList(name, value, &validator);
}
/*!
    Check for a string list.  String lists are allowed to have just about anything
    as element values...therefore, if the validSizes parameter is present, we'll
    do list size checking only, otherwise on list syntax checking.
    \param name : std::string
       Name of the parameter being modified.
    \param value ; std::string
       proposed new value for the parameter.
    \param validSizes : void*
       Actually a pointer to a ListSizeConstraint structure that, if non-null
       defines the list size checking that will take place.  If NULL, any sized
       list (including empty) is allowed.
    \return bool
    \retval true  - List validated.
    \retval false - List not validated

*/
bool
CItemConfiguration::isStringList(string name, string value, void* validSizes)
{
  isListParameter validator;
  validator.s_checker.first  = static_cast<typeChecker>(NULL);
  validator.s_checker.second = NULL;

  if (validSizes) {
    ListSizeConstraint& limits(*static_cast<ListSizeConstraint*>(validSizes));
    validator.s_allowedSize = limits;
  }
  else {
    validator.s_allowedSize = unconstrainedSize;
  }

  return isList(name, value, &validator);
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Utilities ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

// Add the legal true value strings to a set of strings:

void
CItemConfiguration::addTrueValues(set<string>& values)
{
  values.insert("true");  // True values:
  values.insert("yes");
  values.insert("1");
  values.insert("on");
  values.insert("enabled");
}
// Add the legal false value strings to a set of strings.

void
CItemConfiguration::addFalseValues(set<string>& values)
{
  values.insert("false"); // False values.
  values.insert("no");
  values.insert("0");
  values.insert("off");
  values.insert("disabled");
}
// Locate an item in the configuration database or throw a standardized string exception
// if it is not defined:

CItemConfiguration::ConfigIterator
CItemConfiguration::findOrThrow(string name)
{
  ConfigIterator item = m_parameters.find(name);
  if(item == m_parameters.end()) {
    string msg("No such parameter: ");
    msg  += name;
    throw msg;
  }

  return item;
}
