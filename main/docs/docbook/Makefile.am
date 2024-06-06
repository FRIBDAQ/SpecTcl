
install-exec-local: 
	$(mkinstalldirs) @datarootdir@ @datarootdir@/analysis
	$(mkinstalldirs) @datarootdir@/images
	$(mkinstalldirs) @datarootdir@/UserGuide
	$(mkinstalldirs) @datarootdir@/cmdref
	$(mkinstalldirs) @datarootdir@/pgmguide
	$(mkinstalldirs) @datarootdir@/pgmref
	$(mkinstalldirs) @datarootdir@/DDAS
	$(mkinstalldirs) @datarootdir@/parallel
	$(mkinstalldirs) @datarootdir@/qtpy
	$(mkinstalldirs) @datarootdir@/batch
	$(mkinstalldirs) @datarootdir@/caendpp
	$(mkinstalldirs) @datarootdir@/examples
	$(mkinstalldirs) @datarootdir@/examples/caendpp
	$(mkinstalldirs) @datarootdir@/remote
	$(mkinstalldirs) @datarootdir@/mpispectcl
	$(INSTALL_DATA) @builddir@/*.html @datarootdir@/analysis
	cat @srcdir@/images.tar.gz | (cd @datarootdir@/images; tar xzf -)
	$(INSTALL_DATA) @builddir@/userguide/* @srcdir@/userguideimages/* @datarootdir@/UserGuide
	$(INSTALL_DATA) @builddir@/cmdref/* @srcdir@/cmdrefimages/*  @datarootdir@/cmdref
	$(INSTALL_DATA) @builddir@/pgmguide/* @srcdir@/pgmguideimages/* @datarootdir@/pgmguide
	$(INSTALL_DATA) @builddir@/pgmref/*   @datarootdir@/pgmref
	$(INSTALL_DATA) @builddir@/parallel/*      @datarootdir@/parallel
	$(INSTALL_DATA) @builddir@/qtpy/*      @datarootdir@/qtpy
	$(INSTALL_DATA) @builddir@/DDAS/*      @datarootdir@/DDAS
	$(INSTALL_DATA) @builddir@/batch/*    @datarootdir@/batch
	$(INSTALL_DATA) @builddir@/caendpp/*  @datarootdir@/caendpp
	$(INSTALL_DATA) @srcdir@/dppexample/*.cpp @datarootdir@/examples/caendpp
	$(INSTALL_DATA) @srcdir@/dppexample/*.h @datarootdir@/examples/caendpp
	$(INSTALL_DATA) @srcdir@/dppexample/Makefile @datarootdir@/examples/caendpp
	$(INSTALL_DATA) @builddir@/remote/*   @datarootdir@/remote
	$(INSTALL_DATA) @builddir@/mpispectcl/* @datarootdir@/mpispectcl



EXTRA_DIST = analysis.xml UserGuide.xml CommandReference.xml images.tar.gz userguideimages \
	cmdrefimages ProgrammingGuide.xml pgmguideimages pgmref.xml DDAS.xml \
	 parallel.xml qtpy.xml batch.xml CAENDPP.xml dppexample remote.xml mpiSpecTcl.xml

BUILT_SOURCES = @builddir@/docs_built

@builddir@/docs_built: @srcdir@/analysis.xml @srcdir@/UserGuide.xml \
	@srcdir@/CommandReference.xml @srcdir@/ProgrammingGuide.xml
	@HTMLDOCBOOK@ @srcdir@/analysis.xml -o @builddir@
	@HTMLDOCBOOK@ @srcdir@/UserGuide.xml -o @builddir@/userguide
	@HTMLDOCBOOK@ @srcdir@/CommandReference.xml -o @builddir@/cmdref
	@HTMLDOCBOOK@ @srcdir@/ProgrammingGuide.xml -o @builddir@/pgmguide
	@HTMLDOCBOOK@ @srcdir@/pgmref.xml           -o @builddir@/pgmref
	@HTMLDOCBOOK@ @srcdir@/DDAS.xml		    -o @builddir@/DDAS
	@HTMLDOCBOOK@ @srcdir@/parallel.xml	    -o @builddir@/parallel
	@HTMLDOCBOOK@ @srcdir@/qtpy.xml	            -o @builddir@/qtpy
	@HTMLDOCBOOK@ @srcdir@/batch.xml	    -o @builddir@/batch
	@HTMLDOCBOOK@ @srcdir@/CAENDPP.xml          -o @builddir@/caendpp
	@HTMLDOCBOOK@ @srcdir@/remote.xml           -o @builddir@/remote
	@HTMLDOCBOOK@ @srcdir@/mpiSpecTcl.xml       -o @builddir@/mpispectcl
