/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   delgrob.h:
**     This file defines the entry points and data types which
**     are involved in the process of deleting existing graphical objects.
**
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS info:
**   @(#)delgrob.h	8.1 6/23/95 
*/
#ifndef DELGROB_H_INSTALLED
#define DELGROB_H_INSTALLED
#include "XMWidget.h"

void Xamine_DeleteObject(XMWidget *wid, XtPointer ud, XtPointer cd);

#endif
