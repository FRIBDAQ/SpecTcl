
install-exec-local: 
	$(mkinstalldirs) @prefix@/share @prefix@/share/analysis
	$(mkinstalldirs) @prefix@/share/images
	$(mkinstalldirs) @prefix@/share/UserGuide
	$(INSTALL_DATA) @builddir@/*.html @prefix@/share/analysis
	cat @srcdir@/images.tar.gz | (cd @prefix@/share/images; tar xzf -)
	$(INSTALL_DATA) @builddir@/userguide/* @srcdir@/userguideimages/* @prefix@/share/UserGuide


EXTRA_DIST = analysis.xml images.tar.gz userguideimages/*

BUILT_SOURCES = @builddir@/docs_built

@builddir@/docs_built: @srcdir@/analysis.xml @srcdir@/UserGuide.xml
	@HTMLDOCBOOK@ @srcdir@/analysis.xml -o @builddir@
	@HTMLDOCBOOK@ @srcdir@/UserGuide.xml -o @builddir@/userguide
	touch @builddir@/docs_built



