//  CGammaCut.h:
//
//    This file defines the CGammaCut class.
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI  48824-1321
//    mailto:venemaja@msu.edu
//
// Copyright 2001 NSCL, All Rights Reserved.
//
///////////////////////////////////////////////////////////////////

#ifndef __GAMMACUT_H  // Required for current class
#define __GAMMACUT_H

#ifndef __CUT_H       // Required for base class 
#include "Cut.h"
#endif

class CGammaCut : public CCut
{
  UInt_t m_nLow;          // Low level of cut (inclusive)
  UInt_t m_nHigh;         // High level of cut (inclusive)
  vector<string> m_vSpecs;  // Vector of spectrum names to display gate on

 public:
  
  // Default Constructor
  CGammaCut (UInt_t am_nLow, UInt_t am_nHigh) :
    m_nLow (am_nLow),
    m_nHigh (am_nHigh),
    CCut(am_nLow, am_nHigh, 0)
    {
      vector<string> empty;
      m_vSpecs = empty;
    }

  CGammaCut (UInt_t am_nLow, UInt_t am_nHigh, 
	     const vector<string> am_vSpecs) :
    m_nLow (am_nLow),
    m_nHigh (am_nHigh),
    m_vSpecs (am_vSpecs),
    CCut(am_nLow, am_nHigh, 0) { }

  // Destructor
  ~CGammaCut () { }

  // Copy constructor
  CGammaCut (const CGammaCut& aCGammaCut) : CCut (aCGammaCut)
    {
      m_vSpecs = aCGammaCut.m_vSpecs;
      m_nLow = aCGammaCut.m_nLow;
      m_nHigh = aCGammaCut.m_nHigh;
    }

  // Assignment operator
  CGammaCut& operator= (const CGammaCut& aCGammaCut)
    {
      if (this == &aCGammaCut) return *this;
      CCut::operator= (aCGammaCut);
      m_nLow = aCGammaCut.m_nLow;
      m_nHigh = aCGammaCut.m_nHigh;
      m_vSpecs = aCGammaCut.m_vSpecs;

      return *this;
    }

 private:
  
  // Equality operator
  int operator== (const CGammaCut& aCGammaCut);

 public:     // Get accessor functions

  UInt_t getLow() const
    {
      return m_nLow;
    }

  UInt_t getHigh() const
    {
      return m_nHigh;
    }

  virtual vector<string> getSpecs() const
    {
      return m_vSpecs;
    }

 protected:             // Set mutator functions

  void setm_nLow (UInt_t am_nLow)
    {
      m_nLow = am_nLow;
    }

  void setm_nHigh (UInt_t am_nHigh)
    {
      m_nHigh = am_nHigh;
    }

  void setm_vSpecs (vector<string> am_vSpecs)
    {
      m_vSpecs = am_vSpecs;
    }

 public:   // Member functions of the class

  virtual Bool_t operator() (CEvent& rEvent) { return kfTRUE; }
  virtual CGate* clone ();
  virtual std::string GetConstituent (CConstituentIterator& rIterator);
  virtual std::string Type () const;
  virtual Bool_t inGate(CEvent& rEvent, const vector<UInt_t>& Param);
};

#endif
