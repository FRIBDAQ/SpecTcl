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
//  CROI.cpp
//  Implementation of the Class CROI
//  Created on:      20-Jun-2005 10:10:55 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
#include <config.h>
#include "CROI.h"
#include <CAxis.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/*!
    Construct a region of interest.  Note that this is a base class
    that provides services for all regions of interest.
    @param xAxis
        Description of the Xaxis that is used to do coordinate transforms for
        that axis.
    @param yAxis
        Description of the Yaxis that is used to do coordinate transforms for 
	that axis.
*/
CROI::CROI(CAxis& xAxis, CAxis& yAxis) :
  m_xAxisSpecification(*(new CAxis(xAxis))),
  m_yAxisSpecification(*(new CAxis(yAxis)))
{

}


/*!
  Destroy a region of interest. We must delete our axis specification objects.

 */
CROI::~CROI()
{
  delete &m_xAxisSpecification;
  delete &m_yAxisSpecification;
}
/*!
   Copy construction.  We must nonentheless create a new set of axes as targets
   for the assignment of axes.
*/
CROI::CROI(const CROI& rhs) :
  m_xAxisSpecification(*(new CAxis(rhs.m_xAxisSpecification))),
  m_yAxisSpecification(*(new CAxis(rhs.m_yAxisSpecification)))
{
}

/*!
  Assignment.
 */
CROI&
CROI::operator=(const CROI& rhs)
{
  if (this != &rhs) {
    m_xAxisSpecification = rhs.m_xAxisSpecification;
    m_yAxisSpecification = rhs.m_yAxisSpecification;
  }
  return *this;
}
/*!
   Equality compare:
*/
int
CROI::operator==(const CROI& rhs) const
{
  return ( (m_xAxisSpecification == rhs.m_xAxisSpecification)   &&
	   (m_yAxisSpecification == rhs.m_yAxisSpecification));
}
/*!
   Inequality compares
*/
int
CROI::operator!=(const CROI& rhs) const
{
  return !(*this == rhs);
}

/*
 *
 *	Returns the parameter converted to X
 *	parameter values.
 *
 * @param x    An x channel number.
 * 
 */
double 
CROI::xToParam(Int_t x)
{

  return m_xAxisSpecification.AxisToParameter(x);

}


/**
 * Returns the parameter value that
 * corresponds to this y parameter value.
 *
 * @param y    Y channel value.
 * 
 */
double 
CROI::yToParam(Int_t y)
{

  return m_yAxisSpecification.AxisToParameter(y);

}


