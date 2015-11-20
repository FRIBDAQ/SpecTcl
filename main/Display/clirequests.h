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

/*
** Facility:
**   Xamine - NSCL display program.
** Abstract:
**   clirequests.h:
**      This file contains definitions needed for software which sets up the
**      gate notification.  Gate notification is done via an  Xt  User input
**      event.
** Author:
**    Ron FOx
**    NSCL
**    Michigan State University.
**    East Lansing, MI 48824-1321
** SCCS:
**   @(#)clirequests.h	1.1 1/28/94 
*/
#ifndef _CLIREQUESTS_H
#define _CLIREQUESTS_H

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

void Xamine_ProcessClientRequests(XtPointer client_data,
				   int       *source_fd,
				   XtInputId *input_id);

#endif
