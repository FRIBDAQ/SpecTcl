/*
** Facility:
**   Xamine NSCL display program
** Abstract:
**   geometrymenu.h   - This file contains definitions for clients of the
**                      software in geometrymenu.h  In particular, we 
**                      define the dialog generator for the geometry prompt
**                      dialog.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**    @(#)geometrymenu.h	8.1 6/23/95 
*/

#ifndef _GEOMETRYMENU_H
#define _GEOMETRYMENU_H

void Xamine_request_geometry(XMWidget *w, XtPointer cd, XtPointer cb);
void Xamine_reset_geometry(XMWidget *w, XtPointer cd, XtPointer cb);
void Xamine_minimize_geometry(XMWidget *w, XtPointer cd, XtPointer cb);
#endif
