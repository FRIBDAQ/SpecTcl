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

///////////////////////////////////////////////////////////
//  CContourROI.cpp
//  Implementation of the Class CContourROI
//  Created on:      20-Jun-2005 10:10:54 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#include <config.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

#include "CContourROI.h"
#include <CAxis.h>
#include <GateContainer.h>
#include <Contour.h>

#define DESIGN_BY_CONTRACT
#define CHECK_ALL

#include <DesignByContract.h>

using namespace DesignByContract;



/*!
   Construct a contour region of interest.
   @param pGate
      Pointer to the gate container that contains either
      a contour or a gamma contour.  A contract violation exception
      is thrown if another gate type is used.
   @param xAxis
      The  xaxis definition.
   @param yAxis
      The yaxis definitions
*/
CContourROI::CContourROI(CGateContainer* pGate,
			 CAxis& xAxis, CAxis& yAxis) : 
  CROI(xAxis, yAxis),
  m_pContour(pGate)
{
  string gateType((*m_pContour)->Type());
  REQUIRE((gateType == "c") || (gateType == "gc"), "Gate type invalid");
  
  
}

/*!
   Destruction requires nothing of us as the gate container storage is not managed
   by us.
*/

CContourROI::~CContourROI()
{

}
/*!
  Copy construction:
*/
CContourROI::CContourROI(const CContourROI& rhs) :
  CROI(rhs),
  m_pContour(rhs.m_pContour)
{
}
/*!
   Asignment.
*/
CContourROI&
CContourROI::operator=(const CContourROI& rhs) 
{
  CROI::operator=(rhs);
  m_pContour = rhs.m_pContour;
  return *this;
}
/*!
   Compare for equality: Rely on the uniqueness of gate container handles.
*/
int
CContourROI::operator==(const CContourROI& rhs) const
{
  return ( (CROI::operator==(rhs))                &&
	   (m_pContour == rhs.m_pContour));
}
/*!
    Compare for inequality.. note that the operator== is assumed correct.
*/
int 
CContourROI::operator!=(const CContourROI& rhs) const
{
  return !(*this == rhs);
}


/**
 * Returns true if the channel specified
 * is inside the contour.
 * - The spectrum coordinates are
 * transformed to parameter coordinates
 * using xToParam and yToParam in the base
 * class.
 * - The contour's Inside member is
 * invoked to determine the result.
 
 * @param x    Spectrum X coordinate.
 * @param y    Spectrum y coordinate.
 * 
 */
bool 
CContourROI::operator()(int x, int y)
{

  double xP = xToParam(x);
  double yP = yToParam(y);

  // the gate container was verified to point to a contour; or an object
  // derived from one...

  CContour* pContour = dynamic_cast<CContour*>(m_pContour->getGate());
  return pContour->Inside(xP, yP);

}


