/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   eventflags.cc
**     This file implements event flag management routines for the event
**     flags in cluster zero.  There's a bit of a catch-22 that we're trying
**     to get around.  Xt will accept input sources from event flags only
**     in cluster zero.  However the LIB$GET_EF and LIB$FREE_EF routines which
**     we'd like to use to allocate event flags from cluster 1 only.
**     In order to ensure that we don't collide within a program that might
**     have multiple event flag input sources to Xt or whatever, we provide
**     routines here that manage event flags in cluster 1.  The DEC
**     system services guide indicates that event flag 0 and 24-31 are
**     reserved for use by DEC.   Actually 0 is the 'default' event flag
**     and the others are used by RMS etc.
** Author:
**   Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo= "@(#)eventflags.cc	8.1 6/23/95 ";


/*
** Include files:
*/
#include <assert.h>
#include "eventflags.h"
/*
** Defines;
*/
#define True -1
#define False 0

/*
** Below is the allocation table:
*/

static int FreeFlags[32] = {
                             False, /* Event flag 0. */
			     True, True, True, /* 1-3 */
			     True, True, True, True, /* 4-7 */
			     True, True, True, True, /* 8-11 */
			     True, True, True, True, /* 12-15 */
			     True, True, True, True, /* 16-19 */
			     True, True, True, True, /* 20-23 */
			     False, False, False, False	/* 24-31 */
			     };

/*
** Functional Description:
**    getef:
**   Returns a free event flag.  -1 is returned on failure.
*/
int getef()
{
#ifdef VMS
  for(int i = 0; i < 31; i++) {
    if(FreeFlags[i]) {
      FreeFlags[i] = False;
      return i;
    }
  }
  return -1;
#else
  assert(0);
  return -1;
#endif
}

/*
** Functional Description
**   freeef:
**      Frees the event flag that's passed in as an argument.
*/
void freeef(int i)
{
#ifdef VMS
  assert( (i < 32) && (i >= 0));
  FreeFlags[i] = True;
#else
  i++;
  assert(0);
#endif

}

