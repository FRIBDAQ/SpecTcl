lib_LTLIBRARIES=librootfilterformat.la
librootfilterformat_la_LIBADD=$(ROOTLDSW)

INCLUDES=$(SPECTCLCCSW) $(ROOTCCSW) $(TCLCCSW)

librootfilterformat_la_SOURCES	=	CRootFilterOutputStage.cpp	\
					CRootNtupleAccumulator.cpp	\
					CRootFilterOutputStageCreator.cpp \
					rootPackage.cpp

noinst_HEADERS=	CRootFilterOutputStage.h	\
		CRootFilterOutputStageCreator.h	\
		CRootNtupleAccumulator.h


#  The library will get installed in $prefix/lib, nothing we can do
#  about that, but we want it installed in $prefix/TclLibs:

install-exec-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	for f in .libs/lib*; do $(INSTALL_PROGRAM) $$f $(prefix)/TclLibs; done
