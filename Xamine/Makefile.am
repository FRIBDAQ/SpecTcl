INCLUDES = -g -I. -I../Utility -I../Display -I../NSCLException
LDADD = -lXplus	-lException -lXamine
LDFLAGS = -L.

lib_LTLIBRARIES = libXplus.la

libXplus_la_LDFLAGS = -verson-info $(SOVERSION):0

libXplus_la_SOURCES = DisplayGate.cpp Xamine1D.cpp Xamine2D.cpp \
	XamineButton.cpp XamineButtonException.cpp XamineButtonPrompt.cpp \
	XamineConfirmPrompt.cpp XamineEvent.cpp XamineFilePrompt.cpp \
	XamineGateException.cpp XamineGates.cpp \
	XamineNoPrompt.cpp XaminePointsPrompt.cpp \
	XamineSpectrumPrompt.cpp  XaminePushButton.cpp \
	XamineSpectrum.cpp XamineSpectrumIterator.cpp \
	XamineTextPrompt.cpp XamineToggleButton.cpp Xamineplus.cpp

include_HEADERS = ButtonEvent.h DisplayGate.h Point.h Xamine1D.h Xamine2D.h \
	XamineButton.h XamineButtonException.h XamineButtonPrompt.h \
	XamineConfirmPrompt.h XamineEvent.h XamineFilePrompt.h \
	XamineGateException.h XamineGates.h XamineMap1D.h XamineMap2D.h \
	XamineNoPrompt.h XaminePointsPrompt.h XaminePushButton.h \
	XamineSpectrum.h XamineSpectrumIterator.h XamineSpectrumPrompt.h \
	XamineTextPrompt.h XamineToggleButton.h Xamineplus.h

EXTRA_DIST = ClassBreakdown.WC ClassBreakdown.omt
