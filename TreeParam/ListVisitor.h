///////////////////////////////////////////////////////////
//  ListVisitor.h
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



#if !defined(__LISTVISITOR_H)
#define __LISTVISITOR_H

#include "CMatchingVisitor.h"

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// forward definitions:

class CTreeParameter;
class CTCLResult;


/**
 * Class that is used in a for_each to create a listing of the tree parameters.
 * @author Ron Fox
 * @version 1.0
 * @created 01-Apr-2005 02:46:29 PM
 */
class ListVisitor : public CMatchingVisitor
{
private:
  /**
   * Referenced to the result into which the list will be placed.
   */
  CTCLResult& m_OutputList;
  
public:
  virtual ~ListVisitor();
  ListVisitor(std::string pattern, CTCLResult& rResult);
  virtual void OnMatch(CTreeParameter* parameter);
  
  
};




#endif 
