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
	$(INSTALL_DATA) @srcdir@/skel_plot.py @prefix@/Skel
	$(INSTALL_DATA) @srcdir@/fit_skel_creator.py @prefix@/Skel
	if [ -s @prefix@/Skel/Script ] ; \
	then \
	     rm -rf @prefix@/Skel/Script ; \
	fi;
	ln -s @prefix@/Script @prefix@/Skel
	if [ -s @prefix@/Skel/Lib ] ; \
	then \
	     rm -rf @prefix@/Skel/Lib ; \
	fi;
	ln -s @prefix@/lib @prefix@/Skel/Lib
	$(INSTALL_DATA) @srcdir@/Main.py @prefix@/VMUSBSkel
	$(INSTALL_DATA) @srcdir@/shm_parser.py @prefix@/VMUSBSkel
	$(INSTALL_DATA) @srcdir@/skel_creator.py @prefix@/VMUSBSkel
	$(INSTALL_DATA) @srcdir@/skel_plot.py @prefix@/VMUSBSkel
	$(INSTALL_DATA) @srcdir@/fit_skel_creator.py @prefix@/VMUSBSkel
	if [ -s @prefix@/VMUSBSkel/Script ] ; \
	then \
	     rm -rf @prefix@/VMUSBSkel/Script ; \
	fi;
	ln -s @prefix@/Script @prefix@/VMUSBSkel
	if [ -s @prefix@/VMUSBSkel/Lib ] ; \
	then \
	     rm -rf @prefix@/VMUSBSkel/Lib ; \
	fi;
	ln -s @prefix@/lib @prefix@/VMUSBSkel/Lib
	$(INSTALL_DATA) @srcdir@/Main.py @prefix@/DDASSkel
	$(INSTALL_DATA) @srcdir@/shm_parser.py @prefix@/DDASSkel
	$(INSTALL_DATA) @srcdir@/skel_creator.py @prefix@/DDASSkel
	$(INSTALL_DATA) @srcdir@/skel_plot.py @prefix@/DDASSkel
	$(INSTALL_DATA) @srcdir@/fit_skel_creator.py @prefix@/DDASSkel
	if [ -s @prefix@/DDASSkel/Script ] ; \
	then \
	     rm -rf @prefix@/DDASSkel/Script ; \
	fi;
	ln -s @prefix@/Script @prefix@/DDASSkel
	if [ -s @prefix@/DDASSkel/Lib ] ; \
	then \
	     rm -rf @prefix@/DDASSkel/Lib ; \
	fi;
	ln -s @prefix@/lib @prefix@/DDASSkel/Lib

EXTRA_DIST = ConfigGUI.py GUI.py __init__.py Main.py MenuGUI.py OutputGUI.py Plot.py shm_parser.py \
	Clustering.py CannyEdgePlot.py ImgSegPlot.py Joystick.py PeakFinder.py algo_factory.py skel_creator.py skel_plot.py kmean_creator.py gmm_creator.py imgseg_creator.py cannye_creator.py \
	fit_factory.py fit_skel_creator.py fit_gaus_creator.py fit_exp_creator.py fit_p1_creator.py fit_p2_creator.py fit_gp1_creator.py fit_gp2_creator.py
