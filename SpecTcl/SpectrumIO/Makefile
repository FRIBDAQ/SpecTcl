
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
NSCLAsciiSpectrumFormatter.o: /usr/include/standards.h /usr/include/stdlib.h
NSCLAsciiSpectrumFormatter.o: /usr/include/getopt.h /usr/include/sys/types.h
NSCLAsciiSpectrumFormatter.o: /usr/include/time_t.h
NSCLAsciiSpectrumFormatter.o: /usr/include/mach/machine/vm_types.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/select.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/common_protos.h
NSCLAsciiSpectrumFormatter.o: /usr/include/limits.h
NSCLAsciiSpectrumFormatter.o: /usr/include/machine/machlimits.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/syslimits.h
NSCLAsciiSpectrumFormatter.o: /usr/include/machine/machtime.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/rt_limits.h
NSCLAsciiSpectrumFormatter.o: /usr/include/time.h /usr/include/sys/timers.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/sysmisc.h
NSCLAsciiSpectrumFormatter.o: /usr/include/string.h /usr/include/strings.h
NSCLAsciiSpectrumFormatter.o: /usr/include/ctype.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/localedef.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/lc_core.h
NSCLAsciiSpectrumFormatter.o: /usr/include/locale.h /usr/include/nl_types.h
NSCLAsciiSpectrumFormatter.o: /usr/include/mesg.h /usr/include/stdio.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/seek.h /usr/include/va_list.h
NSCLAsciiSpectrumFormatter.o: /usr/include/sys/limits.h
NSCLBinarySpectrumFormatter.o: NSCLBinarySpectrumFormatter.h
NSCLBinarySpectrumFormatter.o: SpectrumFormatter.h ./nsclbin.h
NSCLBinarySpectrumFormatter.o: /usr/include/string.h /usr/include/standards.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/types.h /usr/include/time_t.h
NSCLBinarySpectrumFormatter.o: /usr/include/mach/machine/vm_types.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/select.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/common_protos.h
NSCLBinarySpectrumFormatter.o: /usr/include/strings.h /usr/include/errno.h
NSCLBinarySpectrumFormatter.o: ./nscldata.h ./nsclbinerror.h
NSCLBinarySpectrumFormatter.o: ../SpecTcl/SpectrumPackage.h
NSCLBinarySpectrumFormatter.o: ./SpectrumFormatError.h /usr/include/math.h
NSCLBinarySpectrumFormatter.o: /usr/include/stdlib.h /usr/include/getopt.h
NSCLBinarySpectrumFormatter.o: /usr/include/limits.h
NSCLBinarySpectrumFormatter.o: /usr/include/machine/machlimits.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/syslimits.h
NSCLBinarySpectrumFormatter.o: /usr/include/machine/machtime.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/rt_limits.h
NSCLBinarySpectrumFormatter.o: /usr/include/ctype.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/localedef.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/lc_core.h
NSCLBinarySpectrumFormatter.o: /usr/include/locale.h /usr/include/nl_types.h
NSCLBinarySpectrumFormatter.o: /usr/include/mesg.h /usr/include/stdio.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/seek.h /usr/include/va_list.h
NSCLBinarySpectrumFormatter.o: /usr/include/sys/limits.h
SpectrumFormatError.o: ./SpectrumFormatError.h
SpectrumFormatter.o: SpectrumFormatter.h
SpectrumFormatterFactory.o: SpectrumFormatterFactory.h SpectrumFormatter.h
SpectrumStandardFormatters.o: SpectrumFormatterFactory.h SpectrumFormatter.h
SpectrumStandardFormatters.o: NSCLAsciiSpectrumFormatter.h
SpectrumStandardFormatters.o: NSCLBinarySpectrumFormatter.h ./nsclbin.h
SpectrumStandardFormatters.o: /usr/include/string.h /usr/include/standards.h
SpectrumStandardFormatters.o: /usr/include/sys/types.h /usr/include/time_t.h
SpectrumStandardFormatters.o: /usr/include/mach/machine/vm_types.h
SpectrumStandardFormatters.o: /usr/include/sys/select.h
SpectrumStandardFormatters.o: /usr/include/sys/common_protos.h
SpectrumStandardFormatters.o: /usr/include/strings.h /usr/include/errno.h
SpectrumStandardFormatters.o: ./nscldata.h ./nsclbinerror.h
nsclbin.o: /usr/include/string.h /usr/include/standards.h
nsclbin.o: /usr/include/sys/types.h /usr/include/time_t.h
nsclbin.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
nsclbin.o: /usr/include/sys/common_protos.h /usr/include/strings.h
nsclbin.o: /usr/include/errno.h ./nscldata.h ./nsclbinerror.h ./nscltime.h
nsclbin.o: /usr/include/time.h /usr/include/machine/machtime.h
nsclbin.o: /usr/include/sys/timers.h /usr/include/sys/sysmisc.h ./nsclbin.h
nsclbin.o: /usr/include/math.h /usr/include/stdlib.h /usr/include/getopt.h
nsclbinerror.o: ./nsclbinerror.h
nscldata.o: /usr/include/errno.h /usr/include/standards.h
nscldata.o: /usr/include/string.h /usr/include/sys/types.h
nscldata.o: /usr/include/time_t.h /usr/include/mach/machine/vm_types.h
nscldata.o: /usr/include/sys/select.h /usr/include/sys/common_protos.h
nscldata.o: /usr/include/strings.h ./nscldata.h ./nsclbinerror.h
