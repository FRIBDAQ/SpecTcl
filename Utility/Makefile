#
#   Makefile for utility parts to SpecTcl.
#   This group of classes is used in such a wide variety of places that
#   it is difficult to isolate them and package them in a single subsystem.
#

SPECTCL_VERSION= -DVERSION=$(VERSION)
ECHO=echo
COMPILE=g++ -c $(PROFILE) $(INCLUDEDIRS) $(SPECTCL_VERSION)
LINK=g++ $(PROFILE) $(INCLUDEDIRS) $(LIBDIRS) $(SPECTCL_VERSION)
RM=rm -f
DEPEND=makedepend
INSTALL=install -m 0664
ARCHIVE=ar cr

install: system
	$(INSTALL) *.h $(INSTDIR)/Include

system: NamedItem.h buffer.h buftypes.h Refptr.h daqtypes.h histotypes.h
	$(ECHO) Rebuild complete
