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


#ifndef MATCHTREEPARAMETER_H
#define MATCHTREEPARAMETER_H


#include <string>
#include <utility>


// forward definitions:

class CTreeParameter;

class MatchTreeParameter {
private:
  CTreeParameter* m_pMatchThis;
public:
  MatchTreeParameter(CTreeParameter* pParam) :
    m_pMatchThis(pParam)
  {}

  bool operator()(std::pair<std::string, CTreeParameter*> Item) {
    return (Item.second == m_pMatchThis);
  }
};

#endif
