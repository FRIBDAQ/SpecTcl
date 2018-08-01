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
**   properties.h   - This file contains the definitions needed to set the
**                    properties of the selected pane.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/
#ifndef REFRESH_H
#define REFRESH_H

#define   SCALE_PERCENTAGE 0.50	/* Percentage change for click scaling. */

#include "dispwind.h"

#include "XMWidget.h"
#include "XMPushbutton.h"

void Xamine_SetSelectedProperties(XMWidget *w, XtPointer cli, XtPointer call);
void Xamine_ToggleZoomState(XMWidget *wid, XtPointer ud, XtPointer cd);
void Xamine_ToggleUserMapping(XMWidget *wid, XtPointer ud, XtPointer cd);
void ApplySpectrumMap1d(win_1d*);
void ApplySpectrumMap2d(win_2d*);

#endif
