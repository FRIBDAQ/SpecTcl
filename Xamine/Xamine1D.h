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

#ifndef __XAMINE1D_H  //Required for current class
#define __XAMINE1D_H
                               //Required for base classes
#ifndef __XAMINESPECTRUM_H
#include "XamineSpectrum.h"
#endif                               

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __XAMINE_XAMINE_H
#include <Xamine.h>
#define __XAMINE_XAMINE_H
#endif


class CXamine2d;		// Forward class definition.
                               
class CXamine1D  : public CXamineSpectrum        
{
  Bool_t m_fWord;  // kfTRUE if spectrum is only word width.
  UInt_t m_nChannels;  // // Number of channels in the spectrum.
  
public:
  
  // Constructor:

  CXamine1D(volatile Xamine_shared*     pXamine,
	    const std::string& rName,
	    UInt_t             nChannels,
	    Bool_t             fWord = kfFALSE) :
    CXamineSpectrum(pXamine, rName),
    m_fWord(fWord),
    m_nChannels(nChannels) 
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
                
  }                                     

			//Operator= Assignment Operator

  CXamine1D& operator= (const CXamine1D& aCXamine1D)
  { 
    if (this == &aCXamine1D) return *this;          
    CXamineSpectrum::operator= (aCXamine1D);
    m_fWord     = aCXamine1D.m_fWord;
    m_nChannels = aCXamine1D.m_nChannels;
    
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamine1D& aCXamine1D)
  { 
    return (
	    (CXamineSpectrum::operator== (aCXamine1D)) &&
	    (m_fWord     == aCXamine1D.m_fWord) &&
	    (m_nChannels == aCXamine1D.m_nChannels) 
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
  // Operations:

public:
  virtual   CXamine1D* Oned ()  ;
  virtual   CXamine2D* Twod ()  ;
 
};

#endif
