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

#ifndef __BAND_H  //Required for current class
#define __BAND_H
                               //Required for base classes
#ifndef __POINTLISTGATE_H
#include "PointlistGate.h"
#endif                               
          

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __POINT_H
#include "Point.h"
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif
              
class CBand  : public CPointListGate        
{
  std::vector<UInt_t> m_aLimits;  // // Set of parameter upper limits.
  
public:
			//Default constructor

  CBand (UInt_t nXid, UInt_t nYid,
	 const vector<CPoint>& points);
  CBand (UInt_t nXid, UInt_t nYid,
	 UInt_t nPts, CPoint* pPoints);
  CBand (UInt_t nXid, UInt_t nYid,
	 UInt_t nPts, UInt_t* pX, UInt_t* pY);
  virtual ~CBand ( ) 
  { }
 

			//Copy constructor

  CBand (const CBand& aCBand )   : 
    CPointListGate (aCBand) 
  {   
    m_aLimits = aCBand.m_aLimits;                
  }                                     

			//Operator= Assignment Operator

  CBand& operator= (const CBand& aCBand)
  { 
    if (this == &aCBand) return *this;          
    CPointListGate::operator= (aCBand);
    m_aLimits = aCBand.m_aLimits;
    
    return *this; 
  }                                     

			// Operator== Equality Operator
                        // Gates have no equality operator.
private:
  int operator== (const CBand& aCBand);
public:
  // Selectors

public:
                       //Get accessor function for attribute
  std::vector<UInt_t> getLimits() const
  {
    return m_aLimits;
  }
                       
                       // Set accessor function for attribute
                       // Mutators.
public:
  void setLimits (std::vector<UInt_t> am_aLimits)
  { 
    m_aLimits = am_aLimits;
  }

  //  Operations.
  //
public:                   
  virtual   Bool_t operator()(CEvent& rEvent);
  virtual   CGate* clone ()  ;
  virtual   std::string Type ()  const;
  virtual   Bool_t inGate(CEvent& rEvent, const vector<UInt_t>& Params);
  virtual   Bool_t inGate(CEvent& rEvent);
  //
  // Utilities:
  //
protected:
  virtual void CreateLimits();
};

#endif
