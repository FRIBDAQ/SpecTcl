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

//  CDisplayGate.h:
//
//    This file defines the CDisplayGate class.
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

#ifndef __DISPLAYGATE_H  //Required for current class
#define __DISPLAYGATE_H

                               //Required for 1:M associated classes
#ifndef __POINT_H
#include "Point.h"
#endif

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


// 
// Forward references.
//
class CDisplayCut;
class CDisplayBand;
class CDisplayContour;

//

class CDisplayGate      
{
  UInt_t      m_nSpectrum;	// Number of spectrum on which gate is set
  UInt_t      m_nId;		// Identifier of the gate
  std::string m_sName;		// Textual name of the gate.
  GateType_t  m_eGateType;	// Type of gate in object.
  PointArray  m_vPoints;
  
public:


			//Constructors with arguments

  CDisplayGate(const msg_object& rGateInfo);
  CDisplayGate(UInt_t nSpectrum, UInt_t nId, 
		const std::string& rName);

  virtual ~CDisplayGate ( ) { }       //Destructor

			//Copy constructor
			//Update to access 1:M associated class attributes      
  CDisplayGate (const CDisplayGate& aCDisplayGate ) 
  {   
    m_nSpectrum = aCDisplayGate.m_nSpectrum;
    m_nId       = aCDisplayGate.m_nId;
    m_sName     = aCDisplayGate.m_sName;
    m_eGateType = aCDisplayGate.m_eGateType;
    m_vPoints   = aCDisplayGate.m_vPoints;

  }                                     

			//Operator= Assignment Operator

  CDisplayGate& operator= (const CDisplayGate& aCDisplayGate)
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

  int operator== (const CDisplayGate& aCDisplayGate) const
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

class CDisplayCut : public CDisplayGate {
public:
  CDisplayCut(const msg_object& rGate) : CDisplayGate(rGate)
  { }
  CDisplayCut(UInt_t nSpectrum, UInt_t nId, const std::string& rName) :
    CDisplayGate(nSpectrum, nId, rName) 
    {
      setGateType(kgCut1d);
    }
};

class CDisplayBand : public CDisplayGate {
public:
  CDisplayBand(const msg_object& rgate) : CDisplayGate(rgate)
  {}
  CDisplayBand(UInt_t nSpectrum, UInt_t nId, const std::string& rName) :
    CDisplayGate(nSpectrum, nId, rName)
    {
      setGateType(kgBand2d);
    }
};

class CDisplayContour : public CDisplayGate {
public:
  CDisplayContour(const msg_object& rgate) : CDisplayGate(rgate)
  {}
  CDisplayContour(UInt_t nSpectrum, UInt_t nId, const std::string& rName) :
    CDisplayGate(nSpectrum, nId, rName)
    {
      setGateType(kgContour2d);
    }
};

#endif
