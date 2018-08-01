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
**   toggles.h  - This file defines some utility routines that are used with toggle
**                buttons.
** Author:
**   Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   June 21, 1993
*/
#ifndef TOGGLES_H
#define TOGGLES_H

void Xamine_SetToggle(XMWidget *w, XtPointer udata, XtPointer cdata);
void Xamine_ToggleAndRefresh(XMWidget* w, XtPointer udata, XtPointer cdata);

#endif
