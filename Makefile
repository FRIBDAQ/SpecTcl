#
#  Note: PROFILE builds are only done on SpecTcl, not on Xamine.  This
#        prevents Xamine profiling output from going into SpecTcl's profile
#        output file.  At present to profile Xamine one must manually
#        and temporarily edit the makefile.
# Imported macros:
#    INSTDIR - Installation destination directory.
#    OS      - output of system's uname command with '-' characters removed.
#    PROFILE - If profiling is desired, switches to add to compilation/link.
#

LinuxMAKESW=-j 4
DarwinMAKESW=-j 4

system:
	mkdir -p $(INSTDIR)/Bin
	mkdir -p $(INSTDIR)/Etc
	mkdir -p $(INSTDIR)/Include
	mkdir -p $(INSTDIR)/Lib
	mkdir -p $(INSTDIR)/Script
	mkdir -p $(INSTDIR)/Skel
	mkdir -p $(INSTDIR)/contrib
	mkdir -p $(INSTDIR)/doc
	(cd contrib; \
            tar cf - .| (cd $(INSTDIR)/contrib; tar xf -) \
	)
	(cd Scripts; \
		make install  INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd Utility;   \
	 make system $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd Display;  \
	make  INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)"; \
	if [ -e /opt/intelfortran/bin/ifc -a `uname` = "Linux" ]; \
	then \
	  echo Building aedxamine with intel fortran compiler.; \
	  . /opt/intelfortran/bin/ifcvars.sh; \
	  PATH=$$PATH:/opt/intelfortran/bin; \
	  LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:/opt/intelfortran/lib; \
	  export PATH LD_LIBRARY_PATH; \
	  make aedxamine.o OS=$(OS) INSTDIR=$(INSTDIR) PROFILE="$(PROFILE)"; \
	  ar r libXamine.a aedxamine.o; \
	fi; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd Gri; ./configure; make; make install INSTDIR=$(INSTDIR) \
	GRI_DIRECTORY_LIBRARY=$(INSTDIR)/Etc)
	(cd Exception; \
	make  $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd Xamine;    make   INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd Tape; \
	 make  $($(OS)MAKESW) OS=$(OS) INSTDIR=$(INSTDIR) PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd EventSource; \
	 make  $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd TCL; \
	 make  $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS); PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd Events; \
	 make  $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS); PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd Gates; \
	 make  $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd Sorter;   \
	 make  $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd SpectrumIO; \
	 make  $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd SpecTcl;  \
	 make  $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)"; \
	make install $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")
	(cd TestFiles; \
	 make $($(OS)MAKESW) INSTDIR=$(INSTDIR) OS=$(OS) PROFILE="$(PROFILE)")

clean: 
	(cd Display;   make clean)
	(cd Gri;       make clean)
	(cd Xamine;    make clean)
	(cd Exception; make clean)
	(cd Events;    make clean)
	(cd TCL;       make clean)
	(cd Tape;      make clean)
	(cd Sorter;    make clean)
	(cd EventSource; make clean)
	(cd SpecTcl; make clean)
	(cd Gates;   make clean)
	(cd TestFiles; make clean)
	(cd SpectrumIO; make clean)


depend:
	(cd Display;   make depend)
	(cd Gri;       make depend)
	(cd Xamine;    make depend)
	(cd Exception; make depend)
	(cd Events;    make depend)
	(cd TCL;       make depend)
	(cd Tape;      make depend)
	(cd Sorter;    make depend)
	(cd SpectrumIO; make depend)
	(cd EventSource; make depend)
	(cd SpecTcl; make depend)
	(cd Gates; make depend)
	(cd TestFiles; make depend)
