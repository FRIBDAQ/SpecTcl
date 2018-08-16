
install-exec-local: 
	$(mkinstalldirs) @prefix@/share @prefix@/share/analysis
	$(mkinstalldirs) @prefix@/share/images
	$(mkinstalldirs) @prefix@/share/UserGuide
	$(mkinstalldirs) @prefix@/share/cmdref
	$(mkinstalldirs) @prefix@/share/pgmguide
	$(mkinstalldirs) @prefix@/share/pgmref
	$(mkinstalldirs) @prefix@/share/DDAS
	$(INSTALL_DATA) @builddir@/*.html @prefix@/share/analysis
	cat @srcdir@/images.tar.gz | (cd @prefix@/share/images; tar xzf -)
	$(INSTALL_DATA) @builddir@/userguide/* @srcdir@/userguideimages/* @prefix@/share/UserGuide
	$(INSTALL_DATA) @builddir@/cmdref/* @srcdir@/cmdrefimages/*  @prefix@/share/cmdref
	$(INSTALL_DATA) @builddir@/pgmguide/* @srcdir@/pgmguideimages/* @prefix@/share/pgmguide
	$(INSTALL_DATA) @builddir@/pgmref/*   @prefix@/share/pgmref
	$(INSTALL_DATA) @builddir@/DDAS/*      @prefix@/share/DDAS


EXTRA_DIST = analysis.xml UserGuide.xml CommandReference.xml images.tar.gz userguideimages \
	cmdrefimages ProgrammingGuide.xml pgmguideimages pgmref.xml DDAS.xml

BUILT_SOURCES = @builddir@/docs_built

@builddir@/docs_built: @srcdir@/analysis.xml @srcdir@/UserGuide.xml \
	@srcdir@/CommandReference.xml @srcdir@/ProgrammingGuide.xml
	@HTMLDOCBOOK@ @srcdir@/analysis.xml -o @builddir@
	@HTMLDOCBOOK@ @srcdir@/UserGuide.xml -o @builddir@/userguide
	@HTMLDOCBOOK@ @srcdir@/CommandReference.xml -o @builddir@/cmdref
	@HTMLDOCBOOK@ @srcdir@/ProgrammingGuide.xml -o @builddir@/pgmguide
	@HTMLDOCBOOK@ @srcdir@/pgmref.xml           -o @builddir@/pgmref
	@HTMLDOCBOOK@ @srcdir@/DDAS.xml		    -o @builddir@/DDAS


