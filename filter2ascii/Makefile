MYHOME=$(THEHOME)
#
# installer set TCL_MYHOME 
ifeq ($(TCL_MYHOME), )
TCL_MYHOME=$(MYHOME)
endif
#
# 64-bit or 32-bit
# (mfile not used in this code)
MFILEPATH=/soft/intel/mfile
ifeq ($(BITSYSTEM), 64)
MFILEPATH=/soft/x86_64/mfile
endif

CC = g++

FLAG =  

BIN = $(MYHOME)/bin/bin$(BITSYSTEM)
RUNFILE = filter2ascii

INCLUDE = 
LIBS =  

OBJFILES = CXdrInputStream.o  filter2ascii.o

$(RUNFILE):	$(OBJFILES) 
	$(CC) $(FLAG) $(OBJFILES) -o $(RUNFILE)  $(LIBS) 

clean:
		rm -f $(OBJFILES) $(RUNFILE)
