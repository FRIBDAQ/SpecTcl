clean-local:
	make clean -f Makefile.sip 

install-exec-hook: 
	$(mkinstalldirs) @prefix@/QtGUI
	rm -f @prefix@/QtGUI/CPyConverter.so	
	python3 @srcdir@/configure.py @srcdir@ @libdir@ && make -f Makefile.sip
	cp CPyConverter.so @libdir@/CPyConverter.so

EXTRA_DIST = configure.py Makefile.sip CPyConverter.sip CPyConverter.h dataAccess.h dataTypes.h
