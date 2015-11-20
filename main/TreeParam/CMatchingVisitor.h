///////////////////////////////////////////////////////////
//  CMatchingVisitor.h
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
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//



#if !defined(__CMATCHINGVISITOR_H)

#define __CMATCHINGVISITOR_H

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_UTILITY
#include <utility>
#ifndef __STL_UTILITY
#define __STL_UTILITY
#endif
#endif



// Forward definitions:

class CTreeParameter;

/**
 * Parameter visitor that executes user functionality when the visited parameter
 * matches a pattern.  This is an abstract base class. Derive classes must
 * implement OnMatch which is called for each tree parameter that matches the
 * glob pattern used to construct us.

 * @author Ron Fox
 * @version 1.0
 * @created 30-Mar-2005 11:03:48 AM
 */
class CMatchingVisitor
{
private:
  /**
   * Pattern to match against.
   */
  std::string m_sPattern;
  
public:
  CMatchingVisitor();		//!< Matches all (like raw for_each).
  virtual ~CMatchingVisitor();
  CMatchingVisitor(std::string pattern);
  void operator()(std::pair<const std::string, CTreeParameter*>& element);
  virtual void OnMatch(CTreeParameter* pParam) =0;
  
  
};




#endif
