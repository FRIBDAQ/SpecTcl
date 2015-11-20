lib_LTLIBRARIES = libException.la
libException_la_SOURCES = \
	ErrnoException.cpp Exception.cpp \
	RangeError.cpp StreamIOError.cpp \
	StateException.cpp URIFormatException.cpp \
	MonitorException.cpp CInvalidArgumentException.cpp \
	CDuplicateSingleton.cpp CNoSuchObjectException.cpp

include_HEADERS = \
	@top_srcdir@/include/exception/ErrnoException.h \
	@top_srcdir@/include/exception/Exception.h \
	@top_srcdir@/include/exception/StreamIOError.h \
	@top_srcdir@/include/exception/RangeError.h \
	@top_srcdir@/include/exception/StateException.h \
	@top_srcdir@/include/exception/URIFormatException.h \
	@top_srcdir@/include/exception/MonitorException.h \
	@top_srcdir@/include/exception/CInvalidArgumentException.h \
	@top_srcdir@/include/exception/CDuplicateSingleton.h \
	@top_srcdir@/include/exception/CNoSuchObjectException.h \
	@top_srcdir@/include/daqdatatypes.h

noinst_HEADERS = @top_srcdir@/include/exception/Asserts.h 

EXTRA_DIST=exceptions.xml

libException_la_LDFLAGS = -version-info $(SOVERSION) \
	-Wl,"-rpath-link=$(libdir)" @PTHREAD_LIBS@

libException_la_CXXFLAGS	= @PTHREAD_CFLAGS@ 	\
				$(AM_CXXFLAGS)

INCLUDES = -I@top_srcdir@/include -I@top_srcdir@/include/exception @CPPUNIT_CFLAGS@

install-data-local:
	$(mkinstalldirs) $(DESTDIR)/$(prefix)/include
	rm -f $(DESTDIR)/$(prefix)/include/CErrnoException.h
	$(INSTALL_HEADER) @top_srcdir@/include/exception/ErrnoException.h $(DESTDIR)/$(prefix)/include
	(cd $(DESTDIR)/$(prefix)/include && $(LN_S) ErrnoException.h CErrnoException.h)
#	$(LN_S) $(DESTDIR)/$(prefix)/include/ErrnoException.h \
#	        $(DESTDIR)/$(prefix)/include/CErrnoException.h


noinst_PROGRAMS = unittests
unittests_SOURCES = TestRunner.cpp testException.cpp testErrnoException.cpp \
	testStreamIOError.cpp testRangeError.cpp
unittests_LDADD   = @CPPUNIT_LIBS@ libException.la
TESTS=./unittests

