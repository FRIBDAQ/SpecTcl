
SUBDIRS=scriptable treeparam Grace multiread

check-TESTS:
	- for d in $(SUBDIRS); do (cd $$d; make check-TESTS); done
