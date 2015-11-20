ACLOCAL_AMFLAGS=-I m4

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
		$(SHELL) ./config.status libtool

SUBDIRS= exception tclplus

check-TESTS:
	- for d in $(SUBDIRS); do (cd $$d; make check-TESTS); done