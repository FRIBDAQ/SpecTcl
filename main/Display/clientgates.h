Monticello Family Shaolin Kung Fu
/*
** Facility:
**   Xamine  - NSCL display program (client).
** Abstract:
**   clientgates.h
**     This file describes things internal to Xamine's client library
**     which are needed to call routines from the clientgates.cc module.
**     clientgates.cc is responsible for implementing gates interaction 
**     software.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS:
**   @(#)clientgates.h	8.1 6/23/95 
*/
#ifndef CLIENTGATES_H
#define CLIENTGATES_H
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

int Xamine_OpenPipes();		/* Open message IPC's to Xamine. */

int Xamine_AcceptPipeConnections();

void Xamine_Closepipes();

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
#endif
