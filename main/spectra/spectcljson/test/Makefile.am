

bin_PROGRAMS = unittests


unittests_SOURCES = GateInfoTest.cpp \
										TestRunner.cpp \
										GateCmdParserTest.cpp \
										HistFactoryTest.cpp \
										HistFillerTest.cpp \
										HistInfoTest.cpp \
										JsonParserTest.cpp \
										ParameterCmdParserTest.cpp 

unittests_CXXFLAGS = -I@srcdir@/.. \
										 @JSONCPP_CFLAGS@ \
										 @ROOT_CFLAGS@ \
										 @CPPUNIT_CFLAGS@

unittests_CPPFLAGS = -DJSON_TEST_DIR='"@srcdir@/json"'

 
unittests_LDADD = @builddir@/../libSpecTclJson.la \
										@JSONCPP_LIBS@ \
										@ROOT_LDFLAGS@ \
										@CPPUNIT_LIBS@ 

unittests_LDFLAGS = -Wl,-rpath=@ROOT_LIBRARY_DIR@


TESTS = unittests

