INSTALLEDDIR=/scratch/fox/SpecTcl/2.2test

CXX=g++-2.95
CXXFLAGS= -g -c -ggdb -I.. -I$(INSTALLEDDIR)/include \
	 -I/usr/include/tcl8.4 -D_DEBUG 

LDFLAGS=-L/usr/X11R6/lib -L$(INSTALLEDDIR)/lib \
	-Wl,"-rpath=$(INSTALLEDDIR)/lib" \
	-ltk -ltcl -lXt -lX11 -lgsl -lgslcblas  -lm
.cpp.o:
	$(CXX) $(CXXFLAGS) $<


TESTS=     CommandTest.o FitTest.o CreatorTest.o FactoryTest.o
TARGETS=  ../CFit.o ../CLinearFit.o \
	../CFitCreator.o ../CLinearFitCreator.o \
	../CFitFactory.o ../CFitCommand.o

system: unittests integration

unittests: TestRunner.o $(TESTS) $(TARGETS)
	$(CXX) -o unittests TestRunner.o \
	$(TESTS) \
	$(TARGETS) \
	  -lcppunit -ltclPlus -lException $(LDFLAGS)


integration: $(TARGETS) MySpecTclApp.o
	$(CXX) -o SpecTcl MySpecTclApp.o $(TARGETS) \
	-lTclGrammerCommands \
	-lSorting -lXplus -lGates -lAnalysis -lEventSource -lSpecio \
	-ltclPlus -lException \
	-lXamine -ltape -lFilter \
	$(LDFLAGS)


depend:
	makedepend *.cpp


clean:
	rm -f *.o tests


test: tests
	./tests