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
**   Xamine support routines:
** Abstract:
**   allocator.h
**      This header file provides definitions for a package of software which
**      manages arenas of memory. This is somewhat different from the
**      malloc/free set since it allows the user to position the arena 
**      explicitly in memory.  This can be useful in managing shared memory
**      regions.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdio.h>
#include <sys/types.h>

typedef caddr_t *arenaid;


arenaid alloc_init(caddr_t arena, int size); /* Initialize an arena */
caddr_t alloc_get(arenaid arena_id, int size); /* Allocate memory. */
void    alloc_free(arenaid arena_id, caddr_t storage); /* Free memory. */
void    alloc_done(arenaid arena_id); /* End use of an arena.     */



#endif
