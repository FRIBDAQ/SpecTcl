initLDFLAGS=@TK_LIBS@ @TCL_LIBS@



lib_LTLIBRARIES=libCalibrations.la


libCalibrations_la_SOURCES =	CalibrationPackage.cpp		\
				CCalibratedParameterCommand.cpp	\
				CCalibratedParameter.cpp	\
				CCalibratedParameterManager.cpp	\
				CCalibratedFitCommand.cpp	\
				CFit.cpp			\
				CFitCreator.cpp			\
				CFitFactory.cpp			\
				CLinearFit.cpp			\
				CLinearFitCreator.cpp

noinst_HEADERS	=		CCalibratedParameterCommand.h	\
				CCalibratedParameter.h		\
				CCalibratedParameterManager.h	\
				CCalibratedFitCommand.h		\
				CFitCreator.h			\
				CFitFactory.h			\
				CFit.h				\
				CLinearFitCreator.h		\
				CLinearFit.h

libCalibrations_la_LDFLAGS = -version-info $(SOVERSION):0 \
	@LIBTCLPLUS_LDFLAGS@ -Wl,"-rpath-link=$(libdir)"

libCalibrations_la_CPPFLAGS = -I. -I@top_srcdir@/Utility \
	-I@top_srcdir@/Display      \
	-I@top_srcdir@/factories \
	-I@top_srcdir@/Core \
	@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ \
	-D_DEBUG -DCHECK_ALL -DDESIGN_BY_CONTRACT

EXTRA_DIST = 	CalibIOGUI.tcl Dialogs.tcl gui.tcl TkPrompt.tcl	\
		CalibIO.tcl ErrorRead.tcl packages.tcl	\
		CalibrateMainGui.tcl FitIO.tcl PeakStatistics.tcl \
		CalibrationPoints.tcl  GlobalData.tcl  


#  Install myself in $(prefix)/TclLibs/calibration

install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	$(mkinstalldirs) $(prefix)/TclLibs/calibration
	$(INSTALL_SCRIPT) *.tcl $(prefix)/TclLibs/calibration
	$(INSTALL_PROGRAM) .libs/libCalibrations.* $(prefix)/TclLibs/calibration
	@TCLSH@  packages.tcl $(prefix)/TclLibs/calibration
	echo "package ifneeded Calibrations 1.0 [list load [file join \$$dir libCalibrations.so]]" >> 				\
	$(prefix)/TclLibs/calibration/pkgIndex.tcl
