INSTDIR=/user/e17011/.giordano/SpecTcl6.master

all:
	@$(MAKE) -C src -f Makefile.mod

.PHONY: clean

clean:
	make clean -C src -f Makefile.mod

install:
	cp src/libCPyConverter.so $(INSTDIR)/lib
	cp src/CPyConverter.so $(INSTDIR)/lib
