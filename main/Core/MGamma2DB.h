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


//  CGamma2DW.h:
//
//    This file defines the CGamma2DW class.
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venemaja@msu.edu
//
//  Copyright 2001 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef MGAMMA2DB_H  //Required for current class
#define MGAMMA2DB_H

#include "Gamma2DB.h"
#include <string>
#include <vector>
#include <histotypes.h>

//  Forward class definition:

class CParameter;

class CMGamma2DB : public CGamma2DB
{
  struct MParameterDef {
    UInt_t nParameter;
    UInt_t nScale;
    Float_t nLow;
    Float_t nHigh;
    std::string sUnits;
    int operator==(const MParameterDef& r) const {
      return ((nParameter == r.nParameter) && (nScale == r.nScale) &&
	      (nLow == r.nLow) && (nHigh == r.nHigh) && (sUnits == r.sUnits));
    }
  };
  Float_t m_nXLow;                 // Low limit to use on the x-mapping
  Float_t m_nYLow;                 // Low limit to use on the y-mapping
  Float_t m_nXHigh;                // Upper limit to use on the x-mapping
  Float_t m_nYHigh;                // Upper limit to use on the y-mapping
  UInt_t  m_nXChannels;            // Number of bins on x-axis
  UInt_t  m_nYChannels;            // Number of bins on y-axis
  vector<MParameterDef> m_vParameters; // Vector of parameters
  
public:

  //Constructor(s) with arguments
  CMGamma2DB(const std::string& rName, UInt_t nId,
	     vector<CParameter>& rParameters,
	     Float_t nXLow, Float_t nYLow, Float_t nXHigh, Float_t nYHigh,
	     UInt_t nXChans, UInt_t nYChans);

  virtual  ~ CMGamma2DB( ) { }       //Destructor	
private:
			//Copy constructor [illegal]

  CMGamma2DB(const CMGamma2DB& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CMGamma2DB operator= (const CMGamma2DB& aCMGamma1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CMGamma2DB& aCMGamma)
  { return (
	    (CGamma2DB::operator== (aCMGamma)) &&
	    (m_nXLow == aCMGamma.m_nXLow) &&
	    (m_nYLow == aCMGamma.m_nYLow) &&
	    (m_nXHigh == aCMGamma.m_nXHigh) &&
	    (m_nYHigh == aCMGamma.m_nYHigh) &&
	    (m_nXChannels == aCMGamma.m_nXChannels) &&
	    (m_nYChannels == aCMGamma.m_nYChannels) &&
	    (m_vParameters == aCMGamma.m_vParameters)
	    );
  }
  // Selectors 

public:
  Float_t getXLow() const {
    return m_nXLow;
  }
  Float_t getYLow() const {
    return m_nYLow;
  }
  Float_t getXHigh() const {
    return m_nXHigh;
  }
  Float_t getYHigh() const {
    return m_nYHigh;
  }
  UInt_t getXChannels() const {
    return m_nXChannels;
  }
  UInt_t getYChannels() const {
    return m_nYChannels;
  }
  std::string getXUnits() const {
    return m_vParameters[0].sUnits;
  }
  std::string getYUnits() const {
    return m_vParameters[1].sUnits;
  }
  UInt_t getnParams() const {
    return m_vParameters.size();
  }
  virtual SpectrumType_t getSpectrumType() {
    return keMG2D;
  }

  // Mutators (available to derived classes:

protected:
  void setXLow(Float_t x) {
    m_nXLow = x;
  }
  void setYLow(Float_t y) {
    m_nYLow = y;
  }
  void setXHigh(Float_t x) {
    m_nXHigh = x;
  }
  void setYHigh(Float_t y) {
    m_nYHigh = y;
  }
  void setXChannels(UInt_t xChans) {
    m_nXChannels = xChans;
  }
  void setYChannels(UInt_t yChans) {
    m_nYChannels = yChans;
  }

  //
  //  Operations:
  //   
public:                 
  virtual  void Increment(const CEvent& rEvent)  ;
  virtual  void GammaGateIncrement(const CEvent& Event, std::string sGateType);
  UInt_t        Dimension(UInt_t n) const;
  UInt_t        Randomize(Float_t nChannel);
  UInt_t        GatePointToSpec(UInt_t nPoint, UInt_t nIndex);
  UInt_t        SpecPointToGate(UInt_t nPoint, UInt_t nIndex);
  UInt_t        XParamToSpecPoint(UInt_t nParamPoint, UInt_t nChan);
  UInt_t        YParamToSpecPoint(UInt_t nParamPoint, UInt_t nChan);
  UInt_t        RandomizeToMultipleBins(Float_t nChannel, Float_t nRatio);
};

#endif
