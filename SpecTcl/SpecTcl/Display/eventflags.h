/*
** Facility:
**   Xamine - NSCL Display program.
** Abstract:
**   eventflags.h:
**     This header file provides definitions of functions to do event flag
**     management.  This is only required by the VMS version of Xamine.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS update information:
**   @(#)eventflags.h	8.1 6/23/95 
*/
#ifndef EVENTFLAGS_H
#define EVENTFLAGS_H

int getef();
void freeef(int ef);

#endif
