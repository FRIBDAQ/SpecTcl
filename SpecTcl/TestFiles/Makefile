#
#  Makefile for the TCL Controlled  Histogramming program.
#  targets include:
#     system(default)             - Builds the software.
#     clean                       - Kills off .o, .a files.
#     depend                      - Rebuilds the dependencies with makedepend.
#                                   in current directory.
#
#
INCLUDEDIRS= -I. -I$(INSTDIR)/Include -I/usr/local/include
LIBDIRS= -L. -L$(INSTDIR)/Lib -L/usr/local
LIBS=   -lSorting -lXplus -lAnalysis -lEventSource -ltclPlus -lException \
	 -ltape -lm

ECHO=echo
COMPILE=$($(OS)CPP) -c $(PROFILE) $(INCLUDEDIRS)
LINK=$($(OS)CPP) $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar -r -c

system: testfile ltwrite

clean: 
	$(RM) testfile ltwrite


install: system


depend:
	$(DEPEND) $(INCLUDEDIRS) *.cpp

testfile: testfile.cpp
	$(LINK)  -o testfile   testfile.cpp  $(LIBS)

ltwrite: ltwrite.cpp
	$(LINK)  -o ltwrite ltwrite.cpp $(LIBS)
# DO NOT DELETE

ltwrite.o: /usr/include/errno.h /usr/include/standards.h /usr/include/stdio.h
ltwrite.o: /usr/include/sys/seek.h /usr/include/va_list.h
ltwrite.o: /usr/include/sys/types.h /usr/include/time_t.h
ltwrite.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
ltwrite.o: /usr/include/sys/common_protos.h /usr/include/getopt.h
ltwrite.o: /usr/include/sys/limits.h /usr/include/machine/machlimits.h
ltwrite.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
ltwrite.o: /usr/include/sys/rt_limits.h /usr/include/sys/stat.h
ltwrite.o: /usr/include/sys/mode.h /usr/include/fcntl.h
ltwrite.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
ltwrite.o: /usr/include/sys/access.h
testfile.o: /usr/include/stdio.h /usr/include/standards.h
testfile.o: /usr/include/sys/seek.h /usr/include/va_list.h
testfile.o: /usr/include/sys/types.h /usr/include/time_t.h
testfile.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
testfile.o: /usr/include/sys/common_protos.h /usr/include/getopt.h
testfile.o: /usr/include/sys/limits.h /usr/include/machine/machlimits.h
testfile.o: /usr/include/sys/syslimits.h /usr/include/machine/machtime.h
testfile.o: /usr/include/sys/rt_limits.h /usr/include/stdlib.h
testfile.o: /usr/include/errno.h /usr/include/fcntl.h
testfile.o: /usr/include/sys/fcntl.h /usr/include/sys/mode.h
testfile.o: /usr/include/sys/stat.h /usr/include/unistd.h
testfile.o: /usr/include/sys/access.h
