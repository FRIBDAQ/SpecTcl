// Header that can be used to insulate software
// from existence of <iostream>/<iostream.h>
// differences.  Requires the feature preprocessor
// symbol HAVE_HLESS_HEADERS to distinguish
// the difference:

#ifndef  __ISTREAM_DAQH
#define  __ISTREAM_DAQH

#ifdef HAVE_HLESS_ISTREAM
#include <istream>
#else
#include <istream.h>
#endif


#endif
