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
                               
class CPoint      
{
private:
  Int_t m_x;  // X Coordinate of the point.
  Int_t m_y;  // Y coordinate of the point.
  
public:
			// Default constructor
  CPoint (Int_t x = 0, Int_t y = 0) :  m_x(x),  m_y(y)   { } 
  virtual ~ CPoint ( ) { }       //Destructor
	
                        // Copy constructor:
  CPoint (const CPoint& aCPoint ) 
  {   
    m_x = aCPoint.m_x;
    m_y = aCPoint.m_y;
                
  }                                     

			//Operator= Assignment Operator

  CPoint& operator= (const CPoint& aCPoint)
  { 
    if (this == &aCPoint) return *this;          
  
    m_x = aCPoint.m_x;
    m_y = aCPoint.m_y;
        
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CPoint& aCPoint) const
  { return (
	    (m_x == aCPoint.m_x) &&
	    (m_y == aCPoint.m_y) 
	    );
  }                             
  int operator!=(const CPoint& aCPoint) const
  {
    return !(operator==(aCPoint));
  }
  // selectors:

public:
  Int_t X() const
  {
    return m_x;
  }

                       //Get accessor function for attribute
  Int_t Y() const
  {
    return m_y;
  }
  // Mutators - points are considered essentially constant so 
  //            these are only exposed to derived classes:
protected:
                       //Set accessor function for attribute
  void setX(Int_t am_x)
  { 
    m_x = am_x;
  }
  void setY (Int_t am_y)
  { 
    m_y = am_y;
  }
                       
};
// Useful typedefs which some clients may need:

typedef vector<CPoint> PointArray;
typedef PointArray::iterator PointIterator;

#endif
