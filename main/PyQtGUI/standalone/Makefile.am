bin_PROGRAMS = CutiePie

include_HEADERS = CPyHelper.h

CutiePie_SOURCES = gui.cpp config.cpp SubManagerAPI.cpp SubAPI.cpp
CutiePie_CXXFLAGS = @PYTHON_CXXFLAGS@ -DINSTALLED_IN='"@prefix@"' -lpthread
CutiePie_LDFLAGS = @PYTHON_LDFLAGS@

