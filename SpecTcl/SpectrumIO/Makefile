#
#  Makefile for the C++ interface to Tcl/TK
#  The following targets are supported:
#      system(default)         - Build the library.
#      install                 - Install the system at $(INSTDIR)
#      clean                   - Clean up .o and .a files.
#      tests                   - Build any test programs against local version
#                                of the library.

LinuxLIBS=-ldl
HPUXLIBS = -L/opt/tcltk/lib
HPUXINCS = -I/opt/tcltk/include

INCLUDEDIRS= -I.   -I$(INSTDIR)/Include -I/usr/local/include  \
		$($(OS)INCS) -I../SpecTcl

OBJECTS=SpectrumFormatter.o \
	SpectrumFormatterFactory.o \
	SpectrumStandardFormatters.o \
	NSCLBinarySpectrumFormatter.o \
	NSCLAsciiSpectrumFormatter.o  \
	SpectrumFormatError.o \
	nsclbin.o nsclbinerror.o nscldata.o

LIBRARY=libSpecio.a

LIBDIRS=-L/usr/local -L. -L$(INSTDIR)/Lib
LIBS=   -ltclPlus -lException -ltk -ltcl -lXt -lX11 $($(OS)LIBS)


PROFILE=-g3 -pg
NOPROFILE=-g
ECHO=echo
COMPILE=g++ -c $(PROFILE) $(INCLUDEDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"'
LINK=g++ $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS)
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
NSCLAsciiSpectrumFormatter.o: SpectrumFormatter.h
NSCLAsciiSpectrumFormatter.o: ../SpecTcl/SpectrumPackage.h
NSCLAsciiSpectrumFormatter.o: ./SpectrumFormatError.h /usr/include/math.h
NSCLAsciiSpectrumFormatter.o: /usr/include/features.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/cdefs.h
NSCLAsciiSpectrumFormatter.o: /usr/include/gnu/stubs.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/huge_val.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/mathdef.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/mathcalls.h
NSCLAsciiSpectrumFormatter.o: /usr/include/limits.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/posix1_lim.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/local_lim.h
NSCLAsciiSpectrumFormatter.o: /usr/include/linux/limits.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/posix2_lim.h
NSCLAsciiSpectrumFormatter.o: /usr/include/time.h /usr/include/stddef.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/time.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/types.h /usr/include/string.h
NSCLAsciiSpectrumFormatter.o: /usr/include/ctype.h /usr/include/endian.h
NSCLAsciiSpectrumFormatter.o: /usr/include/bits/endian.h
NSCLBinarySpectrumFormatter.o: NSCLBinarySpectrumFormatter.h
NSCLBinarySpectrumFormatter.o: SpectrumFormatter.h ./nsclbin.h
NSCLBinarySpectrumFormatter.o: /usr/include/string.h /usr/include/features.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/cdefs.h
NSCLBinarySpectrumFormatter.o: /usr/include/gnu/stubs.h /usr/include/stddef.h
NSCLBinarySpectrumFormatter.o: /usr/include/errno.h /usr/include/bits/errno.h
NSCLBinarySpectrumFormatter.o: /usr/include/linux/errno.h
NSCLBinarySpectrumFormatter.o: /usr/include/asm/errno.h ./nscldata.h
NSCLBinarySpectrumFormatter.o: ./nsclbinerror.h ../SpecTcl/SpectrumPackage.h
NSCLBinarySpectrumFormatter.o: ./SpectrumFormatError.h /usr/include/math.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/huge_val.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/mathdef.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/mathcalls.h
NSCLBinarySpectrumFormatter.o: /usr/include/limits.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/posix1_lim.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/local_lim.h
NSCLBinarySpectrumFormatter.o: /usr/include/linux/limits.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/posix2_lim.h
NSCLBinarySpectrumFormatter.o: /usr/include/ctype.h /usr/include/bits/types.h
NSCLBinarySpectrumFormatter.o: /usr/include/endian.h
NSCLBinarySpectrumFormatter.o: /usr/include/bits/endian.h
SpectrumFormatError.o: ./SpectrumFormatError.h
SpectrumFormatter.o: SpectrumFormatter.h
SpectrumFormatterFactory.o: SpectrumFormatterFactory.h SpectrumFormatter.h
SpectrumStandardFormatters.o: SpectrumFormatterFactory.h SpectrumFormatter.h
SpectrumStandardFormatters.o: NSCLAsciiSpectrumFormatter.h
SpectrumStandardFormatters.o: NSCLBinarySpectrumFormatter.h ./nsclbin.h
SpectrumStandardFormatters.o: /usr/include/string.h /usr/include/features.h
SpectrumStandardFormatters.o: /usr/include/sys/cdefs.h
SpectrumStandardFormatters.o: /usr/include/gnu/stubs.h /usr/include/stddef.h
SpectrumStandardFormatters.o: /usr/include/errno.h /usr/include/bits/errno.h
SpectrumStandardFormatters.o: /usr/include/linux/errno.h
SpectrumStandardFormatters.o: /usr/include/asm/errno.h ./nscldata.h
SpectrumStandardFormatters.o: ./nsclbinerror.h
nsclbin.o: /usr/include/string.h /usr/include/features.h
nsclbin.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
nsclbin.o: /usr/include/stddef.h /usr/include/errno.h
nsclbin.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
nsclbin.o: /usr/include/asm/errno.h ./nscldata.h ./nsclbinerror.h
nsclbin.o: ./nscltime.h /usr/include/time.h /usr/include/bits/time.h
nsclbin.o: /usr/include/bits/types.h ./nsclbin.h /usr/include/math.h
nsclbin.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
nsclbin.o: /usr/include/bits/mathcalls.h
nsclbinerror.o: ./nsclbinerror.h
nscldata.o: /usr/include/errno.h /usr/include/features.h
nscldata.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
nscldata.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
nscldata.o: /usr/include/asm/errno.h /usr/include/string.h
nscldata.o: /usr/include/stddef.h ./nscldata.h ./nsclbinerror.h
