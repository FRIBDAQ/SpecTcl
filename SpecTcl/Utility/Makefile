#
#   Makefile for utility parts to SpecTcl.
#   This group of classes is used in such a wide variety of places that
#   it is difficult to isolate them and package them in a single subsystem.
#


ECHO=echo
COMPILE=g++ -c $(PROFILE) $(INCLUDEDIRS)
LINK=g++ $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS)
RM=rm -f
DEPEND=makedepend
INSTALL=cp
ARCHIVE=ar cr

install: system
	$(INSTALL) *.h $(INSTDIR)/Include

system: NamedItem.h buffer.h buftypes.h Refptr.h daqtypes.h histotypes.h
	$(ECHO) Rebuild complete
