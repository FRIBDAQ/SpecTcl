OBJECTS=EventFilter.o FilterDictionary.o GatedEventFilter.o OutputEventStream.o
INCLUDEDIRS= -I. -I$(INSTDIR)/Include -I/usr/local/include
LIBDIRS=-L/usr/local -L. -L$(INSTDIR)/Lib
ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS)
RM=rm -f
DEPEND=makedepend
INSTALL=install -m 0664
ARCHIVE=ar cr

system: libFilter.a
	$(ECHO) Library rebuild completed.

install:
	$(INSTALL) *.h              $(INSTDIR)/Include
	$(INSTALL) libFilter.a $(INSTDIR)/Lib

clean:
	$(RM) $(OBJECTS)
	$(RM) libFilter.a

depend:
	$(DEPEND) -Y. *.cpp

test:
	$(ECHO) No tests.

libFilter.a: $(OBJECTS)
	$(ARCHIVE) libFilter.a $(OBJECTS)
	ranlib libFilter.a

EventFilter.o: EventFilter.cpp
	$(COMPILE) EventFilter.cpp

FilterDictionary.o: FilterDictionary.cpp
	$(COMPILE) FilterDictionary.cpp

GatedEventFilter.o: GatedEventFilter.cpp
	$(COMPILE) GatedEventFilter.cpp

OutputEventStream.o: OutputEventStream.cpp
	$(COMPILE) OutputEventStream.cpp
