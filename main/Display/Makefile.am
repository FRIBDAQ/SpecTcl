bin_PROGRAMS = Xamine colorset

lib_LTLIBRARIES = libXamine.la 


HOMEDEF = -DHOME='"$(prefix)"'
GRIFLAGS = -DGRI_DIR='"@gri_dir@"' -DGRI_CMD_DIR='"@gri_dir@"'
MOTIF_SOURCES = XMCallback.cc XMMenus.cc XMDialogs.cc XMWidget.cc \
	XMPushbutton.cc XMManagers.cc XMText.cc

AM_CPPFLAGS = -I@top_srcdir@ @XINCLUDES@ @MOTIF_INCLUDES@ $(HOMEDEF) $(GRIFLAGS) \
	-I@top_srcdir@/Utility $(GDINCLUDES) -I@top_srcdir@/XamineParsers -I@top_builddir@/XamineParsers

AM_CXXFLAGS = -g -pedantic @TCL_CPPFLAGS@
AM_CFLAGS   = -g -pedantic @TCL_CPPFLAGS@

install-exec-local:
	$(mkinstalldirs) $(prefix)/etc
	$(INSTALL_PROGRAM) $(srcdir)/*.ctbl $(prefix)/etc
	$(INSTALL_PROGRAM) $(srcdir)/*.ico  $(prefix)/etc
	$(INSTALL_PROGRAM) $(builddir)/Xamine $(prefix)/bin/Xamine2_0


BUILT_SOURCES = defaultfile_parse.cc defaultfile_lex.cc defaultfile_lex.h grobfile_parse.cc grobfile_lex.h grobfile_lex.cc windlex.h

LEXOUT = @builddir@/lex.yy.c

defaultfile_parse.cc: @srcdir@/defaultfile.y @top_srcdir@/XamineParsers/dispwind.h @top_srcdir@/XamineParsers/windio.h @srcdir@/dispshare.h @srcdir@/dfltmgr.h @srcdir@/grobjdisplay.h @srcdir@/panemgr.h @srcdir@/printer.h
	$(YACC) -p defaultfile -b defaultfile @srcdir@/defaultfile.y
	mv defaultfile.tab.c @builddir@/defaultfile_parse.cc

grobfile_lex.cc: @srcdir@/grobfile.l @srcdir@/dispgrob.h @srcdir@/grobjio.h @builddir@/grobfile_lex.h
	$(LEX) @srcdir@/grobfile.l
	sed s/yy/grobjfile/g $(LEXOUT) >@builddir@/grobfile_lex.cc

grobfile_parse.cc: @srcdir@/grobfile.y @srcdir@/dispgrob.h @srcdir@/grobjio.h
	$(YACC) -p grobjfile -b grobfile @srcdir@/grobfile.y
	mv grobfile.tab.c grobfile_parse.cc

#grobfile_parse.o: @builddir@/grobfile_parse.cc
#	$(CC) $(XCCFLAGS) @builddir@/grobfile_parse.cc

grobfile_lex.h: @srcdir@/grobfile.y @srcdir@/grobjio.h @builddir@/grobfile_parse.cc
	$(YACC) -p grobjfile -d @srcdir@/grobfile.y
	mv y.tab.h grobfile_lex.h
	rm -f y.tab.c

defaultfile_parse.cc: @srcdir@/defaultfile.y @top_srcdir@/XamineParsers/dispwind.h @top_srcdir@/XamineParsers/windio.h @srcdir@/dispshare.h @srcdir@/dfltmgr.h @srcdir@/grobjdisplay.h @srcdir@/panemgr.h @srcdir@/printer.h
	$(YACC) -p defaultfile -b defaultfile @srcdir@/defaultfile.y
	mv defaultfile.tab.c defaultfile_parse.cc

defaultfile_lex.h: @srcdir@/defaultfile.y @top_srcdir@/XamineParsers/windio.h
	$(YACC) -p defaultfile -b defaultfile -dl @srcdir@/defaultfile.y
	mv defaultfile.tab.h defaultfile_lex.h

defaultfile_lex.cc : @top_srcdir@/XamineParsers/windfile.l @srcdir@/defaultfile.l @top_srcdir@/XamineParsers/dispwind.h @srcdir@/superpos.h \
	@builddir@/defaultfile_lex.h @top_srcdir@/XamineParsers/windio.h
	$(LEX) @srcdir@/defaultfile.l
	sed s/yy/defaultfile/g $(LEXOUT) >defaultfile_lex.cc

windlex.h : @builddir@/defaultfile_lex.h
	cp defaultfile_lex.h windlex.h


#libXamineParsers_la_SOURCES = dispwind.cc \
#							  lexsupport.c \
#							  superpos.cc

#XCCFLAGS =   -g -c -w -I@top_builddir@ -I@srcdir@ -I@top_srcdir@ $(INCLUDES) -DXTFUNCPROTO -DXAMINE

#libXamineParsers_la_CPPFLAGS = -I@top_builddir@ \
#								-I@srcdir@ \
#								-I@top_srcdir@ \
#								-DXTFUNCPROTO -DXAMINE \
#								$(HOMEDEF)

#check_PROGRAMS = Xaminetest
# TESTS = $(check_PROGRAMS)	# needs to start Xamine

include_HEADERS = Tab.h allocator.h dispshare.h    helpmenu.h    refreshctl.h\
TabP.h          axes.h             info.h        scaling.h\
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
XMText.h        copier.h           grobjdisplay.h  prccheck.h   \
XMWidget.h      copyatr.h          grobjinput.h    printer.h   \
XMWlist.h       XMMultiSelectionList.h defaultfile_lex.h        grobjio.h      \
 procede.h      \
Xamine.h        delgrob.h          grobjiomenu.h   properties.h  \
acceptgates.h   dfltmgr.h          grobjmgr.h      ptlist.h      winiomenu.h \
advprompt.h     dispgrob.h         guintegrate.h   queue.h       xaminegc.h \
xamineDataTypes.h wysiwygPrint.h \
XamineSpectrumInterface.h  \
lexsupport.c \
superpos.cc


libXamine_la_LDFLAGS = -version-info $(SOVERSION):0

libXamine_la_SOURCES = client.c clientgates.c  allocator.c \
    specalloc.c clientbuttons.c prccheck.c

libXamine_la_CPPFLAGS = -I@top_srcdir@/XamineParsers -I@top_builddir@/XamineParsers $(HOMEDEF)

Xamine_SOURCES = Xamine.cc XMWidget.cc XMCallback.cc XMMenus.cc XMText.cc \
XMDialogs.cc XMManagers.cc XMWlist.cc XMShell.cc XMPushbutton.cc XMTab.cc \
XMLabel.cc XMList.cc XMScale.cc XMSeparators.cc XMMultiSelectionList.cc \
menusetup.cc exit.cc helpmenu.cc winiomenu.cc \
grobjiomenu.cc geometrymenu.cc logging.cc toggles.cc \
spectra.cc specchoose.cc spcdisplay.cc advprompt.cc properties.cc \
panemgr.cc buttonsetup.cc pkgmgr.cc axisdefault.cc \
titledefault.cc attribdefault.cc reducedefault.cc rend1default.cc \
rend2default.cc printer.cc refreshctl.cc axes.cc scaling.cc text.cc \
titles.cc chanplot.cc plot1d.cc  plot2d.cc colormgr.cc shared.cc \
location.cc convert.cc trackcursor.cc expand.cc select2.cc info.cc \
dispgrob.cc  xprccheck.c \
refreshdefault.cc  marker.cc grafinput.cc \
grobjinput.cc grobjmgr.cc grobjdisplay.cc sumregion.cc integrate.cc \
guintegrate.cc delgrob.cc messages.cc eventflags.cc gateio.cc \
acceptgates.cc clirequests.cc  spccheck.cc \
superprompt.cc objcopy.cc copier.cc lblfont.cc checklist.cc \
compatspec.cc copyatr.cc  buttonreq.cc buttondlg.cc \
spcchoice.cc spccompatchoice.cc spclimchoice.cc ptlist.cc \
gc.cc xaminegc.cc gcmgr.cc Tab.c griprint.cc mapcoord.cc  \
wysiwygPrint.cc defaultfile_lex.cc \
defaultfile_parse.cc \
grobfile_lex.cc \
grobfile_parse.cc \
grobfile.cc \
grobjdb.cc \
lexsupport.c \
dfltmgr.cc \
XamineSpectrumInterface.cc \
errormsg.cc

Xamine_CPPFLAGS = -I@top_builddir@ \
				  -DXTFUNCPROTO -DXAMINE \
				  -I@top_srcdir@ @XINCLUDES@ @MOTIF_INCLUDES@ $(HOMEDEF) $(GRIFLAGS) \
					-I@top_srcdir@/XamineParsers \
					-I@top_builddir@/XamineParsers \
					-I@top_srcdir@/Utility $(GDINCLUDES)
Xamine_CFLAGS = $(Xamine_CPPFLAGS)




colorset_SOURCES = colorset.cc $(MOTIF_SOURCES) errormsg.cc helpmenu.cc \
procede.cc colormgr.cc

#Xaminetest_SOURCES = Xaminetest.c $(CommonXamine_SOURCES)



colorset_LDADD = @XLIBSW@ @MOTIF_LIBSW@ @XTLIBSW@ -lXm -lXt -lX11 -lfl $(X11EXTRA_LDFLAGS)

#Xaminetest_LDFLAGS = -L.

Xamine_DEPENDENCIES = menusetup.o exit.o helpmenu.o winiomenu.o \
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
	grobjdb.o  XMCallback.o XMMenus.o XMDialogs.o XMManagers.o windlex.h \
	XMWlist.o XMShell.o XMTab.o XMText.o colorset.ico libXamine.la 

#libXamineParsers.la

#Xaminetest_DEPENDENCIES = libXamine.la


Xamine_LDFLAGS=  -g
Xamine_LDADD=  @top_builddir@/XamineParsers/libXamineParsers.la @XLIBSW@ @MOTIF_LIBSW@ @XTLIBSW@ @GDLDSW@  \
		@TK_LIBS@ @TCL_LIBS@ 						\
			-lXm -lXt -lX11 -lm -lfl $(X11EXTRA_LDFLAGS)
#Xaminetest_LDADD = $(Xamine_LDADD)


EXTRA_DIST = queue.cc colorset.ico \
	Xamine.ico Xamine.doc Xamine2.ctbl \
	Xamine3.ctbl Xamine4.ctbl Xamine5.ctbl Xamine6.ctbl Xaminedemo.doc \
	defaultfile.y grobfile.y defaultfile.l \
	grobfile.l 
#  These files can cause problems on rebuilds.

MOSTLYCLEANFILES = defaultfile_lex.cc defaultfile_lex.h grobfile_lex.cc \
	grobfile_lex.h               \
	defaultfile_parse.cc grobfile_parse.cc  windlex.h \
	lex.yy.c
