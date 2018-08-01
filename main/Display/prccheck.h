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

#ifndef PRCCHECK_H
#define PRCCHECK_H

#include <sys/types.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

int ProcessAlive(pid_t p);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
