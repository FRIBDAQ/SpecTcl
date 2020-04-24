INSTDIR=/usr/opt/spectcl/5.3-007

all:
	@$(MAKE) -C src 

.PHONY: clean

clean:
	make clean -C src 

install:
	if [ -d "$(INSTDIR)/QtGUI" ]; then echo "QtGUI exists"; else mkdir $(INSTDIR)/QtGUI; fi
	cp src/*.cpp $(INSTDIR)/QtGUI
	cp src/*.h $(INSTDIR)/QtGUI
	cp src/*.py $(INSTDIR)/QtGUI	
	cp src/Makefile $(INSTDIR)/QtGUI
	if [ -f $(INSTDIR)/QtGUI/libCPyConverter.so ] ; then echo "libCPyConverter.so exists"; else ln -s $(INSTDIR)/lib/libCPyConverter.so $(INSTDIR)/QtGUI; fi
	if [ -f $(INSTDIR)/QtGUI/CPyConverter.so ] ; then echo "CPyConverter.so exists"; else ln -s $(INSTDIR)/lib/CPyConverter.so $(INSTDIR)/QtGUI; fi

