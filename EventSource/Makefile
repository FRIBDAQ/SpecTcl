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
INSTALL=cp
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
	$(DEPEND) $(INCLUDEDIRS) *.cpp

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

# DO NOT DELETE

File.o: File.h /usr/include/errno.h /usr/include/sys/errno.h
File.o: /usr/include/sys/reent.h /usr/include/_ansi.h
File.o: /usr/include/sys/config.h /usr/include/sys/_types.h
File.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
File.o: /usr/include/sys/types.h /usr/include/machine/types.h
File.o: /usr/include/sys/stat.h /usr/include/time.h
File.o: /usr/include/machine/time.h /usr/include/sys/features.h
File.o: /usr/include/unistd.h /usr/include/sys/unistd.h /usr/include/getopt.h
GaussianDistribution.o: GaussianDistribution.h Distribution.h
GaussianDistribution.o: /usr/include/stdlib.h /usr/include/_ansi.h
GaussianDistribution.o: /usr/include/sys/config.h /usr/include/sys/reent.h
GaussianDistribution.o: /usr/include/sys/_types.h /usr/include/math.h
GaussianDistribution.o: /usr/include/machine/ieeefp.h /usr/include/time.h
GaussianDistribution.o: /usr/include/machine/time.h /usr/include/sys/types.h
GaussianDistribution.o: /usr/include/machine/types.h
GaussianDistribution.o: /usr/include/sys/features.h /usr/include/string.h
PipeFile.o: PipeFile.h File.h /usr/include/unistd.h /usr/include/sys/unistd.h
PipeFile.o: /usr/include/_ansi.h /usr/include/sys/config.h
PipeFile.o: /usr/include/sys/types.h /usr/include/machine/types.h
PipeFile.o: /usr/include/sys/_types.h /usr/include/sys/features.h
PipeFile.o: /usr/include/getopt.h /usr/include/stdio.h
PipeFile.o: /usr/include/sys/reent.h /usr/include/signal.h
PipeFile.o: /usr/include/sys/signal.h /usr/include/errno.h
PipeFile.o: /usr/include/sys/errno.h /usr/include/fcntl.h
PipeFile.o: /usr/include/sys/fcntl.h /usr/include/sys/stat.h
PipeFile.o: /usr/include/time.h /usr/include/machine/time.h
PipeFile.o: /usr/include/sys/wait.h /usr/include/sys/resource.h
PipeFile.o: /usr/include/sys/time.h /usr/include/string.h
TapeException.o: TapeException.h
TapeFile.o: TapeFile.h File.h /usr/include/errno.h /usr/include/sys/errno.h
TapeFile.o: /usr/include/sys/reent.h /usr/include/_ansi.h
TapeFile.o: /usr/include/sys/config.h /usr/include/sys/_types.h
TapeFile.o: TapeException.h /usr/include/unistd.h /usr/include/sys/unistd.h
TapeFile.o: /usr/include/sys/types.h /usr/include/machine/types.h
TapeFile.o: /usr/include/sys/features.h /usr/include/getopt.h
TapeFile.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
TapeFile.o: /usr/include/sys/stat.h /usr/include/time.h
TapeFile.o: /usr/include/machine/time.h /usr/include/assert.h
TapeFile.o: /usr/include/string.h
TestFile.o: TestFile.h File.h Distribution.h /usr/include/errno.h
TestFile.o: /usr/include/sys/errno.h /usr/include/sys/reent.h
TestFile.o: /usr/include/_ansi.h /usr/include/sys/config.h
TestFile.o: /usr/include/sys/_types.h
