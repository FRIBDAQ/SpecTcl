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
