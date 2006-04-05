///////////////////////////////////////////////////////////
//  SetUnitsVisitor.cpp
//  Implementation of the Class SetUnitsVisitor
//  Created on:      31-Mar-2005 09:43:48 AM
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
#include "SetUnitsVisitor.h"
#include "CTreeParameter.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


SetUnitsVisitor::~SetUnitsVisitor()
{

}


/**
 * Constructor
 * @param pattern    Pattern that must be matched against to modify the parameter.
 * @param units    New units of measure to apply to the parameter.
 * 
 */
SetUnitsVisitor::SetUnitsVisitor(string pattern, string units) : 
  CMatchingVisitor(pattern),
  m_Units(units)
{

}


/**
 * @param pParameter    Paramter to modify.
 * 
 */
void SetUnitsVisitor::OnMatch(CTreeParameter* pParameter)
{

  try {
    pParameter->setUnit(m_Units);
  } 
  catch (...) {			// In case there are unbound params.
  }
}



