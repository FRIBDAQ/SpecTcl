
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
NSCLAsciiSpectrumFormatter.o: SpectrumFormatter.h
NSCLAsciiSpectrumFormatter.o: ../SpecTcl/SpectrumPackage.h
NSCLAsciiSpectrumFormatter.o: ./SpectrumFormatError.h /usr/include/math.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/reent.h /usr/include/_ansi.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/config.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/_types.h
NSCLAsciiSpectrumFormatter.o: /usr/include/machine/ieeefp.h
NSCLAsciiSpectrumFormatter.o: /usr/include/limits.h /usr/include/time.h
NSCLAsciiSpectrumFormatter.o: /usr/include/machine/time.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/types.h
NSCLAsciiSpectrumFormatter.o: /usr/include/machine/types.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/features.h
NSCLAsciiSpectrumFormatter.o: /usr/include/string.h /usr/include/ctype.h
NSCLBinarySpectrumFormatter.o: NSCLBinarySpectrumFormatter.h
NSCLBinarySpectrumFormatter.o: SpectrumFormatter.h ./nsclbin.h
NSCLBinarySpectrumFormatter.o: /usr/include/string.h /usr/include/_ansi.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/config.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/reent.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/_types.h /usr/include/errno.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/errno.h ./nscldata.h
NSCLBinarySpectrumFormatter.o: ./nsclbinerror.h ../SpecTcl/SpectrumPackage.h
NSCLBinarySpectrumFormatter.o: ./SpectrumFormatError.h /usr/include/math.h
NSCLBinarySpectrumFormatter.o: /usr/include/machine/ieeefp.h
NSCLBinarySpectrumFormatter.o: /usr/include/limits.h /usr/include/ctype.h
SpectrumFormatError.o: ./SpectrumFormatError.h
SpectrumFormatter.o: SpectrumFormatter.h
SpectrumFormatterFactory.o: SpectrumFormatterFactory.h SpectrumFormatter.h
SpectrumStandardFormatters.o: SpectrumFormatterFactory.h SpectrumFormatter.h
SpectrumStandardFormatters.o: NSCLAsciiSpectrumFormatter.h
SpectrumStandardFormatters.o: NSCLBinarySpectrumFormatter.h ./nsclbin.h
SpectrumStandardFormatters.o: /usr/include/string.h /usr/include/_ansi.h
SpectrumStandardFormatters.o: /usr/include/sys/config.h
SpectrumStandardFormatters.o: /usr/include/sys/reent.h
SpectrumStandardFormatters.o: /usr/include/sys/_types.h /usr/include/errno.h
SpectrumStandardFormatters.o: /usr/include/sys/errno.h ./nscldata.h
SpectrumStandardFormatters.o: ./nsclbinerror.h
nsclbin.o: /usr/include/string.h /usr/include/_ansi.h
nsclbin.o: /usr/include/sys/config.h /usr/include/sys/reent.h
nsclbin.o: /usr/include/sys/_types.h /usr/include/errno.h
nsclbin.o: /usr/include/sys/errno.h ./nscldata.h ./nsclbinerror.h
nsclbin.o: ./nscltime.h /usr/include/time.h /usr/include/machine/time.h
nsclbin.o: /usr/include/sys/types.h /usr/include/machine/types.h
nsclbin.o: /usr/include/sys/features.h ./nsclbin.h /usr/include/math.h
nsclbin.o: /usr/include/machine/ieeefp.h
nsclbinerror.o: ./nsclbinerror.h
nscldata.o: /usr/include/errno.h /usr/include/sys/errno.h
nscldata.o: /usr/include/sys/reent.h /usr/include/_ansi.h
nscldata.o: /usr/include/sys/config.h /usr/include/sys/_types.h
nscldata.o: /usr/include/string.h ./nscldata.h ./nsclbinerror.h
