///////////////////////////////////////////////////////////
//  UncheckVisitor.cpp
//  Implementation of the Class UncheckVisitor
//  Created on:      31-Mar-2005 02:09:12 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
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



