/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321

*/
//  CXamineMap1D.h:
//
//    This file defines the CXamineMap1D class.
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venema@nscl.msu.edu
//
//  Copyright 2002 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef XAMINEMAP1D_H  //Required for current class
#define XAMINEMAP1D_H
                               //Required for base classes
#include "Xamine1D.h"
#include "Xamine2D.h"
#include <histotypes.h>
#include <Xamine.h>


class CXamineMap1D        
{
  Float_t m_nLow;        // The low limit of the transformation
  Float_t m_nHigh;       // The high limit of the transformation
  std::string m_sUnits;  // The units to label the mapped axis with
  
public:
  
  // Constructor:

  CXamineMap1D() :              // empty ctor for non-mapped spectrum
    m_nLow(0),
    m_nHigh(0),
    m_sUnits("")
    { }

  CXamineMap1D(Float_t am_nLow, Float_t am_nHigh,
	       const std::string& am_sUnits) :
    m_nLow(am_nLow),
    m_nHigh(am_nHigh),
    m_sUnits(am_sUnits)
    { }
  
  ~CXamineMap1D ( ) { }       //Destructor

  //Copy constructor
  CXamineMap1D(const CXamineMap1D& aCXamineMap1D )
    {   
      m_nLow = aCXamineMap1D.m_nLow;
      m_nHigh = aCXamineMap1D.m_nHigh;
      m_sUnits = aCXamineMap1D.m_sUnits;
    }                                     

  //Operator= Assignment Operator
  CXamineMap1D& operator= (const CXamineMap1D& aCXamineMap1D)
  { 
    if (this == &aCXamineMap1D) return *this;          
    m_nLow = aCXamineMap1D.m_nLow;
    m_nHigh = aCXamineMap1D.m_nHigh;
    m_sUnits = aCXamineMap1D.m_sUnits;
    
    return *this;
  }                                     

  //Operator== Equality Operator
  int operator== (const CXamineMap1D& aCXamineMap1D)
  { 
    return (
	    (m_nLow == aCXamineMap1D.m_nLow) &&
	    (m_nHigh == aCXamineMap1D.m_nHigh) &&
	    (m_sUnits == aCXamineMap1D.m_sUnits)
	    );
  }                             
  // Selectors:

public:

  Float_t getLow() const
    {
      return m_nLow;
    }
  Float_t getHigh() const
    {
      return m_nHigh;
    }
  std::string getUnits() const
    {
      return m_sUnits;
    } 
};

#endif
