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

ltwrite.o: /usr/include/errno.h /usr/include/features.h
ltwrite.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ltwrite.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
ltwrite.o: /usr/include/asm/errno.h /usr/include/stdio.h
ltwrite.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
ltwrite.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
ltwrite.o: /usr/include/bits/types.h /usr/include/libio.h
ltwrite.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
ltwrite.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
ltwrite.o: /usr/include/bits/endian.h /usr/include/sys/select.h
ltwrite.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ltwrite.o: /usr/include/sys/sysmacros.h /usr/include/sys/stat.h
ltwrite.o: /usr/include/bits/stat.h /usr/include/fcntl.h
ltwrite.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
ltwrite.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
ltwrite.o: /usr/include/getopt.h
testfile.o: /usr/include/stdio.h /usr/include/features.h
testfile.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
testfile.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
testfile.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
testfile.o: /usr/include/bits/types.h /usr/include/libio.h
testfile.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
testfile.o: /usr/include/stdlib.h /usr/include/sys/types.h
testfile.o: /usr/include/time.h /usr/include/endian.h
testfile.o: /usr/include/bits/endian.h /usr/include/sys/select.h
testfile.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
testfile.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
testfile.o: /usr/include/errno.h /usr/include/bits/errno.h
testfile.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
testfile.o: /Include/histotypes.h /usr/include/string.h /usr/include/math.h
testfile.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
testfile.o: /usr/include/bits/mathcalls.h
testfile.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
testfile.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
testfile.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
testfile.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
testfile.o: /usr/include/bits/confname.h /usr/include/getopt.h
