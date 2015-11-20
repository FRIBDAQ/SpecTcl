
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
#ifndef _CPARAMETER_H
#define _CPARAMETER_H

/**
 * @file CParameter.h
 * @brief Definition of class containing parameter metadata.
 */

#ifndef _STL_STRING
#include <string>
#ifndef _STL_STRING
#define _STL_STRING
#endif
#endif


#ifndef _CVALIDVALUE_H
#include "CValidValue.h"
#endif


#ifndef _STD_EXCEPTION
#include <exception>
#ifndef _STD_EXCEPTION
#define _STD_EXCEPTION
#endif
#endif

/**
 * @class CParameter
 *
 *  This wraps a double valid value in metadata that is used to describe
 *  to humans and the computer the range of likely values, how to bin an axis
 *  for that parameter and what the units of measure of the parameter are.
 *
 *   If the class is in  'autoRegister' mode, objects created will be automatically
 *   registered in the CParameterDictionary singleton and associated with their
 *   underlying values. Note that many to one assocations are possible and likely with
 *   each stage of the event/analysis pipeline instantiating an appropriate set of CParameter
 *   entitites.
 *
 *   The class supports being treated as a double as well; allowing you to use it
 *   freely in computational expressions wherever you would ordinarily use
 *   an actual double.   
 *
 *  @note  Since the underlying data representation of the value is a CValidValue
 *         r-value use of a parameter that is not set is cause for an exception to be
 *         thrown.
 */
class CParameter 
{
  // Static data:
private:

  static bool               m_autoRegister;
  // Local data

private:
  std::string                m_name;
  CValidValue<double>*       m_parameter;
  double                     m_low;
  double                     m_high;
  unsigned                   m_bins;
  std::string                m_units;
  bool                       m_checkLimits;

  // Canonicals
public:
  CParameter(const char* pName);

  CParameter(const char* pName, double low, double high);
  CParameter(const char* pName, double low, double high, const char* pUnits);
  CParameter(const char* pName, double low, double high, unsigned bins, 
	     const char* pUnits);

  virtual ~CParameter();

  int operator==(const CParameter& rhs);

  int operator!=(const CParameter& rhs);

  bool identical(const CParameter& rhs);
  
  CParameter& operator=(double rhs);
  CParameter& operator=(const CParameter& rhs);
  void        dup(const CParameter& rhs);
  operator double();

  // Selectors

  std::string getName()const ;
  double getLow() const ;
  double getHigh() const;
  unsigned getBins() const;
  std::string getUnits() const;

  // mutators
public:
  void setValue(CValidValue<double>* p);
  void changeLow(double newLow);
  void changeHigh(double newHigh);
  void changeBins(unsigned newBins);
  void changeUnits(const char* pNewUnits);
  void enableLimitCheck();
  void disableLimitCheck();

  // Object operations:
public:  
  bool isvalid();
  bool checkLimits() {return m_checkLimits;}
  void registerParameter();
  void reset();

  // Class operations

public:
  static void invalidateAll();
  static void enableAutoRegistration();
  static void disableAutoRegistration();

  // Private utilities:

private:
  void conditionallyRegister();

  // These are handed to foreach handlers to propagate changes.

  void _changeLow(double newLow) { m_low = newLow;}
  void _changeHigh(double newHigh) {m_high = newHigh;}
  void _changeBins(unsigned newBins) {m_bins = newBins;}
  void _changeUnits(const char* pNewUnits) {m_units = pNewUnits;}

  void _check(double value);


};
class parameter_limit : std::exception {
public:
  virtual const char* what() const throw() {
    return "Value for parameter fails limit check";
  }
};

#endif
