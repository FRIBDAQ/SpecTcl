OBJECTS=EventFilter.o GatedEventFilter.o OutputEventStream.o
INCLUDEDIRS= -I. -I$(INSTDIR)/Include -I/usr/local/include
LIBDIRS=-L/usr/local -L. -L$(INSTDIR)/Lib
ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS)
RM=rm -f
DEPEND=makedepend
INSTALL=install -m 0664
ARCHIVE=ar cr

system: libEventFilter.a
	$(ECHO) Library rebuild completed.

install:
	$(INSTALL) *.h              $(INSTDIR)/Include
	$(INSTALL) libEventFilter.a $(INSTDIR)/Lib

clean:
	$(RM) $(OBJECTS)
	$(RM) libEventFilter.a

depend:
	$(DEPEND) -Y. *.cpp

test:
	$(ECHO) No tests.

libEventFilter.a: $(OBJECTS)
	$(ARCHIVE) libEventFilter.a $(OBJECTS)
	ranlib libEventFilter.a

EventFilter.o: EventFilter.cpp
	$(COMPILE) EventFilter.cpp

GatedEventFilter.o: GatedEventFilter.cpp
	$(COMPILE) GatedEventFilter.cpp

OutputEventStream.o: OutputEventStream.cpp
	$(COMPILE) OutputEventStream.cpp
