lib_LTLIBRARIES = libXamineParsers.la

LEXOUT = @builddir@/lex.yy.c

$(LEXOUT):
	$(LEX) @srcdir@/windfile.l

#windfile_lex.cc: $(LEXOUT) @builddir@/windfile_lex.h
#	sed s/yy/windfile/g $(LEXOUT) >windfile_lex.cc
#	cp windfile_lex.h windlex.h

clean-local :
	rm -f *_lex.cc *_lex.h

BUILT_SOURCES = windfile_parse.cc windfile_lex.h 

windfile_FILES =@srcdir@/windfile.y \
			 @srcdir@/dispwind.h \
			 @srcdir@/windio.h 

windfile_parse.cc: @srcdir@/windfile.y
	$(YACC) -p windfile -b windfile @srcdir@/windfile.y
	mv windfile.tab.c windfile_parse.cc

windfile_lex.h: @srcdir@/windfile.y windfile_parse.cc
	$(YACC) -p windfile -b windfile -dl  @srcdir@/windfile.y
	mv windfile.tab.h windfile_lex.h

windfile_lex.cc : @srcdir@/windfile.l @srcdir@/dispwind.h @srcdir@/superpos.h @builddir@/windfile_lex.h
	$(LEX) @srcdir@/windfile.l
	sed s/yy/windfile/g $(LEXOUT) >windfile_lex.cc
	cp windfile_lex.h windlex.h

libXamineParsers_la_SOURCES = windfile_lex.cc \
							  windfile_parse.cc \
							  dispwind.cc \
							  superpos.cc  \
								SpectrumQueryInterface.cc 

libXamineParsers_la_CPPFLAGS = -I@top_builddir@ \
															 -I@srcdir@ \
								-DXTFUNCPROTO -DXAMINE \
								-fPIC 


EXTRA_DIST = \
	windfile.y \
	windfile.l \
	dispwind.h \
	superpos.h \
	SpectrumQueryInterface.h \
	windio.h

#  These files can cause problems on rebuilds.

MOSTLYCLEANFILES = \
	windfile_lex.cc windfile_lex.h              \
	windfile_parse.cc
