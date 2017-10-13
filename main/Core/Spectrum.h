/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

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
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __PARAMETER_H
#include "Parameter.h"
#endif

#ifndef __CAXIS_H
#include "CAxis.h"
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __CRANGEERROR_H
#include <RangeError.h>
#endif

// Forward class definitions:

class CEvent;
class CGateContainer;


extern CGateContainer* pDefaultGate; // Gate set by default on a spectrum
				// This defaults to a CTrueGate.
                         
/*!
     This is an abstract base class for the set of spectrum types
    supported by the histogramming system. Base services are provided including
    a placeholder for the axis translation objects required by a spectrum.

*/
class CSpectrum  : public CNamedItem        
{
public:
  typedef std::vector<CAxis>  Axes;
  typedef Axes::iterator  AxisIterator;
  typedef struct {
    std::string             sName;	    //!< Name of spectrum.
    UInt_t             nId;	    //!< Id of spectrum.
    SpectrumType_t     eType;       //!< Type of spectrum.
    DataType_t         eDataType;   //!< Data type of channels.
    std::vector<UInt_t>     vParameters; //!< Set of parameters.
    std::vector<UInt_t>     vyParameters; //!< Set of y parameters (G2DDD).
    std::vector<UInt_t>     nChannels;   //!< Explicit channel sizes.
    std::vector<Float_t>    fLows;       //!< Explicit lows.
    std::vector<Float_t>    fHighs;      //!< Explicit highs.
  } SpectrumDefinition;		    //!< Returned from GetDefinition

private:
  Address_t       m_pStorage;     //!< Pointer to the channel storage.
  Bool_t          m_fOwnStorage;  //!< kfTRUE if spectrum owns the storage.
  CGateContainer* m_pGate;	//!< Pointer to gate on spectrum.
  DataType_t      m_DataType;   //!< Type of underlying data.
  Axes            m_AxisMappings; //!< Mapping information for the axis.
  std::vector<UInt_t>  m_nChannels;    //!< Number of channels on each axis
  std::vector<Float_t> m_fLows;	//!< std::vector of axis low limits.
  std::vector<Float_t> m_fHighs;	//!< std::vector of axis high limits.
  std::vector<std::string>  m_Units;	//!< Axis units.
  static Bool_t   m_Seedrandom;	//!< True if need to seed.
  std::vector<unsigned> m_underflowCounters;  //!< one per axis.
  std::vector<unsigned> m_overflowCounters;   //!< one per axis.
  std::string     m_description;
public:
			// Constructor

  CSpectrum (const std::string& rName,  UInt_t nId,
	     Axes  Maps,
	     CGateContainer* pGate = pDefaultGate); //!< Constructor

  CSpectrum(const std::string& rName, UInt_t nId,
	    CGateContainer*  pGate = pDefaultGate); //!< No mapping needed.

  virtual ~ CSpectrum ( );

  //
  // Copy constructor may not be allowed..
  //
private:
  CSpectrum (const CSpectrum& aCSpectrum );
public:

			//Operator= Assignment Operator [not allowed]
private:
  CSpectrum& operator= (const CSpectrum& aCSpectrum);
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
  Axes getAxisMaps() const {
    return m_AxisMappings;
  }
  Int_t getAxisMapCount() const {
    return m_AxisMappings.size();
  }
  std::string getTextDescription() const {
    return m_description;
  }
  virtual SpectrumType_t getSpectrumType() = 0;


  // Mutators are available to derived classes:

protected:                   
  virtual void setStorage (Address_t am_pStorage)
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
  void AddAxis(UInt_t nChannels, Float_t fLow,
	       Float_t fHigh, const std::string& Units = std::string("")) {
    m_nChannels.push_back(nChannels);
    m_fLows.push_back(fLow);
    m_fHighs.push_back(fHigh);
    m_Units.push_back(Units);

  }
  bool checkRange(int channel, int nChans, int axis);
  public:
  void setTextDescription(std::string d) {
    m_description = d;
  }//
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

  // Profiling suggests ParameterToAxis should be inlined:

  Float_t  ParameterToAxis(UInt_t nAxis, Float_t fParameterValue) {
    if(nAxis < m_AxisMappings.size()) {
      return m_AxisMappings[nAxis].ParameterToAxis(fParameterValue);
    }
    else {
      throw CRangeError(0, m_AxisMappings.size() - 1, nAxis,
			std::string("CSpectrumParameterToAxis"));
    }
  }
  Float_t AxisToParameter(UInt_t nAxis, UInt_t  nAxisValue);
  Float_t  MappedToAxis(UInt_t nAxis, Float_t fParameterValue);
  Float_t AxisToMapped(UInt_t nAxis, UInt_t nAxisValue);
  
  // Statistics methods:
  
  void createStatArrays(unsigned nAxes);
  void clearStatArrays();
  void logOverflow(unsigned axis, unsigned increment = 1);
  void logUnderflow(unsigned axis, unsigned increment = 1);
  std::vector<unsigned> getUnderflows() const;
  std::vector<unsigned> getOverflows() const;

  // Pure virtual members:

  virtual   Bool_t UsesParameter(UInt_t nId) const   = 0;
  virtual   ULong_t operator[] (const UInt_t* pIndices) const  = 0;
  virtual   void set(const UInt_t* pIndices, ULong_t nValue)= 0;
  virtual   void GetParameterIds(std::vector<UInt_t>& rvIds) = 0;
  virtual   void GetResolutions(std::vector<UInt_t>&  rvResolutions) = 0;
  virtual   void Increment(const CEvent& rEvent) = 0;
  //
  // Functions with default implementation.
  //
  virtual   Size_t Dimension (UInt_t nDimension) const;
  virtual   UInt_t Dimensionality () const;
  virtual   Float_t GetLow(UInt_t nDimension) const;
  virtual   Float_t GetHigh(UInt_t nDimension) const;
  virtual   std::string  GetUnits(UInt_t nDimension) const;

  virtual   void Copy(void* pStorage) const;
  virtual   void Clear ()  ;
  virtual   Size_t StorageNeeded () const;
  virtual   void ReplaceStorage (Address_t pNewLoc, 
				 Bool_t fTransferOwnership=kfTRUE)  ;
  virtual   Bool_t CheckGate(const CEvent& rEvent);
  virtual   SpectrumDefinition& GetDefinition() ;

  virtual   Bool_t needParameter() const;


protected:
  static Int_t Randomize(Float_t channel);

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









