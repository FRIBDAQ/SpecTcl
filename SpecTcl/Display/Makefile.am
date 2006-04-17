HOMEDEF = -DHOME='"$(prefix)"'
GRIFLAGS = -DGRI_DIR='"@gri_dir@"' -DGRI_CMD_DIR='"@gri_cmd_dir@"'
LEXOUT = lex.yy.c
MOTIF_SOURCES = XMCallback.cc XMMenus.cc XMDialogs.cc XMWidget.cc \
	XMPushbutton.cc XMManagers.cc XMText.cc

INCLUDES = -I.. @XINCLUDES@ @MOTIF_INCLUDES@ $(HOMEDEF) $(GRIFLAGS) \
	-I@top_srcdir@/Utility
AM_LDFLAGS = -g @XLIBSW@ @MOTIF_LIBSW@ @XTLIBSW@ 

#  @LD_EXTRA_FLAGS@

XCCFLAGS =   -g -c -w -I. -I.. $(INCLUDES) -DXTFUNCPROTO -DXAMINE 

AM_CXXFLAGS = -g -pedantic
AM_CFLAGS   = -g -pedantic

install-exec-local:
	$(mkinstalldirs) $(prefix)/etc
	$(INSTALL_PROGRAM) $(srcdir)/*.ctbl $(prefix)/etc
	$(INSTALL_PROGRAM) $(srcdir)/*.ico  $(prefix)/etc
	$(INSTALL_PROGRAM) $(srcdir)/Xamine $(prefix)/bin/Xamine2_0

$(LEXOUT):
	rm *_lex.{h,cc}
	$(LEX) windfile.l

windfile_lex.cc: $(LEXOUT)
	sed s/yy/windfile/g $(LEXOUT) >windfile_lex.cc
	cp windfile_lex.h windlex.h

windfile_parse.cc: windfile.y windio.h dispwind.h dispshare.h printer.h
	$(YACC) -p windfile -b windfile windfile.y
	mv windfile.tab.c windfile_parse.cc

windfile_parse.o: windfile_parse.cc windfile.y  dispwind.h superpos.h dispshare.h windio.h
	$(CC) $(XCCFLAGS) windfile_parse.cc

defaultfile_parse.cc: defaultfile.y dispwind.h windio.h dispshare.h dfltmgr.h grobjdisplay.h panemgr.h printer.h
	$(YACC) -p defaultfile -b defaultfile defaultfile.y
	mv defaultfile.tab.c defaultfile_parse.cc

defaultfile_parse.o: defaultfile_parse.cc defaultfile.y dispwind.h superpos.h dispshare.h dfltmgr.h windio.h
	$(CC) $(XCCFLAGS) defaultfile_parse.cc

windfile_lex.h: windfile.y windio.h
	$(YACC) -p windfile -b windfile -dl  windfile.y
	mv windfile.tab.h windfile_lex.h

windfile_lex.o: windfile.l dispwind.h superpos.h windfile_lex.h windio.h
	$(LEX) windfile.l
	sed s/yy/windfile/g $(LEXOUT) >windfile_lex.cc
	cp windfile_lex.h windlex.h
	$(CC) $(XCCFLAGS) windfile_lex.cc

grobfile_lex.o: grobfile.l dispgrob.h grobjio.h grobfile_lex.h 
	$(LEX) grobfile.l
	sed s/yy/grobjfile/g $(LEXOUT) >grobfile_lex.cc
	$(CXX) $(XCCFLAGS) grobfile_lex.cc

grobfile_parse.cc: grobfile.y dispgrob.h grobjio.h
	$(YACC) -p grobjfile -b grobfile grobfile.y
	mv grobfile.tab.c grobfile_parse.cc

grobfile_parse.o: grobfile_parse.cc
	$(CC) $(XCCFLAGS) grobfile_parse.cc

grobfile_lex.h: grobfile.y grobjio.h grobfile_parse.cc
	$(YACC) -p grobjfile -d grobfile.y
	mv y.tab.h grobfile_lex.h
	rm -f y.tab.c



defaultfile_lex.h: defaultfile.y windio.h
	$(YACC) -p defaultfile -b defaultfile -dl defaultfile.y
	mv defaultfile.tab.h defaultfile_lex.h

defaultfile_lex.o: windfile.l defaultfile.l dispwind.h superpos.h \
	defaultfile_lex.h windio.h
	$(LEX) windfile.l
	$(LEX) defaultfile.l
	sed s/yy/defaultfile/g $(LEXOUT) >defaultfile_lex.cc
	cp defaultfile_lex.h windlex.h
	$(CC) $(XCCFLAGS)  defaultfile_lex.cc

bin_PROGRAMS = Xamine colorset

check_PROGRAMS = Xaminetest
TESTS = $(check_PROGRAMS)

include_HEADERS = Tab.h allocator.h dispshare.h    helpmenu.h    refreshctl.h\
TabP.h          axes.h             dispwind.h      info.h        scaling.h\
XBatch.h        buttondlg.h        errormsg.h      integrate.h   select2.h\
XMCallback.h    buttonreq.h        eventflags.h    lblfont.h     spccheck.h\
XMDialogs.h     buttonsetup.h      exit.h          location.h    spcchoice.h\
XMLabel.h       chanplot.h         expand.h        logging.h     spcdisplay.h\
XMList.h        checklist.h        gateio.h        mapcoord.h    specchoose.h\
XMManagers.h    client.h           gc.h            marker.h      sumregion.h\
XMMenus.h       clientgates.h      gcmgr.h         menusetup.h   superpos.h\
XMPushbutton.h  clientops.h        geometrymenu.h  messages.h    superprompt.h\
XMScale.h       clirequests.h      grafinput.h     objcopy.h     text.h\
XMSeparators.h  colormgr.h         griprint.h      optionmenu.h  titles.h\
XMShell.h       compatspec.h       grobfile.tab.h  panemgr.h     toggles.h\
XMTab.h         convert.h          grobfile_lex.h  pkgmgr.h      trackcursor.h\
XMText.h        copier.h           grobjdisplay.h  prccheck.h   windfile_lex.h\
XMWidget.h      copyatr.h          grobjinput.h    printer.h    windfile_tab.h\
XMWlist.h       defaultfile_lex.h  grobjio.h       procede.h     windio.h\
Xamine.h        delgrob.h          grobjiomenu.h   properties.h  windlex.h\
acceptgates.h   dfltmgr.h          grobjmgr.h      ptlist.h      winiomenu.h\
advprompt.h     dispgrob.h         guintegrate.h   queue.h       xaminegc.h

lib_LTLIBRARIES = libXamine.la

libXamine_la_LDFLAGS = -version-info 2:0:1

libXamine_la_SOURCES = client.c clientgates.c  allocator.c \
	specalloc.c clientbuttons.c prccheck.c



Xamine_SOURCES = Xamine.cc XMWidget.cc XMCallback.cc XMMenus.cc XMText.cc \
XMDialogs.cc XMManagers.cc XMWlist.cc XMShell.cc XMPushbutton.cc XMTab.cc \
XMLabel.cc XMList.cc XMScale.cc XMSeparators.cc \
dispwind.cc menusetup.cc exit.cc helpmenu.cc winiomenu.cc \
grobjiomenu.cc geometrymenu.cc errormsg.cc logging.cc toggles.cc \
spectra.cc specchoose.cc spcdisplay.cc advprompt.cc properties.cc \
panemgr.cc buttonsetup.cc pkgmgr.cc dfltmgr.cc axisdefault.cc \
titledefault.cc attribdefault.cc reducedefault.cc rend1default.cc \
rend2default.cc printer.cc refreshctl.cc axes.cc scaling.cc text.cc \
titles.cc chanplot.cc plot1d.cc  plot2d.cc colormgr.cc shared.cc \
location.cc convert.cc trackcursor.cc expand.cc select2.cc info.cc \
dispgrob.cc  grobjdb.cc  grobfile.cc xprccheck.c \
refreshdefault.cc  marker.cc grafinput.cc \
grobjinput.cc grobjmgr.cc grobjdisplay.cc sumregion.cc integrate.cc \
guintegrate.cc delgrob.cc messages.cc eventflags.cc gateio.cc \
acceptgates.cc clirequests.cc  spccheck.cc superpos.cc \
superprompt.cc objcopy.cc copier.cc lblfont.cc checklist.cc \
compatspec.cc copyatr.cc  buttonreq.cc buttondlg.cc \
spcchoice.cc spccompatchoice.cc spclimchoice.cc ptlist.cc \
lexsupport.c gc.cc xaminegc.cc gcmgr.cc Tab.c griprint.cc mapcoord.cc 


PARSEOBJECTS=windfile_parse.o windfile_lex.o grobfile_lex.o grobfile_parse.o \
	defaultfile_lex.o defaultfile_parse.o



colorset_SOURCES = colorset.cc $(MOTIF_SOURCES) errormsg.cc helpmenu.cc \
procede.cc colormgr.cc

Xaminetest_SOURCES = Xaminetest.c


Xamine_LDADD = $(PARSEOBJECTS) -lXm -lXt -lX11 -lm -lfl $(X11EXTRA_LDFLAGS)

colorset_LDADD = -lXm -lXt -lX11 -lfl $(X11EXTRA_LDFLAGS)

Xaminetest_LDADD = -lXamine -lm

Xaminetest_LDFLAGS = -L.

Xamine_DEPENDENCIES = windfile_parse.o windfile_lex.o dispwind.o \
	defaultfile_lex.o menusetup.o exit.o helpmenu.o winiomenu.o \
	grobjiomenu.o geometrymenu.o \
	errormsg.o procede.o logging.o toggles.o specchoose.o spectra.o \
	spcdisplay.o advprompt.o properties.o buttonsetup.o panemgr.o \
	pkgmgr.o dfltmgr.o axisdefault.o titledefault.o attribdefault.o \
	reducedefault.o rend1default.o rend2default.o printer.o refreshctl.o \
	axes.o scaling.o text.o titles.o chanplot.o plot1d.o plot2d.o \
	colormgr.o shared.o location.o convert.o trackcursor.o expand.o \
	select2.o info.o refreshdefault.o defaultfile_parse.o marker.o \
	grafinput.o grobjinput.o grobjmgr.o grobjdisplay.o  sumregion.o \
	integrate.o guintegrate.o delgrob.o messages.o eventflags.o \
	gateio.o acceptgates.o clirequests.o spccheck.o  superpos.o \
	superprompt.o objcopy.o lblfont.o checklist.o compatspec.o copyatr.o \
	buttonreq.o buttondlg.o spcchoice.o spccompatchoice.o spclimchoice.o \
	ptlist.o lexsupport.o xaminegc.o gcmgr.o copier.o gc.o Tab.o \
	griprint.o mapcoord.o dispgrob.o  grobfile_lex.o grobfile_parse.o \
	grobjdb.o  XMCallback.o XMMenus.o XMDialogs.o XMManagers.o \
	XMWlist.o XMShell.o XMTab.o XMText.o colorset.ico libXamine.la

Xaminetest_DEPENDENCIES = libXamine.la

EXTRA_DIST = queue.cc colorset.ico \
	Xamine.ico Xamine.doc Xamine2.ctbl \
	Xamine3.ctbl Xamine4.ctbl Xamine5.ctbl Xamine6.ctbl Xaminedemo.doc \
	defaultfile.y grobfile.y windfile.y defaultfile.l \
	grobfile.l windfile.l

#  These files can cause problems on rebuilds.

MOSTLYCLEANFILES = defaultfile_lex.cc defaultfile_lex.h grobfile_lex.cc \
	grobfile_lex.h windfile_lex.cc windfile_lex.h              \
	defaultfile_parse.cc grobfile_parse.cc windfile_parse.cc