/*
** Facility:
**   Xamine   - NSCL display program.
** Abstract:
**   toggles.cc -- Utility functions for toggle buttons.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   June 21, 1993
*/
static char *sccsinfo = "@(#)toggles.cc	8.1 6/23/95 \n";


/* 
** Include files:
**/
#include <Xm/ToggleB.h>
#include "XMWidget.h"
#include "toggles.h"

/*
** Functional Description:
**   Xamine_SetToggle  - This function is intended to be used as a toggle
**                       button callback. It's purpose is to set a logical
**                       variable according to the state changes of a toggle
**                       button.
** Formal Parameters:
**    XMWidget *w:
**        Pointer to the toggle button that was pushed.
**    XtPointer userd:
**        User data, in this case a pointer to a Boolean to set according to the
**        new toggle state.
**    XtPointer calld:
**        Callback data, in this case a pointer to an XmToggleBUttonCallbackStruct
**        which contains among other things the new button state.
**
*/
void Xamine_SetToggle(XMWidget *w, XtPointer userd, XtPointer calld)
{
  Boolean *variable = (Boolean *)userd;
  XmToggleButtonCallbackStruct *why = (XmToggleButtonCallbackStruct *)calld;

  *variable = (Boolean)why->set;
}
