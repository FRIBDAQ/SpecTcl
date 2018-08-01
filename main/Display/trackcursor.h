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
**   Xamine  - NSCL display program.
** Abstract:
**   trackcursor.h
**     This include file is intended for use in modules which apply the
**     cursor tracking event handler to spectrum panes.
** Author:
**   Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)trackcursor.h	8.1 6/23/95 
*/

#ifndef TRACKCURSOR_H_INSTALLED
#define TRACKCURSOR_H_INSTALLED

#include "XMWidget.h"
#include "location.h"

void Xamine_PointerMotionCallback(Widget wid, XtPointer userd, XEvent *evt,
				  Boolean *cont);

void Xamine_ToggleStatusBar(XMWidget *button, XtPointer ud, XtPointer cd);
Xamine_Location *Xamine_GetCursorLocator();

XMWidget *Xamine_SetupLocator(XMWidget *parent);
#endif
