/*
    This header provides centralizes as many hacks as we can concerning
    TCL API version differences.
*/
#ifndef __TCLVERSIONHACKS_H
#define __TCLVERSIONHACKS_H

#ifndef __TCL_H
#include <tcl.h>
#ifndef __TCL_H
#define __TCL_H
#endif
#endif


#ifndef __CRT_STDLIB_H
#include <stdlib.h>
#ifndef __CRT_STDLIB_H
#define __CRT_STDLIB_H
#endif
#endif



// tclConstCharPtr  - A typedef that's either char* or const char* depending
// on the TCL Version:

#if (TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >3))
typedef const char* tclConstCharPtr;
#else
typedef char* tclConstCharPtr;
#endif

// Some older versions of TCL failed to Tcl_Free everything on unix:


static inline void
tclSplitListFree(void* pointer)
{
#if defined(WIN32) || (TCL_MAJOR_VERSION > 8) || \
    ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION > 3))
  Tcl_Free((char*)pointer);
#else
  free(pointer);
#endif
}

#endif
