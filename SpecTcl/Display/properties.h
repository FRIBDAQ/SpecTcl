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
#ifndef _REFRESH_H
#define _REFRESH_H

#define   SCALE_PERCENTAGE 0.50	/* Percentage change for click scaling. */

#include "XMWidget.h"
#include "XMPushbutton.h"

void Xamine_SetSelectedProperties(XMWidget *w, XtPointer cli, XtPointer call);
void Xamine_ToggleZoomState(XMWidget *wid, XtPointer ud, XtPointer cd);

#endif
