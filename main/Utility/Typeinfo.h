// Header used to insulate software from the differences between
// pre standard libs that have <typeinfo.h> and those that have
// standard headers of the form <typeinfo>

#ifndef TYPEINFO_DAQH
#define TYPEINFO_DAQH


#ifdef HAVE_HLESS_TYPEINFO
#include <typeinfo>
#else
#include <typeinfo.h>
#endif

#endif
