SOURCEHOME=/scratch/fox/SpecTcl/2.2
TCLHEADER=/usr/include/tcl8.3

CXX=g++-2.95
CXXFLAGS= -c -ggdb -I. -I.. -I$(SOURCEHOME)/../2.2test/include \
	-I$(TCLHEADER)



LDFLAGS= -L$(SOURCEHOME)/../2.2test/lib -L/usr/X11R6/lib \
	-lTclGrammerCommands \
	-lSorting -lXplus -lGates -lAnalysis -lEventSource -lSpecio \
	-ltclPlus -lException \
	-lXamine -ltape -lFilter \
	 -Wl,"-rpath=$(SOURCEHOME)/../2.2test/lib" \
	-ltk -ltcl -lXt -lX11 -lm

.cpp.o:
	$(CXX) $(CXXFLAGS) $<

TESTTARGETS= ../CConfigurableObject.o ../CSegmentUnpacker.o \
	../CConfigurationParameter.o ../CIntConfigParam.o ../CBoolConfigParam.o \
	../CStringConfigParam.o ../CIntArrayParam.o \
	../CStringArrayparam.o ../CPacket.o ../CModuleDictionary.o ../CModule.o \
	../CModuleCommand.o ../CPacketCreator.o ../CModuleCreator.o \
	../CScriptableUnpacker.o ../CCAENDigitizerCreator.o \
	../CCAENV830Creator.o ../CCAENDigitizerModule.o \
	../CCAENV830Module.o

TESTS=     SegUnpackerTest.o ModuleTest.o PacketTest.o CreatorTest.o \
	UnpackerTest.o

tests: TestRunner.o $(TESTS) $(TESTTARGETS)
	$(CXX) -o tests TestRunner.o $(TESTS) $(TESTTARGETS) -lcppunit $(LDFLAGS)

depend:
	makedepend *.cpp


clean:
	rm -f *.o tests


test: tests
	./tests