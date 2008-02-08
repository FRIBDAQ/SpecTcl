initLDFLAGS=@TCL_LDFLAGS@

INCLUDES = -I. 						\
	@SPECTCLCCSW@					\
	@TCLCCSW@ 					\
	-D_DEBUG -DCHECK_ALL -DDESIGN_BY_CONTRACT      \
	@GSL_CXXFLAGS@


lib_LTLIBRARIES=libCalibrations.la

libCalibrations_la_LDFLAGS = -Wl,"-rpath-link=$(libdir)" @GSL_LDFLAGS@ 

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


EXTRA_DIST = 	CalibIOGUI.tcl Dialogs.tcl gui.tcl TkPrompt.tcl	\
		CalibIO.tcl ErrorRead.tcl packages.tcl	\
		CalibrateMainGui.tcl FitIO.tcl PeakStatistics.tcl \
		CalibrationPoints.tcl  GlobalData.tcl  calibparams.xml


#  Install myself in $(prefix)/TclLibs/calibration

install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	$(mkinstalldirs) $(prefix)/TclLibs/calibration
	$(INSTALL_SCRIPT) *.tcl $(prefix)/TclLibs/calibration
	$(INSTALL_PROGRAM) .libs/libCalibrations.* $(prefix)/TclLibs/calibration
	$(TCLSH_CMD)  packages.tcl $(prefix)/TclLibs/calibration
	echo "package ifneeded Calibrations 1.0 [list load [file join \$$dir libCalibrations.so]]" >> 				\
	$(prefix)/TclLibs/calibration/pkgIndex.tcl


calibparams.pdf: calibparams.xml
	$(DOCBOOK) $<

docs: calibparams.pdf