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
//  CGammaContour.cpp
//  Implementation of the Class CGammaContour
//  Created on:      22-Apr-2005 12:54:38 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
#include <config.h>
#include "CGammaContour.h"
#include  "Gate.h"
#include <Event.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/**
 * There is no need for the destructor to take any action.
 */
CGammaContour::~CGammaContour()
{

}


/**
 * Construct a gamma  contour given the contour points and the parameters in the
 * gate.
 * @param points
 *    Points in parameter space that define the contour.
 * @param parameters
 *    The vector of parametrer numbers for the gate.
 * 
 */
CGammaContour::CGammaContour(const vector<FPoint>& points, 
			     const ParameterList& parameters) :
  CContour(parameters[0], parameters[1],
	   points),
  m_Parameters(parameters)
{

}


/**
 * Copy constructor.
 * @param rhs
 * 
 */
CGammaContour::CGammaContour(const CGammaContour& rhs) :
  CContour(rhs),
  m_Parameters(rhs.m_Parameters)
{
}


/**
 * Assignment:
 * @param rhs
 * 
 */
CGammaContour& 
CGammaContour::operator=(const CGammaContour& rhs)
{
  
  if (this != &rhs) {
    CContour::operator=(rhs);
    m_Parameters       = rhs.m_Parameters;
  }
  
  return *this;
  
  
}


/**
 * Equivalence comparison.
 * @param rhs    return ((CContour::operator==(rhs)    &
 *          (m_Parameters == m_Parameters);
 * 
 */
int 
CGammaContour::operator==(const CGammaContour& rhs) const
{
  return (CContour::operator==(rhs)      &&
	  (m_Parameters == rhs.m_Parameters));
}


/**
 * Inverse of operator==.
 * @param rhs
 * 
 */
int 
CGammaContour::operator!=(const CGammaContour& rhs) const
{

  return !(*this == rhs);


}

/*
**  Return the a copy of the parameters list.
*/
CGammaContour::ParameterList
CGammaContour::getParameters() const
{

  return m_Parameters;


}


/**
 * Produces a gate that is a faithful copy of *this.
 */
CGate* 
CGammaContour::clone()
{
  
  return new CGammaContour(*this);
  
  
}


/**
 * Returns name of gate type (gc).
 */
string 
CGammaContour::Type() const
{

  return string("gc");


}


/**
 * For a given event evaluates the gate.   This will iterate through the parameter
 * pairs..for each pair of ordered/defined parameters, the underlying contour will
 * be checked to see if the point is inside of it... Returns true if any pair
 * makes the gate.
 * @param rEvent
 *      The event to evaluate the gate on.
 */
Bool_t 
CGammaContour::inGate(CEvent& rEvent)
{
  size_t xSize = m_Parameters.size()-1;
  size_t ySize = m_Parameters.size();
  size_t nPar  = rEvent.size();

  for(int i = 0;  i < xSize; i++) {
    for(int j = i+1; j < ySize; j++) {
      UInt_t iPar   = m_Parameters[i];
      UInt_t jPar   = m_Parameters[j];
      if((iPar < nPar) && (jPar < nPar)) {
	if(rEvent[iPar].isValid()    &&  rEvent[jPar].isValid()) {
	  Float_t ifv = rEvent[iPar];
	  Float_t jfv = rEvent[jPar];
	  if (Inside(ifv, jfv)   || Inside(jfv, ifv)) {    // Check symmetrically about x=y.
	    return kfTRUE;
	  }
	}
      }
    }
  }
  return kfFALSE;
  
}


/**
 * Returns an iterator to the first parameter id.
 */
CGammaContour::parameterIterator 
CGammaContour::parametersBegin()
{

  return m_Parameters.begin();


}


/**
 * Returns a loop end parameter iterator 
 */
CGammaContour::parameterIterator 
CGammaContour::parametersEnd()
{

  return m_Parameters.end();


}


/**
 * Returns the number of points in the arameter list for this gate.
 */
size_t 
CGammaContour::parameterSize()
{

  return m_Parameters.size();


}


/*!
  Returns true if the specified parameter id is used by this gate:
*/
bool
CGammaContour::UsesParameter(UInt_t nParam)
{
  parameterIterator i = parametersBegin();
  while (i != parametersEnd()) {
    if (*i == nParam) {
      return true;
    }
    i++;
  }
  return false;
}
