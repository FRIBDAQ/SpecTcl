
install-exec-local: 
	$(mkinstalldirs) @prefix@/share @prefix@/share/analysis
	$(mkinstalldirs) @prefix@/share/images
	$(INSTALL_DATA) @builddir@/*.html @prefix@/share/analysis
	cat @srcdir@/images.tar.gz | (cd @prefix@/share/images; tar xzf -)


EXTRA_DIST = analysis.xml images.tar.gz

BUILT_SOURCES = @builddir@/docs_built

@builddir@/docs_built: @srcdir@/analysis.xml
	@HTMLDOCBOOK@ $< -o @builddir@
	touch @builddir@/docs_built


