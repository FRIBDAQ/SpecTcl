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
#include "CFilterOutputStage.h"

// implementation of the CFilterOutputStage method (darn few of them implemented).

/*!
  Constructor is needed to anchor the virtual destructor tree:
*/
CFilterOutputStage::~CFilterOutputStage()
{
}

/*!
   onAttachis not essential to all implementations so a null default implementation is
   provided:
*/
void
CFilterOutputStage::onAttach(CEventFilter& filter)
{
}
