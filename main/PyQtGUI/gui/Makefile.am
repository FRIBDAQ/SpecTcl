bin_PROGRAMS = PyQtGUI

include_HEADERS = CPyHelper.h

PyQtGUI_SOURCES = gui.cpp
PyQtGUI_CXXFLAGS = -I/usr/include/python3.7 -I@top_srcdir@/PyQtGUI/CPyLibrary
PyQtGUI_LDFLAGS = -L/usr/local/lib/python3.7 -lpython3.7m -Wl,-rpath,/usr/local/lib/python3.7

install-exec-hook:
	$(mkinstalldirs) @prefix@/QtGUI
	$(INSTALL_PROGRAM) PyQtGUI @prefix@/QtGUI
	$(INSTALL_DATA) @srcdir@/*.py @prefix@/QtGUI

EXTRA_DIST = *.py 
