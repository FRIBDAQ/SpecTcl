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
	$(INSTALL_DATA) @srcdir@/skel_creator.py @prefix@/Skel
	$(INSTALL_DATA) @srcdir@/kmean_creator.py @prefix@/Skel
	$(INSTALL_DATA) @srcdir@/gmm_creator.py @prefix@/Skel
	$(INSTALL_DATA) @srcdir@/imgseg_creator.py @prefix@/Skel
	$(INSTALL_DATA) @srcdir@/cannye_creator.py @prefix@/Skel
	$(INSTALL_DATA) @srcdir@/ImgSegPlot.py @prefix@/Skel
	$(INSTALL_DATA) @srcdir@/CannyEdgePlot.py @prefix@/Skel


EXTRA_DIST = ConfigGUI.py GUI.py __init__.py Main.py MenuGUI.py OutputGUI.py Plot.py shm_parser.py \
	Clustering.py CannyEdgePlot.py ClusterPlot.py Joystick.py PeakFinder.py algo_factory.py skel_creator.py kmean_creator.py gmm_creator.py imgseg_creator.py cannye_creator.py
