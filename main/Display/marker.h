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
**   marker.h:
**     This file is a header file which should be included by clients
**     of the marker setting subsystem.  A marker is a point that is
**     placed on a spectrum.  A marker appears as a small diamond that is 
**     centered on the acceptance point.  There are two coordinates for
**     markers.  On 1-d spectra, the coordinates of a marker are 
**     given in channels and counts while the 2-d's are in x and y channels.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**     @(#)marker.h	8.1 6/23/95 
*/

#ifndef MARKER_H_INSTALLED
#define MARKER_H_INSTALLED

#include "XMWidget.h"


void Xamine_AddMarker(XMWidget *wid, XtPointer ud, XtPointer cd);
void Xamine_DrawMarker(Display *d, Drawable w, GC gc, int x, int y);

#endif
