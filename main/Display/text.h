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
**   Xamine - NSCL display program.
** Abstract:
**   text.h
**     This file is an include file which describes the publics that are
**     implemented by text.cc.  These publics have to do with drawing 
**     character text in X-11.
* Author:
**  Ron FOx
**  NSCL
**  Michigan State University
**  East Lansing, MI 48824-1321
**   @(#)text.h	8.1 6/23/95 
*/

#ifndef TEXT_H
#define TEXT_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>

void Xamine_DrawFittedString(Display *display, Drawable drawable,
			     GC gc, int xl, int yl, int xh, int yh, 
			     char *string);

XFontStruct *Xamine_SelectFont(Display *d, char *string, 
			       int xextent, int yextent);

void Xamine_DrawCenteredString(Display *display, Drawable drawable,
			       XFontStruct *font,
			       GC gc, int xl, int yl, int xh,int yh,
			       char *string);

void Xamine_DrawCenteredStringImage(Display *display, Drawable drawable,
			       GC gc, int xmid, int ybase, char *string);

XFontStruct *Xamine_SelectSmallestFont(Display *d);

int Xamine_GetFontCount(Display *d);	           /* Return number of fonts loaded. */
XFontStruct *Xamine_GetFontByIndex
  (Display *d, int n); /* Return the a font given index */
XmStringCharSet Xamine_GetFontNameByIndex
                (Display *d, int n);	/* Return the name of an indexed font */
int   Xamine_GetSmallestFontIndex(Display *d); /* Return index of smallest sized font */

#endif
