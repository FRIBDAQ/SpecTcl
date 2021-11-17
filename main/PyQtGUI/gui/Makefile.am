bin_PROGRAMS = _CutiePie

include_HEADERS = CPyHelper.h

_CutiePie_SOURCES = gui.cpp 
_CutiePie_CXXFLAGS = @PYTHON_CXXFLAGS@
_CutiePie_LDFLAGS = @PYTHON_LDFLAGS@

install-exec-hook:
	$(mkinstalldirs) @prefix@/share/d3web
	$(INSTALL_DATA) @srcdir@/d3.js/d3.v6.min.js @prefix@/share/d3web
	$(INSTALL_DATA) @srcdir@/d3.js/plotly-1.57.1.js @prefix@/share/d3web
	$(INSTALL_DATA) @srcdir@/d3.js/style.css @prefix@/share/d3web
	$(INSTALL_DATA) @srcdir@/d3.js/TimeAndDate.js @prefix@/share/d3web	
	$(INSTALL_DATA) @srcdir@/d3.js/index.html @prefix@/share/d3web	
	$(INSTALL_DATA) @srcdir@/*.py @prefix@/Script
	$(INSTALL_DATA) @srcdir@/Main.py @prefix@/Skel 
	$(INSTALL_DATA) @srcdir@/algo_skel_creator.py @prefix@/Skel
	$(INSTALL_DATA) @srcdir@/fit_skel_creator.py @prefix@/Skel
	$(INSTALL_DATA) @srcdir@/jupyterqtpy_notebook_config.py @prefix@/Skel
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
	$(INSTALL_DATA) @srcdir@/algo_skel_creator.py @prefix@/VMUSBSkel
	$(INSTALL_DATA) @srcdir@/fit_skel_creator.py @prefix@/VMUSBSkel
	$(INSTALL_DATA) @srcdir@/jupyterqtpy_notebook_config.py @prefix@/VMUSBSkel
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
	$(INSTALL_DATA) @srcdir@/algo_skel_creator.py @prefix@/DDASSkel
	$(INSTALL_DATA) @srcdir@/fit_skel_creator.py @prefix@/DDASSkel
	$(INSTALL_DATA) @srcdir@/jupyterqtpy_notebook_config.py @prefix@/DDASSkel
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

EXTRA_DIST = Main.py GUI.py \
	MenuGUI.py ConfigGUI.py OutputGUI.py 1DfunctionGUI.py 2DfunctionGUI.py JoystickGUI.py PlotGUI.py\
	shm_parser.py CannyEdgePlot.py ImgSegPlot.py algo_factory.py skel_creator.py skel_plot.py kmean_creator.py gmm_creator.py imgseg_creator.py cannye_creator.py \
	fit_factory.py fit_skel_creator.py fit_gaus_creator.py fit_exp_creator.py fit_p1_creator.py fit_p2_creator.py fit_gp1_creator.py fit_gp2_creator.py WebWindow.py notebook_process.py logger.py \
	jupyterqtpy_notebook_config.pyd3.js includes.py

