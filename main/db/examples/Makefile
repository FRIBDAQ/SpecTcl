CXXFLAGS=-I$(SPECTCLHOME)/include

LDFLAGS=-L$(SPECTCLHOME)/lib -Wl,-rpath=$(SPECTCLHOME)/lib -lSpecTclDb \
	-lTclGrammerApp -lSqlite3pp -lsqlite3

all: makedb

makedb: makedb.cpp
	$(CXX) $(CXXFLAGS) makedb.cpp $(LDFLAGS)
