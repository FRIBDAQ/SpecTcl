//  CSpectrum.h:
//
//    This file defines the CSpectrum class.
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
//  Change Log:
//     July 3, 1999:   Added getSpectrumType pure virtual function so that
//                     spectra can report their type.
//
/////////////////////////////////////////////////////////////

#ifndef __SPECTRUM_H  //Required for current class
#define __SPECTRUM_H
                               //Required for base classes
#ifndef __NAMEDITEM_H
#include "NamedItem.h"
#endif      

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __PARAMETER_H
#include "Parameter.h"
#endif

// Forward class definitions:

class CEvent;
class CGateContainer;

extern CGateContainer* pDefaultGate; // Gate set by default on a spectrum
				// This defaults to a CTrueGate.
                         
//
//  This is an abstract base class for the set of spectrum types
//  supported by the histogramming system.
//
class CSpectrum  : public CNamedItem        
{
  Address_t       m_pStorage;     // Pointer to the channel storage.
  Bool_t          m_fOwnStorage;  // kfTRUE if spectrum owns the storage.
  CGateContainer* m_pGate;	// Pointer to gate on spectrum.
  DataType_t      m_DataType;   // Type of underlying data.

public:
			// Constructor

  CSpectrum (const std::string& rName,  UInt_t nId,
	     CGateContainer* pGate = pDefaultGate) :  
    CNamedItem(rName, nId),
    m_pStorage(0),  
    m_fOwnStorage(0) ,
    m_pGate(pGate),
    m_DataType(keUnknown_dt)
    { } 

  virtual ~ CSpectrum ( ) { 
    if(m_fOwnStorage) {
      ReleaseStorage();
    }
  }

  //Constructor with arguments
  
  CSpectrum (  Address_t am_pStorage,  Bool_t am_fOwnStorage,
	       const std::string& rName, UInt_t nId) :       
    CNamedItem(rName, nId),
    m_pStorage (am_pStorage),  
    m_fOwnStorage (am_fOwnStorage),
    m_pGate(pDefaultGate),
    m_DataType(keUnknown_dt)
    { }        
	
  //
  // Copy constructor may not be allowed..
  //
private:
  CSpectrum (const CSpectrum& aCSpectrum );
public:

			//Operator= Assignment Operator [not allowed]
private:
  CSpectrum operator= (const CSpectrum& aCSpectrum);
public:

			//Operator== Equality Operator [not much use]

  int operator== (const CSpectrum& aCSpectrum)
  { 
    return ((CNamedItem::operator== (aCSpectrum)) &&
	    (m_pStorage == aCSpectrum.m_pStorage) &&
	    (m_fOwnStorage == aCSpectrum.m_fOwnStorage) 
	    );
  }                             

  // Selectors:

public:
  const Address_t getStorage() const
  {
    return m_pStorage;
  }
  Bool_t getOwnStorage() const
  {
    return m_fOwnStorage;
  }
  const CGateContainer* getGate() const {
    return m_pGate;
  }
  virtual SpectrumType_t getSpectrumType() = 0;

  // Mutators are available to derived classes:

protected:                   
  void setStorage (Address_t am_pStorage)
  { 
    m_pStorage = am_pStorage;
  }
  void setOwnStorage (Bool_t am_fOwnStorage)
  { 
    m_fOwnStorage = am_fOwnStorage;
  }
  void setStorageType(DataType_t dt) {
    m_DataType = dt;
  }
  //
  // Operations:
  //                   
public:
  void operator() (const CEvent& rEvent);
  CGateContainer* ApplyGate(CGateContainer* pNewGate);
  CGateContainer* Ungate();	// Sets the spectrum gate to a CTrueGate
  Bool_t UsesParameter (const CParameter& rParameter) const {
    return UsesParameter(rParameter.getNumber());
  }
  DataType_t StorageType () const   {return m_DataType; }

  // Pure virtual members:

  virtual   Bool_t UsesParameter(UInt_t nId) const   = 0;
  virtual   Size_t Dimension (UInt_t nDimension) const   = 0;
  virtual   UInt_t Dimensionality () const   = 0;
  virtual   ULong_t operator[] (const UInt_t* pIndices) const  = 0;
  virtual   void set(const UInt_t* pIndices, ULong_t nValue)= 0;
  virtual   void GetParameterIds(vector<UInt_t>& rvIds) = 0;
  virtual   void GetResolutions(vector<UInt_t>&  rvResolutions) = 0;
  virtual   void Increment(const CEvent& rEvent) = 0;
  virtual   Int_t getScale(UInt_t index) = 0;
  //
  // Functions with default implementation.
  //
  virtual   void Copy(void* pStorage) const;
  virtual   void Clear ()  ;
  virtual   Size_t StorageNeeded () const;
  virtual   void ReplaceStorage (Address_t pNewLoc, 
				 Bool_t fTransferOwnership=kfTRUE)  ;
  virtual   Bool_t CheckGate(const CEvent& rEvent);
  virtual   void GammaGateIncrement(const CEvent& rEvent);
protected:
  void ReleaseStorage() {
    switch(StorageType()) {
    case keByte:
      delete [] (char*)m_pStorage;
      break;
    case keWord:
      delete [] (Short_t*)m_pStorage;
      break;
    case keLong:
      delete [] (Long_t*)m_pStorage;
      break;
    case keDouble:
      delete [] (DFloat_t*)m_pStorage;
      break;
    }
    
  }
};



#endif









