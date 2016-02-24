

lib_LTLIBRARIES = libSpecTclJson.la

libSpecTclJson_la_SOURCES = BinInfo.cpp \
														GateCmdParser.cpp \
														GateInfo.cpp \
														HistFactory.cpp \
														HistFiller.cpp \
														HistInfo.cpp \
														JsonParser.cpp \
														ParameterCmdParser.cpp \
														ParameterInfo.cpp

include_HEADERS = BinInfo.h \
									GateCmdParser.h \
									GateInfo.h \
									HistFactory.h \
									HistFiller.h \
									HistInfo.h \
									JsonParser.h \
									ParameterCmdParser.h \
									ParameterInfo.h

libSpecTclJson_la_CXXFLAGS = @ROOT_CFLAGS@ \
														 @JSONCPP_CFLAGS@ 

libSpecTclJson_la_LIBADD = @ROOT_LDFLAGS@ \
													 @JSONCPP_LDFLAGS@ \
													 @JSONCPP_LIBS@
														 

