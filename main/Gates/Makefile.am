INCLUDES = -I. -I../Utility -I../Events -I../Xamine  -I@top_srcdir@/Sorter \
	-I@top_srcdir@/NSCLException
LDADD = -lTclGrammerCommands -lSorting -lXplus -lAnalysis -lEventSource \
	-ltclPlus -lNSCLException -lXamine -ltape -ltk -ltcl -lXt -lX11 -lm


lib_LTLIBRARIES = libGates.la

libGates_la_LDFLAGS = -version-info  $(SOVERSION):0 \
	-Wl,"-rpath-link=$(libdir)"

libGates_la_LIBADD= -L. @TCL_LDFLAGS@

libGates_la_SOURCES = ActualIterator.cpp AndGate.cpp Band.cpp \
	CompoundGate.cpp ConstituentIterator.cpp Contour.cpp Cut.cpp \
	DeletedGate.cpp FalseGate.cpp Gate.cpp GateContainer.cpp \
	GateListIterator.cpp Interior.cpp InteriorRow.cpp Not.cpp OrGate.cpp \
	PointlistGate.cpp PointlistIterator.cpp SingleItemIterator.cpp \
	TrueGate.cpp C2Bands.cpp \
	CGammaCut.cpp CGammaBand.cpp CGammaContour.cpp \
	MaskGates.cpp MaskEqualGate.cpp MaskAndGate.cpp MaskNotGate.cpp \
	CFold.cpp \
	CROI.cpp CEverywhereROI.cpp CContourROI.cpp

include_HEADERS = ActualIterator.h AndGate.h Band.h C2Bands.h CompoundGate.h \
	ConstituentIterator.h Contour.h Cut.h DeletedGate.h FalseGate.h \
	CGammaBand.h CGammaContour.h CGammaCut.h Gate.h GateContainer.h \
	GateListIterator.h Interior.h InteriorRow.h Not.h OrGate.h \
	PointlistGate.h PointlistIterator.h SingleItemIterator.h TrueGate.h \
	MaskGates.h MaskEqualGate.h MaskAndGate.h MaskNotGate.h \
	CFold.h \
	CROI.h CEverywhereROI.h CContourROI.h

EXTRA_DIST = classbreakdown.WC classbreakdown.omt
