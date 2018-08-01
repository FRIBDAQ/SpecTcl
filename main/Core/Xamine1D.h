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


//  CXamine1D.h:
//
//    This file defines the CXamine1D class.
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

#ifndef XAMINE1D_H  //Required for current class
#define XAMINE1D_H
                               //Required for base classes
#include "XamineSpectrum.h"
#include "XamineMap1D.h"
#include <histotypes.h>
#include <Xamine.h>
#include <string>


class CXamine2d;		// Forward class definition.
                               
class CXamine1D  : public CXamineSpectrum        
{
  Bool_t      m_fWord;      // kfTRUE if spectrum is only word width.
  UInt_t      m_nChannels;  // Number of channels in the spectrum.
  CXamineMap1D m_XamineMap; // Transformation information, if applicable
  
public:
  
  // Constructor:

  CXamine1D(volatile Xamine_shared*     pXamine,
	    const std::string& rName,
	    UInt_t             nChannels,
	    Bool_t             fWord = kfFALSE) :
    CXamineSpectrum(pXamine, rName),
    m_fWord(fWord),
    m_nChannels(nChannels),
    m_XamineMap(0.0,0.0,std::string(""))
  { }

  CXamine1D(volatile Xamine_shared* pXamine,
	    const std::string& rName,
	    UInt_t             nChannels,
	    Float_t            nLow,
	    Float_t            nHigh,
	    const std::string& rUnits,
	    Bool_t             fWord = kfFALSE) :
    CXamineSpectrum(pXamine, rName),
    m_fWord(fWord),
    m_nChannels(nChannels),
    m_XamineMap(nLow, nHigh, rUnits)
    { }
  
  CXamine1D(volatile Xamine_shared* pXamine,
	    UInt_t nSlot);
  
  virtual  ~CXamine1D ( ) { }       //Destructor

  //Copy constructor
  CXamine1D (const CXamine1D& aCXamine1D )   : 
    CXamineSpectrum (aCXamine1D) 
  {   
    m_fWord     = aCXamine1D.m_fWord;
    m_nChannels = aCXamine1D.m_nChannels;
    m_XamineMap = aCXamine1D.m_XamineMap;
  }                                     

  //Operator= Assignment Operator
  CXamine1D& operator= (const CXamine1D& aCXamine1D)
  { 
    if (this == &aCXamine1D) return *this;          
    CXamineSpectrum::operator= (aCXamine1D);
    m_fWord     = aCXamine1D.m_fWord;
    m_nChannels = aCXamine1D.m_nChannels;
    m_XamineMap = aCXamine1D.m_XamineMap;
    
    return *this;
  }                                     

  //Operator== Equality Operator
  int operator== (const CXamine1D& aCXamine1D)
  { 
    return (
	    (CXamineSpectrum::operator== (aCXamine1D)) &&
	    (m_fWord     == aCXamine1D.m_fWord) &&
	    (m_nChannels == aCXamine1D.m_nChannels) &&
	    (m_XamineMap == aCXamine1D.m_XamineMap)
	    );
  }                             
  // Selectors:

public:

  Bool_t getWord() const
  {
    return m_fWord;
  }

  //Get accessor function for attribute
  UInt_t getChannels() const
  {
    return m_nChannels;
  }
  CXamineMap1D getXamineMap() const
    {
      return m_XamineMap;
    }

  // Mutators: Only available to derived classes:

protected:
  void setWord (Bool_t am_fWord)
  { 
    m_fWord = am_fWord;
  }
  void setChannels (UInt_t am_nChannels)
  { 
    m_nChannels = am_nChannels;
  }
  void setXamineMap(CXamineMap1D& am_XamineMap)
    {
      m_XamineMap = am_XamineMap;
    }
  // Operations:

public:
  virtual   CXamine1D* Oned ()  ;
  virtual   CXamine2D* Twod ()  ;
};

#endif
