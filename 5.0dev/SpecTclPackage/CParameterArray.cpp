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
 * @file CParameterArray.cpp
 * @brief Implementation of parameter arrays.
 */

#include "CParameterArray.h"
#include "CParameter.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdexcept>



class SimpleMaker : public CParameterArray::CConstructor
{
public:
  virtual CParameter* operator()(const char* pName) {
    return new CParameter(pName);
  }
};

class LowHiMaker : public CParameterArray::CConstructor
{
  double m_low;
  double m_high;
public:
  LowHiMaker(double low, double hi) : m_low(low), m_high(hi) {}

  virtual CParameter* operator()(const char* pName) {
    return new CParameter(pName, m_low, m_high);
  }
};
class UnitsMaker : public CParameterArray::CConstructor
{
  double m_low;
  double m_high;
  std::string m_units;
public:
  UnitsMaker(double low, double high, const char* pUnits) :
    m_low(low), m_high(high), m_units(pUnits) {}
  virtual CParameter* operator()(const char* pName) {
    return new CParameter(pName, m_low, m_high, m_units.c_str());
  }
};
class  BinsMaker : public CParameterArray::CConstructor
{
  double m_low;
  double m_high;
  unsigned m_bins;
  std::string m_units;
public:
  BinsMaker(double low, double high, unsigned bins, const char* pUnits) :
    m_low(low), m_high(high), m_bins(bins), m_units(pUnits) {}
  virtual CParameter* operator()(const char* pName) {
    return new CParameter(pName, m_low, m_high, m_bins, m_units.c_str());
  }
};

/**
 * constructor
 *
 * @param pBaseName - Base name of the array being created.
 * @param num       - Size of the array.
 * @param indexLow  - Smallest index (largest is indexLow + num - 1).
 */
CParameterArray::CParameterArray(const char* pBaseName, unsigned num, unsigned indexLow) :
  m_indexLow(indexLow), m_elementCount(num), m_rangeCheck(false)
{
  SimpleMaker m;
  makeParameters(m, pBaseName, num, indexLow);
  
}
/**
 * constructor
 *
 * @param pBaseName - Base name of the array being created.
 * @param low       - Low value of the parameter.
 * @param high      - High value of the parameter.
 * @param num       - Size of the array.
 * @param indexLow  - Smallest index (largest is indexLow + num - 1).
 */
CParameterArray::CParameterArray(const char* pBaseName, double low, double high,
				 unsigned num, unsigned indexLow) :
  m_indexLow(indexLow), m_elementCount(num), m_rangeCheck(false)
{

  LowHiMaker m(low, high);
  makeParameters(m, pBaseName, num, indexLow);

}
/**
 * constructor
 *
 * @param pBaseName  - Base name of the array.
 * @param low        - Axis low limit on the parameter.
 * @param high       - High limit on the parameter.
 * @param pUnits     - Units of measure associated with the parameter.
 * @param num        - Number of parameters to create.
 * @param indexLow   - Index of the first parameter
 */

CParameterArray::CParameterArray(const char* pBaseName, double low, double high, 
		const char* pUnits,
		unsigned num, unsigned indexLow) :
  m_indexLow(indexLow), m_elementCount(num), m_rangeCheck(false)
{
  UnitsMaker m(low, high, pUnits);
  makeParameters(m, pBaseName, num, indexLow);
}
/**
 * Constructor
 *
 * @param pBaseName  - Base name of the array.
 * @param low        - Axis low limit on the parameter.
 * @param high       - High limit on the parameter.
 * @param bins       - Suggested axis binnint
 * @param pUnits     - Units of measure associated with the parameter.
 * @param num        - Number of parameters to create.
 * @param indexLow   - Index of the first parameter
 */

CParameterArray::CParameterArray(const char* pBaseName, 
		double low, double high, unsigned bins, const char* pUnits,
		unsigned num, unsigned indexLow) :
  m_indexLow(indexLow), m_elementCount(num), m_rangeCheck(false)
{
  BinsMaker m(low, high, bins, pUnits);
  makeParameters(m, pBaseName, num, indexLow);
}
/**
 * destructor
 *   Delete all the parameters we created.  The vector can take care of itself.
 */
CParameterArray::~CParameterArray()
{
  for (int i = 0; i < m_array.size(); i++) {
    delete m_array[i];
  }
}

/**
 * enableRangeCheck
 *   Turns on index range checking.
 */
void 
CParameterArray::enableRangeCheck()
{
  m_rangeCheck = true;
}
/**
 * disableRangeCheck
 *    Disables array index range checking.
 */
void
CParameterArray::disableRangeCheck() 
{
  m_rangeCheck = false;
}
/**
 * operator[]
 *   Return a reference to an element of the array.
 *
 * @param i  - Element to return.
 * @return CParameter& reference to the parameter indexed.
 *
 * @throw std::range_error - if range checking is enabled and
 *        the index is out of range of the array.
 */
CParameter&
CParameterArray::operator[](unsigned i)
{
  if (m_rangeCheck) {
    if ((i < m_indexLow) || ((i - m_indexLow) >= m_elementCount)) {
      throw std::range_error("CParameterArray index is out of bounds");
    }
  }
  unsigned index = i - m_indexLow;

  return *(m_array[i - m_indexLow]);
}
/**
 * changeLow 
 *   Change the low value for all array elements.
 *
 * @param newLow - New low value.
 */
void 
CParameterArray::changeLow(double newLow) {
  for (int i = 0; i < m_array.size(); i++) {
    m_array[i]->changeLow(newLow);
  }
}
/**
 * changeHigh
 *   Change the high value for all array elements.
 *
 * @param newHigh - new high limit value.
 */
void
CParameterArray::changeHigh(double newHigh) 
{
  for (int i =0; i < m_array.size(); i++) {
    m_array[i]->changeHigh(newHigh);
  }
}
/**
 * changeBins
 *
 *  Change the recommended binning for all members of the array.
 *
 * @param newBins New suggested binning
 */
void
CParameterArray::changeBins(unsigned newBins)
{
  for (int i =0; i < m_array.size(); i++) {
    m_array[i]->changeBins(newBins);
  }
}
/**
 * changeUnits
 *   Changes the units of measure fro all elements of the array.
 *
 * @param newUnits  - The new units of measure.
 */
void
CParameterArray::changeUnits(const char* newUnits)
{
  for (int i = 0; i < m_array.size(); i++) {
    m_array[i]->changeUnits(newUnits);
  }
}
/**
 * enableLimitCheck
 *
 *   Turns on value limit checking for all array elements.
 */
void
CParameterArray::enableLimitCheck()
{
  for (int i = 0; i < m_array.size(); i++) {
    m_array[i]->enableLimitCheck();
  }
}
/**
 * disableLimitCheck
 *
 *  Turns off value limit checking for all array elements.
 */
void
CParameterArray::disableLimitCheck()
{
  for (int i = 0; i < m_array.size(); i++) {
    m_array[i]->disableLimitCheck();
  }
}
/*----------------------------------------------------------------------------
 * private utilities.
 */

/**
 * makeParameters
 *
 *  Used the passed in constructor object to create the set of parameters
 *  demanded by n and lowIndex.
 *
 * @param pBaseName - base name for the array.
 * @param maker   - Functor object that can create on CParameter.
 * @param n       - Number of parameters to create.
 * @param lowIndex- Index of the first parameter.
 */
void
CParameterArray::makeParameters(CParameterArray::CConstructor& maker, 
				const char* pBaseName, unsigned n, 
				unsigned lowIndex)
{

  //  how many digits in the biggest index:

  unsigned lastIndex = lowIndex + n - 1;
  unsigned digits    = (int)(log10((double)(lastIndex))) + 1;

  //                                       .            \0
  unsigned namesize  = strlen(pBaseName) + 1 + digits + 1;
  char*    name = new char[namesize];
  for (int i = lowIndex; i <= lastIndex; i++) {
    sprintf(name, "%s.%0*d", pBaseName, digits, i);
    CParameter* pParam = maker(name); 
    m_array.push_back(pParam);
  }
  delete []name;
}
