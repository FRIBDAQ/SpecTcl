///////////////////////////////////////////////////////////
//  SetWidthVisitor.cpp
//  Implementation of the Class SetWidthVisitor
//  Created on:      30-Mar-2005 11:03:55 AM
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

#include <config.h>
#include "SetWidthVisitor.h"

#include "CTreeParameter.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

SetWidthVisitor::~SetWidthVisitor()
{

}


/**
 * Constructor.
 * @param pattern
 *        Pattern to match in order to set.
 * @param width
 *        New Width parameter.
 * 
 */
SetWidthVisitor::SetWidthVisitor(string pattern, double width)  :
  CMatchingVisitor(pattern),
  m_fNewWidth(width)
{

}


/**
 * Set the width of matching parameters.
 * @param parameter 
 *        The parameter to process.
 * 
 */
void 
SetWidthVisitor::OnMatch(CTreeParameter* parameter)
{
  parameter->setInc(m_fNewWidth);
}



