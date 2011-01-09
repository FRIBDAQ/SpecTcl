AM_CFLAGS = -g
AM_CXXFLAGS = -g
AM_LDFLAGS = 

ACLOCAL_AMFLAGS=-I m4

lib_LTLIBRARIES = libSpecTclExperiment.la

noinst_HEADERS=  TclCreate.h TclOpen.h	\
	TclClose.h TclParameter.h TclRuns.h TclDBCommand.h \
	TclUUID.h  Asserts.h handleManager.h \
	TclEvtCreate.h TclEvtClose.h TclEvtOpen.h

libSpecTclExperiment_la_SOURCES= packageStartup.cpp	\
	handleManager.cpp TclCreate.cpp TclOpen.cpp 	\
	TclClose.cpp TclParameter.cpp TclRuns.cpp TclDBCommand.cpp \
	TclUUID.cpp \
	TclEvtCreate.cpp TclEvtClose.cpp TclEvtOpen.cpp

libSpecTclExperiment_la_CXXFLAGS=@TCL_FLAGS@ -I@top_builddir@/Utility -I@top_builddir@/TclPlus
libSpecTclExperiment_la_LIBADD=@top_builddir@/TclPlus/libtclPlus.la \
	@top_builddir@/primitives/libExperiment.la \
	@top_builddir@/NSCLException/libException.la @TCL_LDFLAGS@

INCLUDES = -I. -I@top_srcdir@/NSCLException -I@top_srcdir@/primitives

TESTS= 	handletests tclcreatetests tcltestrunner

EXTRA_DIST=*.test tcltests


check_PROGRAMS= handletests tclcreatetests

handletests_SOURCES=handletests.cpp handleManager.cpp
handletests_CXXFLAGS=@CPPUNIT_CFLAGS@
handletests_LDADD=@CPPUNIT_LIBS@ 

tclcreatetests_SOURCES=tclcreatetests.cpp TclCreate.cpp TclDBCommand.cpp handleManager.cpp
tclcreatetests_CXXFLAGS=@CPPUNIT_CFLAGS@ @TCL_FLAGS@ -I@top_builddir@/Utility -I@top_builddir@/TclPlus
tclcreatetests_LDADD=@CPPUNIT_LIBS@ \
	@top_builddir@/TclPlus/libtclPlus.la \
	@top_builddir@/NSCLException/libException.la @top_builddir@/primitives/libExperiment.la @TCL_LDFLAGS@


tcltestrunner:
	echo "#!/bin/sh" >tcltestrunner
	echo "\\"        >> tcltestrunner
	echo @TCLSH_CMD@ tcltests >> tcltestrunner
	chmod a+x tcltestrunner


.PHONY: tcltestrunner
