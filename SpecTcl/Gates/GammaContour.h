//  CGammaContour.h:
//
//   This file defines the CGammaContour class.
//
//  Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venemaja@msu.edu
//
//  Copyright 2001 NSCL, All Rights Reserved
//
////////////////////////////////////////////////////////////////////

#ifndef __GAMMACONTOUR_H  // Required for the current class
#define __GAMMACONTOUR_H

#ifndef __CONTOUR_H       // Required for the base classes
#include "Contour.h"
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

class CGammaContour : public CContour
{
  vector<string> m_vSpecs;  // Ids of the spectra to display the gate on 

 public:

  // Default constructor
  CGammaContour (const vector<CPoint>& rPoints);
  CGammaContour (const vector<CPoint>& rPoints,
		 const vector<string>& rSpecs);

  // Destructor
  ~CGammaContour () { }

  // Copy constructor
  CGammaContour (const CGammaContour& aCGammaContour) : 
    CContour (aCGammaContour)
    {
      m_vSpecs = aCGammaContour.m_vSpecs;
      setLowerLeft(aCGammaContour.getLowerLeft());
      setUpperRight(aCGammaContour.getUpperRight());
      setInterior(aCGammaContour.getInterior());
    }

  // Assignment operator
  CGammaContour& operator= (const CGammaContour& aCGammaContour)
    {
      if (this == &aCGammaContour) return *this;
      CContour::operator= (aCGammaContour);
      m_vSpecs = aCGammaContour.m_vSpecs;
      setLowerLeft(aCGammaContour.getLowerLeft());
      setUpperRight(aCGammaContour.getUpperRight());
      setInterior(aCGammaContour.getInterior());

      return *this;
    }

 private:
  
  // Equality operator
  int operator== (const CGammaContour& aCGammaContour);

 public:  // Get accessor function
  
  virtual vector<string> getSpecs () const
    {
      return m_vSpecs;
    }

 protected:  // Set mutator function

  void setm_vSpecs (vector<string>& rSpecs)
    {
      m_vSpecs = rSpecs;
    }

 public:  // Member functions for the class.

  virtual Bool_t operator() (CEvent& rEvent) { return kfTRUE; }
  virtual CGate* clone ();
  virtual std::string Type () const;
  virtual Bool_t inGate(CEvent& rEvent, const vector<UInt_t>& Params);
};

#endif
