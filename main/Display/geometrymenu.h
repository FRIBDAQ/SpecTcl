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

#ifndef GEOMETRYMENU_H
#define GEOMETRYMENU_H

void Xamine_request_geometry(XMWidget *w, XtPointer cd, XtPointer cb);
void Xamine_reset_geometry(XMWidget *w, XtPointer cd, XtPointer cb);
void Xamine_minimize_geometry(XMWidget *w, XtPointer cd, XtPointer cb);
#endif
