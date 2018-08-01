/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   guintegrate.h:
**      This file defines types and entry points exported by the grahpical
**      user interface to Xamine's INTEGRATE facility.
** Author:
**   ROn FOx
**   NSCL
**   MIchigan State University
**   East Lansing, MI 48824-1321
** SCCS Info:
**   @(#)guintegrate.h	8.1 6/23/95 
*/
#ifndef GUINTEGRATE_H_INSTALLED
#define GUINTEGRATE_H_INSTALLED

#include "XMWidget.h"


void Xamine_Integrate(XMWidget *wid, XtPointer ud, XtPointer cd);

#endif
