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

File.o: File.h /usr/include/errno.h /usr/include/features.h
File.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
File.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
File.o: /usr/include/asm/errno.h /usr/include/fcntl.h
File.o: /usr/include/bits/fcntl.h /usr/include/sys/types.h
File.o: /usr/include/bits/types.h /usr/include/stddef.h /usr/include/time.h
File.o: /usr/include/endian.h /usr/include/bits/endian.h
File.o: /usr/include/sys/select.h /usr/include/bits/select.h
File.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
File.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
File.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
File.o: /usr/include/bits/confname.h /usr/include/getopt.h
GaussianDistribution.o: GaussianDistribution.h Distribution.h
GaussianDistribution.o: /usr/include/stdlib.h /usr/include/features.h
GaussianDistribution.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GaussianDistribution.o: /usr/include/stddef.h /usr/include/sys/types.h
GaussianDistribution.o: /usr/include/bits/types.h /usr/include/time.h
GaussianDistribution.o: /usr/include/endian.h /usr/include/bits/endian.h
GaussianDistribution.o: /usr/include/sys/select.h /usr/include/bits/select.h
GaussianDistribution.o: /usr/include/bits/sigset.h
GaussianDistribution.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GaussianDistribution.o: /usr/include/math.h /usr/include/bits/huge_val.h
GaussianDistribution.o: /usr/include/bits/mathdef.h
GaussianDistribution.o: /usr/include/bits/mathcalls.h /usr/include/string.h
PipeFile.o: PipeFile.h File.h /usr/include/unistd.h /usr/include/features.h
PipeFile.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
PipeFile.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
PipeFile.o: /usr/include/stddef.h /usr/include/bits/confname.h
PipeFile.o: /usr/include/getopt.h /usr/include/stdio.h /usr/include/stdarg.h
PipeFile.o: /usr/include/libio.h /usr/include/_G_config.h
PipeFile.o: /usr/include/bits/stdio_lim.h /usr/include/signal.h
PipeFile.o: /usr/include/bits/sigset.h /usr/include/bits/signum.h
PipeFile.o: /usr/include/time.h /usr/include/bits/siginfo.h
PipeFile.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
PipeFile.o: /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h
PipeFile.o: /usr/include/errno.h /usr/include/bits/errno.h
PipeFile.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
PipeFile.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
PipeFile.o: /usr/include/sys/types.h /usr/include/endian.h
PipeFile.o: /usr/include/bits/endian.h /usr/include/sys/select.h
PipeFile.o: /usr/include/bits/select.h /usr/include/sys/sysmacros.h
PipeFile.o: /usr/include/sys/wait.h /usr/include/bits/waitflags.h
PipeFile.o: /usr/include/bits/waitstatus.h /usr/include/sys/time.h
PipeFile.o: /usr/include/bits/time.h /usr/include/string.h
TapeException.o: TapeException.h
TapeFile.o: TapeFile.h File.h /usr/include/errno.h /usr/include/features.h
TapeFile.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TapeFile.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
TapeFile.o: /usr/include/asm/errno.h TapeException.h /usr/include/unistd.h
TapeFile.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
TapeFile.o: /usr/include/stddef.h /usr/include/bits/confname.h
TapeFile.o: /usr/include/getopt.h /usr/include/fcntl.h
TapeFile.o: /usr/include/bits/fcntl.h /usr/include/sys/types.h
TapeFile.o: /usr/include/time.h /usr/include/endian.h
TapeFile.o: /usr/include/bits/endian.h /usr/include/sys/select.h
TapeFile.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
TapeFile.o: /usr/include/sys/sysmacros.h /usr/include/sys/stat.h
TapeFile.o: /usr/include/bits/stat.h /usr/include/assert.h
TapeFile.o: /usr/include/string.h
TestFile.o: TestFile.h File.h Distribution.h /usr/include/errno.h
TestFile.o: /usr/include/features.h /usr/include/sys/cdefs.h
TestFile.o: /usr/include/gnu/stubs.h /usr/include/bits/errno.h
TestFile.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
