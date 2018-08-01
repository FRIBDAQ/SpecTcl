///////////////////////////////////////////////////////////
//  SetUnitsVisitor.h
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


#ifndef SETUNITSVISITOR_H
#define SETUNITSVISITOR_H

#include "CMatchingVisitor.h"

#include <string>

// Forward definitions.

class CTreeParameter;

/**
 * Visitor that sets the units of matching parameters.
 * @author Ron Fox
 * @version 1.0
 * @created 31-Mar-2005 09:43:48 AM
 */
class SetUnitsVisitor : public CMatchingVisitor
{
private:
	/**
	 * New Units string.
	 */
	std::string m_Units;

public:
	virtual ~SetUnitsVisitor();
	SetUnitsVisitor(std::string pattern, std::string units);
	virtual void OnMatch(CTreeParameter* pParameter);


};




#endif
