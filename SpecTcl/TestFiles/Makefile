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

ltwrite.o: /usr/include/errno.h /usr/include/sys/errno.h
ltwrite.o: /usr/include/sys/reent.h /usr/include/_ansi.h
ltwrite.o: /usr/include/sys/config.h /usr/include/sys/_types.h
ltwrite.o: /usr/include/stdio.h /usr/include/sys/types.h
ltwrite.o: /usr/include/machine/types.h /usr/include/sys/stat.h
ltwrite.o: /usr/include/time.h /usr/include/machine/time.h
ltwrite.o: /usr/include/sys/features.h /usr/include/fcntl.h
ltwrite.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
ltwrite.o: /usr/include/sys/unistd.h /usr/include/getopt.h
testfile.o: /usr/include/stdio.h /usr/include/_ansi.h
testfile.o: /usr/include/sys/config.h /usr/include/sys/reent.h
testfile.o: /usr/include/sys/_types.h /usr/include/stdlib.h
testfile.o: /usr/include/errno.h /usr/include/sys/errno.h
testfile.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
testfile.o: /usr/include/sys/types.h /usr/include/machine/types.h
testfile.o: /usr/include/sys/stat.h /usr/include/time.h
testfile.o: /usr/include/machine/time.h /usr/include/sys/features.h
testfile.o: /usr/include/unistd.h /usr/include/sys/unistd.h
testfile.o: /usr/include/getopt.h
