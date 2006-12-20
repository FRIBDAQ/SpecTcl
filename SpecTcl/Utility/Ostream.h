// Header that can be used to insulate software
// from existence of <iostream>/<iostream.h>
// differences.  Requires the feature preprocessor
// symbol HAVE_HLESS_HEADERS to distinguish
// the difference:

#ifndef  __OSTREAM_DAQH
#define  __OSTREAM_DAQH

#ifdef HAVE_HLESS_OSTREAM
#include <ostream>
#else
#include <ostream.h>
#endif


#endif
