DEFS = @WCHAR_T_DEF@


bin_PROGRAMS = SpecTcl

SpecTcl_SOURCES = MySpecTclApp.cpp 

SpecTcl_CXXFLAGS = -I@srcdir@ \
									 -I@top_srcdir@/Utility \
									 @LIBTCLPLUS_CFLAGS@ \
									 @TCL_CPPFLAGS@

SpecTcl_CPPFLAGS = -g

SpecTcl_LDADD = @top_builddir@/Core/libSpecTcl.la \
								@top_builddir@/Display/libXamine.la \
								@top_builddir@/Tape/libtape.la \
								-lXt -lX11 -lgsl -lgslcblas

SpecTcl_LDFLAGS = $(X11EXTRA_LDFLAGS)  $(LD_EXTRA_FLAGS) \
	@LIBTCLPLUS_LDFLAGS@	\
	-Wl,"-rpath=$(libdir)" @TK_LIBS@ @TCL_LIBS@ @XLIBSW@

man_MANS = spectclapi.3


EXTRA_DIST = Makefile.skel SPECTCL.WC SpecTclInit.tcl SpecTclRC.tcl \
	 mapped.tcl paramtest.tcl  \
	SpecTcl_Makefile.include $(man_MANS)

install-exec-local:
	$(mkinstalldirs) $(prefix)/etc
	$(mkinstalldirs) $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/SpecTclInit.tcl $(prefix)/etc
	$(INSTALL_PROGRAM) $(srcdir)/MySpecTclApp.cpp $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/MySpecTclApp.h $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/SpecTclRC.tcl $(prefix)/Skel
	$(INSTALL_DATA)    $(srcdir)/Globals.o $(prefix)/lib
	$(INSTALL_DATA)   SpecTcl_Makefile.include $(sysconfdir)
	echo INSTDIR=$(prefix) >Makefile.user
	cat Makefile.skel >> Makefile.user
	$(INSTALL_PROGRAM) $(srcdir)/Makefile.user $(prefix)/Skel/Makefile


# Unit tests.
#
#  Need to figure out why these don't link right
#if CPPUNITDONOTBUILD


noinst_PROGRAMS =  scalertests docbuftests unittests

scalertests_CXXFLAGS=@CPPUNIT_CFLAGS@
docbuftests_CXXFLAGS=@CPPUNIT_CFLAGS@

# TODO: Could probably add quite a few more headers above to this.
#       maybe *Command.h and *Package.h(?)


noinst_HEADERS = Asserts.h RingFormatCommand.h

scalertests_SOURCES = TestRunner.cpp \
	scalercallbacktests.cpp 	\
	controlcallbacktests.cpp

scalertests_DEPENDENCIES = ScalerProcessorCallbacks.o

docbuftests_SOURCES = TestRunner.cpp \
	docexceptiontests.cpp \
	callbacktests.cpp 

docbuftests_DEPENDENCIES = DocumentationException.o  \
	DocumentationCallback.o

unittests_SOURCES = TestRunner.cpp SpectrumPackageTests.cpp

commonTestLdFlags= @top_builddir@/Events/libAnalysis.la \
	@top_builddir@/Analysis/libBufferAnalysis.la \
	@top_builddir@/SpecTcl/libTclGrammerCommands.la \
	@top_builddir@/Filter/libFilter.la	\
	@top_builddir@/Sorter/libSorting.la \
	@top_builddir@/TreeParam/libTreeParam.la \
	@top_builddir@/Fits/libFits.la \
	@top_builddir@/DecoderRing/libDecoderRing.la \
	@top_builddir@/Tape/libtape.la \
	@top_builddir@/SpectrumIO/libSpecio.la \
	@top_builddir@/Xamine/libXplus.la \
	@top_builddir@/DisplayInterface/libDisplayInterface.la \
	@LIBTCLPLUS_LDFLAGS@ \
	@CPPUNIT_LIBS@ \
	@TK_LIBS@ \
	@TCL_LIBS@

scalertests_LDFLAGS= ScalerProcessorCallbacks.o $(commonTestLdFlags)

docbuftests_LDFLAGS = DocumentationException.o DocumentationCallback.o \
	$(commonTestLdFlags)

unittests_LDADD = $(commonTestLdFlags)

check-TESTS:
	./unittests
#	./scalertests ./docbuftests ./unittests

#endif
