#
#  Makefile for the Event source subsystem.
#  targets are:
#
#    system(Default)      - Build new library in current directory.
#    install              - Install library and headers in $(INSTDIR)
#    clean                - Get rid of .o .a files.
#    test                 - Build any test software which may happen to exist
#                           NOTE: Builds are done against lib in current dir.
# Change Log:
#    July 12, 1999 Ron Fox
#           Added the PipeFile data source to the build.
#



OBJECTS=File.o GaussianDistribution.o TapeException.o TapeFile.o TestFile.o \
	PipeFile.o

INCLUDEDIRS= -I. -I$(INSTDIR)/Include -I/usr/local/include

LIBDIRS=-L/usr/local -L. -L$(INSTDIR)/Lib
ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS)
RM=rm -f
DEPEND=makedepend
INSTALL=install -m 0664
ARCHIVE=ar cr


system: libEventSource.a
	$(ECHO) Library rebuild completed.

install:
	$(INSTALL) *.h              $(INSTDIR)/Include
	$(INSTALL) libEventSource.a $(INSTDIR)/Lib

clean:
	$(RM) $(OBJECTS)
	$(RM) libEventSource.a

depend:
	$(DEPEND) -Y. *.cpp

test:
	$(ECHO) No tests.

libEventSource.a: $(OBJECTS)
	$(ARCHIVE) libEventSource.a $(OBJECTS)
	ranlib libEventSource.a

File.o: File.cpp
	$(COMPILE) File.cpp

GaussianDistribution.o: GaussianDistribution.cpp
	$(COMPILE) GaussianDistribution.cpp

TapeException.o: TapeException.cpp
	$(COMPILE) TapeException.cpp

TapeFile.o: TapeFile.cpp
	$(COMPILE) TapeFile.cpp

TestFile.o: TestFile.cpp
	$(COMPILE) TestFile.cpp

PipeFile.o: PipeFile.cpp
	$(COMPILE) PipeFile.cpp

