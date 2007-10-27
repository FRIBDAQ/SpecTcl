initLDFLAGS=@TCL_LDFLAGS@

INCLUDES = -I. -I@top_srcdir@/Utility -I@top_srcdir@/NSCLException \
	-I@top_srcdir@/Xamine -I@top_srcdir@/TCL \
	-I@top_srcdir@/Events -I@top_srcdir@/SpecTcl \
	-I@top_srcdir@/Sorter -I@top_srcdir@/EventSource \
	-I@top_srcdir@/Filter -I@top_srcdir@/Gates       \
	-I@top_srcdir@/Xamine -I@top_srcdir@/Display      \
	@TCL_FLAGS@ \
	-D_DEBUG -DCHECK_ALL -DDESIGN_BY_CONTRACT


lib_LTLIBRARIES=libCalibrations.la

libCalibrations_la_LDFLAGS = -version-info $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

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



#  Install myself in $(prefix)/TclLibs/calibration

install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	$(mkinstalldirs) $(prefix)/TclLibs/calibration
	$(INSTALL_SCRIPT) *.tcl $(prefix)/TclLibs/calibration
	$(INSTALL_PROGRAM) .libs/libCalibrations.* $(prefix)/TclLibs/calibration
	$(TCLSH_CMD)  packages.tcl $(prefix)/TclLibs/calibration
	echo "package ifneeded Calibrations 1.0 [list load [file join \$$dir libCalibrations.so]]" >> 				\
	$(prefix)/TclLibs/calibration/pkgIndex.tcl
