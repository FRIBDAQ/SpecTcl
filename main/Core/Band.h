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


//  CBand.h:
//
//    This file defines the CBand class.
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
/*
   Change Log:
   $Log$
   Revision 5.2  2005/06/03 15:19:04  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.2  2005/05/27 17:47:36  ron-fox
   Re-do of Gamma gates also merged with Tim's prior changes with respect to
   glob patterns.  Gamma gates:
   - Now have true/false values and can therefore be applied to spectra or
     take part in compound gates.
   - Folds are added (fold command); and these perform the prior function
       of gamma gates.

   Revision 5.1.2.1  2004/12/21 17:51:21  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:56:01  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.7  2003/08/25 16:25:31  ron-fox
   Initial starting point for merge with filtering -- this probably does not
   generate a goo spectcl build.

   Revision 4.6  2003/04/15 19:15:46  ron-fox
   To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/

#ifndef BAND_H  //Required for current class
#define BAND_H
                               //Required for base classes
#include "PointlistGate.h"
#include <histotypes.h>
#include "Point.h"
#include <vector>
#include <string>

/*!
  \par
  Defines a band gate.  A band is a polyline in parameter space.
  All points \em below the polyline are considered to be in the gate
  The left most endpoint of the gate is assumed to represent a
  line extending to negative infinity as  is the right most point.
  
  \par 
  The algorithm used to check the gates is to extend a horizontal
  line from the point to check to x = negative infinity and to 
  count the number of boundaries crossed by that line.  An
  even number indicates outsidedness, an odd number insidedness.
  This algorithm works with bands that have sawtooths and other
  pathological shapes... including loops in a consistent and
  intuitive way.  Note that 0 is even.

 */              
class CBand  : public CPointListGate        
{
private:
  FPoint   m_LeftLimit;		//!< Point at left most limit.
  FPoint   m_RightLimit;	//!< Point at right most limit.

public:
			//Default constructor

  CBand (UInt_t nXid, UInt_t nYid,
	 const std::vector<FPoint>& points);
  CBand (UInt_t nXid, UInt_t nYid,
	 UInt_t nPts, FPoint* pPoints);
  CBand (UInt_t nXid, UInt_t nYid,
	 UInt_t nPts, Float_t* pX, Float_t* pY);
  virtual ~CBand ( ) 
  { }
 

			//Copy constructor

  CBand (const CBand& aCBand )   : 
    CPointListGate (aCBand) ,
    m_LeftLimit(aCBand.m_LeftLimit),
    m_RightLimit(aCBand.m_RightLimit)
  {   
                
  }                                     

			//Operator= Assignment Operator

  CBand& operator= (const CBand& aCBand)
  { 
    if (this == &aCBand) return *this;          
    CPointListGate::operator= (aCBand);
    m_LeftLimit = aCBand.m_LeftLimit;
    m_RightLimit= aCBand.m_RightLimit;
    
    return *this; 
  }                                     

			// Operator== Equality Operator
                        // Gates have no equality operator.

  int operator== (const CBand& aCBand) const;
public:
  // Selectors

  //  Operations.
  //
public:                   
  virtual   Bool_t operator()(CEvent& rEvent);
  virtual   CGate* clone ()  ;
  virtual   std::string Type ()  const;

  virtual   Bool_t Inside(Float_t x, Float_t y) {
    return Interior(x, y);
  }
  //
  // Utilities:
  //
protected:
  void GetLRLimits();
  Bool_t Interior(Float_t x, Float_t y);

};

#endif
