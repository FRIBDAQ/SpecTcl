/*
** Facility:
**   Xamine -- NSCL display process.
** Abstract:
**   prccheck.h:
**     This header file is for use with the prccheck.c module.
**     Note that since that module is in C, we need to generate
**     appropriate conditionals for C++ clients.
**     prccheck.c contains code to verify the existence of an arbitrary
**     process given it's id.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**     @(#)prccheck.h	1.1 3/4/94 
*/

#ifndef _PRCCHECK_H_
#define _PRCCHECK_H_

#ifdef unix
#include <sys/types.h>
#endif
#ifdef VMS
#include <types.h>
#endif

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

int ProcessAlive(pid_t p);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
