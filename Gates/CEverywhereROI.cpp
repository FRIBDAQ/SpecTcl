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
//  CEverywhereROI.cpp
//  Implementation of the Class CEverywhereROI
//  Created on:      20-Jun-2005 10:10:54 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#include <config.h>
#include "CEverywhereROI.h"
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
  Constructor - The base class does everything.
*/
CEverywhereROI::CEverywhereROI(CAxis& xAxis, CAxis& yAxis) :
  CROI(xAxis, yAxis)
{

}


/*!
  Destructor - The base class does everything.
*/
CEverywhereROI::~CEverywhereROI()
{

}
/*!
   Copy construction.. delegates all responsibility to the base class
*/
CEverywhereROI::CEverywhereROI(const CEverywhereROI& rhs) :
  CROI(rhs)
{
}
/*!
   Assignment delegates to the base class:
*/
CEverywhereROI& 
CEverywhereROI::operator=(const CEverywhereROI& rhs)
{
  CROI::operator=(rhs);
  return *this;
}
/*!
  Equality compare - delegate to base class.
*/
int
CEverywhereROI::operator==(const CEverywhereROI& rhs) const
{
  return CROI::operator==(rhs);
}

/*!
  Inequality.
*/
int
CEverywhereROI::operator!=(const CEverywhereROI& rhs) const
{
  return !(*this == rhs);
}

/**
 * returns true regardless of X/Y.
 *
 * @param x    X coordinate of the spectrum.
 * @param y    Y coordinate of the spectrum.
 * 
 */
bool 
CEverywhereROI::operator()(int x, int y)
{


  return true;

}


