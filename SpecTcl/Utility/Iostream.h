// Header that can be used to insulate software
// from existence of <iostream>/<iostream.h>
// differences.  Requires the feature preprocessor
// symbol HAVE_HLESS_HEADERS to distinguish
// the difference:

#ifndef  __IOSTREAM_DAQH
#define  __IOSTREAM_DAQH

#ifdef HAVE_HLESS_IOSTREAM
#include <iostream>
#else
#include <iostream.h>
#endif


#endif
