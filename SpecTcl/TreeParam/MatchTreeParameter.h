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


#ifndef __MATCHTREEPARAMETER_H
#define __MATCHTREEPARAMETER_H


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


// forward definitions:

class CTreeParameter;

class MatchTreeParameter {
private:
  CTreeParameter* m_pMatchThis;
public:
  MatchTreeParameter(CTreeParameter* pParam) :
    m_pMatchThis(pParam)
  {}

  bool operator()(STD(pair)<STD(string), CTreeParameter*> Item) {
    return (Item.second == m_pMatchThis);
  }
};

#endif
