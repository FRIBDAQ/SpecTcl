lib_LTLIBRARIES=libSpecTclDb.la libSqlite3pp.la

TCLSCRIPTS=dbconfig.tcl dbgui.tcl dialogwrapper.tcl
IMAGES=configuration.png folder.png openfolder.png spectrum.png events.png
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
	CDBEvents.cpp DBEvents.cpp CDBCommands.cpp CDBPackage.cpp
libSpecTclDb_la_DEPENDENCIES=libSqlite3pp.la

include_HEADERS=CDBEvents.h DBEvents.h CDBCommands.h \
	CSqlite.h CSqliteException.h CSqliteStatement.h \
	 CSqliteTransaction.h CSqliteWhere.h  \
	SpecTclDatabase.h SaveSet.h DBParameter.h


install-exec-hook:
	$(mkinstalldirs) @prefix@/TclLibs/dbconfig
	for f in $(TCLSCRIPTS) ;  do $(INSTALL_SCRIPT) @srcdir@/$$f @prefix@/TclLibs/dbconfig; done
	for f in $(IMAGES); do $(INSTALL_DATA) @srcdir@/$$f @prefix@/TclLibs/dbconfig; done
	rm -f @prefix@/TclLibs/dbconfig/libSpecTclDb.so
	$(LN_S)           @libdir@/libSpecTclDb.so   @prefix@/TclLibs/dbconfig/libSpecTclDb.so
	echo pkg_mkIndex -verbose @prefix@/TclLibs/dbconfig "*.tcl"|@TCLSH@ 
	echo package ifneeded SpecTclDb 1.0 [list load [file join \$$dir libSpecTclDb.so]] >> @prefix@/TclLibs/dbconfig/pkgIndex.tcl
	$(mkinstalldirs) @prefix@/share/database
	@HTMLDOCBOOK@ @srcdir@/SpecTclDB.xml -o @prefix@/share/database
	$(INSTALL_DATA) @srcdir@/GUI.png @prefix@/share/database

################################################################
#  unit tests:

noinst_PROGRAMS=unittests
noinst_HEADERS=Asserts.h

unittests_SOURCES = TestRunner.cpp exceptionTest.cpp connectionTest.cpp \
			statementTest.cpp transactionTest.cpp filtertests.cpp \
			savepttests.cpp specdbtests.cpp savesettests.cpp \
			dbpartests.cpp \
			SpecTclDatabase.cpp  SaveSet.cpp DBParameter.cpp \
			$(libSqlite3pp_la_SOURCES)
unittests_CXXFLAGS=@SQLITE3_CFLAGS@ @CPPUNIT_CFLAGS@ -g
unittests_LDFLAGS=@SQLITE3_LDFLAGS@ @CPPUNIT_LIBS@ -g


TEST_EXTENSIONS=.test
TEST_LOG_COMPILER=@TCLSH@
TESTS_ENVIRONMENT=TCLLIBPATH=@prefix@/TclLibs; export TCLLIBPATH;LD_LIBRARY_PATH=@ROOT_LIBRARY_DIR@

TESTS=unittests names.test
#check-TESTS:
#	(cd @srcdir@; tclsh *.test)

#check: check-TESTS

EXTRA_DIST=names.test $(TCLSCRIPTS) $(IMAGES) $(DOCS)

