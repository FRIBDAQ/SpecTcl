///////////////////////////////////////////////////////////
//  UncheckVisitor.h
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

#if !defined(__UNCHECKVISITOR_H)
#define __UNCHECKVISITOR_H

#ifndef __CMATCHINGVISITOR_H
#include "CMatchingVisitor.h"
#endif

#include "CTreeParameter.h"

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// forward definitions:

class CTreeParameter;

/**
 * Visits tree parameter objects, clearing the changed flag on all matching
 * parameters to false.
 * @author Ron Fox
 * @version 1.0
 * @created 31-Mar-2005 02:09:12 PM
 */
class UncheckVisitor : public CMatchingVisitor
{

public:
	virtual ~UncheckVisitor();
	UncheckVisitor(STD(string) pattern);
	void OnMatch(CTreeParameter* pParam);

};




#endif
