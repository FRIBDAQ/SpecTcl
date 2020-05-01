bin_PROGRAMS = PyQtGUI

include_HEADERS = CPyHelper.h

PyQtGUI_SOURCES = gui.cpp
PyQtGUI_CXXFLAGS = @PYTHON_CXXFLAGS@
PyQtGUI_LDFLAGS = @PYTHON_LDFLAGS@

install-exec-hook:
	$(mkinstalldirs) @prefix@/QtGUI
	$(INSTALL_PROGRAM) PyQtGUI @prefix@/QtGUI
	$(INSTALL_DATA) @srcdir@/*.py @prefix@/QtGUI

EXTRA_DIST = ConfigGUI.py GUI.py __init__.py Main.py MenuGUI.py OutputGUI.py Plot.py shm_parser.py
