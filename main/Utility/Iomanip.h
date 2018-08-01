// Header that can be used to insulate software
// from existence of <iostream>/<iostream.h>
// differences.  Requires the feature preprocessor
// symbol HAVE_HLESS_HEADERS to distinguish
// the difference:

#ifndef  IOMANIP_DAQH
#define  IOMANIP_DAQH

#ifdef HAVE_HLESS_IOMANIP
#include <iomanip>
#else
#include <imanip.h>
#endif


#endif
