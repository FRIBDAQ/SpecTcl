

lib_LTLIBRARIES = libSpecTclJson.la

SUBDIRS = json

libSpecTclJson_la_SOURCES = BinInfo.cpp \
														GateCmdParser.cpp \
														GateInfo.cpp \
														HistFactory.cpp \
														HistFiller.cpp \
														HistInfo.cpp \
														JsonParser.cpp \
														ParameterCmdParser.cpp \
														ParameterInfo.cpp \
														SharedMemoryKeyParser.cpp

include_HEADERS = BinInfo.h \
									GateCmdParser.h \
									GateInfo.h \
									HistFactory.h \
									HistFiller.h \
									HistInfo.h \
									JsonParser.h \
									ParameterCmdParser.h \
									ParameterInfo.h \
									SharedMemoryKeyParser.h

libSpecTclJson_la_CXXFLAGS = @ROOT_CFLAGS@ \
														 @JSONCPP_CFLAGS@ 

libSpecTclJson_la_LIBADD = @ROOT_LDFLAGS@ \
													 @JSONCPP_LIBS@
														 

bin_PROGRAMS = unittests


unittests_SOURCES = GateInfoTest.cpp \
										TestRunner.cpp \
										GateCmdParserTest.cpp \
										HistFactoryTest.cpp \
										HistFillerTest.cpp \
										HistInfoTest.cpp \
										JsonParserTest.cpp \
										ParameterCmdParserTest.cpp \
										SharedMemoryKeyParserTest.cpp

unittests_CXXFLAGS = @JSONCPP_CFLAGS@ \
										 @ROOT_CFLAGS@ \
										 @CPPUNIT_CFLAGS@ \
										 -I@top_srcdir@/Utility

unittests_CPPFLAGS = -DJSON_TEST_DIR='"@srcdir@/json"'

 
unittests_LDADD = @builddir@/libSpecTclJson.la \
										@JSONCPP_LIBS@ \
										@ROOT_LDFLAGS@ \
										@CPPUNIT_LIBS@ 

unittests_LDFLAGS = -Wl,-rpath=@ROOT_LIBRARY_DIR@


TESTS = unittests
