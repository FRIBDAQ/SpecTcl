SUBDIRS =  Replace Scripts Utility factories Display \
	NSCLException Xamine Tape EventSource \
	TCL Events Gates Sorter SpectrumIO Filter Analysis\
	DecoderRing  TreeParam \
	Fits SpecTcl \
	contrib TestFiles Gui splash filtsplit calibratedparams treegui ccusb vmusb

ACLOCAL_AMFLAGS=-I m4

# Gri TestFiles need to be figured out still..

AUX_DIST = $(ac_aux_dir)/config.guess \
	$(ac_aux_dir)/config.sub \
	$(ac_aux_dir)/install-sh \
	$(ac_aux_dir)/ltmain.sh \
	$(ac_aux_dir)/missing \
	$(ac_aux_dir)/mkinstalldirs \
	$(ac_aux_dir)/ylwrap  \
	fix_libtool

EXTRA_DIST = fixdos ReleaseNotesNSCL4.0.txt SpecTclRC.tcl \
	bootstrap $(MYGRIDIR) LICENSE  \
	 doc build.cygwin Directories.include CompilationRules.include \
	CHANGELOG

dist-hook:
	mkdir -p $(distdir)/config
	echo `pwd`
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
	  cp $$file $(distdir)/$$file; \
	done


install-exec-local:
	install  -d   -m 0775 $(prefix) $(prefix)/bin $(prefix)/include
	install  -m 0664 config.h $(prefix)/include
	install  -d   -m 0775 $(prefix)/lib 
	install  -d   -m 0775 $(prefix)/etc
	install -d   -m 0775 $(prefix)/Script 
	install -d   -m 0775 $(prefix)/Skel
	install  -d   -m 0775 $(prefix)/contrib $(prefix)/doc $(prefix)/share
	install -m 0664 config.h $(prefix)/include
	rm -f $(prefix)/Bin
	rm -f $(prefix)/Lib
	rm -f $(prefix)/Include
	rm -f $(prefix)/Etc
	ln -s $(prefix)/bin $(prefix)/Bin
	ln -s $(prefix)/lib $(prefix)/Lib
	ln -s $(prefix)/include $(prefix)/Include
	ln -s $(prefix)/etc     $(prefix)/Etc
	tar czf - doc |(cd $(prefix); tar xzf -)
	echo  TCLINCLUDES=$(TCL_CPPFLAGS)          > Directories.temp
	echo  TCLLIBS    =@TK_LIBS@ @TCL_LIBS@ -lm -ldl  >> Directories.temp 
	echo  X11INCLUDES=$(X11INCLUDES)       >> Directories.temp
	echo  X11LIBS    =$(XLIBSW) $(MOTIF_LIBSW) -lXm -lXt -lX11 >> Directories.temp
	cat   Directories.include              >> Directories.temp
	$(INSTALL_DATA) Directories.temp $(sysconfdir)/Directories.include
	$(INSTALL_DATA) LICENSE    $(prefix)
	echo CXX=$(CXX)                       > CompilationRules.temp
	echo CC=$(CC)                        >> CompilationRules.temp
	echo CXXLD=$(CXX)                  >> CompilationRules.temp
	echo CCLD=$(CC)                    >> CompilationRules.temp
	cat  CompilationRules.include        >> CompilationRules.temp
	$(INSTALL_DATA) CompilationRules.temp $(sysconfdir)/CompilationRules.include
	echo "Version: " SpecTcl-$(VERSION) build on `hostname` `date` >VERSION
	$(INSTALL_DATA) VERSION $(prefix)/VERSION
	$(INSTALL_DATA) CHANGELOG $(prefix)
	$(MANDB) $(prefix)/man

check-TESTS:
	- for d in $(SUBDIRS); do (cd $$d; make check-TESTS); done
