

lib_LTLIBRARIES = libParameterDb.la


libParameterDb_la_SOURCES = Parameters.cpp ParameterDb.cpp dictParameterDb.cpp
libParameterDb_la_CXXFLAGS = @ROOT_CFLAGS@
libParameterDb_la_LIBADD = @ROOT_LDFLAGS@

dbdir = @incdir@/ParameterDb

db_HEADERS = ParameterDb.h Parameters.h

EXTRA_DISTS = LinkDef.h

BUILT_SOURCES = dictParameterDb.cpp

ROOTCINT = "`@ROOTCONFIG@ --bindir`/rootcint"

dictParameterDb.cpp : ParameterDb.h Parameters.h
	$(ROOTCINT) -f $@ -c -p @srcdir@/ParameterDb.h @srcdir@/Parameters.h @srcdir@/LinkDef.h
