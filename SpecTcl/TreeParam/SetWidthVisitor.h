///////////////////////////////////////////////////////////
//  SetWidthVisitor.h
//  Implementation of the Class SetWidthVisitor
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

#if !defined(__SETWIDTHVISITOR_H)
#define __SETWIDTHVISITOR_H

#include "CMatchingVisitor.h"

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// Forward definitions:

class CTreeParameter;

/**
 * This visitor sets the increment on all matching tree parameters as indicated by
 * the class contents.
 * @author Ron Fox
 * @version 1.0
 * @created 30-Mar-2005 11:03:54 AM
 */
class SetWidthVisitor : public CMatchingVisitor
{

private:
  /**
   * New channel width parameter.
   */
  double m_fNewWidth;
  
public:
  virtual ~SetWidthVisitor();
  SetWidthVisitor(STD(string) pattern, double width);
  virtual void OnMatch(CTreeParameter* parameter);
  
};




#endif 
