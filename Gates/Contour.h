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

#ifndef __CONTOUR_H  //Required for current class
#define __CONTOUR_H
                               //Required for base classes
#ifndef __POINTLISTGATE_H
#include "PointlistGate.h"
#endif    

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif
                           
#ifndef __INTERIOR_H
#include "Interior.h"
#endif
                          
#ifndef __POINT_H
#include "Point.h"
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif     

class CContour  : public CPointListGate        
{
  CPoint m_LowerLeft;		// Lower left corner of gate.
  CPoint m_UpperRight;          // Upper right corner of gate.
  mutable CInterior m_Interior;   // An interior specification.
  
public:
			//Default constructor

  CContour (UInt_t nXId, UInt_t nYId, 
	    const vector<CPoint>& Points);
  CContour (UInt_t nXId, UInt_t NYId, UInt_t nPts,
	    UInt_t *xCoords, UInt_t *yCoords);
  CContour (UInt_t nXId, UInt_t NYId, UInt_t nPts,
	    CPoint* pPonts);
  ~CContour ( ) { }		//Destructor
	
			//Copy constructor

  CContour (const CContour& aCContour )   : CPointListGate (aCContour) 
  {   
    m_LowerLeft = aCContour.m_LowerLeft;
    m_UpperRight = aCContour.m_UpperRight;
    m_Interior = aCContour.m_Interior;                
  }                                     

			//Operator= Assignment Operator


  CContour& operator= (const CContour& aCContour)
  { 
    if (this == &aCContour) return *this;  
        
    CPointListGate::operator= (aCContour);
    m_LowerLeft = aCContour.m_LowerLeft;
    m_UpperRight = aCContour.m_UpperRight;
    m_Interior = aCContour.m_Interior;
    
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator
                        // Gates have no concept of equality.
private:
  int operator== (const CContour& aCContour);
public:
  //
  // Selectors:
  //
                       //Get accessor function for attribute
  CPoint getLowerLeft() const
  {
    return m_LowerLeft;
  }

                       //Get accessor function for attribute
  CPoint getUpperRight() const
  {
    return m_UpperRight;
  }

                       //Get accessor function for attribute
  CInterior& getInterior() const
  {
    return m_Interior;
  }

  // Mutators:

protected:        
                       //Set accessor function for attribute
  void setLowerLeft (const CPoint&  am_Lower)
  { 
    m_LowerLeft = am_Lower;
  }

                       //Set accessor function for attribute
  void setUpperRight (const CPoint& am_Upper)
  { 
    m_UpperRight = am_Upper;
  }

                       //Set accessor function for attribute
  void setInterior (const CInterior& am_aInterior)
  { 
    m_Interior = am_aInterior;
  }

  //  Functions on the object:
  //
public:                
  virtual   Bool_t operator()(CEvent& rEvent);
  virtual   CGate* clone ()  ;
  virtual   std::string Type ()  const;
  virtual   Bool_t inGate(CEvent& rEvent, const vector<UInt_t>& Params);
  virtual Bool_t inGate(CEvent& rEvent);

  //
  // Utility functions:
  //
protected:
  virtual void GenerateInterior();
};

#endif
