
lib_LTLIBRARIES = libreplace.la
libreplace_la_LDFLAGS = -version-info  $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"
libreplace_la_SOURCES = dummy.c
include_HEADERS = ansidecl.h libiberty.h
EXTRA_DIST = alloca.c xdr.h
