/*
** Facility:
**   Xamine   - Support software.
** Abstract:
**   errormsg.cc  - This file contains code which produces an error message.
**                  The error message is produced as an error dialog which
**                  is destroyed as soon as the dismiss button is pressed.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**/
#if 0
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
#endif

#include "XMDialogs.h"
#include "XMWidget.h"

/*
** Functional Description:
**   Xamine_error_msg   - This function produces an error message dialog.
** Formal Parameters:
**   XMWidget *parent:
**    Parent widget for the dialog.
**   char *msg:
**    The text message to display in the error dialog.
*/
void Xamine_error_msg(XMWidget *parent, char *msg)
{
  /* 
   * This object deletes itself when dismissed by the user, so no need
   * to hold a pointer to it.
   */
  new XMErrorDialog("Xamine_Error_Message", 
		    *parent, msg,
		    XMDestroyWidget);
}
