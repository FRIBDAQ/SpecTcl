//  Header that insulates users of fstream from the .h or non .h-edness
// of the C++ compiler.

#ifndef FSTREAM_DAQH
#define FSTREAM_DAQH

#ifdef HAVE_HLESS_FSTREAM

#include <fstream>
#else
#include <fstream.h>
#endif


#endif
