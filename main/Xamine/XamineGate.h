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

//  CXamineGate.h:
//
//    This file defines the CXamineGate class.
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

#ifndef XAMINEGATE_H  //Required for current class
#define XAMINEGATE_H

                               //Required for 1:M associated classes

#include "Point.h"
#include <histotypes.h>
#include <string>
#include <Xamine.h>

// 
// Forward references.
//
class CDisplayCut;
class CDisplayBand;
class CDisplayContour;

//

class CXamineGate
{
  UInt_t      m_nSpectrum;	// Number of spectrum on which gate is set
  UInt_t      m_nId;		// Identifier of the gate
  std::string m_sName;		// Textual name of the gate.
  GateType_t  m_eGateType;	// Type of gate in object.
  PointArray  m_vPoints;
  
public:


			//Constructors with arguments

  CXamineGate(const msg_object& rGateInfo);
  CXamineGate(UInt_t nSpectrum, UInt_t nId,
		const std::string& rName);

  virtual ~CXamineGate ( ) { }       //Destructor

			//Copy constructor
			//Update to access 1:M associated class attributes      
  CXamineGate (const CXamineGate& aCDisplayGate )
  {   
    m_nSpectrum = aCDisplayGate.m_nSpectrum;
    m_nId       = aCDisplayGate.m_nId;
    m_sName     = aCDisplayGate.m_sName;
    m_eGateType = aCDisplayGate.m_eGateType;
    m_vPoints   = aCDisplayGate.m_vPoints;

  }                                     

			//Operator= Assignment Operator

  CXamineGate& operator= (const CXamineGate& aCDisplayGate)
  { 
    if (this == &aCDisplayGate) return *this;          
    
    m_nSpectrum = aCDisplayGate.m_nSpectrum;
    m_nId       = aCDisplayGate.m_nId;
    m_sName     = aCDisplayGate.m_sName;
    m_eGateType = aCDisplayGate.m_eGateType;
    m_vPoints   = aCDisplayGate.m_vPoints;
    return *this; 
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineGate& aCDisplayGate) const
  { 
    return (
	    
	    (m_nSpectrum == aCDisplayGate.m_nSpectrum)  &&
	    (m_nId       == aCDisplayGate.m_nId)              &&
	    (m_sName     == aCDisplayGate.m_sName)          &&
	    (m_eGateType == aCDisplayGate.m_eGateType)  &&
	    (m_vPoints   == aCDisplayGate.m_vPoints)
	    );
  }                      
  // Selectors:

public:
  UInt_t getSpectrum() const
  {
    return m_nSpectrum;
  }
  UInt_t getId() const
  {
    return m_nId;
  }
  std::string getName() const
  {
    return m_sName;
  }
  GateType_t getGateType() const
  {
    return m_eGateType;
  }
  PointArray getPoints() const
  {
    return m_vPoints;
  }
  // Mutators are only available to derived classes:

protected:
  void setSpectrum (UInt_t am_nSpectrum)
  { 
    m_nSpectrum = am_nSpectrum;
  }
  void setGateType (GateType_t am_eGateType)
  { 
    m_eGateType = am_eGateType;
  }
  void setPoints(const PointArray& rPts)
  {
    m_vPoints = rPts;
  }
  // Some mutators need to be public.
  //
public:
  void setId (UInt_t am_nId)
  { 
    m_nId = am_nId;
  }
  void setName (std::string am_sName)
  { 
    m_sName = am_sName;
  }

  // Operations:
  //
public:
  PointIterator begin ()  ;
  PointIterator end ()  ;
  UInt_t size ()  const;
  CPoint& operator[] (UInt_t n)  ;
  void AddPoint (const CPoint& rPoint)  ;
  void AddPoint(int x, int y) {
    AddPoint(CPoint(x,y));
  }
  void RemovePoints (UInt_t n=1)  ;

  // Type-save upcasts:

public:
  CDisplayCut* Cut ()  ;
  CDisplayBand* Band ()  ;
  CDisplayContour* Contour ()  ;

  // Adaptors to the Xamine C-API:

  void FormatMessageBlock(msg_object& rMsg) const;

};

//
//  The following are specific types of gates:
//

class CDisplayCut : public CXamineGate {
public:
  CDisplayCut(const msg_object& rGate) : CXamineGate(rGate)
  { }
  CDisplayCut(UInt_t nSpectrum, UInt_t nId, const std::string& rName) :
    CXamineGate(nSpectrum, nId, rName)
    {
      setGateType(kgCut1d);
    }
};

class CDisplayBand : public CXamineGate {
public:
  CDisplayBand(const msg_object& rgate) : CXamineGate(rgate)
  {}
  CDisplayBand(UInt_t nSpectrum, UInt_t nId, const std::string& rName) :
    CXamineGate(nSpectrum, nId, rName)
    {
      setGateType(kgBand2d);
    }
};

class CDisplayContour : public CXamineGate {
public:
  CDisplayContour(const msg_object& rgate) : CXamineGate(rgate)
  {}
  CDisplayContour(UInt_t nSpectrum, UInt_t nId, const std::string& rName) :
    CXamineGate(nSpectrum, nId, rName)
    {
      setGateType(kgContour2d);
    }
};

#endif
