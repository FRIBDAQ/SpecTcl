

CC = g++

FLAG =  -g
CXXFLAGS=-g

RUNFILE = filter2ascii

INCLUDE = 
LIBS =  

OBJFILES = CXdrInputStream.o  filter2ascii.o

$(RUNFILE):	$(OBJFILES) 
	$(CC) $(FLAG) $(OBJFILES) -o $(RUNFILE)  $(LIBS) 

clean:
		rm -f $(OBJFILES) $(RUNFILE)
