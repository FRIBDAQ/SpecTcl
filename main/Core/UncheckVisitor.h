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


#ifndef UNCHECKVISITOR_H
#define UNCHECKVISITOR_H

#include "CMatchingVisitor.h"
#include "CTreeParameter.h"
#include <string>


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
	UncheckVisitor(std::string pattern);
	void OnMatch(CTreeParameter* pParam);

};




#endif
