
install-exec-local: docs_built
	$(mkinstalldirs) @prefix@/share @prefix@/share/analysis
	$(mkinstalldirs) @prefix@/share/images
	$(INSTALL_DATA) @builddir@/*.html @prefix@/share/analysis
	cat @srcdir@/images.tar.gz | (cd @prefix@/share/images; tar xzf -)


EXTRA_DIST = analysis.xml images.tar.gz

docs_built: analysis.xml
	@HTMLDOCBOOK@ $<
	touch docs_built


