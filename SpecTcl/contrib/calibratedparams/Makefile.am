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

MainSources=CFit.cpp  CFitCommand.cpp  CFitCreator.cpp  \
	CFitFactory.cpp  CLinearFit.cpp  CLinearFitCreator.cpp  \
	CCalibratedParameter.cpp CCalibratedParameterManager.cpp

libCalibrations_la_SOURCES = $(MainSources) CCalibratedParameterCommand.cpp 


include_HEADERS = CFit.h  CFitCommand.h  CFitCreator.h  CFitFactory.h  \
	CLinearFit.h  CLinearFitCreator.h CCalibratedParameter.h \
	CCalibratedParameterCommand.h CCalibratedParameterManager.h


EXTRA_DIST=CalibIO.tcl CalibIOGUI.tcl CalibrateMainGui.tcl \
	CalibrationPoints.tcl Dialogs.tcl ErrorRead.tcl FitIO.tcl \
	GlobalData.tcl PeakStatistics.tcl TkPrompt.tcl gui.tcl \
	tunl-logo-seal.gif SpecTclRC.tcl Makefile.am  \
	MySpecTclApp.cpp MySpecTclApp.h installpkg Makefile.skel.in


#
#   Install the stuff. as follows:
#     $(prefix)/contrib/calibratedparams    -top level.
#     $(prefix)/lib/libCalibrations.so      - The support lib.
#     $(prefix)/contrib/calibratedparams/include - *.h
#     $(prefix)/contrib/calibratedparams/skel - MySpecTclApp.{cpp,h}, Makefile.skel
#     $(prefix)/contrib/calibratedparams/Scripts - *.tcl pkg_mkIndex run on this.
#     $(prefix)/contrib/calibratedparams/src     - *.cpp
#
#  Makefile.skel will be installed a Makefile in its target directory.

install-exec-local:
	#  Make the directories we need:
	#
	$(mkinstalldirs)    $(prefix)/lib
	$(mkinstalldirs)    $(prefix)/contrib/calibratedparams
	$(mkinstalldirs)    $(prefix)/contrib/calibratedparams/include
	$(mkinstalldirs)    $(prefix)/contrib/calibratedparams/skel
	$(mkinstalldirs)    $(prefix)/contrib/calibratedparams/Scripts
	$(mkinstalldirs)    $(prefix)/contrib/calibratedparams/src
	#
	#  Put the easy stuff where it belongs.
	#
	$(INSTALL_DATA) *.h   $(prefix)/contrib/calibratedparams/include
	$(INSTALL_DATA) MySpecTclApp.cpp $(prefix)/contrib/calibratedparams/skel
	$(INSTALL_DATA) MySpecTclApp.h   $(prefix)/contrib/calibratedparams/skel
	$(INSTALL_DATA) Makefile.skel    $(prefix)/contrib/calibratedparams/skel/Makefile
	$(INSTALL_DATA) SpecTclRC.tcl    $(prefix)/contrib/calibratedparams/skel
	$(INSTALL_DATA) *.tcl *.gif      $(prefix)/contrib/calibratedparams/Scripts
	$(INSTALL_DATA) *.cpp            $(prefix)/contrib/calibratedparams/src
	#
	#  Finally we need to make the package  index file so that package require
	#  will work on this stuff.
	#
	chmod a+x ./installpkg
	./installpkg $(prefix)/contrib/calibratedparams/Scripts

if CPPUNIT

noinst_PROGRAMS = unittests 

unittests_SOURCES = CommandTest.cpp FitTest.cpp CreatorTest.cpp FactoryTest.cpp \
	CalibParamTest.cpp CalibManagerTest.cpp TestRunner.cpp \
	$(MainSources) 


TestObjects=@top_srcdir@/Events/Event.o @top_srcdir@/SpecTcl/EventProcessor.o

unittests_LDFLAGS = $(TestObjects) $(CPPUNIT_LDFLAGS) -L@top_srcdir@/TCL -ltclPlus  \
	-L@top_srcdir@/NSCLException -lException -lgsl -lgslcblas -lm $(TCL_LDFLAGS)



TESTS=./unittests

endif