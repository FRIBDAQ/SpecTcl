# Makefile.in generated automatically by automake 1.4-p4 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ..

prefix = /home/fox/spectest
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/SpecTcl
pkglibdir = $(libdir)/SpecTcl
pkgincludedir = $(includedir)/SpecTcl

top_builddir = ..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = 
host_triplet = i686-pc-linux-gnu
AS = @AS@
AWK = mawk
CC = gcc
CFLAGS = -g -O2
CPP = gcc -E
CXX = g++
DLLTOOL = @DLLTOOL@
ECHO = echo
EXEEXT = 
F77 = g77
LD_EXTRA_FLAGS = 
LEX = flex
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
LTLIBOBJS = 
MAKEINFO = makeinfo
MOTIF_INCLUDES =  -I/usr/X11R6/include
MOTIF_LIBSW = 
OBJDUMP = @OBJDUMP@
OBJEXT = o
PACKAGE = SpecTcl
RANLIB = ranlib
STRIP = strip
TCL_FLAGS = -I/usr/include/tcl
TCL_LDFLAGS = -L/usr/lib -ltk -ltcl
VERSION = 2.1-pre
WCHAR_T_DEF = 
XINCLUDES = -I/usr/X11R6/include
XLIBSW = -L/usr/X11R6/lib
XTLIBSW = 
YACC = bison -y
ac_aux_dir = config
gri_cmd_dir = /home/fox/spectest/share/gri
gri_dir = /home/fox/spectest/bin
have_convert = yes

DEFS = 
INCLUDES = -I. -I../TCL -I../Events -I../Utility -I../Sorter -I../Exception 	   -I../EventSource -I../Gates -I../SpectrumIO -I../Xamine 	   -I../Display -I../Tape -I../Filter 	    -I/usr/include/tcl -DINSTALLED_IN='"$(prefix)"' -I/usr/X11R6/include


lib_LTLIBRARIES = libTclGrammerCommands.la

libTclGrammerCommands_la_SOURCES = StopRun.cpp StartRun.cpp 	RunControlPackage.cpp ParameterPackage.cpp ParameterCommand.cpp 	SpectrumPackage.cpp ClearCommand.cpp BindCommand.cpp 	UnbindCommand.cpp SpectrumCommand.cpp DataSourcePackage.cpp 	AttachCommand.cpp TapeCommand.cpp GateCommand.cpp ApplyCommand.cpp 	UngateCommand.cpp GatePackage.cpp GateFactory.cpp 	GateFactoryException.cpp XamineEventHandler.cpp TCLHistogrammer.cpp 	PseudoCommand.cpp PseudoScript.cpp ChanCommand.cpp WriteCommand.cpp 	ReadCommand.cpp TCLAnalyzer.cpp EventProcessor.cpp TclGrammerApp.cpp 	Globals.cpp FilterCommand.cpp


include_HEADERS = AppInit.h ApplyCommand.h AttachCommand.h BindCommand.h 	ChanCommand.h ClearCommand.h DataSourcePackage.h EventProcessor.h 	GateCommand.h GateFactory.h GateFactoryException.h GatePackage.h 	Globals.h MySpecTclApp.h ParameterCommand.h ParameterPackage.h 	PseudoCommand.h PseudoScript.h ReadCommand.h RunControlPackage.h 	SpectrumCommand.h SpectrumPackage.h StartRun.h StopRun.h 	TCLAnalyzer.h TCLHistogrammer.h TapeCommand.h TclGrammerApp.h 	UnbindCommand.h UngateCommand.h WriteCommand.h XamineEventHandler.h 	FilterCommand.h


bin_PROGRAMS = SpecTcl

SpecTcl_DEPENDENCIES = libTclGrammerCommands.la

SpecTcl_SOURCES = MySpecTclApp.cpp 	AppInit.h ApplyCommand.h AttachCommand.h BindCommand.h 	ChanCommand.h ClearCommand.h DataSourcePackage.h EventProcessor.h 	GateCommand.h GateFactory.h GateFactoryException.h GatePackage.h 	Globals.h MySpecTclApp.h ParameterCommand.h ParameterPackage.h 	PseudoCommand.h PseudoScript.h ReadCommand.h RunControlPackage.h 	SpectrumCommand.h SpectrumPackage.h StartRun.h StopRun.h 	TCLAnalyzer.h TCLHistogrammer.h TapeCommand.h TclGrammerApp.h 	UnbindCommand.h UngateCommand.h WriteCommand.h XamineEventHandler.h


SpecTcl_LDADD = -lTclGrammerCommands -lSorting -lXplus -lGates -lAnalysis -lEventSource -lSpecio -ltclPlus -lException -lXamine -lFilter -ltape -lXt -lX11
SpecTcl_LDFLAGS = -L. -L/usr/lib -ltk -ltcl -L/usr/X11R6/lib -L../SpectrumIO -L../Tape -L../Sorter -L../Xamine -L../Display -L../Gates -L../EventSource -L../Events -L../TCL -L../Exception -L../Filter

EXTRA_DIST = Makefile.skel SPECTCL.WC SpecTclInit.tcl SpecTclRC.tcl classbreakdown.WC classbreakdown.omt mapped.tcl paramtest.tcl setup.tcl
mkinstalldirs = $(SHELL) $(top_srcdir)/config/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(lib_LTLIBRARIES)

CPPFLAGS = 
LDFLAGS = 
LIBS = -lm -lm -lm 
libTclGrammerCommands_la_LDFLAGS = 
libTclGrammerCommands_la_LIBADD = 
libTclGrammerCommands_la_OBJECTS =  StopRun.lo StartRun.lo \
RunControlPackage.lo ParameterPackage.lo ParameterCommand.lo \
SpectrumPackage.lo ClearCommand.lo BindCommand.lo UnbindCommand.lo \
SpectrumCommand.lo DataSourcePackage.lo AttachCommand.lo TapeCommand.lo \
GateCommand.lo ApplyCommand.lo UngateCommand.lo GatePackage.lo \
GateFactory.lo GateFactoryException.lo XamineEventHandler.lo \
TCLHistogrammer.lo PseudoCommand.lo PseudoScript.lo ChanCommand.lo \
WriteCommand.lo ReadCommand.lo TCLAnalyzer.lo EventProcessor.lo \
TclGrammerApp.lo Globals.lo FilterCommand.lo
bin_PROGRAMS =  SpecTcl$(EXEEXT)
PROGRAMS =  $(bin_PROGRAMS)

SpecTcl_OBJECTS =  MySpecTclApp.$(OBJEXT)
CXXFLAGS = -g -O2
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --mode=compile $(CXX) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(LIBTOOL) --mode=link $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
HEADERS =  $(include_HEADERS)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP_ENV = --best
DEP_FILES =  .deps/ApplyCommand.P .deps/AttachCommand.P \
.deps/BindCommand.P .deps/ChanCommand.P .deps/ClearCommand.P \
.deps/DataSourcePackage.P .deps/EventProcessor.P .deps/FilterCommand.P \
.deps/GateCommand.P .deps/GateFactory.P .deps/GateFactoryException.P \
.deps/GatePackage.P .deps/Globals.P .deps/MySpecTclApp.P \
.deps/ParameterCommand.P .deps/ParameterPackage.P .deps/PseudoCommand.P \
.deps/PseudoScript.P .deps/ReadCommand.P .deps/RunControlPackage.P \
.deps/SpectrumCommand.P .deps/SpectrumPackage.P .deps/StartRun.P \
.deps/StopRun.P .deps/TCLAnalyzer.P .deps/TCLHistogrammer.P \
.deps/TapeCommand.P .deps/TclGrammerApp.P .deps/UnbindCommand.P \
.deps/UngateCommand.P .deps/WriteCommand.P .deps/XamineEventHandler.P
SOURCES = $(libTclGrammerCommands_la_SOURCES) $(SpecTcl_SOURCES)
OBJECTS = $(libTclGrammerCommands_la_OBJECTS) $(SpecTcl_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .cpp .lo .o .obj .s
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu SpecTcl/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-libLTLIBRARIES:

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)

distclean-libLTLIBRARIES:

maintainer-clean-libLTLIBRARIES:

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "$(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

# FIXME: We should only use cygpath when building on Windows,
# and only if it is available.
.c.obj:
	$(COMPILE) -c `cygpath -w $<`

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core
	-rm -f *.$(OBJEXT)

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

.s.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.S.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:

maintainer-clean-libtool:

libTclGrammerCommands.la: $(libTclGrammerCommands_la_OBJECTS) $(libTclGrammerCommands_la_DEPENDENCIES)
	$(CXXLINK) -rpath $(libdir) $(libTclGrammerCommands_la_LDFLAGS) $(libTclGrammerCommands_la_OBJECTS) $(libTclGrammerCommands_la_LIBADD) $(LIBS)

mostlyclean-binPROGRAMS:

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)

distclean-binPROGRAMS:

maintainer-clean-binPROGRAMS:

install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`"; \
	    $(LIBTOOL)  --mode=install $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	list='$(bin_PROGRAMS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	done

SpecTcl$(EXEEXT): $(SpecTcl_OBJECTS) $(SpecTcl_DEPENDENCIES)
	@rm -f SpecTcl$(EXEEXT)
	$(CXXLINK) $(SpecTcl_LDFLAGS) $(SpecTcl_OBJECTS) $(SpecTcl_LDADD) $(LIBS)
.cpp.o:
	$(CXXCOMPILE) -c $<
.cpp.obj:
	$(CXXCOMPILE) -c `cygpath -w $<`
.cpp.lo:
	$(LTCXXCOMPILE) -c $<

install-includeHEADERS: $(include_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(includedir)
	@list='$(include_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(includedir)/$$p"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(includedir)/$$p; \
	done

uninstall-includeHEADERS:
	@$(NORMAL_UNINSTALL)
	list='$(include_HEADERS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(includedir)/$$p; \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = SpecTcl

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu SpecTcl/Makefile
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)

-include $(DEP_FILES)

mostlyclean-depend:

clean-depend:

distclean-depend:
	-rm -rf .deps

maintainer-clean-depend:

%.o: %.c
	@echo '$(COMPILE) -c $<'; \
	$(COMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-cp .deps/$(*F).pp .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm .deps/$(*F).pp

%.lo: %.c
	@echo '$(LTCOMPILE) -c $<'; \
	$(LTCOMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-sed -e 's/^\([^:]*\)\.o[ 	]*:/\1.lo \1.o :/' \
	  < .deps/$(*F).pp > .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm -f .deps/$(*F).pp

%.o: %.cpp
	@echo '$(CXXCOMPILE) -c $<'; \
	$(CXXCOMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-cp .deps/$(*F).pp .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm .deps/$(*F).pp

%.lo: %.cpp
	@echo '$(LTCXXCOMPILE) -c $<'; \
	$(LTCXXCOMPILE) -Wp,-MD,.deps/$(*F).pp -c $<
	@-sed -e 's/^\([^:]*\)\.o[ 	]*:/\1.lo \1.o :/' \
	  < .deps/$(*F).pp > .deps/$(*F).P; \
	tr ' ' '\012' < .deps/$(*F).pp \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm -f .deps/$(*F).pp
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am: install-libLTLIBRARIES install-binPROGRAMS \
		install-exec-local
install-exec: install-exec-am

install-data-am: install-includeHEADERS
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-libLTLIBRARIES uninstall-binPROGRAMS \
		uninstall-includeHEADERS
uninstall: uninstall-am
all-am: Makefile $(LTLIBRARIES) $(PROGRAMS) $(HEADERS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(libdir) $(DESTDIR)$(bindir) \
		$(DESTDIR)$(includedir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-libLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-binPROGRAMS \
		mostlyclean-tags mostlyclean-depend mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-libLTLIBRARIES clean-compile clean-libtool \
		clean-binPROGRAMS clean-tags clean-depend clean-generic \
		mostlyclean-am

clean: clean-am

distclean-am:  distclean-libLTLIBRARIES distclean-compile \
		distclean-libtool distclean-binPROGRAMS distclean-tags \
		distclean-depend distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-libLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-binPROGRAMS maintainer-clean-tags \
		maintainer-clean-depend maintainer-clean-generic \
		distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-libLTLIBRARIES distclean-libLTLIBRARIES \
clean-libLTLIBRARIES maintainer-clean-libLTLIBRARIES \
uninstall-libLTLIBRARIES install-libLTLIBRARIES mostlyclean-compile \
distclean-compile clean-compile maintainer-clean-compile \
mostlyclean-libtool distclean-libtool clean-libtool \
maintainer-clean-libtool mostlyclean-binPROGRAMS distclean-binPROGRAMS \
clean-binPROGRAMS maintainer-clean-binPROGRAMS uninstall-binPROGRAMS \
install-binPROGRAMS uninstall-includeHEADERS install-includeHEADERS \
tags mostlyclean-tags distclean-tags clean-tags maintainer-clean-tags \
distdir mostlyclean-depend distclean-depend clean-depend \
maintainer-clean-depend info-am info dvi-am dvi check check-am \
installcheck-am installcheck install-exec-local install-exec-am \
install-exec install-data-am install-data install-am install \
uninstall-am uninstall all-redirect all-am all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


install-exec-local:
	$(mkinstalldirs) $(prefix)/Etc
	$(mkinstalldirs) $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/SpecTclInit.tcl $(prefix)/Etc
	$(INSTALL_PROGRAM) $(srcdir)/MySpecTclApp.cpp $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/MySpecTclApp.h $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/SpecTclRC.tcl $(prefix)/Skel
	$(INSTALL_PROGRAM) $(srcdir)/setup.tcl $(prefix)/Skel
	$(INSTALL_DATA)    $(srcdir)/Globals.o $(prefix)/lib
	echo INSTDIR=$(prefix) >Makefile.user
	echo OS=`uname|sed s/-//`        >>Makefile.user
	cat Makefile.skel >> Makefile.user
	$(INSTALL_PROGRAM) $(srcdir)/Makefile.user $(prefix)/Skel/Makefile

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
