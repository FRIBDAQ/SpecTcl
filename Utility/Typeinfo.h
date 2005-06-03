// Header used to insulate software from the differences between
// pre standard libs that have <typeinfo.h> and those that have
// standard headers of the form <typeinfo>

#ifndef __TYPEINFO_DAQH
#define __TYPEINFO_DAQH


#ifdef HAVE_HLESS_TYPEINFO
#include <typeinfo>
#else
#include <typeinfo.h>
#endif

#endif
