#MAKEFILE for database sample programs.
# Assumptions:
#   SPECTCLHOME -> installation top level directory for SpecTcl 5.3-007 or higher.
#   ROOTSYS     -> The top level directory of the version of Root SpecTcl
#                  was compiled with.
#
CXXFLAGS=-I$(SPECTCLHOME)/include

LDFLAGS=-L$(SPECTCLHOME)/lib -Wl,-rpath=$(SPECTCLHOME)/lib -lSpecTclDb \
	-lTclGrammerApp -lSqlite3pp -lsqlite3 -Wl,-rpath=$(ROOTSYS)/lib

all: makedb makesaveset lssaveset pardef specdef gatedef applydef vardef \
	evtstore evtget

makedb: makedb.cpp
	$(CXX) -o makedb $(CXXFLAGS) makedb.cpp $(LDFLAGS)

makesaveset: makesaveset.cpp
	$(CXX) -o makesaveset $(CXXFLAGS) makesaveset.cpp $(LDFLAGS)


lssaveset: lssaveset.cpp
	$(CXX) -o lssaveset $(CXXFLAGS) lssaveset.cpp $(LDFLAGS)

pardef: pardef.cpp
	$(CXX) -o pardef  $(CXXFLAGS) pardef.cpp $(LDFLAGS)

specdef: specdef.cpp
	$(CXX) -o specdef  $(CXXFLAGS) specdef.cpp $(LDFLAGS)

gatedef: gatedef.cpp
	$(CXX) -o gatedef $(CXXFLAGS) gatedef.cpp $(LDFLAGS)

applydef: applydef.cpp
	$(CXX) -o applydef $(CXXFLAGS) applydef.cpp $(LDFLAGS)

vardef: vardef.cpp
	$(CXX) -o vardef $(CXXFLAGS) vardef.cpp $(LDFLAGS)

evtstore: evtstore.cpp
	$(CXX) -o evtstore $(CXXFLAGS) evtstore.cpp $(LDFLAGS)

evtget: evtget.cpp
	$(CXX) -o evtget $(CXXFLAGS) evtget.cpp $(LDFLAGS)
