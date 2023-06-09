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
**   delgrob.h:
**     This file defines the entry points and data types which
**     are involved in the process of deleting existing graphical objects.
**
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS info:
**   @(#)delgrob.h	8.1 6/23/95 
*/
#ifndef DELGROB_H_INSTALLED
#define DELGROB_H_INSTALLED

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

void Xamine_DeleteObject(XMWidget *wid, XtPointer ud, XtPointer cd);

#endif
