lib_LTLIBRARIES = libDDASUnpacker.la
if DDASTOYS
lib_LTLIBRARIES += libDDASFitUnpacker.la
endif

##
# Standard DDAS unpacker
#

libDDASUnpacker_la_SOURCES = DDASUnpacker.cpp 		\
	DDASBuiltUnpacker.cpp 				\
	CFileDrivenParameterMapper.cpp 			\
	CCalibratedFileDrivenParameterMapper.cpp

libDDASUnpacker_la_CXXFLAGS = -I@top_srcdir@/Core 	\
	-I@top_srcdir@/Utility                    	\
	-I@top_srcdir@/Core                       	\
	@LIBTCLPLUS_CFLAGS@                       	\
	@TCL_CPPFLAGS@                            	\
	@ROOT_CFLAGS@					\
	@DDASFMT_CPPFLAGS@ 				\
	@UFMT_CPPFLAGS@

libDDASUnpacker_la_LIBADD = @top_builddir@/Core/libTclGrammerApp.la  	\
	-lgsl  							    	\
	@LIBEXCEPTION_LDFLAGS@ 				 	    	\
	@TCL_LIBS@							\
	@ROOT_LDFLAGS@                                              	\
	@DDASFMT_LDFLAGS@ 						\
	@UFMT_LDFLAGS@

include_HEADERS = DDASUnpacker.h 			\
	DDASBuiltUnpacker.h 				\
	ParameterMapper.h 				\
	CFileDrivenParameterMapper.h 			\
	CCalibratedFileDrivenParameterMapper.h

##
# DDAS unpacker for data with fits - the strucuture of the hit extension
# containing the fit results is defined in the fit_extensions.h header
# from the include directory of the DDASToys install SpecTcl is built against
#

if DDASTOYS
libDDASFitUnpacker_la_SOURCES = DDASBuiltFitUnpacker.cpp

libDDASFitUnpacker_la_CXXFLAGS = $(libDDASUnpacker_la_CXXFLAGS)	\
	@DDASTOYS_CPPFLAGS@

libDDASFitUnpacker_la_LIBADD = $(libDDASUnpacker_la_LIBADD)	\
	@DDASTOYS_LDFLAGS@

include_HEADERS += DDASBuiltFitUnpacker.h FitParameterMapper.h
endif

##
# Unit tests
# 

noinst_PROGRAMS = unittests

unittests_SOURCES = TestRunner.cpp 				\
	fileunpackertests.cpp ddascalibmaptest.cpp

unittests_CXXFLAGS = @CPPUNIT_CFLAGS@ 				\
	@ROOT_CFLAGS@ 						\
	-I@top_srcdir@/Utility 					\
	-I@top_srcdir@/Core 					\
	@LIBTCLPLUS_CFLAGS@ 					\
	@TCL_CPPFLAGS@						\
	@DDASFMT_CPPFLAGS@

unittests_LDADD = @builddir@/libDDASUnpacker.la 		\
	@top_builddir@/Core/libTclGrammerApp.la 		\
	@ROOT_LDFLAGS@ 						\
	@CPPUNIT_LIBS@ 						\
	@LIBTCLPLUS_LDFLAGS@ 					\
	@TCL_LIBS@						\
	@DDASFMT_LDFLAGS@

TESTS_ENVIRONMENT=LD_LIBRARY_PATH=$(ROOT_LIBRARY_DIR)
TESTS = ./unittests

SUBDIRS=src
