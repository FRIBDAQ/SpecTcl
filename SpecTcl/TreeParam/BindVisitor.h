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
#ifndef __BINDVISITOR_H
#define __BINDVISITOR_H

#ifndef __CMATCHINGVISITOR_H
#include "CMatchingVisitor.h"
#endif

#ifndef __CTREEPARAMETER_H
#include <CTreeParameter.h>
#endif

/**
 *  for_each generic algorithm visitor class to bind
 *  a tree parameter.  This is used when all CTreeParameters will match
 *  so default construction of the base class is just fine.
*/
class BindVisitor : public CMatchingVisitor
{
public:
  BindVisitor() :
    CMatchingVisitor() {}
  virtual void OnMatch(CTreeParameter* p) {
    p->Bind();
  }
};


#endif
