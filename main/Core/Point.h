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

//  CPoint.h:
//
//    This file defines the CPoint class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __POINT_H  //Required for current class
#define __POINT_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif
      
template <class T>                         
class Point      
{
private:
  T m_x;  // X Coordinate of the point.
  T m_y;  // Y coordinate of the point.
  
public:
			// Default constructor
  Point (T x = 0, T y = 0) :  m_x(x),  m_y(y)   { } 
  virtual ~ Point ( ) { }       //Destructor
	
                        // Copy constructor:
  Point (const Point& aPoint ) 
  {   
    m_x = aPoint.m_x;
    m_y = aPoint.m_y;
                
  }                                     

			//Operator= Assignment Operator

  Point& operator= (const Point& aPoint)
  { 
    if (this == &aPoint) return *this;          
  
    m_x = aPoint.m_x;
    m_y = aPoint.m_y;
        
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const Point& aPoint) const
  { return (
	    (m_x == aPoint.m_x) &&
	    (m_y == aPoint.m_y) 
	    );
  }                             
  int operator!=(const Point& aPoint) const
  {
    return !(operator==(aPoint));
  }
  // selectors:

public:
  T X() const
  {
    return m_x;
  }

                       //Get accessor function for attribute
  T Y() const
  {
    return m_y;
  }
  // Mutators - points are considered essentially constant so 
  //            these are only exposed to derived classes:
protected:
                       //Set accessor function for attribute
  void setX(T am_x)
  { 
    m_x = am_x;
  }
  void setY (T am_y)
  { 
    m_y = am_y;
  }
                       
};
// Useful typedefs which some clients may need:

typedef Point<Int_t> CPoint;
typedef Point<Float_t> FPoint;

typedef std::vector<CPoint> PointArray;
typedef PointArray::iterator PointIterator;

#endif
