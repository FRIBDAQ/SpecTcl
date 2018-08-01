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

#ifndef GAMMACONTOUR_H  // Required for the current class
#define GAMMACONTOUR_H

#include "Contour.h"
#include "Interior.h"
#include "Point.h"
#include <vector>

class CGammaContour : public CContour
{
  std::vector<std::string> m_vSpecs;  // Ids of the spectra to display the gate on 

 public:

  // Default constructor
  CGammaContour (const std::vector<FPoint>& rPoints);
  CGammaContour (const std::vector<FPoint>& rPoints,
		 const std::vector<std::string>& rSpecs);

  // Destructor
  ~CGammaContour () { }

  // Copy constructor
  CGammaContour (const CGammaContour& aCGammaContour) : 
    CContour (aCGammaContour)
    {
      m_vSpecs = aCGammaContour.m_vSpecs;
      setLowerLeft(aCGammaContour.getLowerLeft());
      setUpperRight(aCGammaContour.getUpperRight());
      //      setInterior(aCGammaContour.getInterior());
    }

  // Assignment operator
  CGammaContour& operator= (const CGammaContour& aCGammaContour)
    {
      if (this == &aCGammaContour) return *this;
      CContour::operator= (aCGammaContour);
      m_vSpecs = aCGammaContour.m_vSpecs;
      setLowerLeft(aCGammaContour.getLowerLeft());
      setUpperRight(aCGammaContour.getUpperRight());
      //      setInterior(aCGammaContour.getInterior());

      return *this;
    }

 private:
  
  // Equality operator
  int operator== (const CGammaContour& aCGammaContour);

 public:  // Get accessor function
  
  virtual std::vector<std::string> getSpecs () const
    {
      return m_vSpecs;
    }

 protected:  // Set mutator function

  void setm_vSpecs (std::vector<std::string>& rSpecs)
    {
      m_vSpecs = rSpecs;
    }

 public:  // Member functions for the class.

  virtual Bool_t operator() (CEvent& rEvent) { return kfTRUE; }
  virtual CGate* clone ();
  virtual std::string Type () const;
  virtual Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params);
};

#endif
