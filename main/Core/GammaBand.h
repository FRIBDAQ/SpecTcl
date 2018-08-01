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


//  GammaBand.h:
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

/*
  Change Log:
  $Log$
  Revision 5.2  2005/06/03 15:19:20  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:21  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:02  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.4  2003/04/15 19:15:45  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/


#ifndef GAMMABAND_H  // Required for the current class
#define GAMMABAND_H

#include "Band.h"
#include "Point.h"
#include <vector>
#include <string>

class CGammaBand : public CBand
{
  std::vector<std::string> m_vSpecs;  // std::vector of spectrum Ids to display gate on

 public:

  //! Default Constructor
  CGammaBand (const std::vector<FPoint>& rPoints);
  CGammaBand (const std::vector<FPoint>& rPoints, 
	      const std::vector<std::string> rSpecs);

  CGammaBand (UInt_t nPts, FPoint* pPoints);
  CGammaBand (UInt_t nPts, Float_t* pX, Float_t* pY);

  //! Destructor
  ~CGammaBand () { }

  //! Copy constructor
  CGammaBand (const CGammaBand& aCGammaBand) : CBand (aCGammaBand)
    {
      m_vSpecs = aCGammaBand.m_vSpecs;
    }

  //! Assignment operator
  CGammaBand& operator= (const CGammaBand& aCGammaBand)
    {
      if (this == &aCGammaBand) return *this;
      CBand::operator= (aCGammaBand);
      m_vSpecs = aCGammaBand.m_vSpecs;

      return *this;
    }

  //! Equality comparison.
  int operator== (const CGammaBand& aCGammaBand) const;

 public:  // Get accessor functions

  virtual std::vector<std::string> getSpecs() const
    {
      return m_vSpecs;
    }

 protected:  // Set mutator functions
  
  void setm_vSpecs (std::vector<std::string>& rSpecs) 
    {
      m_vSpecs = rSpecs;
    }

 public:  // Member functions of the class

  virtual Bool_t operator() (CEvent& rEvent) { return kfTRUE; }
  virtual CGate* clone ();
  virtual std::string Type () const;
  virtual Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params);
};

#endif
