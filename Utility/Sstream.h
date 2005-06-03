// Header that can be used to insulate software
// from existence of <sstream>/<sstream.h>
// differences.  Requires the feature preprocessor
// symbol HAVE_HLESS_HEADERS to distinguish
// the difference:

#ifndef  __SSTREAM_DAQH
#define  __SSTREAM_DAQH

#ifdef HAVE_HLESS_SSTREAM
#include <sstream>
#else
#include <sstream.h>
#endif


#endif
