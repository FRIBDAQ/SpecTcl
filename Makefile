INSTDIR=/usr/opt/spectcl/3.3
# Skeleton makefile for 3.1

include $(INSTDIR)/etc/SpecTcl_Makefile.include

#  If you have any switches that need to be added to the default c++ compilation
# rules, add them to the definition below:

USERCXXFLAGS=

#  If you have any switches you need to add to the default c compilation rules,
#  add them to the defintion below:

USERCCFLAGS=$(USERCXXFLAGS)

#  If you have any switches you need to add to the link add them below:

USERLDFLAGS=

#
#   Append your objects to the definitions below:
#

OBJECTS=MySpecTclApp.o ParamMapCommand.o CCUSBUnpacker.o \
	CCCUSBPacket.o CPh7xxUnpacker.o CFixedSizedUnpacker.o

#
#  Finally the makefile targets.
#


SpecTcl: $(OBJECTS)
	$(CXXLD)  -o SpecTcl $(OBJECTS) $(USERLDFLAGS) \
	$(LDFLAGS) 


clean:
	rm -f $(OBJECTS) SpecTcl

depend:
	makedepend $(USERCXXFLAGS) *.cpp *.c

help:
	echo "make                 - Build customized SpecTcl"
	echo "make clean           - Remove objects from previous builds"
	echo "make depend          - Add dependencies to the Makefile. "
