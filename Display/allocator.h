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
#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <stdio.h>

#ifdef unix
#include <sys/types.h>
#endif
#ifdef VMS
#include <types.h>
#endif

#ifdef VMS
#ifndef __SOCKET_TYPEDEFS
#ifndef CADDR_T_DEFINED
#define CADDR_T_DEFINED
typedef char *caddr_t;
#endif
#endif
#endif
typedef caddr_t *arenaid;


arenaid alloc_init(caddr_t arena, int size); /* Initialize an arena */
caddr_t alloc_get(arenaid arena_id, int size); /* Allocate memory. */
void    alloc_free(arenaid arena_id, caddr_t storage); /* Free memory. */
void    alloc_done(arenaid arena_id); /* End use of an arena.     */



#endif
