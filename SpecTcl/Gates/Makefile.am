INCLUDES = -I. -I../Utility -I../Events -I../Xamine 
LDADD = -lTclGrammerCommands -lSorting -lXplus -lAnalysis -lEventSource \
	-ltclPlus -lNSCLException -lXamine -ltape -ltk -ltcl -lXt -lX11 -lm
LDFLAGS = -L. @TCL_LDFLAGS@

lib_LTLIBRARIES = libGates.la

libGates_la_LDFLAGS = -version-info  $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libGates_la_SOURCES = ActualIterator.cpp AndGate.cpp Band.cpp \
	CompoundGate.cpp ConstituentIterator.cpp Contour.cpp Cut.cpp \
	DeletedGate.cpp FalseGate.cpp Gate.cpp GateContainer.cpp \
	GateListIterator.cpp Interior.cpp InteriorRow.cpp Not.cpp OrGate.cpp \
	PointlistGate.cpp PointlistIterator.cpp SingleItemIterator.cpp \
	TrueGate.cpp C2Bands.cpp GammaCut.cpp GammaBand.cpp GammaContour.cpp \
	MaskGates.cpp MaskEqualGate.cpp MaskAndGate.cpp MaskNotGate.cpp

include_HEADERS = ActualIterator.h AndGate.h Band.h C2Bands.h CompoundGate.h \
	ConstituentIterator.h Contour.h Cut.h DeletedGate.h FalseGate.h \
	GammaBand.h GammaContour.h GammaCut.h Gate.h GateContainer.h \
	GateListIterator.h Interior.h InteriorRow.h Not.h OrGate.h \
	PointlistGate.h PointlistIterator.h SingleItemIterator.h TrueGate.h \
	MaskGates.h MaskEqualGate.h MaskAndGate.h MaskNotGate.h

EXTRA_DIST = classbreakdown.WC classbreakdown.omt
