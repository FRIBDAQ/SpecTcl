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
**   Xamine  - Exit confirmation dialog.
** Abstract:
**   exit.h  - Defines the Xamine_confirm_exit function so that external
**             clients can use it.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**    @(#)exit.h	8.1 6/23/95 
*/

#ifndef Exit_H
#define Exit_H

#include "XMWidget.h"

void Xamine_confirm_exit(XMWidget *w, XtPointer client_data, 
			 XtPointer callback_structure);

void Xamine_ChangedWindows();
void Xamine_ChangedGrobjs();
void Xamine_SavedWindows();
void Xamine_SavedGrobjs();
#endif
