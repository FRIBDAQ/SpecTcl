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

/**
 * @file CParameterArray.h
 * @brief Array like object for CParameter objects.
 */
#ifndef _CPARAMETERARRAY_H
#define _CPARAMETERARRAY_H

#ifndef _STD_VECTOR
#include <vector>
#ifndef _STD_VECTOR
#define _STD_VECTOR
#endif
#endif

class CParameter;

/**
 * @class CParameterArray
 *
 *   CParameterArray is a class that looks like an array of CParameter
 *   objects.  What makes it useful is that it:
 *   - Makes all objects with the same metadat.
 *   - Generates subscript like parameter names for the elements of the form
 *     basename.nnn where nnn is autosized with leading zeroes so that the
 *     Elements lexically sort (e.g. with 100 elements, 01, 02...99.
 *   - index range checking can be optionally enabled/disabled.
 */
class CParameterArray {
private:
  unsigned m_indexLow;
  unsigned m_elementCount;
  bool     m_rangeCheck;
  std::vector<CParameter*> m_array;

public:
  class CConstructor {
  public:
    virtual CParameter* operator()(const char* pName) = 0;
  };
public:
  // Construction:

  CParameterArray(const char* pBaseName, 
		  unsigned num, unsigned indexLow = 0);
  CParameterArray(const char* pBaseName, double low, double high,
		  unsigned num, unsigned indexLow = 0);
  CParameterArray(const char* pBaseName, double low, double high, 
		  const char* pUnits,
		  unsigned num, unsigned indexLow = 0);
  CParameterArray(const char* pBaseName, 
		  double low, double high, unsigned bins, const char* pUnits,
		  unsigned num, unsigned indexLow = 0);
  virtual ~CParameterArray();

  // Methods for the array.
public:
  void enableRangeCheck();
  void disableRangeCheck();

  // Delegational methods
public:
  CParameter& operator[](unsigned int);
  void changeLow(double newLow);
  void changeHigh(double newHigh);
  void changeBins(unsigned newBins);
  void changeUnits(const char* newUnits);
  void enableLimitCheck();
  void disableLimitCheck();

  // Private utilities:

  void makeParameters(CConstructor& maker, const char* pBaseName, 
		      unsigned n, unsigned lowIndex);

};

#endif
