///////////////////////////////////////////////////////////
//  CTreeParameterArray.cpp
//  Implementation of the Class CTreeParameterArray
//  Created on:      30-Mar-2005 11:03:51 AM
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
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//


#include <config.h>
#include "CTreeParameterArray.h"
#include "CTreeParameter.h"
#include "CTreeException.h"

#include <stdio.h>

#include <math.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

static inline float
rfmax(double a, double b) 
{
  if (a > b) {
    return a;
  } else {
    return b;
  }
}

/**
 * Default constructor.  Builds a tree parameter with no elements. 
 *  m_nFirstIndex  = 0.
 */
CTreeParameterArray::CTreeParameterArray() :
  m_nFirstIndex(0)
{

}


/**
 * Builds a tree parameter array. The parameter base name is supplied as a
 * parameter as is the resolution of the parameters, the size of the array, and
 * the initial index of the array (defaulting to zero).  Tree parameter names are
 * created and the tree parameter generated.
 * @param baseName    The name of the array to create.
 * @param resolution    Bits of resolution in each element of the array.
 * @param numElements    Size of the array.
 * @param baseIndex    First index (may be negative!).
 * 
 */
CTreeParameterArray::CTreeParameterArray(string baseName, 
					 UInt_t resolution, UInt_t numElements, 
					 Int_t baseIndex)
{
  Initialize(baseName, resolution, numElements, baseIndex);
}


/**
 * Constructs a tree parameter array.  The parameters are generated and registered,
 * using the name, resolution, start, stop/width, and units passed in.  The size
 * of the array is also fixed by parameter and the first index, if nessary,
 * defaults to 0.
 * @param baseName
 * @param resolution    Number of bits of resolution in the underlying parameters.
 * @param lowLimit    Parameter default low limit.
 * @param highOrWidth    Channel width or high limit.
 * @param units    Units of measure.
 * @param widthOrHighGiven    Indicates what the highOrWidth actually provides.
 * @param elements    Number of array elements.
 * @param firstIndex    Index (possibly negative) of the first array element.
 * 
 */
CTreeParameterArray::CTreeParameterArray(string baseName, UInt_t resolution, 
					 double lowLimit, double highOrWidth, 
					 string units, bool widthOrHighGiven,
					 UInt_t elements, Int_t firstIndex)
{
  Initialize(baseName, resolution, lowLimit, highOrWidth, units, widthOrHighGiven,
	     elements, firstIndex);
}


/**
 * Constructs a tree parameter array from the basename of the parameter, the size
 * of the array and base index.
 * @param baseName    Name of the array.
 * @param elements    Number of elements in the array.
 * @param baseIndex    Lowest (possibly negative) array index.
 * 
 */
  CTreeParameterArray::CTreeParameterArray(string baseName, UInt_t elements, 
					   Int_t baseIndex)
{
  Initialize(baseName, elements, baseIndex);
}


/**
 * Creates a tree parameter array given the array base name, the size, base index
 * and units of the parameter.
 * @param baseName    Name of the array.
 * @param units    Parameter units of measure.
 * @param elements    Number of elements in the array.
 * @param firstIndex    Base index into the array (possbily negative!).
 * 
 */
CTreeParameterArray::CTreeParameterArray(string baseName, string units, 
					 UInt_t elements, Int_t firstIndex)
{
  Initialize(baseName, units, elements, firstIndex);
}


/**
 * Creates a tree parameter array given the basename, the start index and size,
 * and the axis limits as well as units.
 * @param baseName    Name of the array.
 * @param low    Low axis default limit for the parameters.
 * @param high    default parameter axis high limits.
 * @param units    Units of measuer of the treeparameter.
 * @param elements    Number of elements in the array.
 * @param firstIndex    lowest acceptable index.
 * 
 */
CTreeParameterArray::CTreeParameterArray(string baseName, 
					 double low, double high, 
					 string units, UInt_t elements, 
					 Int_t firstIndex)
{
  Initialize(baseName, low, high, units, elements, firstIndex);
}


/**
 * Creates a tree parameter array given the name, size and starting index.  For
 * the parameters we also have a common set of axis limits and default channel
 * count.
 * @param baseName    Name of the array
 * @param channels    Number of channels on a default GUI axis.
 * @param low    Low liimit on a default GUI generated axis.
 * @param high    High limit on a default GUI generated axis.
 * @param units    Units of measure of the parameter.
 * @param elements    Number of elements in the array
 * @param firstIndex    Index to use to get to the first array element (could be
 * negative!).
 * 
 * 
 */
CTreeParameterArray::CTreeParameterArray(string baseName, UInt_t channels, 
					 double low, double high, string units, 
					 UInt_t elements, Int_t firstIndex)
{
  Initialize(baseName, channels, low, high, units, elements, firstIndex);
}


/**
 * Destroys the array.. also destroys all the elements in the array.
 */
CTreeParameterArray::~CTreeParameterArray()
{
  DeleteParameters();
}


/**
 * Creates the parameters associated with this array, the name, units, low, high
 * and channels are used to define the parameter.
 * @param baseName    The base name of the array. Parameters will be created with
 * names of the form baseName.nnn  where nnn has as many zero filled digits as
 * required to be the largest index.  Note that if the array includes negative
 * indices these will look like: baseName.-005  for example... a perfectly legal
 * parameter number and, given the  english character set collating sequence will
 * sort properly.
 * 
 * 
 * @param size    
 *        Number of array elements to create.
 * @param Template
 *        Tree parameter that will be duplicated (except for the name
 *        obviously) to create the array.
 * 
 */
void 
CTreeParameterArray::CreateParameters(string baseName, UInt_t size, 
				      CTreeParameter& Template)
{
  
  //// Get rid off any prior array contents.
  //
  //
  DeleteParameters();
  //
  //// Figure out the worst case number of digits in the index... for large
  //  negative  m_nFirstIndex, this may be at the lower index range.
  //
  double lowdigits = log10((double)(abs((long int)m_nFirstIndex))) + 1.0;
  double hidigits  = log10((double)(abs((long int)(m_nFirstIndex + size)))) + 1.0;
  
  int  numDigits    = (int)(rfmax(lowdigits, hidigits));
  
  //// Produce the digit creation format
  //
  char format[100];
  snprintf(format, sizeof(format), "%s.%%%d.%dd", baseName.c_str(), 
	   numDigits, numDigits);
  
  for (int i =0; i < size; i++) {
    int index = i + m_nFirstIndex; // The element number the user will use.
    char name[100];
    snprintf(name, sizeof(name),  format,  index);
  
    CTreeParameter* pParameter = new CTreeParameter(name,  Template);
    m_Parameters.push_back(pParameter);
  }
}


/**
 * Deletes all the parameters this vector holds.
 */
void 
CTreeParameterArray::DeleteParameters()
{

  // Keep deleting elements off the back of the array
  // until there are none left.
  //
  while(!m_Parameters.empty()) {
    delete m_Parameters.back();
    m_Parameters.pop_back();
  }
  
  

}


/**
 * Indexes into the tree parameter.  If the array index is out of bounds, throws
 * an exception (CTreeException with BadIndex reason).
 * @param nIndex    
 *        Index into the array.  Note that the index is adjusted by
 *        subtracting m_nFirstIndex.
 * 
 */
CTreeParameter& 
CTreeParameterArray::operator[](Int_t nIndex)
{

  nIndex -= m_nFirstIndex;	// Remove first index bias.
  if ((nIndex < 0) || (nIndex >=  m_Parameters.size())) {
    throw CTreeException(CTreeException::InvalidIndex, "CTreeParamterArray::operator[]");
  }
  return *(m_Parameters[nIndex]);
  
  
}


/**
 * Invalidates the values of all elements of the tree parameter array.
 */
void 
CTreeParameterArray::Reset()
{
  
  vector<CTreeParameter*>::iterator p = begin();
  while (p != end()) {
    (*p)->Reset();
    p++;
  }
  
  
}


/**
 * Initializes a parameter array given the basename, resolution, base index and
 * size.  If the arrary of parameters is not empty, its elements will first be
 * deleted.
 * @param baseName
 *        Name of the vector... used to derive then names of the
 *        elements.
 * 
 * @param resolution
 *        Number of bits of resolution in the parameters that will
 *        be created.
 * @param elements
 *        Number of array elements.
 * @param baseIndex
 *       Lowest acceptable index value for the array (indices go
 *       from baseIndex -> baseIndex+elements -1.
 * 
 */
void 
CTreeParameterArray::Initialize(string baseName, UInt_t resolution, 
				UInt_t elements, Int_t baseIndex)
{

  m_nFirstIndex = baseIndex;
  //
  // Below we create a template parameter that we then hand off to
  // CreateParameters to create the rest:
  
  CTreeParameter sample(baseName, resolution); // Name is not important.

  // Create elements of them using sample as a template.

  CreateParameters(baseName, elements,  sample); 


}


/**
 * Given the basename, resolution and range of the underlying parameters (either
 * in [low, high] or in low slope form), initializes data members and creates the
 * underlying paramters.  If m_Parametesr is not empty, it will be emptied and all
 * its elements deleted.
 * @param baseName    
 *        Name of the vector... used to derive the name of the
 *        underlying parameters.
 * @param resolution
 *        Number of bits of resolution in the parameters that will
 *        be created.
 * @param lowLimit
 *        Default low axis limits for axes created on this parameter
 *        by the GUI.
 * @param widthOrHeight
 *        Either the channel width or upper limit of axes created
 *        by the GUI on this parameter.
 * @param units
 *        Units of measure associated with the parameter.
 * @param widthOrHeightGiven
 *        - true - the parameter widthOrHeight is the
 *          channel width.
 *        - false - The parameter widthOrHeight is the high limit.
 * @param elements
 *        Number of array elements to create.
 * @param firstIndex
 *        First permissible index into the array.
 * 
 */
void 
CTreeParameterArray::Initialize(string baseName, UInt_t resolution, 
				double lowLimit, double widthOrHeight, 
				string units, bool widthOrHeightGiven, 
				UInt_t elements, Int_t firstIndex)
{
  
  m_nFirstIndex = firstIndex;
  CTreeParameter Sample(baseName, resolution, lowLimit, widthOrHeight,
			units, widthOrHeightGiven);
  CreateParameters(baseName, elements, Sample);
}


/**
 * Given a base parameter name,  a base index and size, initializes member data,
 * including creating the subordinate parameter.  If m_Parameters is not empty,
 * the array is emptied and all parameters destroyed.
 * @param baseName    Name of the vector, also used to derive names of the
 * elements of the vector.
 * @param elements    
 *        Number of tree parameters created and put in the vector.
 * @param firstIndex
 *        Lowest legal index value.
 * 
 */
void 
CTreeParameterArray::Initialize(string baseName, UInt_t elements, 
				Int_t firstIndex)
{
  m_nFirstIndex = firstIndex;
  CTreeParameter Sample(baseName);
  CreateParameters(baseName, elements, Sample);
}


/**
 * Given a parameter name, units, staring index and array size, builds the array.
 * Note that if the m_Parameters element is already nonempty, it is cleared and
 * all elements deleted.
 * @param baseName
 *        Name of the array to create.
 * @param units
 *        Units of measure of the parameters being created.
 * @param elements
 *        Number of elements to create.
 * @param firstIndex
 *        Index of the first legal array element.
 * 
 */
void CTreeParameterArray::Initialize(string baseName, string units, 
				     UInt_t elements, Int_t firstIndex)
{
  m_nFirstIndex = firstIndex;
  CTreeParameter Sample(baseName, units);
  CreateParameters(baseName, elements, Sample);
}


/**
 * Given the basename of an array, then low and high default limits, and units,
 * the size of the array and smallest index,  initializes the array.  This
 * involves creating the subordinate parameters and putting them into m_Parameters.
 *  Note that if m_Parameters is not empty, its elements are deleted and the array
 * is cleared.
 * @param baseName
 *        Name of the vector we will create.
 * @param lowLimit
 *        Low limit of the underlying parameters when the GUI creates
 *        a default axis involving this parameter
 * @param highLimit
 *        High limit of the underlying parameters when the GUI
 *        creates a default axis involving this parameter
 * @param units
 *        Units of measure of the underlying parameter.
 * @param elements
 *        Number of elements in the array.
 * @param firstIndex
 *        Smallest acceptable index into  the array.
 * 
 */
void 
CTreeParameterArray::Initialize(string baseName, double lowLimit, 
				double highLimit, string units, 
				UInt_t elements, Int_t firstIndex)
{
  m_nFirstIndex = firstIndex;
  
  CTreeParameter Sample(baseName, lowLimit, highLimit, units);
  CreateParameters(baseName, elements, Sample);
}


/**
 * Given the basename, the default bins, limits, and units, the size and starting
 * index, creates the array parameter.  If m_Parameters is nonempty, its elements
 * are deleted and the vector is cleared. Tree parameters are created for each
 * element and put into the vector.
 * @param baseName    
 *        Name of the vector.
 * @param channels
 *        Number of channels in default GUI made axes.
 * @param lowLimit
 *        Low paramete rlimit for axes created on this parameter by
 *        the GUI.
 * @param highLimit
 *        High limit of axes created by GUI by default for this
 *        parameter.
 * @param units
 *        Units of measure for the parameters
 * @param elements
 *        Number of elements in the array.
 * @param firstIndex
 *        Element of first array index.
 * 
 */
void CTreeParameterArray::Initialize(string baseName, UInt_t channels, 
				     double lowLimit, double highLimit, 
				     string units, UInt_t elements, 
				     Int_t firstIndex)
{
  m_nFirstIndex = firstIndex;
  CTreeParameter Sample(baseName, channels, lowLimit, highLimit, units);
  CreateParameters(baseName, elements, Sample);
}


/**
 * Returns begin iteration iterator.
 */
vector<CTreeParameter*>::iterator 
CTreeParameterArray::begin()
{
  return m_Parameters.begin();
}


/**
 * Returns end of iteration iterator.
 */
vector<CTreeParameter*>::iterator 
CTreeParameterArray::end()
{
  return m_Parameters.end();
}


/**
 * Returns number of elements in the array.
 */
UInt_t 
CTreeParameterArray::size()
{
  return m_Parameters.size();
}


/**
 * Returns the smalles accepted index in the array.
 */
Int_t 
CTreeParameterArray::lowIndex()
{
  return m_nFirstIndex;
}



