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
#include <config.h>
#include "CRootFilterOutputStageCreator.h"
#include "CRootFilterOutputStage.h"

using namespace std;

static const string mytype("rootntuple");

/*!
   copy construction is needed for clone but is a no-op.
*/
CRootFilterOutputStageCreator::CRootFilterOutputStageCreator(const CRootFilterOutputStageCreator& rhs)
{}

/*!
   If the type matches mytype, then we can create a filter output stage. If not, return a NULL.
   \param type - The output stage type being requested.
*/
CFilterOutputStage* 
CRootFilterOutputStageCreator::operator()(string type)
{
  if (type == mytype) {
    return new CRootFilterOutputStage;
  }
  else {
    return reinterpret_cast<CFilterOutputStage*>(NULL);
  }
}

/*!
   provide a one-line documentation describing the type of output stage produced. this is
   used by the filter usage command to document what is available.
*/
string
CRootFilterOutputStageCreator::document() const
{
  string doc = mytype;
  doc       += " - Root file containing an ntuple named spectcl.";
  return doc;
}
/*!
  Clones a copy of this.
*/
CFilterOutputStageCreator*
CRootFilterOutputStageCreator::clone()
{
  return new CRootFilterOutputStageCreator(*this);
}
