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

// Class: CGateFactory                     //ANSI C++
//
// Provides gate generation support.
//  The main entry, CreateGate takes a gate description
//   and generates a gate from it. This is overloaded to provide support
//   for gates which are compound gates and primary gates as well.
//   Each overload is capable of making particular subsets of gates
//  and throws a CGateFactory Exception if the gate could not be made.
//  The exception describes why thme gate could not be made.
//

//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved GateFactory.h
//



#ifndef GATEFACTORY_H  //Required for current class
#define GATEFACTORY_H

#include <config.h>

#include <Point.h>
#include <GateContainer.h>
#include <Spectrum.h>
#include <vector>
#include <string>
#include <list>

//
//  Class forward definitions
//

class CHistogrammer;	     
class CTrueGate;
class CFalseGate;
class CGate;
class CBand;
class CContour;
class C2Bands;
class CNot;
class CAndGate;
class COrGate;
class CMaskEqualGate;
class CMaskAndGate;
class CMaskNotGate;
class CCut;
class CDeletedGate;
class CGammaCut;
class CGammaBand;
class CGammaContour;
//
//  The GateFactory Class:
//
/*!
  Gate factories well, they produce gates.  Given a generic 
  description of a gate the factory produces an instance of the
  specific type of gate being described.  All the real work is done
  by the three overloaded CreateGate functions.
*/
class CGateFactory      
{                       
private:
  static UInt_t m_nGateId;
			
   CHistogrammer* m_pHistogrammer; //!< Ptr to histogrammer.        
public:
  // Data types... 

   //! enum for the type sof gates which can be created:
  enum GateType {
    And,			//!< And of multiple gates.
    band,			//!< 2-d band.
    bandcontour,		//!< Contour from band pair.
    contour,			//!< contour
    cut,			//!< Cut on 1-d spectrum.
    deleted,			//!< Deleted gate placeholder.
    falseg,			//!< Always false gate.
    Not,			//!< Inverse of contained gate.
    Or,				//!< OR of multiple gates.
    trueg,			//!< Alwayw made gate.
    gammacut,                   //!< Gamma cut
    gammaband,                  //!< Gamma band
    gammacontour,                //!< Gamma contour
    em,				//!< mask ==
    am,				//!< mask & != 0
    nm				//!< mask !=
  };

public:

  // Constructors/destructors and other cannonical functions:
  //
  
  CGateFactory(CHistogrammer* pHistogrammer) :
     m_pHistogrammer(pHistogrammer)
    {}
  virtual ~ CGateFactory ( ) { }  //Destructor
  
  

  CGateFactory (const CGateFactory& aCGateFactory ) 
  {
    m_pHistogrammer = aCGateFactory.m_pHistogrammer;  
  }                                     

   // Operator= Assignment Operator alternative to compiler provided 
   // default operator= 

  CGateFactory& operator= (const CGateFactory& aCGateFactory);
 
   //Operator== Equality Operator 

  int operator== (const CGateFactory& aCGateFactory) const;
  
  // Selectors:

public:

  CHistogrammer* getHistogrammer() const
  { 
    return m_pHistogrammer;
  }
                       
// Mutators:

protected:

  void setHistogrammer (CHistogrammer* am_pHistogrammer)
  { 
    m_pHistogrammer = am_pHistogrammer;
  }
  // Class operations:
   
public:

   CGate* CreateGate (GateType nGateType, 
		      const std::vector<std::string>& rGates);
   CGate* CreateGate (GateType eType, 
		      const std::vector<std::string>& rParameters, 
		      const std::vector<FPoint>& rPoints);
   CGate* CreateGate (GateType eType, 
		      const std::vector<FPoint>& rPoints, 
		      const std::vector<UInt_t>& rParameters);
  CGate* CreateGate(GateType eType,
                    const std::vector<std::string>& rParameters, 
		    long comparison);
  

            

   CTrueGate* CreateTrueGate ();
   CFalseGate* CreateFalseGate ();
   CDeletedGate* CreateDeletedGate ();
   CBand* CreateBand (const std::vector<std::string>& rParameters, 
		     const std::vector<FPoint>& rPoints);
   CContour* CreateContour (const std::vector<std::string>& rParameters, 
			    const std::vector<FPoint>& rPoints);
   C2Bands* CreateBandContour (const std::vector<std::string>& rBands);
   CNot* CreateNotGate (const std::string& rGateNames);
   CAndGate* CreateAndGate (const std::vector<std::string>& rGateNames);
   COrGate* CreateOrGate (const std::vector<std::string>& rGateNames)    ;
   CGate* CreateOrGate(
        std::string finalGateName,  bool x,
        const std::vector<std::string>& paramNames, std::vector<FPoint> points,
        const std::vector<UInt_t>& paramIds
    );
   CCut* CreateCut (const std::string& rParameterName, 
		    Float_t nLow, Float_t nHigh);
   CGammaCut* CreateGammaCut (Float_t nLow, Float_t nHigh,
			      const std::vector<UInt_t>& rParameters);
   CGammaBand* CreateGammaBand (const std::vector<FPoint>& rPoints,
				const std::vector<UInt_t>& rParameters);
   CGammaContour* CreateGammaContour (const std::vector<FPoint>& rPoints,
				      const std::vector<UInt_t>& rParameters);
  CMaskEqualGate* CreateMaskEqualGate(const STD(vector<std::string>)& rParameter,
                                      long Compare);
  CMaskAndGate* CreateMaskAndGate(const STD(vector<std::string>)& rParameter,
				  long Compare);
  CMaskNotGate* CreateMaskNotGate(const STD(vector<std::string>)& rParameter,
				  long Compare);
  
  static UInt_t  AssignId();
  static GateType stringToGateType(const std::string& sType);
protected:
  UInt_t ParameterToId(const std::string& rName, GateType eType, 
		       const char* pWhich) const;
  CSpectrum* NameToSpec(const std::string& rName, GateType eType,
			const char* pWhich) const;
  CGateContainer& NameToGate(const std::string& rName, GateType eType,
			     const char* pWhich)const;
  void CreateGateList(STD(list)<CGateContainer*>& Gates,
		      const std::vector<std::string>& rNames,
		      GateType eType, const char* pWhich) const;
};

#endif
