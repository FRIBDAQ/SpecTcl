bin_PROGRAMS = PyQtGUI

include_HEADERS = CPyHelper.h

PyQtGUI_SOURCES = gui.cpp
PyQtGUI_CXXFLAGS = @PYTHON_CXXFLAGS@
PyQtGUI_LDFLAGS = @PYTHON_LDFLAGS@

install-exec-hook:
	$(mkinstalldirs) @prefix@/QtGUI
	$(INSTALL_DATA) @srcdir@/*.py @prefix@/Script
	$(INSTALL_DATA) @srcdir@/Main.py @prefix@/Skel
	$(INSTALL_DATA) @srcdir@/shm_parser.py @prefix@/Skel

EXTRA_DIST = ConfigGUI.py GUI.py __init__.py Main.py MenuGUI.py OutputGUI.py Plot.py shm_parser.py \
	Clustering.py CannyEdge.py ClusterPlot.py Joystick.py PeakFinder.py
