/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
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

#include <config.h>
#include "errormsg.h"

/*
** Functional Description:
**   Xamine_error_msg   - This function produces an error message dialog.
** Formal Parameters:
**   XMWidget *parent:
**    Parent widget for the dialog.
**   char *msg:
**    The text message to display in the error dialog.
*/

void
Xamine_error_msg (XMWidget *parent, const char *msg)
{
  /* 
   * This object deletes itself when dismissed by the user, so no need
   * to hold a pointer to it.
   */
  new XMErrorDialog ("Xamine_Error_Message", 
		     *parent, msg,
		     XMDestroyWidget);
}
