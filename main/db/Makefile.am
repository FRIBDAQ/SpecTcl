lib_LTLIBRARIES=libSqlite3pp.la libSpecTclDb.la libSpecTclDbTcl.la

TCLSCRIPTS=dbconfig.tcl dbgui.tcl dialogwrapper.tcl
2IMAGES=configuration.png folder.png openfolder.png spectrum.png events.png
DOCS=SpecTclDB.xml GUI.png

libSqlite3pp_la_SOURCES=CSqlite.cpp CSqliteException.cpp CSqliteStatement.cpp \
	CSqliteTransaction.cpp CSqliteWhere.cpp
libSqlite3pp_la_CXXFLAGS=@SQLITE3_CFLAGS@ -g
libSqlite3pp_la_LDFLAGS=@SQLITE3_LDFLAGS@

libSpecTclDb_la_CXXFLAGS=-I@top_srcdir@/Core @SQLITE3_CFLAGS@ \
	-I@top_srcdir@/Utility @ROOT_CFLAGS@ @TCL_CPPFLAGS@ \
	@LIBTCLPLUS_CFLAGS@ -I@top_srcdir@/factories \
	-DTCLLIBS=@prefix@/TclLibs -g
libSpecTclDb_la_LDFLAGS=libSqlite3pp.la \
	@builddir@/../Core/libTclGrammerApp.la   \
	@SQLITE3_LDFLAGS@ @ROOT_LDFLAGS@ @LIBTCLPLUS_LDFLAGS@

libSpecTclDb_la_SOURCES= SpecTclDatabase.cpp SaveSet.cpp DBParameter.cpp \
	DBSpectrum.cpp DBGate.cpp \
	CDBEvents.cpp DBEvents.cpp \
	DBApplications.cpp DBTreeVariable.cpp
libSpecTclDb_la_DEPENDENCIES=libSqlite3pp.la

include_HEADERS=CDBEvents.h DBEvents.h  \
	CSqlite.h CSqliteException.h CSqliteStatement.h \
	 CSqliteTransaction.h CSqliteWhere.h  \
	SpecTclDatabase.h SaveSet.h DBParameter.h DBSpectrum.h \
	DBGate.h DBApplications.h DBTreeVariable.h

libSpecTclDbTcl_la_DEPENDENCIES = libSpecTclDb.la libSqlite3pp.la
libSpecTclDbTcl_la_CXXFLAGS=@SQLITE3_CFLAGS@ -g @TCL_CPPFLAGS@ \
	@LIBTCLPLUS_CFLAGS@ @ROOT_CFLAGS@ -I@top_srcdir@/Core \
	-I@top_srcdir@/Utility -I@top_srcdir@/factories
libSpecTclDbTcl_la_LDFLAGS=libSpecTclDb.la libSqlite3pp.la \
        @builddir@/../Core/libTclGrammerApp.la   \
        @SQLITE3_LDFLAGS@ @ROOT_LDFLAGS@ @LIBTCLPLUS_LDFLAGS@ @ROOT_LDFLAGS@

libSpecTclDbTcl_la_SOURCES = SpecTclDbTCLPackage.cpp DBTcl.h DBTcl.cpp \
	CDBCommands.cpp CDBCommands.h 

install-exec-hook:
	$(mkinstalldirs) @prefix@/TclLibs/dbconfig
	for f in $(TCLSCRIPTS) ;  do $(INSTALL_SCRIPT) @srcdir@/$$f @prefix@/TclLibs/dbconfig; done
	for f in $(IMAGES); do $(INSTALL_DATA) @srcdir@/$$f @prefix@/TclLibs/dbconfig; done
	rm -f @prefix@/TclLibs/dbconfig/libSpecTclDb.so
	$(LN_S)           @libdir@/libSpecTclDb.so   @prefix@/TclLibs/dbconfig/libSpecTclDb.so
	echo pkg_mkIndex -verbose @prefix@/TclLibs/dbconfig "*.tcl"|@TCLSH@ 
	$(mkinstalldirs) @prefix@/share/database
	@HTMLDOCBOOK@ @srcdir@/SpecTclDB.xml -o @prefix@/share/database
	$(INSTALL_DATA) @srcdir@/GUI.png @prefix@/share/database
	$(mkinstalldirs) @prefix@/TclLibs/database
	$(INSTALL_PROGRAM) @builddir@/.libs/libSpecTclDbTcl.* @prefix@/TclLibs/database
	echo package ifneeded SpecTclDB 1.0 \
	[list load [file join \$$dir libSpecTclDbTcl.so]] > \
	@prefix@/TclLibs/database/pkgIndex.tcl
	$(mkinstalldirs) @prefix@/share/dbexamples
	$(INSTALL_DATA) @srcdir@/*.cpp *.tcl @prefix@/share/dbexamples
	$(INSTALL_DATA) @srcdir@/Makefile @prefix@/share/dbexamples


################################################################
#  unit tests:

noinst_PROGRAMS=unittests
noinst_HEADERS=Asserts.h

unittests_SOURCES = TestRunner.cpp exceptionTest.cpp connectionTest.cpp \
			statementTest.cpp transactionTest.cpp filtertests.cpp \
			savepttests.cpp specdbtests.cpp savesettests.cpp \
			dbpartests.cpp dbspectests.cpp dbgtests.cpp \
			gapptests.cpp dbvtests.cpp eventtests.cpp \
			SpecTclDatabase.cpp  SaveSet.cpp DBParameter.cpp \
			DBSpectrum.cpp DBGate.cpp DBApplications.cpp \
			DBTreeVariable.cpp \
			$(libSqlite3pp_la_SOURCES)
unittests_CXXFLAGS=@SQLITE3_CFLAGS@ @CPPUNIT_CFLAGS@ -g -I@top_srcdir@/Utility \
	@ROOT_CFLAGS@
unittests_LDFLAGS=@SQLITE3_LDFLAGS@ @CPPUNIT_LIBS@ -g


TEST_EXTENSIONS=.test
TEST_LOG_COMPILER=@TCLSH@
AM_TESTS_ENVIRONMENT=TCLLIBPATH=@prefix@/TclLibs; export TCLLIBPATH; LD_LIBRARY_PATH=@ROOT_LIBRARY_DIR@; export LD_LIBRARY_PATH;

TESTS=unittests dbtcl.test names.test
#check-TESTS:
#	(cd @srcdir@; tclsh *.test)

#check: check-TESTS

EXTRA_DIST=names.test dbtcl.test  $(TCLSCRIPTS) $(IMAGES) $(DOCS)

