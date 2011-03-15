AM_CFLAGS = -g
AM_CXXFLAGS = -g
AM_LDFLAGS = 

ACLOCAL_AMFLAGS=-I m4

lib_LTLIBRARIES = libExperiment.la 

libExperiment_la_SOURCES = spectcl_experiment.c \
	spectcl_parameter.c \
	spectcl_runs.c		\
	spectcl_uuid.c			\
	spectcl_errors.c		\
	spectcl_experimentInternal.c	\
	spectcl_events.c		\
	spectcl_workspace_base.c	\
	spectcl_workspace_spectypes.c

libExperiment_la_LIBADD=@SQLITE3_LDFLAGS@ -luuid

include_HEADERS=spectcl_experiment.h 
noinst_HEADERS=spectcl_experimentInternal.h 



INCLUDES = -I. -I@top_srcdir@/NSCLException @SQLITE3_CFLAGS@

TESTS=  c_workspacespectra \
	 c_workspacesptypes c_workspacebase c_eventstest	\
	c_exptests  c_runtests c_uuidtests c_paramtests



check_PROGRAMS=c_workspacesptypes \
	c_workspacebase c_exptests c_paramtests c_runtests c_uuidtests \
	c_eventstest c_workspacespectra

c_exptests_SOURCES=spectcl_experiment_check.c
c_exptests_CFLAGS=@CHECK_CFLAGS@  @SQLITE3_CFLAGS@
c_exptests_LDADD=@CHECK_LIBS@ libExperiment.la

c_paramtests_SOURCES=spectcl_paramtests.c
c_paramtests_CFLAGS=@CHECK_CFLAGS@  @SQLITE3_CFLAGS@
c_paramtests_LDADD=@CHECK_LIBS@ libExperiment.la

c_runtests_SOURCES=spectcl_runstest.c
c_runtests_CFLAGS=@CHECK_CFLAGS@  @SQLITE3_CFLAGS@
c_runtests_LDADD=@CHECK_LIBS@ libExperiment.la

c_uuidtests_SOURCES=spectcl_uuidtest.c
c_uuidtests_CFLAGS=@CHECK_CFLAGS@  @SQLITE3_CFLAGS@
c_uuidtests_LDADD=@CHECK_LIBS@ libExperiment.la

c_eventstest_SOURCES=eventstest.c
c_eventstest_CFLAGS=@CHECK_CFLAGS@  @SQLITE3_CFLAGS@
c_eventstest_LDADD=@CHECK_LIBS@ libExperiment.la


c_workspacebase_SOURCES=spectcl_wsbasetest.c
c_workspacebase_CFLAGS=@CHECK_CFLAGS@  @SQLITE3_CFLAGS@
c_workspacebase_LDADD=@CHECK_LIBS@ libExperiment.la


c_workspacesptypes_SOURCES=spectcl_wssptypetest.c
c_workspacesptypes_CFLAGS=@CHECK_CFLAGS@  @SQLITE3_CFLAGS@
c_workspacesptypes_LDADD=@CHECK_LIBS@ libExperiment.la

c_workspacespectra_SOURCES=spectcl_wsspectest.c
c_workspacespectra_CFLAGS=@CHECK_CFLAGS@  @SQLITE3_CFLAGS@
c_workspacespectra_LDADD=@CHECK_LIBS@ libExperiment.la


