
#
#  Makefile for the C++ interface to Tcl/TK
#  The following targets are supported:
#      system(default)         - Build the library.
#      install                 - Install the system at $(INSTDIR)
#      clean                   - Clean up .o and .a files.
#      tests                   - Build any test programs against local version
#                                of the library.

LinuxLIBS=-ldl
HPUXLIBDIR = -L/opt/tcltk/lib
HPUXINCS = -I/opt/tcltk/include
DarwinINCS = -I/usr/local/include
CYGWININCS=-I/usr/local/tcl/include
CYGWINLIBDIR=-L/usr/local/tcl/lib

INCLUDEDIRS= $($(OS)INCS) -I.   -I$(INSTDIR)/Include -I/usr/local/include  \
		 -I../SpecTcl

OBJECTS=SpectrumFormatter.o \
	SpectrumFormatterFactory.o \
	SpectrumStandardFormatters.o \
	NSCLBinarySpectrumFormatter.o \
	NSCLAsciiSpectrumFormatter.o  \
	SpectrumFormatError.o \
	nsclbin.o nsclbinerror.o nscldata.o

LIBRARY=libSpecio.a

LIBDIRS=$($(OS)LIBDIR) -L/usr/local -L. -L$(INSTDIR)/Lib
LIBS=   -ltclPlus -lException -ltk -ltcl -lXt -lX11 $($(OS)LIBS)


PROFILE=-g3 -pg
NOPROFILE=-g
ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"'
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr


system: $(LIBRARY)
	$(ECHO) Library rebuild complete.

clean: 
	$(RM) $(OBJECTS) 
	$(RM) $(LIBRARY)

depend:
	makedepend $(INCLUDEDIRS) *.cpp

install:
	$(INSTALL) $(LIBRARY)   $(INSTDIR)/Lib
	$(INSTALL) *.h          $(INSTDIR)/Include

tests: 
	$(ECHO) Tests rebuilt.

$(LIBRARY): $(OBJECTS)
	$(ARCHIVE) $(LIBRARY)       $(OBJECTS)

SpectrumFormatter.o: SpectrumFormatter.cpp
	$(COMPILE) SpectrumFormatter.cpp

SpectrumFormatterFactory.o: SpectrumFormatterFactory.cpp
	$(COMPILE) SpectrumFormatterFactory.cpp

SpectrumStandardFormatters.o: SpectrumStandardFormatters.cpp
	$(COMPILE) SpectrumStandardFormatters.cpp

NSCLBinarySpectrumFormatter.o: NSCLBinarySpectrumFormatter.cpp
	$(COMPILE) NSCLBinarySpectrumFormatter.cpp

NSCLAsciiSpectrumFormatter.o: NSCLAsciiSpectrumFormatter.cpp
	$(COMPILE) NSCLAsciiSpectrumFormatter.cpp

SpectrumFormatError.o: SpectrumFormatError.cpp
	$(COMPILE) SpectrumFormatError.cpp

nsclbin.o: nsclbin.cpp
	$(COMPILE) nsclbin.cpp

nsclbinerror.o: nsclbinerror.cpp
	$(COMPILE) nsclbinerror.cpp

nscldata.o: nscldata.cpp
	$(COMPILE) nscldata.cpp

# DO NOT DELETE

NSCLAsciiSpectrumFormatter.o: NSCLAsciiSpectrumFormatter.h
NSCLAsciiSpectrumFormatter.o: /Include/histotypes.h /usr/include/string.h
NSCLAsciiSpectrumFormatter.o: /usr/include/features.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/cdefs.h
NSCLAsciiSpectrumFormatter.o: /usr/include/gnu/stubs.h
NSCLAsciiSpectrumFormatter.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
NSCLAsciiSpectrumFormatter.o: /usr/include/math.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/huge_val.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/mathdef.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/mathcalls.h
NSCLAsciiSpectrumFormatter.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
NSCLAsciiSpectrumFormatter.o: SpectrumFormatter.h
NSCLAsciiSpectrumFormatter.o: ../SpecTcl/SpectrumPackage.h
NSCLAsciiSpectrumFormatter.o: ./SpectrumFormatError.h /Include/Exception.h
NSCLAsciiSpectrumFormatter.o: /Include/StreamIOError.h /usr/include/limits.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/posix1_lim.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/local_lim.h
NSCLAsciiSpectrumFormatter.o: /usr/include/linux/limits.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/posix2_lim.h
NSCLAsciiSpectrumFormatter.o: /usr/include/time.h /usr/include/bits/time.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/types.h /usr/include/ctype.h
NSCLAsciiSpectrumFormatter.o: /usr/include/endian.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/endian.h
NSCLBinarySpectrumFormatter.o: NSCLBinarySpectrumFormatter.h
NSCLBinarySpectrumFormatter.o: SpectrumFormatter.h ./nsclbin.h
NSCLBinarySpectrumFormatter.o: /usr/include/string.h /usr/include/features.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/cdefs.h
NSCLBinarySpectrumFormatter.o: /usr/include/gnu/stubs.h
NSCLBinarySpectrumFormatter.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
NSCLBinarySpectrumFormatter.o: /usr/include/errno.h /usr/include/bits/errno.h
NSCLBinarySpectrumFormatter.o: /usr/include/linux/errno.h
NSCLBinarySpectrumFormatter.o: /usr/include/asm/errno.h ./nscldata.h
NSCLBinarySpectrumFormatter.o: ./nsclbinerror.h /Include/Exception.h
NSCLBinarySpectrumFormatter.o: ../SpecTcl/SpectrumPackage.h
NSCLBinarySpectrumFormatter.o: ./SpectrumFormatError.h
NSCLBinarySpectrumFormatter.o: /Include/StreamIOError.h /usr/include/math.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/huge_val.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/mathdef.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/mathcalls.h
NSCLBinarySpectrumFormatter.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
NSCLBinarySpectrumFormatter.o: /usr/include/limits.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/posix1_lim.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/local_lim.h
NSCLBinarySpectrumFormatter.o: /usr/include/linux/limits.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/posix2_lim.h
NSCLBinarySpectrumFormatter.o: /usr/include/ctype.h /usr/include/bits/types.h
NSCLBinarySpectrumFormatter.o: /usr/include/endian.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/endian.h
SpectrumFormatError.o: ./SpectrumFormatError.h /Include/Exception.h
SpectrumFormatter.o: SpectrumFormatter.h
SpectrumFormatterFactory.o: SpectrumFormatterFactory.h SpectrumFormatter.h
SpectrumFormatterFactory.o: /Include/histotypes.h /usr/include/string.h
SpectrumFormatterFactory.o: /usr/include/features.h /usr/include/sys/cdefs.h
SpectrumFormatterFactory.o: /usr/include/gnu/stubs.h
SpectrumFormatterFactory.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
SpectrumFormatterFactory.o: /usr/include/math.h /usr/include/bits/huge_val.h
SpectrumFormatterFactory.o: /usr/include/bits/mathdef.h
SpectrumFormatterFactory.o: /usr/include/bits/mathcalls.h
SpectrumFormatterFactory.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
SpectrumStandardFormatters.o: SpectrumFormatterFactory.h SpectrumFormatter.h
SpectrumStandardFormatters.o: /Include/histotypes.h /usr/include/string.h
SpectrumStandardFormatters.o: /usr/include/features.h
SpectrumStandardFormatters.o: /usr/include/sys/cdefs.h
SpectrumStandardFormatters.o: /usr/include/gnu/stubs.h
SpectrumStandardFormatters.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
SpectrumStandardFormatters.o: /usr/include/math.h
SpectrumStandardFormatters.o: /usr/include/bits/huge_val.h
SpectrumStandardFormatters.o: /usr/include/bits/mathdef.h
SpectrumStandardFormatters.o: /usr/include/bits/mathcalls.h
SpectrumStandardFormatters.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
SpectrumStandardFormatters.o: NSCLAsciiSpectrumFormatter.h
SpectrumStandardFormatters.o: NSCLBinarySpectrumFormatter.h ./nsclbin.h
SpectrumStandardFormatters.o: /usr/include/errno.h /usr/include/bits/errno.h
SpectrumStandardFormatters.o: /usr/include/linux/errno.h
SpectrumStandardFormatters.o: /usr/include/asm/errno.h ./nscldata.h
SpectrumStandardFormatters.o: ./nsclbinerror.h
nsclbin.o: /usr/include/string.h /usr/include/features.h
nsclbin.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
nsclbin.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
nsclbin.o: /usr/include/errno.h /usr/include/bits/errno.h
nsclbin.o: /usr/include/linux/errno.h /usr/include/asm/errno.h ./nscldata.h
nsclbin.o: ./nsclbinerror.h ./nscltime.h /usr/include/time.h
nsclbin.o: /usr/include/bits/time.h /usr/include/bits/types.h ./nsclbin.h
nsclbin.o: /usr/include/math.h /usr/include/bits/huge_val.h
nsclbin.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
nsclbin.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
nsclbinbad.o: /usr/include/string.h /usr/include/features.h
nsclbinbad.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
nsclbinbad.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
nsclbinbad.o: /usr/include/errno.h /usr/include/bits/errno.h
nsclbinbad.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
nsclbinbad.o: ./nscldata.h ./nsclbinerror.h ./nscltime.h /usr/include/time.h
nsclbinbad.o: /usr/include/bits/time.h /usr/include/bits/types.h ./nsclbin.h
nsclbinbad.o: /usr/include/math.h /usr/include/bits/huge_val.h
nsclbinbad.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
nsclbinbad.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
nsclbinerror.o: ./nsclbinerror.h
nscldata.o: /usr/include/errno.h /usr/include/features.h
nscldata.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
nscldata.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
nscldata.o: /usr/include/asm/errno.h /usr/include/string.h
nscldata.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h ./nscldata.h
nscldata.o: ./nsclbinerror.h
