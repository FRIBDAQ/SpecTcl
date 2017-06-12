
install-exec-local: 
	$(mkinstalldirs) @prefix@/share @prefix@/share/analysis
	$(mkinstalldirs) @prefix@/share/images
	$(mkinstalldirs) @prefix@/share/UserGuide
	$(mkinstalldirs) @prefix@/share/cmdref
	$(INSTALL_DATA) @builddir@/*.html @prefix@/share/analysis
	cat @srcdir@/images.tar.gz | (cd @prefix@/share/images; tar xzf -)
	$(INSTALL_DATA) @builddir@/userguide/* @srcdir@/userguideimages/* @prefix@/share/UserGuide
	$(INSTALL_DATA) @builddir@/cmdref/* @srcdir@/cmdrefimages/*  @prefix@/share/cmdref

EXTRA_DIST = analysis.xml UserGuide.xml CommandReference.xml images.tar.gz userguideimages/* \
	cmdrefimages/*

BUILT_SOURCES = @builddir@/docs_built

@builddir@/docs_built: @srcdir@/analysis.xml @srcdir@/UserGuide.xml @srcdir@/CommandReference.xml
	@HTMLDOCBOOK@ @srcdir@/analysis.xml -o @builddir@
	@HTMLDOCBOOK@ @srcdir@/UserGuide.xml -o @builddir@/userguide
	@HTMLDOCBOOK@ @srcdir@/CommandReference.xml -o @builddir@/cmdref
	touch @builddir@/docs_built



