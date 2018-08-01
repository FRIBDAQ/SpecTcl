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
**   expand.h:
**     This header file should be included by source files which reference the
**     spectrum expansion user interface.  It presents the user interface
**     functions to control expansion and unexpansion.
** Author:
**    Ron Fox
**    NSCL 
**    Michigan State University
**    East Lansing, MI 48824-1321
**     @(#)expand.h	8.1 6/23/95 
*/
#ifndef EXPAND_H_INSTALLED
#define EXPAND_H_INSTALLED

#include "XMWidget.h"
#include "dispwind.h"

void Xamine_Expand(XMWidget *widget, XtPointer user_data, XtPointer call_data);
void Xamine_UnExpand(XMWidget *widget, XtPointer user_data, 
		     XtPointer call_data);

#endif
