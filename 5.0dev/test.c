#include <uriparser/Uri.h>
#if (defined(URI_VER_MAJOR) && defined(URI_VER_MINOR) && defined(URI_VER_RELEASE)) \
&& ((URI_VER_MAJOR > 0) \
|| ((URI_VER_MAJOR == 0) && (URI_VER_MINOR > 6)) \
|| ((URI_VER_MAJOR == 0) && (URI_VER_MINOR == 6) && (URI_VER_RELEASE >= 4)) \
)
/* FINE */
#else
# error uriparser not recent enough
#endif
