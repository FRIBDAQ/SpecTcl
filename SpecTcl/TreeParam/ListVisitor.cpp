///////////////////////////////////////////////////////////
//  ListVisitor.cpp
//  Implementation of the Class ListVisitor
//  Created on:      01-Apr-2005 02:46:29 PM
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
#include "ListVisitor.h"

#include "CTreeParameter.h"
#include <TCLResult.h>

#include <stdio.h>
#include <TCLString.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


ListVisitor::~ListVisitor()
{

}


/**
 * Constructing the list visitor.
 *
 * @param pattern
 *        Pattern that must match for a treeparameter to be added to
 *        the listed set.
 * @param rResult
 *        Result into which to fill in the list.
 * 
 */
ListVisitor::ListVisitor(string pattern, CTCLResult& rResult) :
  CMatchingVisitor(pattern),
  m_OutputList(rResult)
{

}


/**
 * Responsible for generating a list describing a single tree parameter.
 * @param parameter    The parameter to process.
 * 
 */
void 
ListVisitor::OnMatch(CTreeParameter* parameter)
{
  CTCLString item;

  char conversion[100];

  item.AppendElement(parameter->getName());

  snprintf(conversion, sizeof(conversion), "%d", parameter->getBins());
  item.AppendElement(conversion);

  snprintf(conversion, sizeof(conversion), "%g", parameter->getStart());
  item.AppendElement(conversion);

  snprintf(conversion, sizeof(conversion), "%g", parameter->getStop());
  item.AppendElement(conversion);

  snprintf(conversion, sizeof(conversion), "%g", parameter->getInc());
  item.AppendElement(conversion);

  item.AppendElement(parameter->getUnit());

  m_OutputList.AppendElement((const char*)item);
}




