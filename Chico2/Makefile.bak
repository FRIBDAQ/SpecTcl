INSTDIR=/usr/opt/spectcl/3.3

# Skeleton makefile for 3.3

include $(INSTDIR)/etc/SpecTcl_Makefile.include

#  If you have any switches that need to be added to the default c++ compilation
# rules, add them to the definition below:
USERCXXFLAGS=
#
#  Uncomment the next line to enable reportage of bad events to stderr.
#USERCXXFLAGS=-DREPORT_BAD_EVENTS


#  If you have any switches you need to add to the default c compilation rules,
#  add them to the defintion below:

USERCCFLAGS=$(USERCXXFLAGS)

#  If you have any switches you need to add to the link add them below:

USERLDFLAGS=

#
#   Append your objects to the definitions below:
#

OBJECTS=MySpecTclApp.o CParamMapCommand.o	\
	CRateProcessor.o CRateList.o CRateCommand.o CFitButton.o \
	CStackMapCommand.o CModuleUnpacker.o C785Unpacker.o CNADC2530Unpacker.o \
	CStackUnpacker.o CMADC32Unpacker.o CRateEventProcessor.o	\
	CV1x90Unpacker.o CV977Unpacker.o


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
