///////////////////////////////////////////////////////////
//  CTreeVariableArray.cpp
//  Implementation of the Class CTreeVariableArray
//  Created on:      30-Mar-2005 11:03:53 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
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
#include "CTreeVariableArray.h"
#include "CTreeVariable.h"
#include "CTreeException.h"
#include "CTreeVariableProperties.h"

#include <stdio.h>
#include <math.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/**
 * Default constructor... Creates a tree variable array that has an empty vector,
 * and therefore no elements.
 */
CTreeVariableArray::CTreeVariableArray() :
  m_nFirstIndex(0)
{
}


/**
 * Creates a tree variable array completely constructied.  Taking a basename, an
 * initial value, units, size and first index.
 * Each of the tree variable constituents is created and inserted into the vector.
 * The names of the constituents will be
 * basename.nn where nn is the index number.
 * @param baseName
 *        base name of the array. This is used to determine the
 *        individual element names.
 * @param initialValue
 *        Initial value to be held by each of the elements.
 * @param units
 *        Units of measure associated with the variable.
 * @param size
 *        Number of array elements.
 * @param firstIndex
 *        Lowest valued index.
 * 
 */
CTreeVariableArray::CTreeVariableArray(string baseName, double initialValue, 
				       string units, UInt_t size, int firstIndex)
{

  Initialize(baseName, initialValue, units, size, firstIndex);
  

}


/**
 * Copy constructor.  The array of variables is deep copied to produce a duplicate
 * set, bound to the same underlying variables... unless the vector is empty in
 * which case we have duplicated an empty variable.
 * @param rhs    The source of the copy construction
 * 
 */
CTreeVariableArray::CTreeVariableArray(const CTreeVariableArray& rhs)
{

  CopyArray(rhs);


}


/**
 * Destroys the tree variable and its contents.  Note that while the binding to
 * the underlying TCLVariables is obviously undone, the variables themselves are
 * left alone.
 */
CTreeVariableArray::~CTreeVariableArray()
{

  DestroyArray();


}


/**
 * Builds up the new array.
 * @param basename    
 *        Base name used to construct the individual names.
 * @param size
 *        Number of elements in the array.
 * @param example
 *        Sample properties that will be used to construct the
 *        individual elements.
 * 
 */
void 
CTreeVariableArray::BuildArray(string basename, unsigned int size, 
			       const CTreeVariableProperties& example)
{

  DestroyArray();                    // Get rid of prior array if it exists.
  
  // Figure out the format in which names  will be given.
  // Note that given the fact that the initial index may be negative,
  // it's possible that the lower index will determine the number of digits
  // needed.

  double highindex = size + m_nFirstIndex;
  double highdigits  =  log10(fabs((double)highindex)) + 1.0;
  
  double lowdigits   =  log10(fabs((double)m_nFirstIndex)) + 1.0;
  int digits         =  (int)max(highdigits, lowdigits);
  
  char format[100];
  snprintf(format, sizeof(format), "%s.%%%d.%dd", basename.c_str(), digits, digits);
  
  // Now loop through creating the tree paramters:
  
  for (int i =0; i < size; i++)  {
    int index = i + m_nFirstIndex;
    char elementName[100];
    snprintf(elementName, sizeof(elementName), format, index);
    CTreeVariableProperties* pProperties  =
                 new CTreeVariableProperties(elementName, example);
    CTreeVariable*  pVariable   = new  CTreeVariable(*pProperties);
    m_TreeVariables.push_back(pVariable);
    
  }
  
  
}


/**
 * Destroys the contents of the array and empties it out.  Note that while the
 * tree
 * variables get destroyed, the underlying properties don't.. .this ensures sanity
 * if
 * - There are other tree variables bound to the same properties element.
 * - The variable is linked to a TCL variable already.
 */
void CTreeVariableArray::DestroyArray()
{
  
  while (!m_TreeVariables.empty()) {
     CTreeVariable* pVariable =  m_TreeVariables.back();
     m_TreeVariables.pop_back();
     delete pVariable;
  }


}


/**
 * Copys the member elements from a rhs CTreeVariableArray into the current object.
 *  This utility is used both by assignment and copy construction.
 * @param rhs
 *        The source of the copy.
 * 
 */
void CTreeVariableArray::CopyArray(const CTreeVariableArray&  rhs)
{
  
  DestroyArray();                      // Get rid of any pre-existing array.
  
  // Duplicate the rhs. into us:
  
  m_nFirstIndex      = rhs.m_nFirstIndex;
  int         n      = rhs.m_TreeVariables.size();
  int     index      = m_nFirstIndex;
  while(n) {
    CTreeVariable& p(((CTreeVariableArray&)rhs)[index]);
    m_TreeVariables.push_back(new CTreeVariable(p));
    index++;
    n--;
  }

}


/**
 * Intended for use with a variable array that was default constructed.Any
 * existing array is destroyed, and a new one built.
 * @param baseName
 *        Name used to construct element names.
 * @param initialValue
 *        The value to be assigned to the elements.
 * @param units
 *        The units of measure of the elements of the array.
 * @param size
 *        number of array elements.
 * @param firstElement
 *        Lowest numbered index.
 * 
 */
void 
CTreeVariableArray::Initialize(string baseName, double initialValue, 
			       string units, UInt_t size, Int_t firstElement)
{

  DestroyArray();
  
  // Create the sample properties object that will be cloned into our
  // elements:
  
  CTreeVariableProperties example(baseName, initialValue,  units);
  example = initialValue;
  
  m_nFirstIndex = firstElement;
  BuildArray(baseName, size, example);
  

}


/**
 * Indexes in to the array of tree variables and returns the selected element
 * (throw on range errors).
 * 
 * @param index
 *        Index into the array.
 * 
 */
CTreeVariable& 
CTreeVariableArray::operator[](Int_t index)
{

  int vectorIndex = index - m_nFirstIndex;
  
  
  if ((vectorIndex < 0) || (vectorIndex >= m_TreeVariables.size())) {
    throw CTreeException(CTreeException::InvalidIndex, "CTreeVariableArray::operator[]");
  }
  return *(m_TreeVariables[vectorIndex]);


}


/**
 * The current array is destroyed and a new one built from the specification of
 * the rhs.  Note that this is done much like a copy construction.
 * @param rhs
 * 
 */
CTreeVariableArray&
CTreeVariableArray::operator=(const CTreeVariableArray& rhs)
{
  
  DestroyArray();
  CopyArray(rhs);
  
  return *this;

}
/**
 * return the number of elements in the array (could be 0).
 *
 */
UInt_t 
CTreeVariableArray::size()
{
  return m_TreeVariables.size();
}
/**
 *  Return the value of the lowest index (could be negative).
 */
Int_t
CTreeVariableArray::firstIndex()
{
  return m_nFirstIndex;
}
