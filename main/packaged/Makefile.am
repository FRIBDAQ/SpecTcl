lib_LTLIBRARIES=libSpectcl.la

libSpectcl_la_SOURCES=Spectclpackage.cpp CPipelineMaker.cpp CRingBlockReader.cpp \
	CRingFileBlockReader.cpp CFileDataGetter.cpp CDistributeToAnalyzer.cpp \
	CAnalyzeCommand.cpp CFileSourceCommand.cpp CAnalysisSinkCommand.cpp

include_HEADERS=CPipelineMaker.h CDataGetter.h CRingBlockReader.h \
	CRingFileBlockReader.h CFileDataGetter.h CDataDistributor.h \
	CDistributeToAnalyzer.h CAnalyzeCommand.h

noinst_HEADERS=CRingBlockReader.h CFileSourceCommand.h CAnalysisSinkCommand.h


libSpectcl_la_CXXFLAGS=-I@top_srcdir@/Core -I@top_srcdir@/Utility \
	-I@top_srcdir@/factories -I@top_srcdir@/Display \
		@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ \
		-DINSTALLED_IN='"@prefix@"'

libSpectcl_la_LIBADD=@top_builddir@/Core/libTclGrammerApp.la   \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ 

install-exec-hook: @builddir@/libSpectcl.la
	$(mkinstalldirs) @prefix@/TclLibs/spectclpackage
	$(INSTALL_PROGRAM) @libdir@/libSpectcl.so @prefix@/TclLibs/spectclpackage
	echo package ifneeded spectcl 1.0 [list load [file join \$$dir libSpectcl.so]] > @prefix@/TclLibs/spectclpackage/pkgIndex.tcl
	$(mkinstalldirs) @prefix@/BatchSkel
	$(INSTALL_DATA) @srcdir@/CPipelineMaker.{h,cpp}    @prefix@/BatchSkel
	$(INSTALL_DATA) @srcdir@/Skeleton.cpp @prefix@/BatchSkel
	$(INSTALL_DATA) Makefile_skel @prefix@/BatchSkel/Makefile


EXTRA_DIST=Skeleton.cpp Makefile_skel.in
