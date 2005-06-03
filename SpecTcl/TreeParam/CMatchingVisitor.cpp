///////////////////////////////////////////////////////////
//  CMatchingVisitor.cpp
//  Implementation of the Class CMatchingVisitor
//  Created on:      30-Mar-2005 11:03:48 AM
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
#include "CMatchingVisitor.h"

#include <tcl.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


CMatchingVisitor::CMatchingVisitor() :
  m_sPattern("*")		// Matches all names.
{

}



CMatchingVisitor::~CMatchingVisitor()
{

}


/**
 * Constructor.
 * @param pattern    Pattern to match.
 * 
 */
CMatchingVisitor::CMatchingVisitor(std::string pattern) :
  m_sPattern(pattern)
{

}


/**
 * Match and call the virtual function OnMatch if pattern matches.
 * @param element    The tree parameter dictionary element we are visiting.
 * 
 */
void CMatchingVisitor::operator()(pair<const string, CTreeParameter*>& element)
{
  if(Tcl_StringMatch(element.first.c_str(), m_sPattern.c_str())) {
    OnMatch(element.second);
  }
}



