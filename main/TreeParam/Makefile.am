lib_LTLIBRARIES = libTreeParam.la

libTreeParam_la_SOURCES = CMatchingVisitor.cpp \
	CTreeException.cpp \
	CTreeParameter.cpp \
	CTreeParameterArray.cpp \
	CTreeParameterCommand.cpp \
	CTreeVariable.cpp \
	CTreeVariableArray.cpp \
	CTreeVariableCommand.cpp \
	CTreeVariableProperties.cpp \
	ListVisitor.cpp \
	SetAllVisitor.cpp \
	SetChannelsVisitor.cpp \
	SetLimitsVisitor.cpp \
	SetUnitsVisitor.cpp \
	SetWidthVisitor.cpp \
	UncheckVisitor.cpp

LibraryObjects =  CMatchingVisitor.o   \
	CTreeException.o   \
	CTreeParameter.o   \
	CTreeParameterArray.o   \
	CTreeParameterCommand.o   \
	CTreeVariable.o   \
	CTreeVariableArray.o   \
	CTreeVariableCommand.o   \
	CTreeVariableProperties.o   \
	ListVisitor.o   \
	SetAllVisitor.o   \
	SetChannelsVisitor.o   \
	SetLimitsVisitor.o   \
	SetUnitsVisitor.o   \
	SetWidthVisitor.o   \
	UncheckVisitor.o  

include_HEADERS = Asserts.h \
	BindVisitor.h \
	CMatchingVisitor.h \
	CTreeException.h \
	CTreeParameter.h \
	CTreeParameterArray.h \
	CTreeParameterCommand.h \
	CTreeVariable.h \
	CTreeVariableArray.h \
	CTreeVariableCommand.h \
	CTreeVariableProperties.h \
	ListVisitor.h \
	MatchTreeParameter.h \
	ResetVisitor.h \
	SetAllVisitor.h \
	SetChannelsVisitor.h \
	SetLimitsVisitor.h \
	SetUnitsVisitor.h \
	SetWidthVisitor.h \
	UncheckVisitor.h \
	TreeParameter.h


EXTRA_DIST = GateGenerator.tcl \
	ParameterGenerator.tcl \
	ParameterManipulator.tcl \
	SpecTclGui.tcl \
	SpectrumGenerator.tcl \
	TopGui.tcl \
	TreeParameter.tcl \
	TreeVariable.tcl \
	VariableManipulator.tcl \
	mclistbox.tcl \
	notebook.tcl \
	server.tcl \
	tabnbook.tcl

AM_CPPFLAGS = @LIBTCLPLUS_CFLAGS@ \
	-I@top_srcdir@/Sorter \
	-I@top_srcdir@/Events \
	-I@top_srcdir@/SpecTcl \
	-I@top_srcdir@/Utility \
	-I@top_srcdir@/Xamine \
	-I@top_srcdir@/Display \
	-I@top_srcdir@/Gates \
	-I@top_srcdir@/Filter \
	-I@top_srcdir@/factories \
	@TCL_CPPFLAGS@


libTreeParam_la_LDFLAGS = -version-info $(SOVERSION):0

SCRIPTDEST = $(prefix)/Script
install-exec-local:
	$(mkinstalldirs) $(SCRIPTDEST)
	$(INSTALL_PROGRAM) *.tcl $(SCRIPTDEST)


##
# For now circular dependencies prevent this from being done until I get smarter
# and figure out how to build this in the check-TESTS target.. or until I separate this
# out into a separate test dir...because this depends on libTclGrammerCommands but
# other folks depend on us first ...including libTclGrammerCommands.
#

if THIS_IS_NOT_DEFINED
if CPPUNIT

noinst_PROGRAMS=TestRunner



TestRunner_CXXFLAGS=@CPPUNIT_CFLAGS@

TestRunner_SOURCES = ListTest.cpp       SetWidthTest.cpp          TreeParameterTest.cpp \
	MatchTest.cpp      TestRunner.cpp            TreeVariableArrayTests.cpp \
	SetAllVTest.cpp    TestSuite.cpp             TreeVariableCommandTests.cpp \
	SetChansTest.cpp   TreeExceptionTest.cpp     TreeVariablePropertyTests.cpp \
	SetLimitsTest.cpp  TreeParamArrayTest.cpp    TreeVariableTests.cpp \
	SetUnitsTest.cpp   TreeParamCommandTest.cpp  UncheckTest.cpp  testStubs.cpp 

TestRunner_DEPENDENCIES = $(LibraryObjects)

TestRunner_LDADD = @top_builddir@/SpecTcl/libTclGrammerCommands.la \
	@top_builddir@/Sorter/libSorting.la   \
	@top_builddir@/Fits/libFits.la  \
	@top_builddir@/Filter/libFilter.la \
	@top_builddir@/DecoderRing/libDecoderRing.la \
	@top_builddir@/Events/libAnalysis.la \
	@top_builddir@/SpectrumIO/libSpecio.la \
	@top_builddir@/Tape/libtape.la \
	@top_builddir@/Analysis/libBufferAnalysis.la

TestRunner_LDFLAGS = $(LibraryObjects) \
	 @LIBTCLPLUS_LDFLAGS@	\
	@TK_LIBS@ @TCL_LIBS@ @CPPUNIT_LIBS@

noinst_HEADERS = Asserts.h TreeTestSupport.h

TESTS=./TestRunner

endif
endif
