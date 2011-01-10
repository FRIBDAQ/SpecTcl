INSTDIR=/usr/opt/spectcl/current

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
	CV1x90Unpacker.o CV977Unpacker.o CMASE.o CHINP.o CPSD.o


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
# DO NOT DELETE

C785Unpacker.o: C785Unpacker.h CModuleUnpacker.h CParamMapCommand.h
C785Unpacker.o: /usr/include/stdint.h /usr/include/features.h
C785Unpacker.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
C785Unpacker.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
C785Unpacker.o: /usr/include/bits/wchar.h
CFitButton.o: CFitButton.h
CLLNLUnpacker.o: CLLNLUnpacker.h /usr/include/stdint.h
CLLNLUnpacker.o: /usr/include/features.h /usr/include/sys/cdefs.h
CLLNLUnpacker.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
CLLNLUnpacker.o: /usr/include/gnu/stubs-32.h /usr/include/bits/wchar.h
CLLNLUnpacker.o: CParamMapCommand.h
CMADC32Unpacker.o: CMADC32Unpacker.h CModuleUnpacker.h CParamMapCommand.h
CMADC32Unpacker.o: /usr/include/stdint.h /usr/include/features.h
CMADC32Unpacker.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
CMADC32Unpacker.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
CMADC32Unpacker.o: /usr/include/bits/wchar.h
CModuleUnpacker.o: CModuleUnpacker.h CParamMapCommand.h
CNADC2530Unpacker.o: CNADC2530Unpacker.h CModuleUnpacker.h CParamMapCommand.h
CNADC2530Unpacker.o: /usr/include/stdint.h /usr/include/features.h
CNADC2530Unpacker.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
CNADC2530Unpacker.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
CNADC2530Unpacker.o: /usr/include/bits/wchar.h
CParamMapCommand.o: CParamMapCommand.h
CRateCommand.o: CRateCommand.h CRateList.h CRateProcessor.h
CRateCommand.o: /usr/include/stdint.h /usr/include/features.h
CRateCommand.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
CRateCommand.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
CRateCommand.o: /usr/include/bits/wchar.h
CRateEventProcessor.o: CRateEventProcessor.h /usr/include/stdint.h
CRateEventProcessor.o: /usr/include/features.h /usr/include/sys/cdefs.h
CRateEventProcessor.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
CRateEventProcessor.o: /usr/include/gnu/stubs-32.h /usr/include/bits/wchar.h
CRateEventProcessor.o: /usr/include/stdlib.h /usr/include/sys/types.h
CRateEventProcessor.o: /usr/include/bits/types.h
CRateEventProcessor.o: /usr/include/bits/typesizes.h /usr/include/time.h
CRateEventProcessor.o: /usr/include/endian.h /usr/include/bits/endian.h
CRateEventProcessor.o: /usr/include/sys/select.h /usr/include/bits/select.h
CRateEventProcessor.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
CRateEventProcessor.o: /usr/include/sys/sysmacros.h
CRateEventProcessor.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
CRateList.o: CRateList.h
CRateProcessor.o: CRateProcessor.h /usr/include/stdint.h
CRateProcessor.o: /usr/include/features.h /usr/include/sys/cdefs.h
CRateProcessor.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
CRateProcessor.o: /usr/include/gnu/stubs-32.h /usr/include/bits/wchar.h
CRateProcessor.o: CRateList.h
CStackMapCommand.o: CStackMapCommand.h CParamMapCommand.h
CStackUnpacker.o: CStackUnpacker.h /usr/include/stdint.h
CStackUnpacker.o: /usr/include/features.h /usr/include/sys/cdefs.h
CStackUnpacker.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
CStackUnpacker.o: /usr/include/gnu/stubs-32.h /usr/include/bits/wchar.h
CStackUnpacker.o: CModuleUnpacker.h CParamMapCommand.h C785Unpacker.h
CStackUnpacker.o: CNADC2530Unpacker.h CMADC32Unpacker.h CV1x90Unpacker.h
CStackUnpacker.o: CV977Unpacker.h CStackMapCommand.h
CV1x90Unpacker.o: CV1x90Unpacker.h /usr/include/stdint.h
CV1x90Unpacker.o: /usr/include/features.h /usr/include/sys/cdefs.h
CV1x90Unpacker.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
CV1x90Unpacker.o: /usr/include/gnu/stubs-32.h /usr/include/bits/wchar.h
CV1x90Unpacker.o: /usr/include/string.h /usr/include/stdio.h
CV1x90Unpacker.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
CV1x90Unpacker.o: /usr/include/libio.h /usr/include/_G_config.h
CV1x90Unpacker.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
CV1x90Unpacker.o: /usr/include/bits/sys_errlist.h
CV977Unpacker.o: CV977Unpacker.h /usr/include/stdint.h
CV977Unpacker.o: /usr/include/features.h /usr/include/sys/cdefs.h
CV977Unpacker.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
CV977Unpacker.o: /usr/include/gnu/stubs-32.h /usr/include/bits/wchar.h
MySpecTclApp.o: MySpecTclApp.h CParamMapCommand.h CStackMapCommand.h
MySpecTclApp.o: CStackUnpacker.h /usr/include/stdint.h
MySpecTclApp.o: /usr/include/features.h /usr/include/sys/cdefs.h
MySpecTclApp.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
MySpecTclApp.o: /usr/include/gnu/stubs-32.h /usr/include/bits/wchar.h
MySpecTclApp.o: CRateCommand.h CRateEventProcessor.h CFitButton.h
