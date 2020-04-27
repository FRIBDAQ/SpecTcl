all-local:
	python3 @srcdir@/configure.py && make -f Makefile.sip

install-exec-hook: @builddir@/CPyConverter.so
	$(mkinstalldirs) @prefix@/lib
	rm -f @prefix@/QtGUI/CPyConverter.so
	$(LN_S) @libdir@/CPyConverter.so @prefix@/QtGUI

