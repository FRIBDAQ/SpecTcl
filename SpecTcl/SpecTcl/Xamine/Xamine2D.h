//  CXamine2D.h:
//
//    This file defines the CXamine2D class.
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

#ifndef __XAMINE2D_H  //Required for current class
#define __XAMINE2D_H
                               //Required for base classes
#ifndef __XAMINESPECTRUM_H
#include "XamineSpectrum.h"
#endif

#ifndef __XAMINE_XAMINE_H
#include <Xamine.h>
#define __XAMINE_XAMINE_H
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

class CXamine1D;		// Forward definition.
                               
class CXamine2D  : public CXamineSpectrum        
{
  Bool_t m_fByte;		// kfTRUE if spectrum channels are byte.
  UInt_t m_nXchannels;		// no. of X channels.
  UInt_t m_nYchannels;		// No of y Channels.
  
public:
  // Constructor:

  CXamine2D(Xamine_shared* pXamine, const std::string& rName,
	    UInt_t nXchans, UInt_t nYchans, Bool_t fBytes = kfTRUE) :
    CXamineSpectrum(pXamine, rName),
    m_fByte(fBytes),
    m_nXchannels(nXchans),
    m_nYchannels(nYchans)
  {}

  CXamine2D(Xamine_shared* pXamine, UInt_t  nSlot);
  virtual ~ CXamine2D ( ) { }       //Destructor
	
			//Copy constructor

  CXamine2D (const CXamine2D& aCXamine2D )   : 
    CXamineSpectrum (aCXamine2D) 
  {   
    m_fByte      = aCXamine2D.m_fByte;
    m_nXchannels = aCXamine2D.m_nXchannels;
    m_nYchannels = aCXamine2D.m_nYchannels;
    
  }                                     
  
  //Operator= Assignment Operator
  
  CXamine2D& operator= (const CXamine2D& aCXamine2D)
  { 
    if (this == &aCXamine2D) return *this;          
    CXamineSpectrum::operator= (aCXamine2D);
    m_fByte      = aCXamine2D.m_fByte;
    m_nXchannels = aCXamine2D.m_nXchannels;
    m_nYchannels = aCXamine2D.m_nYchannels;
    
    return *this;
  }                                     
  
  //Operator== Equality Operator
  
  int operator== (const CXamine2D& aCXamine2D)
  { 
    return (
	    (CXamineSpectrum::operator== (aCXamine2D)) &&
	    
	    (m_fByte == aCXamine2D.m_fByte) &&
	    (m_nXchannels == aCXamine2D.m_nXchannels) &&
	    (m_nYchannels == aCXamine2D.m_nYchannels) 
	    );
  }           
  // Selectors:                  
  
public:
  Bool_t getByte() const
  {
    return m_fByte;
  }
  UInt_t getXchannels() const
  {
    return m_nXchannels;
  }
  UInt_t getYchannels() const
  {
    return m_nYchannels;
  }
  // Mutators are available only to derived classes:

protected:                       

  void setByte (Bool_t am_fByte)
  { 
    m_fByte = am_fByte;
  }
  void setXchannels (UInt_t am_nXchannels)
  { 
    m_nXchannels = am_nXchannels;
  }
  void setYchannels (UInt_t am_nYchannels)
  { 
    m_nYchannels = am_nYchannels;
  }
  // operations on the class:

public:
  virtual   CXamine1D* Oned ()  ;
  virtual   CXamine2D* Twod ()  ;
 
};

#endif
