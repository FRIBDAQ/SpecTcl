/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   info.h:
**     This file defines the functions and data types associated with 
**     the subsystem of Xamine that displays information about a 
**     spectrum window.
** Author:
**    Ron FOx
**    NSCL
**    MIchigan State University
**    East Lansing, MI 48824-1321
**    @(#)info.h	8.1 6/23/95 
*/
#ifndef _INFO_H_INSTALLED
#define _INFO_H_INSTALLED

#include "XMWidget.h"

void Xamine_DisplayInfo(XMWidget *parent, XtPointer client_d, 
			XtPointer call_d);

#endif
