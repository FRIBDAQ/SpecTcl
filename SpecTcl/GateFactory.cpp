// Class: CGateFactory
// Provides gate generation support.
//  The main entry, CreateGate takes a gate description
//   and generates a gate from it. This is overloaded to provide support
//   for gates which are compound gates and primary gates as well.
//   Each overload is capable of making particular subsets of gates
//  and throws a CGateFactory Exception if the gate could not be made.
//  The exception describes why the gate could not be made.
//
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "GateFactory.h"    				
#include "GateFactoryException.h"

#include <GateContainer.h>
#include <AndGate.h>
#include <OrGate.h>
#include <TrueGate.h>
#include <FalseGate.h>
#include <DeletedGate.h>
#include <Band.h>
#include <Contour.h>
#include <C2Bands.h>
#include <Cut.h>
#include <Not.h>
#include <GammaCut.h>
#include <GammaBand.h>
#include <GammaContour.h>

#include <histotypes.h>
#include <Parameter.h>
#include <Histogrammer.h>


#include <assert.h>
#include <list>
#include <algorithm>

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved GateFactory.cpp \n";

// Functions for class CGateFactory

static inline int max(int i, int j) {
  return (i > j ) ? i : j;
}
static inline int min(int i, int j) {
  return (i < j) ? i : j ;
}

/////////////////////////////////////////////////////////////////////////////////
//  Function:       CreateGate(GateType nGateType, 
//                             const vector<string>& rGates)
//  Operation Type: Creational
CGate* 
CGateFactory::CreateGate(GateType nGateType, 
			 const vector<string>& rGates)  
{
  // Creates a gate given the input data.  This can create
  // gates which are primitive gates without point arrays
  // and compound gates.
  //
  // Formal Parameters:
  //      CGateFactory::GateType nGateType:
  //         This must be one  of:
  //               {bandcountour, not, and, or, true, false, deleted}
  //       std::vector<std::string>& Gates:
  //            List of gates which are used to generate the final gate.
  // Returns:
  //   Pointer to dynamically generated gate.  It is up to the
  //  caller to delete the gate when done with it.
  
  switch(nGateType) {
  case And:
    return CreateAndGate(rGates);
  case bandcontour:
    return CreateBandContour(rGates);
  case deleted:
    if(rGates.size() != 0) {
      throw CGateFactoryException(CGateFactoryException::WrongGateCount,
				  nGateType,
				  "Creating a deleted gate in CreateGate");
    }
    return CreateDeletedGate();
  case falseg:
    if(rGates.size() != 0) {
      throw CGateFactoryException(CGateFactoryException::WrongGateCount,
				  nGateType,
				  "Creating a False gate in CreateGate");
    }
    return CreateFalseGate();
  case Not:
    if(rGates.size() != 1) {
      throw CGateFactoryException(CGateFactoryException::WrongGateCount,
				  nGateType,
				  "Creating a Not gate in CreateGate");
    }
    return CreateNotGate(rGates[0]);
  case Or:
    return CreateOrGate(rGates);
  case trueg:
    if(rGates.size() != 0) {
      throw CGateFactoryException(CGateFactoryException::WrongGateCount,
				  nGateType,
				  "Creating a TRUE gate in CreateGate");
    }
    return CreateTrueGate();
  default:
    throw CGateFactoryException(CGateFactoryException::WrongConstructor,
			       nGateType,
			       "Figuring out gate type in CreateGate");
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Function:       CreateGate(GateType eType, 
//                            const vector<string>& rParameters, 
//                            const vector<CPoint>& rPoints)
//  Operation Type: Creational
CGate* 
CGateFactory::CreateGate(GateType eType, 
			 const vector<string>& rParameters, 
			 const vector<CPoint>& rPoints)  
{
  // Creates a gate from a list of points.
  // 
  // Formal Parameters:
  //      CGateFactory::GateType:
  //             Type of gate being created.
  //              Should be one of:
  //                    { cut, contour, band}
  //      vector<string>& rParameters:
  //             Set of parameters involved in the gate.
  //      vector<CPoint>& rPoints:
  //           A set of points involved in the gate.
  //   Returns:
  //      Pointer to a created gate or throws a CGateFactoryException if the 
  //      gate could not be made.
  //

  switch(eType) {
  case band:
    return CreateBand(rParameters, rPoints);
  case cut:
    if(rParameters.size() != 1) {
      throw CGateFactoryException(CGateFactoryException::WrongParameterCount,
				  eType,
				  "Creating cut in CreateGate");
    }
    if(rPoints.size() != 2) {
      throw CGateFactoryException(CGateFactoryException::WrongPointCount,
				  eType,
				  "Creating cut in CreateGate");
    }
      return CreateCut(rParameters[0], 
		       min(rPoints[0].X(), rPoints[1].X()), 
		       max(rPoints[0].X(), rPoints[1].X()));
  case contour:
    return CreateContour(rParameters, rPoints);
  default:
    throw CGateFactoryException(CGateFactoryException::WrongConstructor,
				eType,
				"Determining type of gate in CreateGate");
  }
}

CGate* 
CGateFactory::CreateGate(GateType eType,
			 const vector<CPoint>& rPoints,
			 const vector<string>& rSpectrum) {
  switch(eType) {
  case gammacut:
    if(rPoints.size() != 2) {
      throw CGateFactoryException(CGateFactoryException::WrongPointCount,
				  eType,
				  "Creating gamma cut in CreateGate");
    }
    return CreateGammaCut(min(rPoints[0].X(), rPoints[1].X()), 
			  max(rPoints[0].X(), rPoints[1].X()), rSpectrum);
  case gammaband:
    return CreateGammaBand(rPoints, rSpectrum);
  case gammacontour:
    return CreateGammaContour(rPoints, rSpectrum);
  default:
    throw CGateFactoryException(CGateFactoryException::WrongConstructor,
				eType,
				"Determining type of gate in CreateGate");
  }
}
//  Function:       CreateTrueGate()
//  Operation Type: creational
CTrueGate* 
CGateFactory::CreateTrueGate()  
{
  // Creates a True gate.  A True gate
  // is one which always evaluates as made.
  //
  //
  return new CTrueGate;
}
//  Function:       CreateFalseGate()
//  Operation Type: Creational
CFalseGate* 
CGateFactory::CreateFalseGate()  
{
  // Returns a new false gate.  A false gate
  // is one which is never made.
  return new CFalseGate;
}
//  Function:       CreateDeletedGate()
//  Operation Type: Creational
CDeletedGate* 
CGateFactory::CreateDeletedGate()  
{
  // Creates a 'deleted' gate.  This is a special subclass of
  // a false gate... it is used to represent gates which have been
  // deleted.
  return new CDeletedGate;
}
//  Function:       CreateBand(const vector<string>& rParameters, 
//                             const vector<CPoint>& rPoints)
//  Operation Type: Creational
CBand* 
CGateFactory::CreateBand(const vector<string>& rParameters,
			       const vector<CPoint>& rPoints)  
{
  // Creates a band gate.
  //  Band gates are made when the event is below the
  //  band.
  //     Formal Parameters:
  //        const vector<string>& rParams:
  //           2 element vector containing x and y parameter names.
  //        const vector<CPoint>& rPoints
  //           2 element vector containing x,y points of the band.
  // 
  
  if(rParameters.size() != 2) {	// Bands have exactly 2 parameters.
    throw CGateFactoryException(CGateFactoryException::WrongParameterCount,
				band,
				"Creating band in CreateBand");
  }
  // There must be at least 2 points in a band:
  //
  if(rPoints.size() < 2) {
    throw CGateFactoryException(CGateFactoryException::WrongPointCount,
				band,
				"Creating band in CreateBand");
  }
  //
  // Now try to create and return the band:
  //
  return new CBand(ParameterToId(rParameters[0], band, 
				 "Translating X parameter"),
		   ParameterToId(rParameters[1], band, 
				 "Translating Y parameter"),
		   rPoints);
  
}
////////////////////////////////////////////////////////////////////////////
//
//  Function:       CreateContour(const vector<string>& rParameters,
//                                const vector<CPoint>& rPoints)
//  Operation Type: Creational
//
CContour* 
CGateFactory::CreateContour(const vector<string>& rParameters, 
			    const vector<CPoint>& rPoints)  
{
  // Creates a contour gate.  Contour gates
  // are those formed from the interior of a closed
  // contour.  The odd crossing rule is used to
  // determine which points are inside the contour.
  // In addition a circumscribing rectangle is used
  // to make a quick, space-saving make/break check
  // on inclusion.
  //
  // Formal Parameters:
  //     const vector<string>& rParameters:
  //              A 2 element vector containing the
  //              x (element 0) and y (element 1) parameter
  //             names of the parameters being checked in the gate.
  //     const vector<CPoint>& rPoints:
  //             The set of points which define the edge of the
  //              contour.
  
  if(rParameters.size() != 2) {	// Exactly 2 parameters.
    throw CGateFactoryException(CGateFactoryException::WrongParameterCount,
				contour,
				"Creating contour in CreateContour");
  }
  if(rPoints.size() < 3) {	// At least 3 points make a closed contour.
    throw CGateFactoryException(CGateFactoryException::WrongPointCount,
				contour,
				"Creating contour in CreateContour");
  }
  // Try to create the gate:
  //
  return new CContour(ParameterToId(rParameters[0], contour, 
				    "Translating X parameter"), 
		      ParameterToId(rParameters[1], contour,
				    "Translating Y parameter"),
		      rPoints);
}

CGammaCut*
CGateFactory::CreateGammaCut(UInt_t nLow, UInt_t nHigh, 
			     const vector<string>& rSpectrum)
{
  if(rSpectrum.size() > 0) {
    //vector<CSpectrum*> Specs;
    //for(UInt_t i = 0; i < rSpectrum.size(); i++) {
    //Specs.push_back(NameToSpec(rSpectrum[i], gammacut,
    //			 "Translating spectrum"));
    //}
    return new CGammaCut(nLow, nHigh, rSpectrum);
  }
  else
    return new CGammaCut(nLow, nHigh);
}

CGammaBand*
CGateFactory::CreateGammaBand(const vector<CPoint>& rPoints,
			      const vector<string>& rSpectrum)
{
  if(rPoints.size() < 2) {	// At least 2 points make a band.
    throw CGateFactoryException(CGateFactoryException::WrongPointCount,
				gammaband,
				"Creating gammaband in CreateGammaBand");
  }
  
  if(rSpectrum.size() > 0) {
    //vector<CSpectrum*> Specs;
    //for(UInt_t i = 0; i < rSpectrum.size(); i++) {
    //Specs.push_back(NameToSpec(rSpectrum[i], gammaband,
    //			 "Translating spectrum"));
    //}
    return new CGammaBand(rPoints, rSpectrum);
  }
  else
    return new CGammaBand(rPoints);
}

CGammaContour*
CGateFactory::CreateGammaContour(const vector<CPoint>& rPoints,
				 const vector<string>& rSpectrum)
{
  if(rPoints.size() < 3) {	// At least 3 points make a closed contour.
    throw CGateFactoryException(CGateFactoryException::WrongPointCount,
				gammacontour,
				"Creating gammacontour in CreateGammaContour");
  }

  if(rSpectrum.size() > 0) {
    //vector<CSpectrum*> Specs;
    //for(UInt_t i = 0; i < rSpectrum.size(); i++) {
    //Specs.push_back(NameToSpec(rSpectrum[i], gammacontour,
    //			 "Translating spectrum"));
    //}
    return new CGammaContour(rPoints, rSpectrum);
  }
  else
    return new CGammaContour(rPoints);
}

////////////////////////////////////////////////////////////////////////////
//
//  Function:       CreateBandContour(const vector<string>& rBands)
//  Operation Type: Creational

C2Bands* 
CGateFactory::CreateBandContour(const vector<string>& rBands)  
{
  // Creates a contour given a pair of bands.
  // The left most and right most points of each band
  // are joined to form the full contour.  Note that
  // the two bands which are used to create the
  // contour must be created on the same set of 
  // parameters.
  //
  //   Formal Parameters:
  //       const vector<string>& rGates
  //           2 element vector containing
  //          the names of the upper and lower band
  //          which make up the contour/gate.

  if(rBands.size() != 2) {	// Exactly 2 bands in one of these:
    throw CGateFactoryException(CGateFactoryException::WrongGateCount,
				bandcontour,
				"Creating band contour in CreateBandContour");
  }
  //
  // Next, the Gate names are translated, both gates must be bands:
  //
  CGateContainer& rGate1(NameToGate(rBands[0], bandcontour, 
				    "Creating 2-band contour"));
  CGateContainer& rGate2(NameToGate(rBands[1], bandcontour,
				    "Creating 2-band contour"));
  //  The gates must both be bands:
  //
  if( (rGate1->Type() != "b") || (rGate2->Type() != "b") ) {
    throw CGateFactoryException(CGateFactoryException::MustBeBand,
				bandcontour,
				"Checking band types in CreateBandContour");
  }
  // The gates must be defined on the same parameters.. if order is swapped,
  // then the points of one must have the x/y's swapped.
  //
  CBand& Band1((CBand&)*rGate1);
  CBand& Band2((CBand&)*rGate2);
  vector<CPoint> Pts1 = Band1.getPoints();
  vector<CPoint> Pts2 = Band2.getPoints();
  
  UInt_t x1 = Band1.getxId();
  UInt_t y1 = Band1.getyId();
  UInt_t x2 = Band2.getxId();
  UInt_t y2 = Band2.getyId();

  if( (x1 == x2) && (y1 == y2)) { // Same parameters, same order....
    // No preparation required.
  }
  else if ((x1 == y2) && (y1 == x2)) { // Flipped order.
    // Pts2 must have coordinates flipped

    for(UInt_t i = 0; i < Pts2.size(); i++) { // for_each requires const f...
      Pts2[i] = CPoint(Pts2[i].Y(), Pts2[i].X());
    }
  }
  else {			// Bad parameter set...
    throw CGateFactoryException(CGateFactoryException::MustBeSameParams,
				bandcontour,
				"Creating contour from two band gates.");
  }
  return new C2Bands(x1,y1,
		     Pts1, Pts2);	// Create the band
  
}
/////////////////////////////////////////////////////////////////////////
//
//  Function:       CreateNotGate(const string& rBandName)
//  Operation Type: Creational.
//
CNot* 
CGateFactory::CreateNotGate(const string& rBandName)  
{
  // Creates a band which returns true if it's component
  // band returns false.
  //
  // Formal Parameters:
  //      const string& rBand:
  //         Name of the band to 'not'.
  //

  return new CNot(NameToGate(rBandName, Not,
			     "Creating inverse gate"));
}
///////////////////////////////////////////////////////////////////////
//
//  Function:       CreateAndGate(const vector<string>& rBandNames)
//  Operation Type: Creational
//
CAndGate* 
CGateFactory::CreateAndGate(const vector<string>& rB)  
{
  // Creates a gate which is true only if
  // all component gates are true.
  //
  // Formal Parameters:
  //        const vector<string>& rNames:
  //            Names of bands which are in the gate.

  vector<string>& rBandNames((vector<string>&)rB);
  list<CGateContainer*> Gates;
  CreateGateList(Gates, rBandNames, And, "Constructing and gate list");
  return new CAndGate(Gates);
  
}
////////////////////////////////////////////////////////////////////////////
//
//  Function:       CreateOrGate(const vector<string>& rGateNames)
//  Operation Type: Creational
COrGate* CGateFactory::CreateOrGate(const vector<string>& rG)  
{
  // Creates a gate which is true if any of the
  // component gates is true.
  //
  // Formal Parameters:
  //     const vector<string>& rGateNamess:
  //         Names of gates to contain in this gate.
  vector<string>& rGateNames((vector<string>&)rG);
  list<CGateContainer*> Gates;
  CreateGateList(Gates, rGateNames, Or , "Constructing or gate list");
  return new COrGate(Gates);

}
///////////////////////////////////////////////////////////////////////////
//
//  Function:       CreateCut(const string& rParameterName, 
//                            UInt_t nLow, UInt_t nHigh)
//  Operation Type: Creational
//
CCut* CGateFactory::CreateCut(const string& rParameterName,
			      UInt_t nLow, UInt_t nHigh)  
{
  // Creates a cut gate.
  //   Cut gates are a 1-d lower/upper limit gate.
  //
  // Formal parameters:
  //     const string& rParameter:
  //         Name of the parameter on which the gate is set.
  //      UInt_t nLow:
  //         Low cutoff.
  //      UInt_t nHigh
  //          High level cutoff.
  //
  UInt_t Id = ParameterToId(rParameterName, cut, 
			    "Translating parameter name for cut");
  return new CCut(nLow, nHigh, Id);
  
  
}
////////////////////////////////////////////////////////////////////////
//
//  UInt_t ParameterToId(const string& rName, GateType eType, 
//		         const char* pWhich) const
// Operation Type:
//     Protected utility
//
UInt_t
CGateFactory::ParameterToId(const string& rName, GateType eType,
			    const char* pWhich) const
{
  // Converts a parameter name to it's id... if the parameter name
  // doesn't convert, then the appropriate CGateFactoryException is
  // constructed and thrown.
  //
  // Formal Parameters:
  //    const string& rName:
  //       Name of the parameter to convert.
  //    GateType eType:
  //       Type of gate being created (used in constructing exceptions).
  //    const char* pWhich:
  //       Text associated with exception.
  assert(m_pHistogrammer != (CHistogrammer*)kpNULL);
  CParameter *p = m_pHistogrammer->FindParameter(rName);
  if(!p) {
    throw CGateFactoryException(CGateFactoryException::NoSuchParameter,
				eType,
				pWhich);
  }
  else {
    return p->getNumber();
  }
}

CSpectrum*
CGateFactory::NameToSpec(const string& rName, GateType eType,
			 const char* pWhich) const
{
  assert(m_pHistogrammer != (CHistogrammer*)kpNULL);
  CSpectrum* pS = m_pHistogrammer->FindSpectrum(rName);
  if(!pS) {
    throw CGateFactoryException(CGateFactoryException::NoSuchSpectrum,
				eType, pWhich);
  }
  else {
    return pS;
  }
}

/////////////////////////////////////////////////////////////////////////
//
// Function:
//     CGateContainer& NameToGate(const string& rName, GateType eType,
//			          const char* pWhich)const
// Operation Type:
//     protected utility
//
CGateContainer&
CGateFactory::NameToGate(const string& rName, GateType eType,
					 const char* pWhich) const
{
  //     Translate a gate name to a gate.
  // 
  // Formal Parameters:
  //    const string& rName:
  //         Reference to a gate name string to translate.
  //    GateType eType:
  //         type of gate being constructed (used in instantiateing exception.
  //    const char* pWhich:
  //         String used in constructing exceptions.
  //
  assert(m_pHistogrammer != (CHistogrammer*)kpNULL);
  CGateContainer* pGateC = m_pHistogrammer->FindGate(rName);
  if(! pGateC) {
    throw CGateFactoryException(CGateFactoryException::NoSuchGate,
				eType, pWhich);
  }
  return *pGateC;
}
/////////////////////////////////////////////////////////////////////////////
//
// Function:
//   void CreateGateList(list<CGateContainer*>& Gates, vector<string>& rNames)
// Operation Type:
//   Protected utility.
//
void
CGateFactory::CreateGateList(list<CGateContainer*>& Gates, 
			     const vector<string>& rNames,
			     GateType eType, const char* pWhich) const
{
  for(UInt_t i = 0; i < rNames.size(); i++) {
    Gates.push_back(&NameToGate(rNames[i], eType, pWhich));
  }
}
