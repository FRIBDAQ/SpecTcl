#
#   Makefile for the exception class library.
#
#    The following targets are supported:
#        system:           - Default, build the library.
#        install:          - Install the library  to $(INSTDIR).
#        clean:            - Clean up objects etc.
#        depend:           - Update makefile dependencies.
#        tests:            - Make any test programs which may exist.
#
#

INCLUDEDIRS=-I$(INSTDIR)/Include -I.
OBJECTS = DisplayGate.o Xamine1D.o Xamine2D.o XamineButton.o \
	XamineButtonException.o XamineButtonPrompt.o XamineConfirmPrompt.o \
	XamineEvent.o XamineFilePrompt.o XamineGateException.o XamineGates.o \
	XamineNoPrompt.o XaminePointsPrompt.o XaminePushButton.o \
	XamineSpectrum.o XamineSpectrumIterator.o \
	XamineTextPrompt.o XamineToggleButton.o Xamineplus.o
LIBDIRS=-L. -L$(INSTDIR)/Lib

LIBS=-lXplus -lXamine


ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS)
LINK=$($(OS)CPP) $(NOPROFILE) $(INCLUDEDIRS) $(LIBDIRS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr

OBJECTS=ErrnoException.o Exception.o RangeError.o StreamIOError.o

system:	libException.a
	$(ECHO) Build complete


depend: 
	$(DEPEND) $(INCLUDEDIRS) *.cpp

clean:
	$(RM) $(OBJECTS)
	$(RM) libException.a

install: system
	$(INSTALL) libException.a $(INSTDIR)/Lib
	$(INSTALL) *.h            $(INSTDIR)/Include

libException.a: $(OBJECTS)
	$(ARCHIVE) libException.a $(OBJECTS)	


ErrnoException.o: ErrnoException.cpp
	$(COMPILE) ErrnoException.cpp

Exception.o: Exception.cpp
	$(COMPILE) Exception.cpp

RangeError.o: RangeError.cpp
	$(COMPILE) RangeError.cpp

StreamIOError.o: StreamIOError.cpp
	$(COMPILE) StreamIOError.cpp


# DO NOT DELETE

ErrnoException.o: ErrnoException.h Exception.h /usr/include/errno.h
ErrnoException.o: /usr/include/standards.h
Exception.o: Exception.h /usr/include/string.h /usr/include/standards.h
Exception.o: /usr/include/sys/types.h /usr/include/time_t.h
Exception.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
Exception.o: /usr/include/sys/common_protos.h /usr/include/strings.h
RangeError.o: RangeError.h Exception.h /usr/include/stdio.h
RangeError.o: /usr/include/standards.h /usr/include/sys/seek.h
RangeError.o: /usr/include/va_list.h /usr/include/sys/types.h
RangeError.o: /usr/include/time_t.h /usr/include/mach/machine/vm_types.h
RangeError.o: /usr/include/sys/select.h /usr/include/sys/common_protos.h
RangeError.o: /usr/include/getopt.h /usr/include/sys/limits.h
RangeError.o: /usr/include/machine/machlimits.h /usr/include/sys/syslimits.h
RangeError.o: /usr/include/machine/machtime.h /usr/include/sys/rt_limits.h
StreamIOError.o: StreamIOError.h Exception.h /usr/include/string.h
StreamIOError.o: /usr/include/standards.h /usr/include/sys/types.h
StreamIOError.o: /usr/include/time_t.h /usr/include/mach/machine/vm_types.h
StreamIOError.o: /usr/include/sys/select.h /usr/include/sys/common_protos.h
StreamIOError.o: /usr/include/strings.h
