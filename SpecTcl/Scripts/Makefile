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

SCRIPTDIR = $(srcdir)
SCRIPTDEST = $(prefix)/Script

EXTRA_DIST = dtree.tcl fileall.tcl gui.tcl handle.tcl listall.tcl tkcon.tcl tree.tcl CopyGates.tcl save.head
mkinstalldirs = $(SHELL) $(top_srcdir)/config/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP_ENV = --best
all: all-redirect
.SUFFIXES:
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu Scripts/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status

tags: TAGS
TAGS:


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = Scripts

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu Scripts/Makefile
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
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am: install-exec-local
install-exec: install-exec-am

install-data-am:
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am:
uninstall: uninstall-am
all-am: Makefile
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: tags distdir info-am info dvi-am dvi check check-am \
installcheck-am installcheck install-exec-local install-exec-am \
install-exec install-data-am install-data install-am install \
uninstall-am uninstall all-redirect all-am all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


install-exec-local:
	$(mkinstalldirs) $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/*.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/fileall.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/gui.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/handle.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/listall.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/save.head  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/tkcon.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/tree.tcl  $(SCRIPTDEST)
	$(INSTALL_PROGRAM) $(SCRIPTDIR)/CopyGates.tcl  $(SCRIPTDEST)

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
