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

/*
  Change log:
  $Log$
  Revision 5.2  2005/06/03 15:19:20  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:22  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:03  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.3  2003/04/15 19:15:44  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/

#ifndef GAMMACUT_H  // Required for current class
#define GAMMACUT_H

#include "Cut.h"
#include <vector>
#include <string>


/*!
   This class defines a cut or slice on gamma spectra. 
   Gamma gates are rather
   weird spectcl entities.  They are defined on a set of parameters 
   and only have meaning when evaluated on gamma spectra.  When
   evaluated on a gamma spectrum, a gamma cut is checked against all
   parameters on that spectrum.  If a parameter makes  the cut,
   the spectrum is incremented for all other parameters the
   spectrum is defined on.
*/
class CGammaCut : public CCut
{
  Float_t m_nLow;          //!< Low level of cut (inclusive)
  Float_t m_nHigh;         //!< High level of cut (inclusive)
  std::vector<std::string> m_vSpecs;  //!< std::vector of spectrum names to display gate on

 public:
  
  //! Constructor
  CGammaCut (Float_t am_nLow, Float_t am_nHigh) :
    m_nLow (am_nLow),
    m_nHigh (am_nHigh),
    CCut(am_nLow, am_nHigh, 0)
    {
      // The default constructor of a std::vector yields an empty std::vector.
    }

  CGammaCut (UInt_t am_nLow, UInt_t am_nHigh, 
	     const std::vector<std::string> am_vSpecs) :
    m_nLow (am_nLow),
    m_nHigh (am_nHigh),
    m_vSpecs (am_vSpecs),
    CCut(am_nLow, am_nHigh, 0) { }

  //! Destructor
  ~CGammaCut () { }

  //! Copy constructor
  CGammaCut (const CGammaCut& aCGammaCut) : CCut (aCGammaCut)
    {
      m_vSpecs = aCGammaCut.m_vSpecs;
      m_nLow = aCGammaCut.m_nLow;
      m_nHigh = aCGammaCut.m_nHigh;
    }

  //! Assignment operator
  CGammaCut& operator= (const CGammaCut& aCGammaCut)
    {
      if (this == &aCGammaCut) return *this;
      CCut::operator= (aCGammaCut);
      m_nLow = aCGammaCut.m_nLow;
      m_nHigh = aCGammaCut.m_nHigh;
      m_vSpecs = aCGammaCut.m_vSpecs;

      return *this;
    }

  //! Equality operator
  int operator== (const CGammaCut& aCGammaCut) const;

 public:     // Get accessor functions

  Float_t getLow() const
    {
      return m_nLow;
    }

  Float_t getHigh() const
    {
      return m_nHigh;
    }

  virtual std::vector<std::string> getSpecs() const
    {
      return m_vSpecs;
    }

 protected:             // Set mutator functions

  void setLow (Float_t am_nLow)
    {
      m_nLow = am_nLow;
    }

  void setHigh (Float_t am_nHigh)
    {
      m_nHigh = am_nHigh;
    }

  void setm_vSpecs (std::vector<std::string> am_vSpecs)
    {
      m_vSpecs = am_vSpecs;
    }

 public:   // Member functions of the class

  virtual Bool_t operator() (CEvent& rEvent) { return kfTRUE; }
  virtual CGate* clone ();
  virtual std::string GetConstituent (CConstituentIterator& rIterator);
  virtual std::string Type () const;
  virtual Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Param);
};

#endif




