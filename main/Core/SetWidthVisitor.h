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
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//

#ifndef SETWIDTHVISITOR_H
#define SETWIDTHVISITOR_H

#include "CMatchingVisitor.h"

#include <string>

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
  SetWidthVisitor(std::string pattern, double width);
  virtual void OnMatch(CTreeParameter* parameter);
  
};




#endif 
