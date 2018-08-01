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
**   grobjdisplay.h:
**      This file is intended for clients of the graphical object display
**      software.  The routines provided draw graphical contexts from the
**      graphical object database.
** Author:
**   Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS information:
**    @(#)grobjdisplay.h	8.1 6/23/95 
*/
#ifndef GROBJDISPLAY_H_INSTALLED
#define GROBJDISPLAY_H_INSTALLED

#include "XMWidget.h"
#include "convert.h"
#include "dispgrob.h"
#include "dispwind.h"
#include "refreshctl.h"

void Xamine_DrawObjects(Xamine_RefreshContext *ctx,
			win_attributed *attributes);
void Xamine_LabelGrobj(Display *d, Drawable w, GC gc, XMWidget *wid, 
		       char *text, int x, int y);

GC Xamine_CreateGrobjGC(Display *d, Window w, XMWidget *wid, 
			win_attributed *att);


GC Xamine_CreateTentativeGrobjGC(Display *d, Drawable w, XMWidget *pane, 
				 win_attributed *att);

void Xamine_PlotMarker(Display *d, Drawable w, GC gc, XMWidget *wid,
		       Boolean label_marker, Xamine_Converter *cvt,
		       grobj_generic *object,
		       win_attributed *att);
void Xamine_PlotObject(grobj_generic *object, Boolean final);


XFontStruct *Xamine_GetObjectLabelFont(Display *d);   /* Return current obj lbl fnt. */
int          Xamine_GetObjectLabelIndex(Display *d);  /* Return object lbl fnt idx   */
void         Xamine_SetObjectLabelIndex(Display *d, 
					int i);  /* Set object label fnt index  */

#endif
