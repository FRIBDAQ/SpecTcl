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

#ifndef _TRACKCURSOR_H_INSTALLED
#define _TRACKCURSOR_H_INSTALLED
#include "XMWidget.h"
#include "location.h"
void Xamine_PointerMotionCallback(Widget wid, XtPointer userd, XEvent *evt,
				  Boolean *cont);

void Xamine_ToggleStatusBar(XMWidget *button, XtPointer ud, XtPointer cd);
Xamine_Location *Xamine_GetCursorLocator();

XMWidget *Xamine_SetupLocator(XMWidget *parent);
#endif
