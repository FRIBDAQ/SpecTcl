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
#ifndef _ACCEPTGATES_H
#define _ACCEPTGATES_H
#include "XMWidget.h"
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
