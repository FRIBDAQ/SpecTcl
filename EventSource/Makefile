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
COMPILE=g++ -c $(PROFILE) $(INCLUDEDIRS) -D$(OS)
LINK=g++ $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) -D$(OS)
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

File.o: File.h /usr/include/errno.h /usr/include/standards.h
File.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
File.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
File.o: /usr/include/sys/select.h /usr/include/sys/seek.h
File.o: /usr/include/sys/mode.h /usr/include/sys/stat.h /usr/include/unistd.h
File.o: /usr/include/sys/access.h /usr/include/getopt.h
GaussianDistribution.o: GaussianDistribution.h Distribution.h
GaussianDistribution.o: /usr/include/stdlib.h /usr/include/standards.h
GaussianDistribution.o: /usr/include/getopt.h /usr/include/sys/types.h
GaussianDistribution.o: /usr/include/mach/machine/vm_types.h
GaussianDistribution.o: /usr/include/sys/select.h /usr/include/math.h
GaussianDistribution.o: /usr/include/time.h /usr/include/machine/machtime.h
GaussianDistribution.o: /usr/include/sys/timers.h /usr/include/sys/sysmisc.h
GaussianDistribution.o: /usr/include/string.h /usr/include/strings.h
PipeFile.o: PipeFile.h File.h /usr/include/unistd.h /usr/include/standards.h
PipeFile.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
PipeFile.o: /usr/include/sys/select.h /usr/include/sys/access.h
PipeFile.o: /usr/include/sys/seek.h /usr/include/getopt.h
PipeFile.o: /usr/include/stdio.h /usr/include/va_list.h
PipeFile.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
PipeFile.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
PipeFile.o: /usr/include/sys/rt_limits.h /usr/include/signal.h
PipeFile.o: /usr/include/sys/siginfo.h /usr/include/sys/sysmisc.h
PipeFile.o: /usr/include/machine/signal.h /usr/include/machine/context.h
PipeFile.o: /usr/include/sys/procset.h /usr/include/errno.h
PipeFile.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
PipeFile.o: /usr/include/sys/mode.h /usr/include/sys/wait.h
PipeFile.o: /usr/include/sys/param.h /usr/include/sys/machine/machparam.h
PipeFile.o: /usr/include/machine/endian.h /usr/include/sys/signal.h
PipeFile.o: /usr/include/sys/time.h /usr/include/time.h
PipeFile.o: /usr/include/sys/timers.h /usr/include/string.h
PipeFile.o: /usr/include/strings.h
TapeException.o: TapeException.h
TapeFile.o: TapeFile.h File.h /usr/include/errno.h /usr/include/standards.h
TapeFile.o: TapeException.h /usr/include/unistd.h /usr/include/sys/types.h
TapeFile.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
TapeFile.o: /usr/include/sys/access.h /usr/include/sys/seek.h
TapeFile.o: /usr/include/getopt.h /usr/include/fcntl.h
TapeFile.o: /usr/include/sys/fcntl.h /usr/include/sys/mode.h
TapeFile.o: /usr/include/sys/stat.h /usr/include/assert.h
TapeFile.o: /usr/include/string.h /usr/include/strings.h
TestFile.o: TestFile.h File.h Distribution.h /usr/include/errno.h
TestFile.o: /usr/include/standards.h
