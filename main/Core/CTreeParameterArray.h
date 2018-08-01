///////////////////////////////////////////////////////////
//  CTreeParameterArray.h
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



#ifndef CTREEPARAMETERARRAY_H
#define CTREEPARAMETERARRAY_H

#include <histotypes.h>
#include <vector>
#include <string>


// Forward definitions.

class CTreeParameter;

/**
 * This class is a container for an array of Tree parameters.  Given a base
 * parameter name, it will create a set of parameters named basename.n  where n is
 * a number.  The number will be left filled with zeroes to ensure that it sorts
 * correctly. Eg.:
 * \verbatim
 * basename.00
 * basename.01
 * ..
 * basename.17
 * \endverbatim
 * for an 18 element array.
 * @author Ron Fox
 * @version 1.0
 * @created 30-Mar-2005 11:03:51 AM
 */
class CTreeParameterArray
{
private:

  /**
   * Index of first element (there's really no reason this could not also be
   * negative BTW).
   */
  int m_nFirstIndex;  
  std::vector<CTreeParameter*> m_Parameters;
  
public:
  /**
   * Vector of parameter pointers.
   */
  CTreeParameterArray();
  CTreeParameterArray(std::string baseName, 
		      UInt_t resolution, UInt_t numElements, Int_t baseIndex);
  CTreeParameterArray(std::string baseName, 
		      UInt_t resolution, 
		      double lowLimit, double highOrWidth, 
		      std::string units, bool widthOrHighGiven, 
		      UInt_t elements, Int_t firstIndex);
  CTreeParameterArray(std::string baseName, UInt_t elements, Int_t baseIndex);
  CTreeParameterArray(std::string baseName, std::string units, 
		      UInt_t elements, Int_t firstIndex);
  CTreeParameterArray(std::string baseName, 
		      double low, double high, std::string units, 
		      UInt_t elements, Int_t firstIndex);
  CTreeParameterArray(std::string baseName, UInt_t channels, 
		      double low, double high, std::string units, 
		      UInt_t elements, Int_t firstIndex);
  ~CTreeParameterArray();

  CTreeParameter& operator[](Int_t nIndex);
  void Reset();
  void Initialize(std::string baseName, UInt_t resolution, 
		  UInt_t elements, Int_t baseIndex);
  void Initialize(std::string baseName, UInt_t resolution, 
		  double lowLimit, double widthOrHeight,
		  std::string units, bool widthOrHeightGiven, 
		  UInt_t elements, Int_t firstIndex);
  void Initialize(std::string baseName, UInt_t elements, Int_t firstIndex);
  void Initialize(std::string baseName, std::string units, UInt_t elements, 
		  Int_t firstIndex);
  void Initialize(std::string baseName, double lowLimit, double highLimit, 
		  std::string units, UInt_t elements, Int_t firstIndex);
  void Initialize(std::string baseName, UInt_t channels, 
		  double lowLimit, double highLimit, std::string units, 
		  UInt_t elements, Int_t firstIndex);
  std::vector<CTreeParameter*>::iterator begin();
  std::vector<CTreeParameter*>::iterator end();
  UInt_t size();
  Int_t lowIndex();
  bool isBound() const;
  void Bind();
protected:
  void CreateParameters(std::string baseName, 
			UInt_t size, CTreeParameter& Template);
  void DeleteParameters();
  
  
};




#endif
