INCLUDES = -I. -I..

lib_LTLIBRARIES = libreplace.la
libreplace_la_LDFLAGS = -version-info  $(SOVERSION):0
libreplace_la_SOURCES = dummy.c
libreplace_la_LIBADD = @LTLIBOBJS@
include_HEADERS = ansidecl.h libiberty.h
EXTRA_DIST = alloca.c xdr.h
