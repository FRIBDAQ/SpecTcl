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

INCLUDES = -I@top_srcdir@/TCL \
	-I@top_srcdir@/Sorter \
	-I@top_srcdir@/Events \
	-I@top_srcdir@/SpecTcl \
	-I@top_srcdir@/Utility \
	-I@top_srcdir@/NSCLException \
	-I@top_srcdir@/Xamine \
	-I@top_srcdir@/Display \
	-I@top_srcdir@/Gates \
	-I@top_srcdir@/Filter \
	$(TCL_FLAGS)


libTreeParam_la_LDFLAGS = -version-info $(SOVERSION):0