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
#ifndef __RESETVISITOR_H
#define __RESETVISITOR_H

#ifndef __CMATCHINGVISITOR_H
#include <CMatchingVisitor.h>
#endif

#ifndef __CTREEPARAMETER_H
#include <CTreeParameter.h>
#endif

class ResetVisitor  : public CMatchingVisitor
{
public:
  virtual void OnMatch(CTreeParameter* pParam) {
    pParam->Reset();
  }
  
};


#endif
