///////////////////////////////////////////////////////////
//  SetAllVisitor.h
//  Implementation of the Class SetAllVisitor
//  Created on:      01-Apr-2005 02:56:43 PM
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

#if !defined(__SETALLVISITOR_H)
#define __SETALLVISITOR_H

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
 * for_each visitor to set all the characteristics  of all matching tree
 * parameters.
 * We support Glob matching as an extension to the original command.
 * @author Ron Fox
 * @version 1.0
 * @created 01-Apr-2005 02:56:43 PM
 */
class SetAllVisitor : public CMatchingVisitor
{
private:
  /**
   * Units of measure to give the parameter.
   */
  STD(string) m_units;
  /**
   * Number of default channels.
   */
  int m_nChannels;
  /**
   * Low limit of default axis.
   */
  double m_fLow;
  /**
   * High limit of default axis.
   */
  double m_fHigh;
  
public:

  virtual ~SetAllVisitor();
  SetAllVisitor(STD(string) pattern, int channels, 
		double low, double high, STD(string) units);
  virtual void OnMatch(CTreeParameter* parameter);
};




#endif
