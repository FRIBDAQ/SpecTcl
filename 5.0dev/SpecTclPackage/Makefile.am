lib_LTLIBRARIES = libSpecTcl.la libCatDecoder.la libNullProcessor.la libWcPackage.la libTpDump.la


#  The base SpecTcl package.

libSpecTcl_la_SOURCES	= PackageInit.cpp \
	CPipelineCommand.cpp  CFileDataSource.cpp CDataSource.cpp \
	CDataSourceFactory.cpp CAttachCommand.cpp CSpecTclInterpreter.cpp \
	CAnalysisPipeline.cpp CEventProcessor.cpp CFileDataSourceCreator.cpp \
	CValidValue.cpp CParameterDictionary.cpp CParameterArray.cpp \
	CTreeParameterCommand.cpp CParameter.cpp CTrueCondition.cpp CFalseCondition.cpp \
	CAxis.cpp CSpectrum.cpp CHistogrammerFactory.cpp CSpecTclSpectrumAllocator.cpp \
	CSpectrumIncrementer.cpp CSpecTcl1dIncrementer.cpp CSpecTcl2dIncrementer.cpp \
	CSpecTclSum1dIncrementer.cpp CSpecTcl1dBaseIncrementer.cpp CSpecTclCombo2dIncrementer.cpp \
	CSpecTcl2dBaseIncrementer.cpp CSpecTclSummaryIncrementer.cpp CSpecTcl2dSumIncrementer.cpp \
	CSpecTclGammaSummaryIncrementer.cpp CSpectrumDictionary.cpp CSpectrumCommand.cpp \
	CSpecTclHistogrammerFactory.cpp

include_HEADERS = CPipelineCommand.h CDataSource.h CFileDataSource.h CDataSourceCreator.h \
	CFileDataSourceCreator.h CParameter.h \
	CAttachCommand.h CSpecTclInterpreter.h CAnalysisPipeline.h CDecoder.h \
	CEventProcessor.h CObservable.h CValidValue.h CParameterDictionary.h CParameterArray.h \
	CTreeParameterCommand.h CCondition.h CTrueCondition.h CFalseCondition.h CAxis.h \
	CSpectrumAllocator.h CSpectrumIncrementer.h CSpectrum.h CHistogrammerFactory.h \
	CSpecTclSpectrumAllocator.h CSpecTcl1dIncrementer.h CSpecTcl2dIncrementer.h \
	CSpecTclSum1dIncrementer.h CSpecTcl1dBaseIncrementer.h CSpecTclCombo2dIncrementer.h \
	CSpecTcl2dBaseIncrementer.h CSpecTclSummaryIncrementer.h CSpecTcl2dSumIncrementer.h \
	CSpecTclGammaSummaryIncrementer.h CDataSourceFactory.h CSpectrumDictionary.h CSpectrumCommand.h \
	CSpecTclHistogrammerFactory.h


noinst_HEADERS=Asserts.h

libSpecTcl_la_LIBADD	= @LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @URIPARSER_LDFLAGS@
libSpecTcl_la_LDFLAGS   = -Wl,"-rpath=@libdir@"


# Acceptance tests rely on the libCatDecoder package as well:

libCatDecoder_la_SOURCES = catDecoderPackage.cpp
libCatDecoder_la_LIBADD  =  libSpecTcl.la @LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@
libCatDecoder_la_LDFLAGS =  -Wl,"-rpath=@libdir@"

libWcPackage_la_SOURCES = wcPackage.cpp
libWcPackage_la_LIBADD  =  libSpecTcl.la @LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@
libWcPackage_la_LDFLAGS =  -Wl,"-rpath=@libdir@"


libTpDump_la_SOURCES = tpdump.cpp
libTpDump_la_LIBADD  = libSpecTcl.la @LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@
libTpDump_la_LDFLAGS = -Wl,"-rpath=@libdir@"

libNullProcessor_la_SOURCES = nullProcessor.cpp
libNullProcessor_la_LIBADD  = libSpecTcl.la @LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@
libNullProcessor_la_LDFLAGS =  -Wl,"-rpath=@libdir@"





INCLUDES			= @TCL_CPPFLAGS@ @URIPARSER_LDFLAGS@ @LIBTCLPLUS_CFLAGS@


#
#  Install time builds the pkgIndex.tcl file.
#
install-exec-local:
	$(mkinstalldirs) @libdir@
	echo package ifneeded spectcl 5.0 [list load [file join \$$dir libSpecTcl.so]]          > @libdir@/pkgIndex.tcl
	echo package ifneeded catdecoder 1.0 [list load [file join \$$dir libCatDecoder.so]]    >> @libdir@/pkgIndex.tcl
	echo package ifneeded null       1.0 [list load [file join \$$dir libNullProcessor.so]] >> @libdir@/pkgIndex.tcl
	echo package ifneeded wc         1.0 [list load [file join \$$dir libWcPackage.so]]     >> @libdir@/pkgIndex.tcl
	echo package ifneeded tpdump     1.0 [list load [file join \$$dir libTpDump.so]]        >> @libdir@/pkgIndex.tcl


TESTS=	ParameterDictionaryTests ValidValueTests  \
	fileDSTest DSFactoryTest SpecTclInterpTest AttachTest PipelineDecoderTests PipelineCommandTests \
	AnalysisPipelineTest ObserverTest ParameterTests ParameterArrayTests TreeParamCmdTests \
	ConditionTest AxisTest SpectrumTest SpectrumDictTest SpecTclAllocatorTest \
	SpecTcl1dIncTest SpecTcl2dIncTest SpecTclSum1Test SpecTclComboTest \
	SpecTclSummaryTest SpecTcl2dSumIncTest SpecTclGammaSummaryIncTest \
	SpectrumCommandTest SpecTclFactoryTests

# The LD_LFAGS is needed else autotools look for a target named TEST.

TEST_CPPFLAGS=  $(CPPUNIT_CFLAGS) $(LIBTCLPLUS_CFLAGS) $(TCL_CPPFLAGS)
TEST_LD_FLAGS = $(CPPUNIT_LIBS) $(LIBTCLPLUS_LDFLAGS) $(TCL_LIBS) $(URIPARSER_LDFLAGS)

check_PROGRAMS = $(TESTS)

fileDSTest_SOURCES =TestRunner.cpp FileDataSourceTest.cpp
fileDSTest_CXXFLAGS= $(TEST_CPPFLAGS)
fileDSTest_LDFLAGS = $(TEST_LD_FLAGS)
fileDSTest_LDADD   = CFileDataSource.lo CDataSource.lo

DSFactoryTest_SOURCES=TestRunner.cpp  DataSourceFactoryTest.cpp
DSFactoryTest_CXXFLAGS= $(TEST_CPPFLAGS)
DSFactoryTest_LDFLAGS = $(TEST_LD_FLAGS)
DSFactoryTest_LDADD   = CDataSourceFactory.lo CDataSource.lo

AttachTest_SOURCES = TestRunner.cpp AttachTests.cpp
AttachTest_CXXFLAGS= $(TEST_CPPFLAGS)
AttachTest_LDFLAGS = $(TEST_LD_FLAGS)
AttachTest_LDADD   = CAttachCommand.lo CDataSourceFactory.lo CSpecTclInterpreter.lo CDataSource.lo

SpecTclInterpTest_SOURCES = TestRunner.cpp InterpSingletonTest.cpp
SpecTclInterpTest_CXXFLAGS= $(TEST_CPPFLAGS) 
SpecTclInterpTest_LDFLAGS = $(TEST_LD_FLAGS)
SpecTclInterpTest_LDADD   = CSpecTclInterpreter.lo 

PipelineDecoderTests_SOURCES = TestRunner.cpp PipelineDecoderTests.cpp
PipelineDecoderTests_CXXFLAGS= $(TEST_CPPFLAGS)
PipelineDecoderTests_LDFLAGS = $(TEST_LD_FLAGS)
PipelineDecoderTests_LDADD   = CAnalysisPipeline.lo CDataSource.lo CSpecTclInterpreter.lo \
	CEventProcessor.lo


PipelineCommandTests_SOURCES = TestRunner.cpp PipelineCommandTests.cpp
PipelineCommandTests_CXXFLAGS= $(TEST_CPPFLAGS)
PipelineCommandTests_LDFLAGS = $(TEST_LD_FLAGS)
PipelineCommandTests_LDADD   = CSpecTclInterpreter.lo CPipelineCommand.lo \
	CAnalysisPipeline.lo CEventProcessor.lo


AnalysisPipelineTest_SOURCES = TestRunner.cpp AnalysisPipelineTests.cpp
AnalysisPipelineTest_CXXFLAGS= $(TEST_CPPFLAGS)
AnalysisPipelineTest_LDFLAGS = $(TEST_LD_FLAGS)
AnalysisPipelineTest_LDADD  = CAnalysisPipeline.lo CDataSource.lo CSpecTclInterpreter.lo \
	CEventProcessor.lo

ObserverTest_SOURCES = TestRunner.cpp ObserverTest.cpp
ObserverTest_CXXFLAGS= $(CPPUNIT_CFLAGS)
ObserverTest_LDFLAGS = $(CPPUNIT_LIBS)

ValidValueTests_SOURCES = TestRunner.cpp ValidValueTests.cpp
ValidValueTests_CXXFLAGS= $(CPPUNIT_CFLAGS)
ValidValueTests_LDFLAGS = $(CPPUNIT_LIBS)
ValidValueTests_LDADD   = CValidValue.lo

ParameterDictionaryTests_SOURCES = TestRunner.cpp ParamDictTests.cpp
ParameterDictionaryTests_CXXFLAGS= @CPPUNIT_CFLAGS@
ParameterDictionaryTests_LDFLAGS = @CPPUNIT_LIBS@
ParameterDictionaryTests_LDADD   = CParameterDictionary.lo CParameter.lo

ParameterTests_SOURCES = TestRunner.cpp ParameterTests.cpp
ParameterTests_CXXFLAGS= @CPPUNIT_CFLAGS@
ParameterTests_LDFLAGS = @CPPUNIT_LIBS@
ParameterTests_LDADD   = CParameterDictionary.lo CParameter.lo 

ParameterArrayTests_SOURCES= TestRunner.cpp ParameterArrayTests.cpp
ParameterArrayTests_CXXFLAGS= @CPPUNIT_CFLAGS@
ParameterArrayTests_LDFLAGS = @CPPUNIT_LIBS@
ParameterArrayTests_LDADD=CParameterDictionary.lo CParameter.lo CParameterArray.lo

TreeParamCmdTests_SOURCES= TestRunner.cpp TreeParamCmdTest.cpp
TreeParamCmdTests_CXXFLAGS=@CPPUNIT_CFLAGS@
TreeParamCmdTests_LDFLAGS= $(TEST_LD_FLAGS)
TreeParamCmdTests_LDADD= CTreeParameterCommand.lo CParameterDictionary.lo \
	CParameter.lo CParameterArray.lo  CSpecTclInterpreter.lo

ConditionTest_SOURCES = TestRunner.cpp ConditionTest.cpp
ConditionTest_CXXFLAGS=@CPPUNIT_CFLAGS@
ConditionTest_LDFLAGS=@CPPUNIT_LIBS@
ConditionTest_LDADD= CTrueCondition.lo CFalseCondition.lo

AxisTest_SOURCES   = TestRunner.cpp AxisTest.cpp
AxisTest_CXXFLAGS =  @CPPUNIT_CFLAGS@
AxisTest_LDFLAGS  =  @CPPUNIT_LIBS@
AxisTest_LDADD    = CAxis.lo

SpectrumTest_SOURCES = TestRunner.cpp SpectrumTest.cpp
SpectrumTest_CXXFLAGS =  @CPPUNIT_CFLAGS@
SpectrumTest_LDFLAGS  =  @CPPUNIT_LIBS@
SpectrumTest_LDADD    = CAxis.lo CSpectrum.lo CTrueCondition.lo CFalseCondition.lo

SpectrumDictTest_SOURCES=TestRunner.cpp SpectrumDictTest.cpp
SpectrumDictTest_CXXFLAGS = @CPPUNIT_CFLAGS@
SpectrumDictTest_LDFLAGS=@CPPUNIT_LIBS@
SpectrumDictTest_LDADD = CSpectrumDictionary.lo CAxis.lo CSpectrum.lo \
	CTrueCondition.lo CFalseCondition.lo

SpecTclAllocatorTest_SOURCES=TestRunner.cpp SpecTclAllocatorTest.cpp
SpecTclAllocatorTest_CXXFLAGS=@CPPUNIT_CFLAGS@
SpecTclAllocatorTest_LDFLAGS=@CPPUNIT_LIBS@
SpecTclAllocatorTest_LDADD=CSpectrum.lo CSpecTclSpectrumAllocator.lo CTrueCondition.lo CAxis.lo

SpecTcl1dIncTest_SOURCES = TestRunner.cpp SpecTcl1dIncTest.cpp
SpecTcl1dInctest_CXXFLAGS=@CPPUNIT_CFLAGS@
SpecTcl1dIncTest_LDFLAGS=@CPPUNIT_LIBS@
SpecTcl1dIncTest_LDADD=CSpecTcl1dIncrementer.lo \
	CSpectrum.lo CSpecTclSpectrumAllocator.lo CTrueCondition.lo CAxis.lo \
	CParameter.lo CParameterDictionary.lo  CSpectrumIncrementer.lo \
	CSpecTcl1dBaseIncrementer.lo

SpecTcl2dIncTest_SOURCES = TestRunner.cpp SpecTcl2dIncTest.cpp
SpecTcl2dInctest_CXXFLAGS=@CPPUNIT_CFLAGS@
SpecTcl2dIncTest_LDFLAGS=@CPPUNIT_LIBS@
SpecTcl2dIncTest_LDADD=CSpecTcl2dIncrementer.lo \
	CSpectrum.lo CSpecTclSpectrumAllocator.lo CTrueCondition.lo CAxis.lo \
	CParameter.lo CParameterDictionary.lo CSpectrumIncrementer.lo \
	CSpecTcl2dBaseIncrementer.lo


SpecTclSum1Test_SOURCES = TestRunner.cpp SpecTclSum1Test.cpp
SpecTclSum1Test_CXXFLAGS=@CPPUNIT_CFLAGS@
SpecTclSum1Test_LDFLAGS =@CPPUNIT_LIBS@
SpecTclSum1Test_LDADD = CSpecTclSum1dIncrementer.lo \
	CSpectrum.lo CSpecTclSpectrumAllocator.lo CTrueCondition.lo CAxis.lo \
	CParameter.lo CParameterDictionary.lo CSpectrumIncrementer.lo \
	CSpecTcl1dBaseIncrementer.lo


SpecTclComboTest_SOURCES = TestRunner.cpp SpecTclComboTest.cpp
SpecTclComboTest_CXXFLAGS= @CPPUNIT_CFLAGS@
SpecTclComboTest_LDFLAGS= @CPPUNIT_LIBS@
SpecTclComboTest_LDADD=  CSpecTclCombo2dIncrementer.lo \
	CSpectrum.lo CSpecTclSpectrumAllocator.lo CTrueCondition.lo CAxis.lo \
	CParameter.lo CParameterDictionary.lo CSpectrumIncrementer.lo \
	CSpecTcl2dBaseIncrementer.lo

SpecTclSummaryTest_SOURCES = TestRunner.cpp SpecTclSummaryTest.cpp
SpecTclSummaryTest_CXXFLAGS= @CPPUNIT_CFLAGS@
SpecTclSummaryTest_LDFLAGS= @CPPUNIT_LIBS@
SpecTclSummaryTest_LDADD= CSpecTclSummaryIncrementer.lo \
	CSpectrum.lo CSpecTclSpectrumAllocator.lo CTrueCondition.lo CAxis.lo \
	CParameter.lo CParameterDictionary.lo CSpectrumIncrementer.lo \
	CSpecTcl2dBaseIncrementer.lo

SpecTcl2dSumIncTest_SOURCES = TestRunner.cpp SpecTcl2dSumIncTest.cpp
SpecTcl2dSumIncTest_CXXFLAGS = @CPPUNIT_CFLAGS@
SpecTcl2dSumIncTest_LDFLAGS  = @CPPUNIT_LIBS@
SpecTcl2dSumIncTest_LDADD    = CSpecTcl2dSumIncrementer.lo \
	CSpectrum.lo CSpecTclSpectrumAllocator.lo CTrueCondition.lo CAxis.lo \
	CParameter.lo CParameterDictionary.lo CSpectrumIncrementer.lo \
	CSpecTcl2dBaseIncrementer.lo

SpecTclGammaSummaryIncTest_SOURCES = TestRunner.cpp SpecTclGammaSummaryIncTest.cpp
SpecTclGammaSummaryIncTest_CXXFLAGS = @CPPUNIT_CFLAGS@
SpecTclGammaSummaryIncTest_LDFLAGS  = @CPPUNIT_LIBS@
SpecTclGammaSummaryIncTest_LDADD    = CSpecTclGammaSummaryIncrementer.lo \
	CSpectrum.lo CSpecTclSpectrumAllocator.lo CTrueCondition.lo CAxis.lo \
	CParameter.lo CParameterDictionary.lo CSpectrumIncrementer.lo \
	CSpecTcl2dBaseIncrementer.lo

SpectrumCommandTest_SOURCES = TestRunner.cpp spectrumcmdtests.cpp
SpectrumCommandTest_CXXFLAGS= $(TEST_CPPFLAGS)
SpectrumCommandTest_LDFLAGS = $(TEST_LD_FLAGS)
SpectrumCommandTest_LDADD   = CSpectrumCommand.lo CSpecTclInterpreter.lo


SpecTclFactoryTests_SOURCES = TestRunner.cpp spectclfactorytests.cpp
SpecTclFactoryTests_CXXFLAGS= $(TEST_CPPFLAGS)
SpecTclFactoryTests_LDFLAGS = $(TEST_LD_FLAGS)
SpecTclFactoryTests_LDADD   = CSpecTclHistogrammerFactory.lo \
				CHistogrammerFactory.lo \
				CSpecTclSpectrumAllocator.lo \
				CSpectrum.lo CAxis.lo \
				CParameterDictionary.lo  CParameter.lo \
				CSpectrumIncrementer.lo CSpecTcl1dIncrementer.lo \
				CSpecTcl1dBaseIncrementer.lo \
				CSpecTcl2dIncrementer.lo CSpecTcl2dBaseIncrementer.lo \
				CSpecTclSum1dIncrementer.lo