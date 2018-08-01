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
**   Xamine -- NSCL display program.
** Abstract:
**   acceptgates.h:
**     This file is intended for use by clients of the acceptgates.cc module. 
**     That module accepts gate objects from user interactions and therefore
**     also includes routines to draw the objects that are being accepted.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS:
**   @(#)acceptgates.h	8.1 6/23/95 
*/
#ifndef ACCEPTGATES_H
#define ACCEPTGATES_H

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#include "convert.h"
#include "dispgrob.h"

/*
**  The function below is the callback that's invoked to set contours,
**  cuts or bands:
*/
void Xamine_AcceptGate(XMWidget *wid, XtPointer clientd, XtPointer calld);
/*
** These functions are in charge of displaying gate objects:
*/

void Xamine_DisplayCut(Display *d, Drawable w, GC gc, XMWidget *wid,
		       Xamine_Converter *cvt,
		       int flipped, int label, grobj_generic *object);

void Xamine_DisplayContour(Display *d, Drawable w, GC gc,
			   XMWidget *wid,
			   Xamine_Converter *cvt,
			   int flipped, int label,
			   grobj_generic *object);

void Xamine_DisplayBand(Display *d, Drawable w, GC gc,
			XMWidget *wid,
			Xamine_Converter *cvt,
			int flipped, int label,
			grobj_generic *obejct);

#endif
