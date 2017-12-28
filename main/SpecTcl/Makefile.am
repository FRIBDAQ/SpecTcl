DEFS = @WCHAR_T_DEF@


bin_PROGRAMS = SpecTcl

SpecTcl_SOURCES = MySpecTclApp.cpp  MySpecTclApp.h

SpecTcl_CXXFLAGS = -I@top_srcdir@/Core \
		 -I@top_srcdir@/Utility \
		 @LIBTCLPLUS_CFLAGS@ \
		 @TCL_CPPFLAGS@

SpecTcl_CPPFLAGS = -g

# Remove:	@top_builddir@/Tape/libtape.la

LDFLAGS = $(X11EXTRA_LDFLAGS)  $(LD_EXTRA_FLAGS) \
	@LIBTCLPLUS_LDFLAGS@	\
	 @TK_LIBS@ @TCL_LIBS@ @XLIBSW@

SpecTcl_LDADD = @top_builddir@/Core/libTclGrammerApp.la \
		@top_builddir@/Display/libXamine.la \
		-lXt -lX11 -lgsl -lgslcblas $(LDFLAGS)

SpecTcl_LDFLAGS = -Wl,"-rpath=$(libdir)"



EXTRA_DIST = Makefile.skel SpecTclInit.tcl SpecTclRC.tcl \
	 mapped.tcl paramtest.tcl  \
	SpecTcl_Makefile.include $(man_MANS)

install-exec-local:
	$(mkinstalldirs) @prefix@/etc
	$(mkinstalldirs) @prefix@/Skel
	$(INSTALL_PROGRAM) @srcdir@/SpecTclInit.tcl @prefix@/etc
	$(INSTALL_PROGRAM) @srcdir@/MySpecTclApp.cpp @prefix@/Skel
	$(INSTALL_PROGRAM) @srcdir@/MySpecTclApp.h @prefix@/Skel
	$(INSTALL_PROGRAM) @srcdir@/SpecTclRC.tcl @prefix@/Skel
	$(INSTALL_DATA)   @srcdir@/SpecTcl_Makefile.include $(sysconfdir)
	echo INSTDIR=@prefix@ >Makefile.user
	cat @srcdir@/Makefile.skel >> Makefile.user
	$(INSTALL_PROGRAM) @builddir@/Makefile.user @prefix@/Skel/Makefile

