CXXFLAGS=-I$(SPECTCLHOME)/include

LDFLAGS=-L$(SPECTCLHOME)/lib -Wl,-rpath=$(SPECTCLHOME)/lib -lSpecTclDb \
	-lTclGrammerApp -lSqlite3pp -lsqlite3 -Wl,-rpath=$(ROOTSYS)/lib

all: makedb makesaveset lssaveset

makedb: makedb.cpp
	$(CXX) -o makedb $(CXXFLAGS) makedb.cpp $(LDFLAGS)

makesaveset: makesaveset.cpp
	$(CXX) -o makesaveset $(CXXFLAGS) makesaveset.cpp $(LDFLAGS)


lssaveset: lssaveset.cpp
	$(CXX) -o lssaveset $(CXXFLAGS) lssaveset.cpp $(LDFLAGS)
