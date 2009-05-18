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

static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";

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

/*
   Change log:
   $Log$
   Revision 5.3  2005/09/22 12:40:37  ron-fox
   Port in the bitmask spectra

   Revision 5.2  2005/06/03 15:19:26  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.5  2005/05/27 17:47:38  ron-fox
   Re-do of Gamma gates also merged with Tim's prior changes with respect to
   glob patterns.  Gamma gates:
   - Now have true/false values and can therefore be applied to spectra or
     take part in compound gates.
   - Folds are added (fold command); and these perform the prior function
       of gamma gates.

   Revision 5.1.2.2  2005/03/15 17:28:52  ron-fox
   Add SpecTcl Application programming interface and make use of it
   in spots.

   Revision 5.1.2.1  2004/12/15 17:24:04  ron-fox
   - Port to gcc/g++ 3.x
   - Recast swrite/sread in terms of tcl[io]stream rather than
     the kludgy thing I had done of decoding the channel fd.
     This is both necessary due to g++ 3.x's runtime and
     nicer too!.

   Revision 5.1  2004/11/29 16:56:10  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.3.4.1  2004/10/22 19:19:40  ron-fox
    Force gamma gate creation to throw gate factory exceptions when the gate
    spectra don't have names of spectra that exist.

   Revision 4.3  2003/04/15 19:25:21  ron-fox
   To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs. Modifications support the input, listing and application information when gate coordinates are floating point.

*/
#include <config.h>
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
#include <CGammaCut.h>
#include <CGammaBand.h>
#include <CGammaContour.h>
#include <MaskGates.h>
#include <MaskAndGate.h>
#include <MaskEqualGate.h>
#include <MaskNotGate.h>

#include <histotypes.h>
#include <Parameter.h>
#include <Histogrammer.h>


#include <assert.h>
#include <list>
#include <algorithm>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


static const  char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved GateFactory.cpp \n";

// Functions for class CGateFactory

static inline Float_t max(Float_t i, Float_t j) {
  return (i > j ) ? i : j;
}
static inline Float_t  min(Float_t i, Float_t j) {
  return (i < j) ? i : j ;
}

/// Static storage:

UInt_t CGateFactory::m_nGateId(0);

/////////////////////////////////////////////////////////////////////////////////
//  Function:       CreateGate(GateType nGateType, 
//                             const vector<string>& rGates)
//  Operation Type: Creational

/*!

\para Functionality:
 Creates a gate given the input data.  This can create
 gates which are primitive gates without point arrays
 and compound gates.

 \para Formal Parameters:
      \param <TT>GateType (CGateFactory [in])</TT>
         This must be one  of:
           - bandcountour  Contour formed from two bands.
	   - not  Inverse of a gate.
	   - and  And of a list of gates.
	   - or   Or of a list of gates.
	   - true Always true
	   - false Always false
	   - deleted Gate that has been deleted.
       \param Gates (std::vector<std::string>& [in]):
            List of gates which are used to generate the final gate.

 \para Returns:
 \retval CGate*
     Pointer to dynamically generated gate.  It is up to the
     caller to delete the gate when done with it.

 \para Exceptions:
 \throw CGateFactoryException
    If there was a problem createing a gate, an exception is thrown indicating
    the type of gate being created and why the exception was thrown.
*/
CGate* 
CGateFactory::CreateGate(GateType nGateType, 
			 const vector<string>& rGates)  
{

  
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
//                            const vector<FPoint>& rPoints)
//  Operation Type: Creational
/*!
  \para Functionality:
  Creates a gate from a list of points.
   
  \para Formal Parameters:
        \param <TT> GateType (CGateFactory [in])</TT>
               Type of gate being created.
                Should be one of:
		- cut     Upper lower limit cut (slice).
		- contour Interior of closed polygon.
		- band    Below a polyline.
        \param rParameters (vector<string>& [in]):
               Set of parameters involved in the gate.
        \param rPoints (vector<FPoint>& [in]):
             A set of points involved in the gate.

     \para Returns:
     \retval CGate*
        Pointer to a created gate or throws a CGateFactoryException if the 
        gate could not be made.

     \para Throws:
     \throw CGateFactoryException
        If the gate could not be made.  The exception describes the type
	of gate being made along with why the exception was thrown.
  
*/
CGate* 
CGateFactory::CreateGate(GateType eType, 
			 const vector<string>& rParameters, 
			 const vector<FPoint>& rPoints)  
{


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
/*!
  
  \para Functionality:
   Create a gamma gate.  These are really accepted on a spectrum unlike
   the other gates that are accepted on parameters.
   
   \para Formal Parameters:
   \param <TT>eType (GateType [in]):</TT>
       Type of gate that is to be created.  This must be one of:
       - gammacut A cut on a set of gamma parameters.
       -gammaband A band on a set of ordered pairs of gamma params.
       -gammacontour A contour on a set of ordered pairs of gamma
          parameters.
   \param <TT>rPoints (const vector<FPoint>& [in]):</TT>
       The set of points that define the contour. 
   \param <TT>rParameters (const vector<UInt_t>& [in]):</TT>
       The set of parameters on which this gate should be displayed.

   \para Returns:
   \retval CGate*
     A pointer to the gate that was created.

   \para Exceptions:
   \throw CGateFactoryException
     In the event creating the gate is impossible.  The exception
     indicates the type of the gate being created and why the
     gate could not be created.
*/
CGate* 
CGateFactory::CreateGate(GateType eType,
			 const vector<FPoint>& rPoints,
			 const vector<UInt_t>& rParameters) {
  switch(eType) {
  case gammacut:
    if(rPoints.size() != 2) {
      throw CGateFactoryException(CGateFactoryException::WrongPointCount,
				  eType,
				  "Creating gamma cut in CreateGate");
    }
    return CreateGammaCut(min(rPoints[0].X(), rPoints[1].X()), 
			  max(rPoints[0].X(), rPoints[1].X()), rParameters);
  case gammaband:
    return CreateGammaBand(rPoints, rParameters);
  case gammacontour:
    return CreateGammaContour(rPoints, rParameters);
  default:
    throw CGateFactoryException(CGateFactoryException::WrongConstructor,
				eType,
				"Determining type of gate in CreateGate");
  }
}
/*!

Switch statement for Mask Gates

*/

CGate* 
CGateFactory::CreateGate(GateType eType, 
			 const vector<string>& rParameters,
			 long comparison)  
{


  switch(eType) {
  case em:
    return CreateMaskEqualGate(rParameters, comparison);
  case am:
    return CreateMaskAndGate(rParameters, comparison);
  case nm: 
    return CreateMaskNotGate(rParameters, comparison);
  }
  return (CGate*)NULL;
}

///////////////////////////////////////////////////////

// The functions below create the gates of various types.

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
//                             const vector<FPoint>& rPoints)
//  Operation Type: Creational
CBand* 
CGateFactory::CreateBand(const vector<string>& rParameters,
			       const vector<FPoint>& rPoints)  
{
  // Creates a band gate.
  //  Band gates are made when the event is below the
  //  band.
  //     Formal Parameters:
  //        const vector<string>& rParams:
  //           2 element vector containing x and y parameter names.
  //        const vector<FPoint>& rPoints
  //           at least 2 element vector containing x,y 
  //           points of the band.
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
//                                const vector<FPoint>& rPoints)
//  Operation Type: Creational
//
CContour* 
CGateFactory::CreateContour(const vector<string>& rParameters, 
			    const vector<FPoint>& rPoints)  
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
  //     const vector<FPoint>& rPoints:
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
CGateFactory::CreateGammaCut(Float_t nLow, Float_t nHigh, 
			     const vector<UInt_t>& rParameters)
{
  
  // Create the cut:
  
  return new CGammaCut(nLow, nHigh, rParameters);
}

CGammaBand*
CGateFactory::CreateGammaBand(const vector<FPoint>& rPoints,
			      const vector<UInt_t>& rParameters)
{
  if(rPoints.size() < 2) {	// At least 2 points make a band.
    throw CGateFactoryException(CGateFactoryException::WrongPointCount,
				gammaband,
				"Creating gammaband in CreateGammaBand");
  }


  
  return new CGammaBand(rPoints, rParameters);

}

CGammaContour*
CGateFactory::CreateGammaContour(const vector<FPoint>& rPoints,
				 const vector<UInt_t>& rParameters)
{
  if(rPoints.size() < 3) {	// At least 3 points make a closed contour.
    throw CGateFactoryException(CGateFactoryException::WrongPointCount,
				gammacontour,
				"Creating gammacontour in CreateGammaContour");
  }

  return new CGammaContour(rPoints, rParameters);
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
    return (C2Bands*)NULL;
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
    return (C2Bands*)NULL;
  }
  // The gates must be defined on the same parameters.. if order is swapped,
  // then the points of one must have the x/y's swapped.
  //
  CBand& Band1((CBand&)*rGate1);
  CBand& Band2((CBand&)*rGate2);
  vector<FPoint> Pts1 = Band1.getPoints();
  vector<FPoint> Pts2 = Band2.getPoints();
  
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
      Pts2[i] = FPoint(Pts2[i].Y(), Pts2[i].X());
    }
  }
  else {			// Bad parameter set...
    throw CGateFactoryException(CGateFactoryException::MustBeSameParams,
				bandcontour,
				"Creating contour from two band gates.");
    return (C2Bands*)NULL;
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
			      Float_t nLow, Float_t nHigh)  
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
// Function:         CreateMaskEqualGate(const string& rParamterName,
//                                       char Compare)
//
// Operation Type:   Create
//
CMaskEqualGate* CGateFactory::CreateMaskEqualGate(const vector<string>& rParameterName,
						  long Compare) 
{
  // Creates a new Mask Equal Gate
  //   Mask Equal Gates return true when an event = Compare
  //
  // Formal parameters:
  //     const string& rParameterName:
  //          Name of the parameter on which the gate is set
  //     char Compare:
  //          String to compare the parameter to
  //
  UInt_t Id = ParameterToId(rParameterName[0], em, 
			    "Translating parameter name for Masked Equal");
  return new CMaskEqualGate(Id, Compare);

}

////////////////////////////////////////////////////////////////////////
//
// Function:         CreateMaskAndGate(const string& rParamterName,
//                                       char Compare)
//
// Operation Type:   Create
//
CMaskAndGate* CGateFactory::CreateMaskAndGate(const vector<string>& rParameterName,
						  long Compare) 
{
  // Creates a new Mask And Gate
  //
  // Formal parameters:
  //     const string& rParameterName:
  //          Name of the parameter on which the gate is set
  //     char Compare:
  //          String to compare the parameter to
  //
  UInt_t Id = ParameterToId(rParameterName[0], em, 
			    "Translating parameter name for Masked Equal");
  return new CMaskAndGate(Id, Compare);

}

////////////////////////////////////////////////////////////////////////
//
// Function:         CreateMaskEqualGate(const string& rParamterName,
//                                       char Compare)
//
// Operation Type:   Create
//
CMaskNotGate* CGateFactory::CreateMaskNotGate(const vector<string>& rParameterName,
						  long Compare) 
{
  // Creates a new Mask Not Gate 
  //
  // Formal parameters:
  //     const string& rParameterName:
  //          Name of the parameter on which the gate is set
  //     char Compare:
  //          String to compare the parameter to
  //
  UInt_t Id = ParameterToId(rParameterName[0], em, 
			    "Translating parameter name for Masked Equal");
  return new CMaskNotGate(Id, Compare);

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
/*!
  Look up the spectrum object associated with a spectrum name:
  If not found, an GateFactoryException is thrown.

  \param rName  [const string& in]:
      Name of the spectrum object to look up.
  \param eType [GateType]:
     The type of gate being created (used to throw the exception).
  \param pWhich [const char* in]:
     Additional exception text.

  \return CSpectrum
  \retval Pointer to the spectrum description.

  \exception CGateFactoryException in the event the name does not
             correspond to a spectrum.

 */
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
/*!
   Assign a unique gate id.
*/
UInt_t
CGateFactory::AssignId()
{
  return m_nGateId++;
}
