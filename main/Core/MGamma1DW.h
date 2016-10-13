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
//  CGamma1DW.h:
//
//    This file defines the CGamma1DW class.
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venemaja@msu.edu
//
//  Copyright 2002 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __MGAMMA1DW_H  // Required for current class
#define __MGAMMA1DW_H

#ifndef __GAMMA1DW_H  // Required for base class
#include "Gamma1DW.h"
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

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

//  Forward class definition:

class CParameter;

class CMGamma1DW : public CGamma1DW
{
  struct MParameterDef {
    UInt_t  nParameter;
    UInt_t  nScale;
    Float_t nLow;
    Float_t nHigh;
    std::string sUnits;
    int operator==(const MParameterDef& r) const {
      return ((nParameter == r.nParameter) && (nScale == r.nScale) &&
	      (nLow == r.nLow) && (nHigh == r.nHigh) && (sUnits == r.sUnits));
    }
  };
  Float_t m_nLow;                 // Low limit to use on the mapping
  Float_t m_nHigh;                // Upper limit to use on the mapping
  UInt_t  m_nChannels;            // Number of bins
  std::vector<MParameterDef> m_vParameters;    // The parameters to histogram

 public:

  //Constructor(s) with arguments
  CMGamma1DW(const std::string& rName, UInt_t nId, std::vector<CParameter> rrParameters,
	     Float_t nLow, Float_t nHigh, UInt_t nChannels);

  virtual ~CMGamma1DW( ) { }   // Destructor

 private:
  
  // Copy constructor [illegal]
  CMGamma1DW(const CMGamma1DW& aCMGamma1DW);

  // Operator= Assignment operator [illegal]
  CMGamma1DW operator= (const CMGamma1DW& aCMGamma1DW);

 public:

  //Operator== Equality operator [Not too useful still]
  int operator== (const CMGamma1DW& aCMGamma1D)
    {
      return (
	      (CGamma1DW::operator==(aCMGamma1D)) &&
	      (m_nLow == aCMGamma1D.m_nLow) &&
	      (m_nHigh == aCMGamma1D.m_nHigh) &&
	      (m_nChannels = aCMGamma1D.m_nChannels) &&
	      (m_vParameters == aCMGamma1D.m_vParameters)
	      );
    }

  // Selectors:

 public:

  Float_t getLow() const
    {
      return m_nLow;
    }
  Float_t getHigh() const
    {
      return m_nHigh;
    }
  UInt_t getChannels() const 
    {
      return m_nChannels;
    }
  UInt_t getnParams() const
    {
      return m_vParameters.size();
    }
  virtual SpectrumType_t getSpectrumType()
    {
      return keMG1D;
    }
  std::string getUnits() const {
    return m_vParameters[0].sUnits;
  }
  
  // Mutators (available to derived classes):

 protected:

  void setLow(Float_t am_nLow)
    {
      m_nLow = am_nLow;
    }
  void setHigh(Float_t am_nHigh)
    {
      m_nHigh = am_nHigh;
    }
  void setChannels(UInt_t am_nChannels)
    {
      m_nChannels = am_nChannels;
    }
  void setParameters(std::vector<MParameterDef>& am_vParameters) 
    {
      m_vParameters = am_vParameters;
    }

  // Operations:

 public:

  virtual void Increment (const CEvent& rEvent);
  virtual void GammaGateIncrement(const CEvent& rEvent, std::string sGateType);
  UInt_t Dimension(UInt_t n) const {
    return ((n == 0) ? m_nChannels : 0);
  }
  UInt_t       Randomize(Float_t nChannel);
  UInt_t       GatePointToSpec(UInt_t nPoint);
  UInt_t       SpecPointToGate(UInt_t nPoint);
  UInt_t       ParamToSpecPoint(UInt_t nParamPoint, UInt_t nChan);
  UInt_t       RandomizeToMultipleBins(Float_t nChannel, Float_t nRatio);
};

#endif
