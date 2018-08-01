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


//  CCut.h:
//
//    This file defines the CCut class.
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

  Revision 4.4  2003/04/15 19:15:45  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/

#ifndef CUT_H  //Required for current class
#define CUT_H
                               //Required for base classes
#include "Gate.h"
#include <vector>
#include <string>

/*! Encapsulates a cut on a parameter.
  The cut is described by a high and low limit 
  in parameter space.  In this implementation the cut is expressed
  as a pair of floating point limits.  
*/                               
class CCut  : public CGate        
{
  Float_t m_nLow;		//!< low level of cut (inclusive)
  Float_t m_nHigh;		//!< High level of cut (inclusive)
  UInt_t  m_nId;		//!< Id of parameter to cut.
  
public:

  //! Constructor (Requires parameterization).

  CCut (  Float_t am_nLow,  Float_t am_nHigh,  UInt_t am_nId  ) : 
    m_nLow (am_nLow),  
    m_nHigh (am_nHigh),  
    m_nId (am_nId)  
    { }        
  ~ CCut ( ) { }       //!< Destructor

	
  //! Copy constructor

  CCut (const CCut& aCCut )   : CGate (aCCut) 
  {   
    m_nLow  = aCCut.m_nLow;
    m_nHigh = aCCut.m_nHigh;
    m_nId   = aCCut.m_nId;               
  }                                     

  //! Operator= Assignment Operator

  CCut& operator= (const CCut& aCCut) 
  { 
    if (this == &aCCut) return *this;          
    CGate::operator= (aCCut);
    m_nLow  = aCCut.m_nLow;
    m_nHigh = aCCut.m_nHigh;
    m_nId   = aCCut.m_nId;
    
    return *this;                                                                                                 
  }                                     

  //!< Operator== Equality Operator
  
  int operator== (const CCut& aCCut) const;

  // Selectors

public:				//Get accessor function for attribute
  Float_t getLow() const
  {
    return m_nLow;
  }

                       //Get accessor function for attribute
  Float_t getHigh() const
  {
    return m_nHigh;
  }

                       //Get accessor function for attribute
  UInt_t getId() const
  {
    return m_nId;
  }
  //!< Return involved spectra (for compatibility with gamma gates.

  virtual std::vector<std::string> getSpecs() const { 
    std::vector<std::string> empty;
    return         empty;	// Must return an empty std::vector.
  }

  // Mutators.
                       //Set accessor function for attribute
protected:
  void setLow (Float_t am_nLow)
  { 
    m_nLow = am_nLow;
  }

                       //Set accessor function for attribute
  void setHigh (Float_t am_nHigh)
  { 
    m_nHigh = am_nHigh;
  }

                       //Set accessor function for attribute
  void setId (UInt_t am_nId)
  { 
    m_nId = am_nId;
  }

  // Member functions of the class.

public:                   
  virtual   Bool_t operator()(CEvent& rEvent);
  virtual   CGate* clone ()  ;
  virtual   CConstituentIterator Begin ()  ;
  virtual   CConstituentIterator End ()  ;
  virtual   UInt_t Size ()  ;
  virtual   std::string GetConstituent (CConstituentIterator& rIterator)  ;
  virtual   std::string Type ()  const;
  virtual   Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params);
  virtual   Bool_t inGate (CEvent& rEvent);
  virtual   Bool_t inGate (Float_t x);
};

#endif

