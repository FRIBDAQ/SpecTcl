//  CXamineSpectrum.h:
//
//    This file defines the CXamineSpectrum class.
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

#ifndef __XAMINESPECTRUM_H  //Required for current class
#define __XAMINESPECTRUM_H
          
#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __XAMINE_XAMINE_H
#include <Xamine.h>
#define __XAMINE_XAMINE_H
#endif

// Forward class definitions:

class CXamine1D;
class CXamine2D;
                     
class CXamineSpectrum      
{
  Address_t m_pStorage;  // Points to spectrum storage.
  UInt_t m_nSlot;  // Xamine spectrum slot number.
  std::string m_sTitle;  // Name of the spectrum.
  Xamine_shared* m_pXamineMemory;  // Xamine shared memory region.
  
public:

                        // Constructor with parameters:

  CXamineSpectrum(Xamine_shared* pXamine, const std::string& rName) :
    m_pStorage(kpNULL),
    m_nSlot(0),
    m_sTitle(rName),
    m_pXamineMemory(pXamine) {}

  CXamineSpectrum(Xamine_shared* pXamine, UInt_t nSlot);
  virtual  ~CXamineSpectrum ( ) { }       //Destructor
	
			//Copy constructor

  CXamineSpectrum (const CXamineSpectrum& aCXamineSpectrum ) 
  {   
    m_pStorage      = aCXamineSpectrum.m_pStorage;
    m_nSlot         = aCXamineSpectrum.m_nSlot;
    m_sTitle        = aCXamineSpectrum.m_sTitle;
    m_pXamineMemory = aCXamineSpectrum.m_pXamineMemory;
    
  }                                     

			//Operator= Assignment Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  CXamineSpectrum& operator= (const CXamineSpectrum& aCXamineSpectrum)
  { 
    if (this == &aCXamineSpectrum) return *this;          
 
    m_pStorage      = aCXamineSpectrum.m_pStorage;
    m_nSlot         = aCXamineSpectrum.m_nSlot;
    m_sTitle        = aCXamineSpectrum.m_sTitle;
    m_pXamineMemory = aCXamineSpectrum.m_pXamineMemory;
    
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineSpectrum& aCXamineSpectrum)
  { 
    return (
	    (m_pStorage      == aCXamineSpectrum.m_pStorage) &&
	    (m_nSlot         == aCXamineSpectrum.m_nSlot) &&
	    (m_sTitle        == aCXamineSpectrum.m_sTitle) &&
	    (m_pXamineMemory == aCXamineSpectrum.m_pXamineMemory) 
	    );
  }
  // Selectors

public:
  Address_t getStorage() const	// NOTE: Modification of spectrum is allowed.
  {
    return m_pStorage;
  }
  UInt_t getSlot() const
  {
    return m_nSlot;
  }
  const std::string& getTitle() const
  {
    return m_sTitle;
  }
  const Xamine_shared* getXamineMemory() const //  Descriptions are const.
  {
    return m_pXamineMemory;
  }
  // Mutators: Available only to derived classes:

protected:

  void setStorage (Address_t am_pStorage)
  { 
    m_pStorage = am_pStorage;
  }
  void setSlot (UInt_t am_nSlot)
  { 
    m_nSlot = am_nSlot;
  }
  void setTitle (const std::string& am_sTitle)
  { 
    m_sTitle = am_sTitle;
  }
  void setXamineMemory (Xamine_shared* am_pXamineMemory)
  { 
    m_pXamineMemory = am_pXamineMemory;
  }
  // Class operations:

public:                   
  spec_type getSpectrumType () {
    return m_pXamineMemory->dsp_types[m_nSlot];
  }
  Bool_t is1d ()  ;

  //
  // Pure virtual member functions:
  //
  virtual   CXamine1D* Oned () {
    return (CXamine1D*)kpNULL;
  }
  virtual   CXamine2D* Twod () {
    return (CXamine2D*)kpNULL;
  }
};

#endif











