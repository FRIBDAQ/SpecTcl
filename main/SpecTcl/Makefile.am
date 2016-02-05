DEFS = @WCHAR_T_DEF@


bin_PROGRAMS = SpecTcl

SpecTcl_SOURCES = MySpecTclApp.cpp 

SpecTcl_CXXFLAGS = -I@top_srcdir@/Core \
									 -I@top_srcdir@/Utility \
									 @LIBTCLPLUS_CFLAGS@ \
									 @TCL_CPPFLAGS@

SpecTcl_CPPFLAGS = -g

SpecTcl_LDADD = @top_builddir@/Core/libTclGrammerApp.la \
								@top_builddir@/Display/libXamine.la \
								@top_builddir@/Tape/libtape.la \
								-lXt -lX11 -lgsl -lgslcblas

SpecTcl_LDFLAGS = $(X11EXTRA_LDFLAGS)  $(LD_EXTRA_FLAGS) \
	@LIBTCLPLUS_LDFLAGS@	\
	-Wl,"-rpath=$(libdir)" @TK_LIBS@ @TCL_LIBS@ @XLIBSW@



EXTRA_DIST = Makefile.skel SpecTclInit.tcl SpecTclRC.tcl \
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

