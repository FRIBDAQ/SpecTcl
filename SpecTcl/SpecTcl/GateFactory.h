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

#ifndef __GATEFACTORY_H  //Required for current class
#define __GATEFACTORY_H

#ifndef __POINT_H
#include <Point.h>
#endif

#ifndef __GATECONTAINER_H
#include <GateContainer.h>
#endif

#ifndef __SPECTRUM_H
#include <Spectrum.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_LIST
#include <list>
#define __STL_LIST
#endif

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
class CCut;
class CDeletedGate;
class CGammaCut;
class CGammaBand;
class CGammaContour;
//
//  The GateFactory Class:
//
class CGateFactory      
{                       
			
   CHistogrammer* m_pHistogrammer; //Ptr to histogrammer.        
public:
  // Data types... in this case the enum for the type sof gates which
  // can be created:

  enum GateType {
    and,			// And of multiple gates.
    band,			// 2-d band.
    bandcontour,		// Contour from band pair.
    contour,			// contour
    cut,			// Cut on 1-d spectrum.
    deleted,			// Deleted gate placeholder.
    falseg,			// Always false gate.
    not,			// Inverse of contained gate.
    or,				// OR of multiple gates.
    trueg,			// Alwayw made gate.
    gammacut,                   // Gamma cut
    gammaband,                  // Gamma band
    gammacontour                // Gamma contour
  };

public:

  // Constructors/destructors and other cannonical functions:
  //
  
  CGateFactory(CHistogrammer* pHistogrammer) :
     m_pHistogrammer(pHistogrammer)
    {}
  virtual ~ CGateFactory ( ) { }  //Destructor
  
  
   //Copy constructor alternative to compiler provided default copy constructor
   //Copy alternatives for association objects:
   //  (1) initialize association object to nullAssociation Object
   //  (2) Shallow copy to copy pointers of association objects 
   //  (3) Deep copy to create new association objects
   //      and copy values of association objects

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

   CGate* CreateGate (GateType nGateType, const vector<string>& rGates);
   CGate* CreateGate (GateType eType, const vector<string>& rParameters, 
		      const vector<CPoint>& rPoints);
   CGate* CreateGate (GateType eType, const vector<CPoint>& rPoints, 
		      const vector<string>& Ids);

   CTrueGate* CreateTrueGate ();
   CFalseGate* CreateFalseGate ();
   CDeletedGate* CreateDeletedGate ();
   CBand* CreateBand (const vector<string>& rParameters, 
		     const vector<CPoint>& rPoints);
   CContour* CreateContour (const vector<string>& rParameters, 
			    const vector<CPoint>& rPoints);
   C2Bands* CreateBandContour (const vector<string>& rBands);
   CNot* CreateNotGate (const string& rGateNames);
   CAndGate* CreateAndGate (const vector<string>& rGateNames);
   COrGate* CreateOrGate (const vector<string>& rGateNames)    ;
   CCut* CreateCut (const string& rParameterName, 
		    UInt_t nLow, UInt_t nHigh);
   CGammaCut* CreateGammaCut (UInt_t nLow, UInt_t nHigh,
			      const vector<string>& Specs);
   CGammaBand* CreateGammaBand (const vector<CPoint>& rPoints,
				const vector<string>& Specs);
   CGammaContour* CreateGammaContour (const vector<CPoint>& rPoints,
				      const vector<string>& Specs);
 
protected:
  UInt_t ParameterToId(const string& rName, GateType eType, 
		       const char* pWhich) const;
  CSpectrum* NameToSpec(const string& rName, GateType eType,
			const char* pWhich) const;
  CGateContainer& NameToGate(const string& rName, GateType eType,
			     const char* pWhich)const;
  void CreateGateList(list<CGateContainer*>& Gates,
		      const vector<string>& rNames,
		      GateType eType, const char* pWhich) const;
};

#endif
