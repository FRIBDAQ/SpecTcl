AM_CFLAGS = -g
AM_CXXFLAGS = -g
AM_LDFLAGS = 

ACLOCAL_AMFLAGS=-I m4

lib_LTLIBRARIES = libExperiment.la libSpecTclExperiment.la

libExperiment_la_SOURCES = spectcl_experiment.c \
	spectcl_parameter.c \
	spectcl_runs.c		\
	spectcl_uuid.c			\
	spectcl_errors.c		\
	spectcl_experimentInternal.c

libExperiment_la_LIBADD=-lsqlite3 -luuid

include_HEADERS=spectcl_experiment.h
noinst_HEADERS=spectcl_experimentInternal.h \
	handleManager.h Asserts.h TclCreate.h TclOpen.h	\
	TclClose.h TclParameter.h TclRuns.h TclDBCommand.h \
	TclUUID.h


libSpecTclExperiment_la_SOURCES= packageStartup.cpp	\
	handleManager.cpp TclCreate.cpp TclOpen.cpp 	\
	TclClose.cpp TclParameter.cpp TclRuns.cpp TclDBCommand.cpp \
	TclUUID.cpp


libSpecTclExperiment_la_CXXFLAGS=@TCL_FLAGS@ -I@top_builddir@/Utility -I@top_builddir@/TclPlus
libSpecTclExperiment_la_LIBADD=@top_builddir@/TclPlus/libtclPlus.la \
	@top_builddir@/NSCLException/libException.la libExperiment.la @TCL_LDFLAGS@



INCLUDES = -I. -I@top_srcdir@/NSCLException

TESTS=  tcltestrunner \
	c_exptests  c_runtests c_uuidtests c_paramtests \
	handletests tclcreatetests

EXTRA_DIST=*.test tcltests

check_PROGRAMS=c_exptests c_paramtests c_runtests c_uuidtests handletests tclcreatetests

c_exptests_SOURCES=spectcl_experiment_check.c
c_exptests_CFLAGS=@CHECK_CFLAGS@
c_exptests_LDADD=@CHECK_LIBS@ libExperiment.la

c_paramtests_SOURCES=spectcl_paramtests.c
c_paramtests_CFLAGS=@CHECK_CFLAGS@
c_paramtests_LDADD=@CHECK_LIBS@ libExperiment.la

c_runtests_SOURCES=spectcl_runstest.c
c_runtests_CFLAGS=@CHECK_CFLAGS@
c_runtests_LDADD=@CHECK_LIBS@ libExperiment.la

c_uuidtests_SOURCES=spectcl_uuidtest.c
c_uuidtests_CFLAGS=@CHECK_CFLAGS@
c_uuidtests_LDADD=@CHECK_LIBS@ libExperiment.la


handletests_SOURCES=handletests.cpp handleManager.cpp
handletests_CXXFLAGS=@CPPUNIT_CFLAGS@
handletests_LDADD=@CPPUNIT_LIBS@ 

tclcreatetests_SOURCES=tclcreatetests.cpp TclCreate.cpp TclDBCommand.cpp handleManager.cpp
tclcreatetests_CXXFLAGS=@CPPUNIT_CFLAGS@ @TCL_FLAGS@ -I@top_builddir@/Utility -I@top_builddir@/TclPlus
tclcreatetests_LDADD=@CPPUNIT_LIBS@ \
	@top_builddir@/TclPlus/libtclPlus.la \
	@top_builddir@/NSCLException/libException.la libExperiment.la @TCL_LDFLAGS@


tcltestrunner:
	echo "#!/bin/sh" >tcltestrunner
	echo "\\"        >> tcltestrunner
	echo @TCLSH_CMD@ tcltests >> tcltestrunner
	chmod a+x tcltestrunner


.PHONY: tcltestrunner
