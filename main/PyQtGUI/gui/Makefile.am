bin_PROGRAMS = PyQtGUI

include_HEADERS = CPyHelper.h

PyQtGUI_SOURCES = gui.cpp
PyQtGUI_CXXFLAGS = $(shell python3-config --includes)
PyQtGUI_LDFLAGS = $(shell python3-config --ldflags)

install-exec-hook:
	$(mkinstalldirs) @prefix@/QtGUI
	$(INSTALL_PROGRAM) PyQtGUI @prefix@/QtGUI
	$(INSTALL_DATA) @srcdir@/*.py @prefix@/QtGUI

EXTRA_DIST = *.py 
