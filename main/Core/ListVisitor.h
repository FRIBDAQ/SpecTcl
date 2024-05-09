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



#ifndef LISTVISITOR_H
#define LISTVISITOR_H

#include "CMatchingVisitor.h"

#include <string>
#include <set>

// forward definitions:

class CTreeParameter;
class CTCLInterpreter;


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
  CTCLInterpreter& m_OutputList;
  
  // Prevent duplicate lists - arguably this filtering could be done
  // in MatchingVisitor but that's a policy decision about how to deal with
  // dups we don't want to make there.
  
  std::set<std::string> m_seen;
  
public:
  virtual ~ListVisitor();
  ListVisitor(std::string pattern, CTCLInterpreter& rResult);
  virtual void OnMatch(CTreeParameter* parameter);
  
  
};




#endif 
