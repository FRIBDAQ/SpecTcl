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

#ifndef XAMINEMAP2D_H  //Required for current class
#define XAMINEMAP2D_H
                               //Required for base classes
#include <histotypes.h>
#include <Xamine.h>


class CXamineMap2D        
{
  Float_t m_nXLow;        // The low limit of the x-transformation
  Float_t m_nYLow;        // The low limit of the y-transform
  Float_t m_nXHigh;       // The high limit of the x-transformation
  Float_t m_nYHigh;       // The high limit of the y-transform
  std::string m_sXUnits;  // The units to label the mapped x-axis
  std::string m_sYUnits;  // The units to label the mapped y-axis
  
public:
  
  // Constructor:

  CXamineMap2D() :             // empty ctor for non-mapped spectrum
    m_nXLow(0),
    m_nYLow(0),
    m_nXHigh(0),
    m_nYHigh(0),
    m_sXUnits(""),
    m_sYUnits("")
    { }

  CXamineMap2D(Float_t am_nXLow, Float_t am_nYLow,
	       Float_t am_nXHigh, Float_t am_nYHigh,
	       const std::string& am_sXUnits, const std::string& am_sYUnits) :
    m_nXLow(am_nXLow),
    m_nYLow(am_nYLow),
    m_nXHigh(am_nXHigh),
    m_nYHigh(am_nYHigh),
    m_sXUnits(am_sXUnits),
    m_sYUnits(am_sYUnits)
    { }
  
  ~CXamineMap2D ( ) { }       //Destructor

  //Copy constructor
  CXamineMap2D(const CXamineMap2D& aCXamineMap2D )
    {   
      m_nXLow = aCXamineMap2D.m_nXLow;
      m_nYLow = aCXamineMap2D.m_nYLow;
      m_nXHigh = aCXamineMap2D.m_nXHigh;
      m_nYHigh = aCXamineMap2D.m_nYHigh;
      m_sXUnits = aCXamineMap2D.m_sXUnits;
      m_sYUnits = aCXamineMap2D.m_sYUnits;
    }                                     

  //Operator= Assignment Operator
  CXamineMap2D& operator= (const CXamineMap2D& aCXamineMap2D)
  { 
    if (this == &aCXamineMap2D) return *this;          
    m_nXLow = aCXamineMap2D.m_nXLow;
    m_nYLow = aCXamineMap2D.m_nYLow;
    m_nXHigh = aCXamineMap2D.m_nXHigh;
    m_nYHigh = aCXamineMap2D.m_nYHigh;
    m_sXUnits = aCXamineMap2D.m_sXUnits;
    m_sYUnits = aCXamineMap2D.m_sYUnits;
    
    return *this;
  }                                     

  //Operator== Equality Operator
  int operator== (const CXamineMap2D& aCXamineMap2D)
  { 
    return (
	    (m_nXLow == aCXamineMap2D.m_nXLow) &&
	    (m_nYLow == aCXamineMap2D.m_nYLow) &&
	    (m_nXHigh == aCXamineMap2D.m_nXHigh) &&
	    (m_nYHigh == aCXamineMap2D.m_nYHigh) &&
	    (m_sXUnits == aCXamineMap2D.m_sXUnits) &&
	    (m_sYUnits == aCXamineMap2D.m_sYUnits)
	    );
  }                             
  // Selectors:

public:

  Float_t getXLow() const
    {
      return m_nXLow;
    }
  Float_t getYLow() const
    {
      return m_nYLow;
    }
  Float_t getXHigh() const
    {
      return m_nXHigh;
    }
  Float_t getYHigh() const
    {
      return m_nYHigh;
    }
  std::string getXUnits() const
    {
      return m_sXUnits;
    } 
  std::string getYUnits() const
    {
      return m_sYUnits;
    }
};

#endif
