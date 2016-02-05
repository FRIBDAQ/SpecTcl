

EXTRA_DIST	=	CCreator.h		\
			CExtensibleFactory.h

install-data-local:
	$(INSTALL_DATA) CCreator.h CExtensibleFactory.h @prefix@/include

install-exec-local:
	install -d -m 0755 $(prefix)/include
	install -m 0744 *.h $(prefix)/include
