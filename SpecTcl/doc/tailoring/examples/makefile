INSTDIR=/usr/local/spectcl
OS=OSF1
#
#   Makefile to build user tailored SpecTcl.
#


OBJECTS=AppInit.o UserCode.o
INCLUDEDIRS= -I. -I$(INSTDIR)/Include -I/usr/local/include \
	-I/usr/local/daq/include
LIBDIRS= -L$(INSTDIR)/Lib -L/usr/local -L/usr/local/daq/lib -L/usr/X11/lib
LIBS=   -lTclGrammerCommands \
	-lSorting -lXplus -lAnalysis -lEventSource -ltclPlus -lException \
	-lXamine -ltape \
	-ltk -ltcl -lXt -lX11 -lm
ECHO=echo
COMPILE=g++ -c -g $(INCLUDEDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"'
LINK=g++ -g $(INCLUDEDIRS) $(LIBDIRS) -D$(OS) -DINSTALLED_IN='"$(INSTDIR)"'
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr
CONCAT=cat

SpecTcl: $(OBJECTS)
	$(LINK) $(LIBDIRS) -o SpecTcl $(INSTDIR)/Lib/Globals.o $(OBJECTS) \
	 $(LIBS)

AppInit.o: AppInit.cpp
	$(COMPILE) AppInit.cpp

UserCode.o: UserCode.cpp
	$(COMPILE) UserCode.cpp
