///////////////////////////////////////////////////////////
//  UncheckVisitor.cpp
//  Implementation of the Class UncheckVisitor
//  Created on:      31-Mar-2005 02:09:12 PM
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
#include "UncheckVisitor.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

UncheckVisitor::~UncheckVisitor()
{

}


/**
 * Consructor
 * @param pattern    Pattern of name that should be matched.
 * 
 */
UncheckVisitor::UncheckVisitor(string pattern) :
  CMatchingVisitor(pattern)
{

}


/**
 * Called for a match.
 * @param pParam    The parameter to modify.
 * 
 */
void UncheckVisitor::OnMatch(CTreeParameter* pParam)
{
  pParam->resetChanged();
}



