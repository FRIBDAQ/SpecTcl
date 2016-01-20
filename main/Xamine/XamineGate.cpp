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

static const char* Copyright = "(C) Copyright Michigan State University 2011, All rights reserved";
//  CDisplayGate.cpp
// Encapsulates a spectrum gate/condition as
// it is expected to be passed to/from the displayer
// Type safe casts are included to support conversion of
// this to a pointer to a specific kind of display gate.  These
// are really only used to promote type checking.
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include <config.h>
#include "XamineGate.h"
#include "RangeError.h"
#include <string.h>
#include <assert.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Functions for class CDisplayGate
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CDisplayGate(const msg_object& rGateInfo)
// Operation Type:
//    Constructor
//
CXamineGate::CXamineGate(const msg_object& rGateInfo) :
  m_nSpectrum(rGateInfo.spectrum),
  m_nId(rGateInfo.id)
{
  if(rGateInfo.hasname) {
    m_sName = rGateInfo.name;
  }
  switch(rGateInfo.type) {
  case Xamine_cut1d:
    m_eGateType = kgCut1d;
    break;
  case Xamine_contour2d:
    m_eGateType = kgContour2d;
    break;
  case Xamine_band:
    m_eGateType = kgBand2d;
    break;
  case peak1d:
    m_eGateType = kgPeak1d;
    break;
  case fitline:
    m_eGateType = kgFitline;
    break;
  default:
    assert(kfFALSE);		// Very bad..
    break;
  }
  for(int i = 0; i < rGateInfo.npts; i++) {
    m_vPoints.push_back(CPoint(rGateInfo.points[i].x,
			       rGateInfo.points[i].y));
  }
}
//////////////////////////////////////////////////////////////////////////
//
// Funtction:
//    CDisplayGate(UInt_t nSpectrum, UInt_t nId, 
//		    const std::string& rName)
// Operation Type:
//     Constructor
//
CXamineGate::CXamineGate(UInt_t nSpectrum, UInt_t nId, 
			   const std::string& rName) :
  m_nSpectrum(nSpectrum),
  m_nId(nId),
  m_sName(rName),
  m_eGateType(kgUnSpecified)
{
  // Constructs a display gate for clients who will be entering the
  // gate into Xamine's internal gate data base.
  // 
  // Formal Parameters:
  //    UInt_t nSpectrum:
  //       Xamine spectrum slot gate is to be displayed on.
  //    UInt_t nId:
  //       Gate id.
  //    const std::string& rName:
  //       Refers to the name of the gate.
  // NOTE:
  //   Gate is constructed of an unspecified gate type and
  //   the point array is empty (should be filled in by client).
  //
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    PointIterator begin (  )
//  Operation Type:
//     selector
//
PointIterator 
CXamineGate::begin() 
{
// Returns a point vector iterator corresponding
// to the first point in the gate.
// If there are not yet points in the gate, then 
// end() is returned.
//

  return m_vPoints.begin();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    PointIterator end (  )
//  Operation Type:
//     Selector
//
PointIterator 
CXamineGate::end() 
{
// Returns an iterator representing the end
// of the points vector.
// Exceptions:  

  return m_vPoints.end();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t& size (  )
//  Operation Type:
//     Selector
//
UInt_t 
CXamineGate::size() const
{
// Returns the number of points in the gate.
// Exceptions:  

  return m_vPoints.size();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CPoint& operator[] ( UInt_t n )
//  Operation Type:
//     Selector
//
CPoint& 
CXamineGate::operator[](UInt_t n) 
{
// Returns a reference to the n'th point
// in the gate array.  If n >= size(), then 
// a CRangeError is thrown.
//
//  Formal Parameters:
//     UInt_t n
//        element of the gate point list to retrieve.
//

  if(n >= m_vPoints.size()) {
    CRangeError except(0, m_vPoints.size()-1, n,
		       "CDisplayGate::operator[] indexing gate point array");
    throw except;
  }
  return m_vPoints[n];
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void AddPoint ( const CPoint& rPoint )
//  Operation Type:
//     Mutator
//
void 
CXamineGate::AddPoint(const CPoint& rPoint) 
{
// Adds a point to the end of the gate point array
//
// Formal Parameters:
//       const CPoint& rPoint:
//            Refers to the point to add.

  m_vPoints.push_back(rPoint);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void RemovePoints ( UInt_t n=1 )
//  Operation Type:
//     Mutator
//
void 
CXamineGate::RemovePoints(UInt_t n) 
{
// Delete's the last n points from the point list.
// Throws a CRangeError if asked to delete past
//  beginning.
// Formal Parameters:
//     UInt_t n = 1
//        Number of points to delete.

  if(n > m_vPoints.size()) {
    CRangeError e(0, m_vPoints.size(), n,
			   "CDisplayGate::RemovePoints removing points");
    throw e;
  }
  for(int i = 0; i < n; i++) {
    m_vPoints.pop_back();
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CDisplayCut* Cut (  )
//  Operation Type:
//     Type safe cast.
//
CDisplayCut* 
CXamineGate::Cut() 
{
// Returns this cast to a CCut* if the 
// gate is in fact a cut.
// Otherwise returns kpNULL
//

  return ( (m_eGateType == kgCut1d) ? (CDisplayCut*)this : 
	                              (CDisplayCut*)kpNULL);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CDisplayBand* Band (  )
//  Operation Type:
//     Type safe cast.
//
CDisplayBand* 
CXamineGate::Band() 
{
// Returns this cast as a Band. or NULL if it isn't a band.

  return ( (m_eGateType == kgBand2d) ? (CDisplayBand*)this : 
	                               (CDisplayBand*)kpNULL);


}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CDisplayContour* Contour (  )
//  Operation Type:
//     Type safe cast.
//
CDisplayContour* 
CXamineGate::Contour() 
{
// Returns this cast as  a CDisplayContour if
// gate is a contour or NULL if not.

  return ( (m_eGateType == kgContour2d) ? (CDisplayContour*)this : 
	                                  (CDisplayContour*)kpNULL);

}
///////////////////////////////////////////////////////////////////////////
//
//  Function:
//     void FormatMessageBlock(const msg_object& rMsg) const
// Operation Type:
//     Adaptor (to Xamine's C-API
//
void
CXamineGate::FormatMessageBlock(msg_object& rMsg) const
{
  rMsg.spectrum = m_nSpectrum;
  rMsg.id       = m_nId;
  switch(m_eGateType) {
  case kgCut1d:
    rMsg.type = Xamine_cut1d;
    break;
  case kgContour2d:
    rMsg.type = Xamine_contour2d;
    break;
  case kgBand2d:
    rMsg.type = Xamine_band;
    break;
  default:
    assert(kfFALSE);
  }
  rMsg.hasname = (int)kfTRUE;
  memset(rMsg.name, 0, sizeof(rMsg.name));
  strncpy(rMsg.name, m_sName.c_str(), sizeof(rMsg.name) - 1);
  //
  // If m_vPoints has too many points that's a range error exception:
  //
  if(m_vPoints.size() > GROBJ_MAXPTS) {
    CRangeError re(0, GROBJ_MAXPTS, m_vPoints.size(),
	  "CDisplayGate::FormatMessageBlock -- copying points array");
    throw re;
  }
  rMsg.npts = m_vPoints.size();
  for(int i = 0; i < m_vPoints.size(); i++) {
    rMsg.points[i].x = m_vPoints[i].X();
    rMsg.points[i].y = m_vPoints[i].Y();
  }
}
