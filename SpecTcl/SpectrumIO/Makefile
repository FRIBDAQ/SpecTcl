
#
#  Makefile for the C++ interface to Tcl/TK
#  The following targets are supported:
#      system(default)         - Build the library.
#      install                 - Install the system at $(INSTDIR)
#      clean                   - Clean up .o and .a files.
#      tests                   - Build any test programs against local version
#                                of the library.

SPECTCL_VERSION = -DVERSION=$(VERSION)
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
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"' $(SPECTCL_VERSION)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) $(SPECTCL_VERSION)
RM=rm -f
DEPEND=makedepend
INSTALL=install -m 0664
ARCHIVE=ar cr


system: $(LIBRARY)
	$(ECHO) Library rebuild complete.

clean: 
	$(RM) $(OBJECTS) 
	$(RM) $(LIBRARY)

depend:
	makedepend -Y. *.cpp

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
NSCLAsciiSpectrumFormatter.o: SpectrumFormatter.h ./SpectrumFormatError.h
NSCLBinarySpectrumFormatter.o: NSCLBinarySpectrumFormatter.h
NSCLBinarySpectrumFormatter.o: SpectrumFormatter.h ./nsclbin.h ./nscldata.h
NSCLBinarySpectrumFormatter.o: ./nsclbinerror.h ./SpectrumFormatError.h
SpectrumFormatError.o: ./SpectrumFormatError.h
SpectrumFormatter.o: SpectrumFormatter.h
SpectrumFormatterFactory.o: SpectrumFormatterFactory.h SpectrumFormatter.h
SpectrumStandardFormatters.o: SpectrumFormatterFactory.h SpectrumFormatter.h
SpectrumStandardFormatters.o: NSCLAsciiSpectrumFormatter.h
SpectrumStandardFormatters.o: NSCLBinarySpectrumFormatter.h ./nsclbin.h
SpectrumStandardFormatters.o: ./nscldata.h ./nsclbinerror.h
nsclbin.o: ./nscldata.h ./nsclbinerror.h ./nscltime.h ./nsclbin.h
nsclbinerror.o: ./nsclbinerror.h
nscldata.o: ./nscldata.h ./nsclbinerror.h
