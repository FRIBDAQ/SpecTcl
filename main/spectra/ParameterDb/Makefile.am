

lib_LTLIBRARIES = libParameterDb.la


libParameterDb_la_SOURCES = ParameterDb.cpp dictParameterDb.cpp
libParameterDb_la_CXXFLAGS = @ROOT_CFLAGS@
libParameterDb_la_LIBADD = @ROOT_LDFLAGS@

include_HEADERS = ParameterDb.h

EXTRA_DISTS = LinkDef.h

BUILT_SOURCES = dictParameterDb.cpp

ROOTCINT = "`@ROOTCONFIG@ --bindir`/rootcint"

dictParameterDb.cpp : ParameterDb.h
	$(ROOTCINT) -f $@ -c -p @srcdir@/ParameterDb.h @srcdir@/LinkDef.h
