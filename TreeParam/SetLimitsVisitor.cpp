///////////////////////////////////////////////////////////
//  SetLimitsVisitor.cpp
//  Implementation of the Class SetLimitsVisitor
//  Created on:      30-Mar-2005 11:03:54 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
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
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//



#include <config.h>
#include "SetLimitsVisitor.h"
#include "CTreeParameter.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/**
 * Constructor
 * @param pattern
 *        Only change parameters that match this visitor.
 * @param low
 *        New Low limit
 * @param high
 *        New High limit.
 * 
 */
SetLimitsVisitor::SetLimitsVisitor(string pattern, double low, double high) :
  CMatchingVisitor(pattern),
  m_fLow(low),
  m_fHigh(high)
{

}


/**
 * Set the limits on the current parameter.
 * @param parameter
 *        The tree parameter on which to set the upper/lower limits.
 * 
 */
void SetLimitsVisitor::OnMatch(CTreeParameter* parameter)
{
  parameter->setStart(m_fLow);
  parameter->setStop(m_fHigh);
}



//! Destructor

SetLimitsVisitor::~SetLimitsVisitor() 
{}
