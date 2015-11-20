/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include "CParameter.h"
#include "CParameterDictionary.h"

#include <algorithm>

/**
 * @file CParameter.cpp
 * @brief Implementation of the CParameter class, see CParameter.h
 */

// class static data:


// i'm too stupid to know how to make these template classes.
// These are used by a for_each to propagate metadata changes.

bool CParameter::m_autoRegister(true);

class op {
private:
  typedef void (CParameter::*funcptr)(double);
  funcptr m_member;
  double  m_value;
public:
  op(funcptr f, double value) :
    m_member(f), m_value(value) {}
  void operator()(CParameter* pObject) {
    (pObject->*m_member)(m_value);
  }
};
class opu {
private:
  typedef void (CParameter::*funcptr)(unsigned);
  funcptr m_member;
  unsigned  m_value;
public:
  opu(funcptr f, unsigned value) :
    m_member(f), m_value(value) {}
  void operator()(CParameter* pObject) {
    (pObject->*m_member)(m_value);
  }
};
class ops {
private:
  typedef void (CParameter::*funcptr)(const char*);
  funcptr m_member;
  std::string  m_value;
public:
  ops( funcptr f, std::string value) :
    m_member(f), m_value(value) {}
  void operator()(CParameter* pObject) {
    (pObject->*m_member)(m_value.c_str());
  }
};
// Used to traverse dict in invalidateAll method.

static void ResetDictItem(std::pair<std::string, CParameterDictionary::pParameterInfo> p) 
{
  p.second->s_references.front()->reset();
}

/**
 * constructor
 *
 *   Construct and optionally register (if m_autoRegister is true)
 *   the parameter.  Note that if there is already a parameter with this
 *   name but different characteristics; this method will throw an
 *   exception.
 *
 * @param pName - name of the parameter as it will be registered in the
 *               dictionary.
 */
CParameter::CParameter(const char* pName) :
 m_name(pName), m_parameter(0),m_low(0), m_high(100), 
 m_bins(101), m_units(""), m_checkLimits(false)
{
  conditionallyRegister();
}

/**
 * constructor
 *    As above, but the low/high limit guideline metadata
 *    are supplied:
 *
 * @param pName  - Name of the parameter.
 * @param low    - smallest value the parameter is likely to have.
 * @param high   - largest value the parameter is likely to have.
 */
CParameter::CParameter(const char* pName, double low, double high) :
  m_name(pName), m_parameter(0), m_low(low), m_high(high),
  m_bins(static_cast<int>(high-low+1)), m_units(""), m_checkLimits(false)
{
  conditionallyRegister();
}

/**
 * constructor
 *
 *  Same as the constructors above, but the user can also supply
 *  units of measure.
 * @param pName  - Name of the parameter.
 * @param low    - smallest value the parameter is likely to have.
 * @param high   - largest value the parameter is likely to have.
 * @param units  - Units of measure.
*/
CParameter::CParameter(const char* pName, double low, double high, const char* pUnits) :
  m_name(pName), m_parameter(0), m_low(low), m_high(high), 
  m_bins(static_cast<int>(high-low+1)),  m_units(pUnits), m_checkLimits(false)
{
  conditionallyRegister();
}
/**
 * constructor
 *
 *  Same as all other constructors but allows the binning suggestion
 *  to be supplied.
 * @param pName  - Name of the parameter.
 * @param low    - smallest value the parameter is likely to have.
 * @param high   - largest value the parameter is likely to have.
 * @param bins   - Suggested axis binning.
 * @param units  - Units of measure.
 */
CParameter::CParameter(const char* pName, double low, double high, unsigned bins, 
		       const char* pUnits) :
   m_name(pName), m_parameter(0), m_low(low), m_high(high),
   m_bins(bins),   m_units(pUnits), m_checkLimits(false)
{
  conditionallyRegister();
}

/**
 * destructor
 *
 *   Remove ourselves from the parameter dictionary if we are in it.
 */
CParameter::~CParameter()
{
  CParameterDictionary* pDict = CParameterDictionary::instance();
  if (pDict->find(m_name) != pDict->end()) {
    pDict->remove(this);
  }
}
/**
 * operator double
 *
 *  @return the value of the underlying parameter as a double.
 */
CParameter::operator double()
{
  return (double)(*this->m_parameter);
}

/**
 * operator==
 *   returns true if the underlying values are true.  This allows
 *   CParameter objects to be used in conditionals just like their
 *   value.  To see if two objects define the same value, see identical
 *   below
 *
 * @parameter rhs - the parameter on the right side of the ==
 *
 * @return int - nonzero if equal zero if not
 */
int
CParameter::operator==(const CParameter& rhs)
{
  return (*m_parameter) == (*rhs.m_parameter);
}
/**
 * operator!=
 *
 *  This is defined as !(operator==).
 */
int
CParameter::operator!=(const CParameter& rhs)
{
  return !(*this == rhs);
}
/**
 * identical
 *   Attempt to determine if a pair of CParameters are identically
 *   defined.  This is so if

 *   - Their low/high/units and binning are the same.
 */
bool
CParameter::identical(const CParameter& rhs)
{
  return (m_low == rhs.m_low)      &&
    (m_high == rhs.m_high)         &&
    (m_bins == rhs.m_bins)         &&
    (m_units == rhs.m_units);
} 
/**
 * operator=
 *   Assigns from a double.
 * 
 * @param rhs - new value fro the underlyhnig parameter
 *
 * @return CParameter& (*this)
 */
CParameter&
CParameter::operator=(double rhs)
{
  _check(rhs);
  *m_parameter = rhs;
}
/**
 * operator=
 *   Assigns from an object.  This just assigns the values
 *   of the underling valid values.
 *
 * @param rhs  - Refernce to the source of the assignment.
 * @return CParameter& (*this)
 */
CParameter&
CParameter::operator=(const CParameter& rhs)
{
  _check(*rhs.m_parameter);
  *m_parameter = *rhs.m_parameter;
  return *this;
}
/**
 * dup
 *
 *  Duplicates metadata for the parameter.  The name and
 *  the value of the underlying parameter are not changed.
 *
 * @param rhs - source of the metadata.
 */
void
CParameter::dup(const CParameter& rhs)
{
  m_low   = rhs.m_low;
  m_high  = rhs.m_high;
  m_bins  = rhs.m_bins;
  m_units = rhs.m_units;
}
/**
 * @return name of the parameter.
 */
std::string 
CParameter::getName() const
{
  return m_name;
}
/**
 * @return low limit on parameter.
 */
double
CParameter::getLow() const
{
  return m_low;
}
/**
 * @return high limit on parameter.
 */
double
CParameter::getHigh() const
{
  return m_high;
}
/**
 * @return unsigned - number of bins suggested.
 */
unsigned
CParameter::getBins() const
{
  return m_bins;
}
/**
 * @return std::string - units of measure for the param.
 */
std::string
CParameter::getUnits() const
{
  return m_units;
}

/**
 * setValue
 *   Set the underlying value of the parameter.   This should really only
 *   be used by CParameterDictionary when adding a parameter
 *
 * @param pValue - Pointerr to the underlying value.
 */
void
CParameter::setValue(CValidValue<double>* pValue)
{
    m_parameter = pValue;

}
/**
 * changeLow
 *
 *  Change the value of the low level of the parameter. Note that
 *  low can be larger than high to make the axis 'run backwards'.
 *
 * @param newLow - new value for the low limit.
 */
void
CParameter::changeLow(double newLow)
{
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator p = pDict->find(m_name);
  if (p == pDict->end()) {
    // Unregistered param:
    m_low = newLow;
  } else {
    // registered -- change all bound parameters.

    op function(&CParameter::_changeLow, newLow);
    std::for_each(p->second->s_references.begin(), p->second->s_references.end(), function);

  }
}
/**
 * changeHigh
 *
 *  Change the vale of the high levelof the parameter.
 *
 * @param newHigh -  new value for the high.
 */
void
CParameter::changeHigh(double newHigh)
{

  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator p = pDict->find(m_name);
  if (p == pDict->end()) {
    m_high = newHigh;
  } else {
    op function(&CParameter::_changeHigh, newHigh);
    std::for_each(p->second->s_references.begin(), p->second->s_references.end(), function);
  }
}
/**
 * changeBins
 *
 * @parameter newBins - new suggested binning for the parameter.
 */
void
CParameter::changeBins(unsigned newBins) 
{
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator p = pDict->find(m_name);
  if (p == pDict->end()) {
    m_bins = newBins;
  } else {
    opu function(&CParameter::_changeBins, newBins);
    std::for_each(p->second->s_references.begin(), p->second->s_references.end(), function);
  }  
}
/**
 * changeUnits
 * 
 * @param newUnits - New units of measure string.
 */
void
CParameter::changeUnits(const char* newUnits)
{  
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator p = pDict->find(m_name);
  if (p == pDict->end()) {
    m_units = newUnits;
  } else {
    ops function(&CParameter::_changeUnits, newUnits);
    std::for_each(p->second->s_references.begin(), p->second->s_references.end(), 
		  function);
  }  
}
/**
 * enableLimitCheck
 *
 *    Turns on limit checking for all stores that go through this 
 *    binding to the underlying value.
 */
void
CParameter::enableLimitCheck()
{
  m_checkLimits =  true;
}
/**
 * disableLimitCheck
 *
 *  Turns off limit checking for all stores that go throug this
 *  binding to the underlying value.
 */
void
CParameter::disableLimitCheck()
{
  m_checkLimits = false;
}
/**
 * isvalid
 *
 *   Returns true if the underlying variable is valid too..
 */
bool
CParameter::isvalid()
{
  return m_parameter->isvalid();
}
/**
 * registerParameter
 *   Register the parameter with the dictionary.
 *   this can throw an exception if the parameter
 *   is already registered.
 */
void
CParameter::registerParameter()
{
    CParameterDictionary* pDict = CParameterDictionary::instance();
    pDict->add(this);
}
/**
 * reset
 *   Invalidate this parameter.
 */
void CParameter::reset()
{
  m_parameter->reset();
}


/*-------------------------------------------------------
** Static methods
*/
/**
 * invalidateAll
 *
 *  Invalidates all parameters in the dictionary.
 */
void
CParameter::invalidateAll()
{
  CParameterDictionary* pDict = CParameterDictionary::instance();
  for_each(pDict->begin(), pDict->end(), ResetDictItem);
  
}
/**
 * enableAutoRegistration
 *
 *  Turns on the auto registration flag.  While this flag is enabled,
 *  parameters register themselves with the parameter dictionary
 *  singleton when constructed.
 */
void
CParameter::enableAutoRegistration()
{
  m_autoRegister = true;
}
/**
 * disableAutoRegistration
 *
 *  Turns off the autoregistration flag.  While this flag is off,
 *  construction does not register a parameter.  Parameters thus
 *  created must be explicitly registered via CParameterDictionary::add.
 */
void
CParameter::disableAutoRegistration()
{
  m_autoRegister = false;
}
/*-------------------------------------------------------
**  Private utililties.
*/

/**
 * conditionallyRegister
 *
 *  If m_autoRegister is true, register this
 */
void
CParameter::conditionallyRegister()
{
  if (m_autoRegister) registerParameter();
}
/**
 * _check
 *    If limit checking is true, throws a parameter_limit exception if the 
 *   proposed new value is outside [m_low, m_high].
 *
 * @param proposed - Proposed new value
 */
void
CParameter::_check(double proposed)
{
  if (m_checkLimits &&
      ((proposed < m_low) || (proposed > m_high))) {
    throw parameter_limit();
  }
}
