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


//  CContour.h:
//
//    This file defines the CContour class.
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
  Change Log
  $Log$
  Revision 5.2  2005/06/03 15:19:20  ron-fox
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

  Revision 5.1  2004/11/29 16:56:02  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.6  2003/04/15 19:15:45  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/


#ifndef CONTOUR_H  //Required for current class
#define CONTOUR_H
                               //Required for base classes
#include "PointlistGate.h"
#include <histotypes.h>
#include "Interior.h"
#include "Point.h"
#include <vector>

/*!
   Contour gate:  This gate is capable of evaluating itself on an arbitrary
   set of real valued paraemters. Interiorness is defined as for flood-fills:
   If a line extended from the point in questino to infnity crosses an odd
   number of edges, the point is interior.

   For more information on the algorithm used, see the inGate member function
   documentation.
*/
class CContour  : public CPointListGate        
{
  FPoint m_LowerLeft;		//!< Lower left corner of gate.
  FPoint m_UpperRight;          //!< Upper right corner of gate.
  //  mutable CInterior m_Interior;   // An interior specification.
  
public:
			//Default constructor

  CContour (UInt_t nXId, UInt_t nYId, 
	    const std::vector<FPoint>& Points);
  CContour (UInt_t nXId, UInt_t NYId, UInt_t nPts,
	    Float_t *xCoords, Float_t *yCoords);
  CContour (UInt_t nXId, UInt_t NYId, UInt_t nPts,
	    FPoint* pPonts);
  ~CContour ( ) { }		//Destructor
	
			//Copy constructor

  CContour (const CContour& aCContour )   : 
    CPointListGate (aCContour) 
  {   
    m_LowerLeft = aCContour.m_LowerLeft;
    m_UpperRight = aCContour.m_UpperRight;
    //    m_Interior = aCContour.m_Interior;                
  }                                     

			//Operator= Assignment Operator


  CContour& operator= (const CContour& aCContour)
  { 
    if (this == &aCContour) return *this;  
        
    CPointListGate::operator= (aCContour);
    m_LowerLeft = aCContour.m_LowerLeft;
    m_UpperRight = aCContour.m_UpperRight;
    //    m_Interior = aCContour.m_Interior;
    
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator


  int operator== (const CContour& aCContour) const;

  //
  // Selectors:
  //
                       //Get accessor function for attribute
  FPoint getLowerLeft() const
  {
    return m_LowerLeft;
  }

                       //Get accessor function for attribute
  FPoint getUpperRight() const
  {
    return m_UpperRight;
  }


  // Mutators:

protected:        
                       //Set accessor function for attribute
  void setLowerLeft (const FPoint&  am_Lower)
  { 
    m_LowerLeft = am_Lower;
  }

                       //Set accessor function for attribute
  void setUpperRight (const FPoint& am_Upper)
  { 
    m_UpperRight = am_Upper;
  }


  //  Functions on the object:
  //
public:                
  virtual   Bool_t operator()(CEvent& rEvent);
  virtual   CGate* clone ()  ;
  virtual   std::string Type ()  const;
  virtual   Bool_t Inside(Float_t x, Float_t y);

  //
  // Utility functions:
  //
 protected:
  virtual void GenerateInterior();

};

#endif
