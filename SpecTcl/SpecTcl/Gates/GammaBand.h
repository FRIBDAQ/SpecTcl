//  CBand.h:
//
//    This file defines the CGammaBand class.
//
//  Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI  48824-1321
//    mailto:venemaja@msu.edu
//
//  Copyright 2001 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////////

#ifndef __GAMMABAND_H  // Required for the current class
#define __GAMMABAND_H

#ifndef __BAND_H       // Required for the base classes
#include "Band.h"
#endif

#ifndef __POINT_H
#include "Point.h"
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

class CGammaBand : public CBand
{
  vector<string> m_vSpecs;  // vector of spectrum Ids to display gate on

 public:

  // Default Constructor
  CGammaBand (const vector<CPoint>& rPoints);
  CGammaBand (const vector<CPoint>& rPoints, 
	      const vector<string> rSpecs);

  CGammaBand (UInt_t nPts, CPoint* pPoints);
  CGammaBand (UInt_t nPts, UInt_t* pX, UInt_t* pY);

  // Destructor
  ~CGammaBand () { }

  // Copy constructor
  CGammaBand (const CGammaBand& aCGammaBand) : CBand (aCGammaBand)
    {
      m_vSpecs = aCGammaBand.m_vSpecs;
      setLimits(aCGammaBand.getLimits());
    }

  // Assignment operator
  CGammaBand& operator= (const CGammaBand& aCGammaBand)
    {
      if (this == &aCGammaBand) return *this;
      CBand::operator= (aCGammaBand);
      m_vSpecs = aCGammaBand.m_vSpecs;
      setLimits(aCGammaBand.getLimits());

      return *this;
    }

 private:
  
  // Equality operator
  int operator== (const CGammaBand& aCGammaBand);

 public:  // Get accessor functions

  virtual vector<string> getSpecs() const
    {
      return m_vSpecs;
    }

 protected:  // Set mutator functions
  
  void setm_vSpecs (vector<string>& rSpecs) 
    {
      m_vSpecs = rSpecs;
    }

 public:  // Member functions of the class

  virtual Bool_t operator() (CEvent& rEvent) { return kfTRUE; }
  virtual CGate* clone ();
  virtual std::string Type () const;
};

#endif
