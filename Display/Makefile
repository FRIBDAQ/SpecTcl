#
#   Make file for the display program.
#
#
#   The definition below is the default destination directory tree root for
#   the INSTALL Target.  It can be overriden on the command line using
#   DESTDIR="someotherdir"
#
DESTDIR = $(INSTDIR)
HOMEDEF = -DHOME='"$(DESTDIR)"'
#
#  MOTIFINCLUDES may need to be overridden. This is correct for Linux.
#
MOTIFINCLUDES=-I/usr/X11/include

CCFLAGS= -c -w -I. -DXTFUNCPROTO $(MOTIFINCLUDES) $(HOMEDEF) -D$(OS)
XCCFLAGS=-c -w -I. -DXTFUNCPROTO -DXAMINE $(MOTIFINCLUDES) $(HOMEDEF) -D$(OS)
CC      = gcc  -g
CPP     = g++  -g
LD     = g++  -L/usr/X11/lib   
YACC   = bison
LEX    = flex
LEXOUT = lex.yy.c
F77    = echo


.DEFAULT:
	sccs get $<

system: XAMINE_FILES WINDOW_FILE  GROBJ_FILE  DISP_SHARED \
	MOTIF_LIB COLORSET Xamine CLIENT
	echo "System rebuild complete"

depend:
	makedepend $(MOTIFINCLUDES) -I. *.cc

clean:
	rm -rf *.o *.a Xamine *_lex.cc *_parse.cc *_lex.h

install:
	cp aedpublic.for $(DESTDIR)/Include/aedpublic.for
	cp aedshare.for xamineshare.for $(DESTDIR)/Include
	cp client.h $(DESTDIR)/Include/Xamine.h
	cp clientops.h $(DESTDIR)/Include/clientops.h
	cp clientgates.h $(DESTDIR)/Include/clientgates.h
	strip Xamine
	strip colorset
	cp Xamine $(DESTDIR)/Bin/Xamine2_0
	cp colorset $(DESTDIR)/Bin/colorset
	chmod a+x $(DESTDIR)/Bin/Xamine2_0 $(DESTDIR)/Bin/colorset
	cp libXamine.a $(DESTDIR)/Lib
	cp *.ctbl $(DESTDIR)/Etc
	cp *.ico  $(DESTDIR)/Etc
	echo "Xamine installation complete"

WINDOW_FILE: windfile_parse.o windfile_lex.o dispwind.o defaultfile_lex.o
	touch WINDOW_FILE
	echo "Window file I/O subsystem built"

WINDOW_TEST: wintest
	echo "Windows file test suite finished"
	touch WINDOW_TEST

XAMINE_FILES: menusetup.o exit.o helpmenu.o winiomenu.o grobjiomenu.o geometrymenu.o \
	errormsg.o procede.o logging.o toggles.o specchoose.o spectra.o \
	spcdisplay.o advprompt.o properties.o buttonsetup.o panemgr.o \
	pkgmgr.o dfltmgr.o axisdefault.o titledefault.o attribdefault.o \
	reducedefault.o rend1default.o rend2default.o printer.o refreshctl.o \
	axes.o scaling.o text.o titles.o chanplot.o plot1d.o plot2d.o \
	colormgr.o shared.o location.o convert.o trackcursor.o expand.o \
	select2.o info.o refreshdefault.o defaultfile_parse.o marker.o \
	grafinput.o grobjinput.o grobjmgr.o grobjdisplay.o  sumregion.o \
	integrate.o guintegrate.o delgrob.o messages.o eventflags.o \
	gateio.o acceptgates.o clirequests.o spccheck.o prccheck.o superpos.o \
	superprompt.o objcopy.o lblfont.o checklist.o compatspec.o copyatr.o \
	buttonreq.o buttondlg.o spcchoice.o spccompatchoice.o spclimchoice.o \
  	ptlist.o lexsupport.o xaminegc.o gcmgr.o copier.o gc.o 
	echo "Xamine objects built"
	touch XAMINE_FILES

wintest: wintest.cc WINDOW_FILE spectra.o
	$(CPP) $(XCCFLAGS) wintest.cc
	$(LD) -o wintest wintest.o dispwind.o windfile_parse.o windfile_lex.o \
	spectra.o \
	-ly -lfl 

dispwind.o: dispwind.cc dispwind.h superpos.h errormsg.h panemgr.h
	$(CPP) $(XCCFLAGS) dispwind.cc

windfile_parse.o: windfile.y  dispwind.h superpos.h dispshare.h windio.h
	$(YACC) -p windfile -b windfile windfile.y
	mv windfile.tab.c windfile_parse.cc
	$(CPP) $(XCCFLAGS)  windfile_parse.cc

defaultfile_parse.o: defaultfile.y dispwind.h superpos.h dispshare.h dfltmgr.h windio.h
	$(YACC) -p defaultfile -b defaultfile defaultfile.y
	mv defaultfile.tab.c defaultfile_parse.cc
	$(CPP) $(XCCFLAGS) defaultfile_parse.cc

windfile_lex.h: windfile.y windio.h
	$(YACC) -p windfile -b windfile -dl  windfile.y
	mv windfile.tab.h windfile_lex.h

windfile_lex.o: windfile.l dispwind.h superpos.h windfile_lex.h windio.h
	$(LEX) windfile.l
	sed s/yy/windfile/g $(LEXOUT) >windfile_lex.cc
	cp windfile_lex.h windlex.h
	$(CPP) $(XCCFLAGS) windfile_lex.cc

GROBJ_FILE: dispgrob.o grobfile_lex.o grobfile_parse.o grobjdb.o grobfile.o
	echo "GROBJ_FILE rebuilt"
	touch GROBJ_FILE

dispgrob.o: dispgrob.cc dispgrob.h
	$(CPP) $(XCCFLAGS) dispgrob.cc

grobjdb.o: grobjdb.cc dispgrob.h
	$(CPP) $(XCCFLAGS) grobjdb.cc

grobfile_lex.o: grobfile.l dispgrob.h grobjio.h grobfile_lex.h 
	$(LEX) grobfile.l
	sed s/yy/grobjfile/g $(LEXOUT) >grobfile_lex.cc
	$(CPP) $(XCCFLAGS) grobfile_lex.cc

grobfile_parse.o: grobfile.y dispgrob.h grobjio.h
	$(YACC) -p grobjfile -b grobfile grobfile.y
	mv grobfile.tab.c  grobfile_parse.cc
	$(CPP) $(XCCFLAGS) grobfile_parse.cc

grobfile_lex.h: grobfile.y grobjio.h
	$(YACC) -p grobjfile -d grobfile.y
	mv grobfile.tab.h  grobfile_lex.h

grobfile.o: grobfile.cc dispgrob.h grobjio.h panemgr.h errormsg.h
	$(CPP) $(XCCFLAGS) grobfile.cc

GROBJ_TEST: grobjtest
	echo "Graphical object tests built"
	touch GROBJ_TEST

grobjtest: grobjtest.cc GROBJ_FILE
	$(CPP) $(XCCFLAGS) grobjtest.cc
	$(LD) -o grobjtest grobjtest.o dispgrob.o grobfile_lex.o \
		grobfile_parse.o grobjdb.o grobfile.o -ly -lfl


DISP_SHARED: spectra.o
	echo "Spectrum access package rebuilt"
	touch DISP_SHARED

spectra.o: spectra.cc dispshare.h
	$(CPP) $(XCCFLAGS) spectra.cc

MOTIF_LIB: XMcallback.o XMMenus.o XMDialogs.o XMManagers.o XMWlist.o \
	XMShell.o copier.o
	touch MOTIF_LIB
	echo "MOTIF Library built"

XMcallback.o: XMcallback.cc XMWidget.h
	$(CPP) $(XCCFLAGS) XMcallback.cc

XMMenus.o: XMMenus.cc XMWidget.h XMPushbutton.h XMMenus.h
	$(CPP) $(XCCFLAGS) XMMenus.cc

Xamine: Xamine.o \
	 MOTIF_LIB \
	 WINDOW_FILE \
	 XAMINE_FILES \
	GROBJ_FILE
	$(LD) -o Xamine Xamine.o XMcallback.o XMMenus.o XMDialogs.o \
	XMManagers.o XMWlist.o XMShell.o \
	windfile_parse.o windfile_lex.o dispwind.o \
	menusetup.o exit.o helpmenu.o winiomenu.o grobjiomenu.o \
	geometrymenu.o errormsg.o logging.o toggles.o spectra.o specchoose.o \
	spcdisplay.o advprompt.o properties.o panemgr.o buttonsetup.o pkgmgr.o \
	dfltmgr.o axisdefault.o titledefault.o attribdefault.o reducedefault.o \
	rend1default.o rend2default.o printer.o refreshctl.o axes.o scaling.o \
	text.o titles.o chanplot.o plot1d.o  plot2d.o colormgr.o shared.o \
	location.o convert.o trackcursor.o expand.o select2.o info.o \
	dispgrob.o grobfile_lex.o grobfile_parse.o grobjdb.o grobfile.o \
	refreshdefault.o defaultfile_parse.o marker.o grafinput.o \
	grobjinput.o grobjmgr.o grobjdisplay.o sumregion.o integrate.o \
	guintegrate.o delgrob.o messages.o eventflags.o \
	gateio.o acceptgates.o clirequests.o \
	prccheck.o spccheck.o superpos.o superprompt.o objcopy.o copier.o \
	lblfont.o checklist.o compatspec.o copyatr.o \
	defaultfile_lex.o buttonreq.o buttondlg.o spcchoice.o \
	spccompatchoice.o spclimchoice.o ptlist.o lexsupport.o gc.o xaminegc.o gcmgr.o \
	-lfl -lXm -lXt -lX11 -lm

Xamine.o: Xamine.cc XMPushbutton.h XMWidget.h  dispshare.h XMManagers.h \
	panemgr.h pkgmgr.h optionmenu.h colormgr.h \
	menusetup.h dfltmgr.h clirequests.h prccheck.h spccheck.h
	$(CPP) $(XCCFLAGS) Xamine.cc

XMDialogs.o: XMDialogs.cc XMDialogs.h XMWidget.h
	$(CPP) $(XCCFLAGS) XMDialogs.cc


menusetup.o: menusetup.cc XMMenus.h exit.h winiomenu.h grobjiomenu.h \
	geometrymenu.h toggles.h logging.h specchoose.h properties.h \
	menusetup.h dispshare.h helpmenu.h winiomenu.h spcdisplay.h \
	advprompt.h panemgr.h pkgmgr.h optionmenu.h printer.h refreshctl.h \
	expand.h info.h XMWidget.h XMPushbutton.h XMManagers.h XMDialogs.h \
	XMText.h XMLabel.h XMScale.h marker.h dispwind.h superpos.h sumregion.h \
	dispgrob.h convert.h guintegrate.h delgrob.h XMList.h grobjinput.h \
	acceptgates.h XMSeparators.h grafinput.h errormsg.h trackcursor.h \
	location.h superprompt.h
	$(CPP) $(XCCFLAGS) menusetup.cc

exit.o: exit.cc XMDialogs.h panemgr.h helpmenu.h exit.h winiomenu.h \
	XMWidget.h dispwind.h
	$(CPP) $(XCCFLAGS) exit.cc

helpmenu.o: helpmenu.cc helpmenu.h XMDialogs.h XMWidget.h XMPushbutton.h \
	XMManagers.h
	$(CPP) $(XCCFLAGS) helpmenu.cc


winiomenu.o: winiomenu.cc XMDialogs.h helpmenu.h winiomenu.h panemgr.h \
	dispwind.h superpos.h exit.h refreshctl.h XMWidget.h XMPushbutton.h XMManagers.h \
	errormsg.h 
	$(CPP) $(XCCFLAGS) winiomenu.cc


grobjiomenu.o: grobjiomenu.cc grobjiomenu.h XMDialogs.h helpmenu.h \
	XMWidget.h XMPushbutton.h XMManagers.h panemgr.h grobjmgr.h \
	dispgrob.h errormsg.h winiomenu.h
	$(CPP) $(XCCFLAGS) grobjiomenu.cc


geometrymenu.o: geometrymenu.cc XMLabel.h XMDialogs.h helpmenu.h \
	errormsg.h geometrymenu.h menusetup.h XMPushbutton.h XMManagers.h \
	XMSeparators.h exit.h XMWidget.h
	$(CPP) $(XCCFLAGS) geometrymenu.cc

errormsg.o: errormsg.cc XMDialogs.h XMWidget.h
	$(CPP) $(XCCFLAGS) errormsg.cc

COLORSET: colorset colorset.ico
	touch COLORSET
	echo "colorset - color map generation tool rebuilt."

colorset: colorset.o MOTIF_LIB errormsg.o helpmenu.o procede.o colormgr.o
	$(LD)  -o colorset colorset.o errormsg.o helpmenu.o procede.o \
	XMDialogs.o XMMenus.o XMcallback.o colormgr.o -lXm -lXt -lX11

colorset.o: colorset.cc XMDialogs.h XMWidget.h XMPushbutton.h XMMenus.h \
	helpmenu.h errormsg.h procede.h colormgr.h
	$(CPP) $(XCCFLAGS) colorset.cc

procede.o: procede.cc procede.h XMDialogs.h XMWidget.h
	$(CPP) $(XCCFLAGS) procede.cc

logging.o: logging.cc logging.h XMDialogs.h helpmenu.h errormsg.h XMWidget.h
	$(CPP) $(XCCFLAGS) logging.cc

toggles.o: toggles.cc toggles.h XMWidget.h
	$(CPP) $(XCCFLAGS) toggles.cc

specchoose.o: specchoose.cc specchoose.h XMDialogs.h helpmenu.h errormsg.h \
	spcdisplay.h XMWidget.h
	$(CPP) $(XCCFLAGS) specchoose.cc

spcdisplay.o: spcdisplay.cc spcdisplay.h dispwind.h superpos.h panemgr.h dfltmgr.h \
	      refreshctl.h dfltmgr.h exit.h pkgmgr.h superpos.h errormsg.h
	$(CPP) $(XCCFLAGS) spcdisplay.cc

advprompt.o: advprompt.cc XMDialogs.h helpmenu.h advprompt.h spcdisplay.h \
	errormsg.h XMWidget.h
	$(CPP) $(XCCFLAGS)  advprompt.cc

properties.o: properties.cc XMWidget.h XMDialogs.h XMManagers.h helpmenu.h \
	errormsg.h properties.h spcdisplay.h dispwind.h superpos.h optionmenu.h \
	panemgr.h refreshctl.h colormgr.h exit.h XMText.h XMScale.h
	$(CPP) $(XCCFLAGS) properties.cc

XMManagers.o: XMManagers.cc XMManagers.h XMWidget.h
	$(CPP) $(XCCFLAGS) XMManagers.cc

panemgr.o: panemgr.cc XMManagers.h XMWidget.h panemgr.h menusetup.h \
	buttonsetup.h pkgmgr.h dispshare.h trackcursor.h colormgr.h
	$(CPP) $(XCCFLAGS) panemgr.cc

buttonsetup.o: buttonsetup.cc XMManagers.h XMPushbutton.h buttonsetup.h \
	geometrymenu.h panemgr.h specchoose.h spcdisplay.h pkgmgr.h \
	refreshctl.h location.h expand.h info.h marker.h sumregion.h \
	dispgrob.h convert.h XMWidget.h guintegrate.h XMList.h grafinput.h \
	trackcursor.h
	$(CPP) $(XCCFLAGS) buttonsetup.cc

XMWlist.o: XMWlist.cc XMWlist.h XMWidget.h XMPushbutton.h
	$(CPP) $(XCCFLAGS) XMWlist.cc

pkgmgr.o: pkgmgr.cc XMWlist.h pkgmgr.h XMWidget.h
	$(CPP) $(XCCFLAGS) pkgmgr.cc

dfltmgr.o: dfltmgr.cc dfltmgr.h dispwind.h superpos.h XMWidget.h panemgr.h refreshctl.h
	$(CPP) $(XCCFLAGS) dfltmgr.cc

axisdefault.o: axisdefault.cc  XMPushbutton.h XMDialogs.h XMWidget.h \
	helpmenu.h dispwind.h superpos.h dfltmgr.h optionmenu.h XMText.h XMScale.h \
	panemgr.h refreshctl.h	
	$(CPP) $(XCCFLAGS) axisdefault.cc

titledefault.o: titledefault.cc XMPushbutton.h  XMDialogs.h XMWidget.h \
	helpmenu.h dispwind.h superpos.h dfltmgr.h optionmenu.h XMText.h XMScale.h \
	refreshctl.h panemgr.h
	$(CPP) $(XCCFLAGS) titledefault.cc

attribdefault.o: attribdefault.cc XMDialogs.h XMPushbutton.h XMLabel.h \
	XMText.h dispwind.h superpos.h dfltmgr.h optionmenu.h helpmenu.h XMScale.h \
	XMWidget.h
	$(CPP) $(XCCFLAGS) attribdefault.cc

reducedefault.o: reducedefault.cc XMDialogs.h XMPushbutton.h XMManagers.h \
	dispwind.h superpos.h dfltmgr.h optionmenu.h helpmenu.h XMScale.h XMWidget.h \
	refreshctl.h panemgr.h
	$(CPP) $(XCCFLAGS) reducedefault.cc
rend1default.o: rend1default.cc XMDialogs.h XMPushbutton.h XMManagers.h \
	optionmenu.h dfltmgr.h dispwind.h superpos.h helpmenu.h XMScale.h \
	refreshctl.h panemgr.h
	$(CPP) $(XCCFLAGS) rend1default.cc

rend2default.o: rend2default.cc XMDialogs.h XMPushbutton.h XMManagers.h \
	optionmenu.h dfltmgr.h dispwind.h superpos.h helpmenu.h colormgr.h XMScale.h \
	XMWidget.h refreshctl.h panemgr.h
	$(CPP) $(XCCFLAGS) rend2default.cc

printer.o: printer.cc XMDialogs.h XMManagers.h XMPushbutton.h XMText.h  \
	printer.h XMWidget.h
	$(CPP) $(XCCFLAGS) printer.cc

refreshctl.o: refreshctl.cc refreshctl.h XMWidget.h panemgr.h axes.h titles.h \
	chanplot.h dfltmgr.h scaling.h dispshare.h grobjdisplay.h XMWidget.h \
	queue.h queue.cc
	$(CPP) $(XCCFLAGS) refreshctl.cc

axes.o: axes.cc axes.h XMWidget.h dispwind.h superpos.h dispshare.h \
	XBatch.h text.h refreshctl.h
	$(CPP) $(XCCFLAGS) axes.cc

scaling.o: scaling.cc scaling.h XMWidget.h dispwind.h superpos.h scaling.h dispshare.h \
	axes.h
	$(CPP) $(XCCFLAGS) scaling.cc

text.o: text.cc text.h
	$(CPP) $(XCCFLAGS) text.cc

titles.o: titles.cc dispshare.h titles.h axes.h panemgr.h text.h refreshctl.h
	$(CPP) $(XCCFLAGS) titles.cc

chanplot.o: chanplot.cc refreshctl.h dispwind.h superpos.h axes.h scaling.h chanplot.h \
	panemgr.h XMWidget.h colormgr.h superpos.h
	$(CPP) $(XCCFLAGS) chanplot.cc

plot1d.o: plot1d.cc XMWidget.h XBatch.h chanplot.h panemgr.h  \
	dispshare.h dispwind.h
	$(CPP) $(XCCFLAGS) plot1d.cc

plot2d.o: plot2d.cc XMWidget.h XBatch.h dispwind.h superpos.h dispshare.h chanplot.h \
	scaling.h colormgr.h refreshctl.h
	$(CPP) $(XCCFLAGS)  plot2d.cc

colormgr.o: colormgr.cc XMWidget.h colormgr.h
	$(CPP) $(XCCFLAGS) colormgr.cc

CLIENT: libXamine.a Xaminetest demo
	echo "Client software and tests built"
	touch CLIENT

libXamine.a: client.o  clientgates.o prccheck.o allocator.o \
	specalloc.o clientbuttons.o
	rm -f libXamine.a 
	ar cq libXamine.a client.o  clientgates.o prccheck.o \
	allocator.o specalloc.o clientbuttons.o
	ranlib libXamine.a

Xaminetest: Xaminetest.o libXamine.a
	$(LD) -g -L. -o Xaminetest Xaminetest.o client.o -lXamine -lm

Xaminetest.o: Xaminetest.c client.h
	$(CC) $(CCFLAGS) Xaminetest.c

client.o: client.c client.h clientgates.h allocator.h
	$(CC) -g $(CCFLAGS) client.c

shared.o: shared.cc dispshare.h
	$(CPP) $(CCFLAGS) shared.cc

aedxamine.o: aedxamine.for aedshare.for xamineshare.for
	$(F77) -c -g aedxamine.for

demo: demo.o libXamine.a
#	$(F77) -g -L. -L/daq/lib -o demo demo.o -lXamine -lc -lqtree -lfile -lcvt
	$(F77) -g  -L. -L/daq/lib -o demo demo.o \
	 -lXamine  -lqtree -lfile -lcvt

demo.o: demo.for aedshare.for aedpublic.for
	$(F77) -g -c demo.for


location.o: location.cc XMManagers.h XMLabel.h location.h XMWidget.h
	$(CPP) $(XCCFLAGS) location.cc

convert.o: convert.cc convert.h XMWidget.h dispwind.h superpos.h axes.h
	$(CPP) $(XCCFLAGS) convert.cc

trackcursor.o: trackcursor.cc XMWidget.h trackcursor.h buttonsetup.h \
	location.h panemgr.h dispwind.h superpos.h dispshare.h convert.h
	$(CPP) $(XCCFLAGS) trackcursor.cc

expand.o: expand.cc XMWidget.h XMDialogs.h panemgr.h helpmenu.h expand.h \
	refreshctl.h exit.h errormsg.h select2.h dispshare.h panemgr.h \
	convert.h colormgr.h grafinput.h
	$(CPP) $(XCCFLAGS) expand.cc

select2.o: select2.cc XMDialogs.h XMLabel.h XMText.h XMPushbutton.h \
	helpmenu.h select2.h XMWidget.h 
	$(CPP) $(XCCFLAGS) select2.cc

info.o: info.cc XMWidget.h XMDialogs.h XMText.h \
	info.h logging.h dispshare.h panemgr.h errormsg.h dispwind.h superpos.h \
	dispgrob.h grobjmgr.h
	$(CPP) $(XCCFLAGS) info.cc

refreshdefault.o: refreshdefault.cc XMScale.h dfltmgr.h optionmenu.h  \
	helpmenu.h dispwind.h superpos.h XMWidget.h panemgr.h refreshctl.h
	$(CPP) $(XCCFLAGS) refreshdefault.cc

marker.o: marker.cc marker.h XMWidget.h XMText.h XMLabel.h XMPushbutton.h \
	XBatch.h errormsg.h grafinput.h grobjinput.h dispgrob.h panemgr.h \
	dispshare.h colormgr.h convert.h grobjmgr.h
	$(CPP) $(XCCFLAGS) marker.cc

grafinput.o: grafinput.cc XMDialogs.h grafinput.h helpmenu.h panemgr.h \
	refreshctl.h convert.h dispshare.h XMWidget.h
	$(CPP) $(XCCFLAGS) grafinput.cc

grobjinput.o: grobjinput.cc XMManagers.h XMDialogs.h XMSeparators.h \
	grobjinput.h XMWidget.h
	$(CPP) $(XCCFLAGS) grobjinput.cc

grobjmgr.o: grobjmgr.cc dispgrob.h grobjmgr.h
	$(CPP) $(XCCFLAGS) grobjmgr.cc

grobjdisplay.o: grobjdisplay.cc XMWidget.h panemgr.h grobjdisplay.h \
	dispwind.h superpos.h text.h dispgrob.h dispshare.h errormsg.h convert.h \
	colormgr.h grobjmgr.h XBatch.h sumregion.h chanplot.h acceptgates.h \
	XMList.h
	$(CPP) $(XCCFLAGS) grobjdisplay.cc

sumregion.o: sumregion.cc XMWidget.h XMPushbutton.h XMLabel.h XMText.h \
	XMList.h dispgrob.h dispwind.h superpos.h dispshare.h grobjmgr.h sumregion.h \
	panemgr.h convert.h errormsg.h grobjinput.h colormgr.h chanplot.h \
	grobjdisplay.h XBatch.h
	$(CPP) $(XCCFLAGS) sumregion.cc

integrate.o: integrate.cc integrate.h dispshare.h dispgrob.h
	$(CPP) $(XCCFLAGS) integrate.cc

guintegrate.o: guintegrate.cc XMDialogs.h XMText.h guintegrate.h integrate.h \
	logging.h dispshare.h dispgrob.h grobjmgr.h XMWidget.h panemgr.h \
	errormsg.h
	$(CPP) $(XCCFLAGS) guintegrate.cc

delgrob.o: delgrob.cc XMWidget.h XMDialogs.h dispgrob.h dispwind.h superpos.h \
	helpmenu.h delgrob.h grobjmgr.h panemgr.h errormsg.h
	$(CPP) $(XCCFLAGS) delgrob.cc

messages.o: messages.cc eventflags.h XMWidget.h messages.h
	$(CPP) $(XCCFLAGS) -DXAMINE  messages.cc

eventflags.o: eventflags.cc eventflags.h
	$(CPP) $(XCCFLAGS) eventflags.cc

gateio.o: gateio.cc XMWidget.h messages.h gateio.h
	$(CPP) $(XCCFLAGS) gateio.cc

acceptgates.o: acceptgates.cc XMWidget.h XBatch.h convert.h dispgrob.h \
	acceptgates.h sumregion.h
	$(CPP) $(XCCFLAGS) acceptgates.cc

clirequests.o: clirequests.cc dispshare.h XMWidget.h errormsg.h panemgr.h \
	dispgrob.h clirequests.h gateio.h grobjmgr.h
	$(CPP) $(XCCFLAGS) clirequests.cc

clientgates.o: clientgates.c clientgates.h client.h messages.h
	$(CC) -g $(CCFLAGS) clientgates.c


spccheck.o: spccheck.cc spccheck.h panemgr.h dispwind.h superpos.h refreshctl.h \
	dispshare.h
	$(CPP) $(XCCFLAGS) spccheck.cc

prccheck.o: prccheck.c prccheck.h
	$(CC) $(XCCFLAGS) prccheck.c

superpos.o: superpos.cc superpos.h
	$(CPP) $(XCCFLAGS) superpos.cc

superprompt.o: superprompt.cc XMDialogs.h superprompt.h dispwind.h superpos.h \
	spcdisplay.h dispshare.h helpmenu.h panemgr.h errormsg.h specchoose.h
	$(CPP) $(XCCFLAGS) superprompt.cc

allocator.o: allocator.c allocator.h
	$(CC) $(XCCFLAGS) allocator.c

specalloc.o: specalloc.c client.h
	$(CC) $(XCCFLAGS) specalloc.c


copier.o: copier.cc copier.h XMPushbutton.h XMManagers.h XMList.h XMLabel.h
	$(CPP) $(XCCFLAGS) copier.cc

objcopy.o: objcopy.cc XMDialogs.h panemgr.h dispshare.h objcopy.h dispgrob.h \
	   grobjmgr.h gateio.h helpmenu.h compatspec.h copier.h clirequests.h
	$(CPP) $(XCCFLAGS) objcopy.cc

lblfont.o: lblfont.cc XMDialogs.h XMPushbutton.h XMLabel.h refreshctl.h \
	grobjdisplay.h helpmenu.h lblfont.h dfltmgr.h text.h
	$(CPP) $(XCCFLAGS) lblfont.cc

checklist.o: checklist.cc XMManagers.h XMPushbutton.h XMLabel.h checklist.h
	$(CPP) $(XCCFLAGS) checklist.cc

compatspec.o: compatspec.cc compatspec.h dispshare.h XMManagers.h XMList.h \
	XMPushbutton.h panemgr.h
	$(CPP) $(XCCFLAGS) compatspec.cc

copyatr.o: copyatr.cc dispwind.h panemgr.h checklist.h compatspec.h \
	superpos.h XMDialogs.h XMManagers.h XMCallback.h compatspec.h \
	errormsg.h helpmenu.h refreshctl.h XMCallback.cc
	$(CPP) $(XCCFLAGS) copyatr.cc

clientbuttons.o: clientbuttons.c client.h clientops.h XMWidget.h
	$(CC) $(XCCFLAGS) clientbuttons.c 

buttondlg.o: buttondlg.cc XMManagers.h XMPushbutton.h XMShell.h buttondlg.h \
	XMCallback.h
	$(CPP) $(XCCFLAGS) buttondlg.cc

buttonreq.o: buttonreq.cc XMCallback.h XMShell.h XMPushbutton.h errormsg.h \
	clientops.h buttonreq.h messages.h gateio.h buttondlg.h panemgr.h \
	spcchoice.h client.h ptlist.h XMCallback.cc
	$(CPP) $(XCCFLAGS) buttonreq.cc

XMShell.o: XMShell.cc XMShell.h
	$(CPP) $(XCCFLAGS) XMShell.cc

spcchoice.o: spcchoice.cc errormsg.h dispshare.h spcchoice.h
	$(CPP) $(XCCFLAGS) spcchoice.cc

spccompatchoice.o:  spccompatchoice.cc dispshare.h spcchoice.h dispwind.h \
	panemgr.h
	$(CPP) $(XCCFLAGS) spccompatchoice.cc

spclimchoice.o: spclimchoice.cc dispshare.h spcchoice.h
	$(CPP) $(XCCFLAGS) spclimchoice.cc

ptlist.o: ptlist.cc errormsg.h dispgrob.h ptlist.h panemgr.h XMWidget.h \
	sumregion.h
	$(CPP) $(XCCFLAGS) ptlist.cc

Xwarning:	Xwarning.o MOTIF_LIB
	$(LD) $(LDFLAGS) -o Xwarning Xwarning.o  XMMenus.o \
	XMDialogs.o XMManagers.o XMWlist.o copier.o XMShell.o \
	errormsg.o helpmenu.o XMcallback.o \
	-lXm -lXt -lX11

Xwarning.o:	Xwarning.cc XMWidget.h XMManagers.h XMLabel.h XMPushbutton.h
	$(CPP) $(XCCFLAGS) Xwarning.cc

lexsupport.o:	lexsupport.c
	$(CC) $(CCFLAGS) lexsupport.c

f77test: f77test.f xamineshare_unix.for
	rm -f xamineshare.for
	cp xamineshare_unix.for xamineshare.for
	f77 -o f77test f77test.f -L. -lXamine


gc.o: gc.cc gc.h
	$(CPP) $(XCCFLAGS) gc.cc

xaminegc.o: xaminegc.cc gc.h xaminegc.h text.h colormgr.h chanplot.h
	$(CPP) $(XCCFLAGS) xaminegc.cc

gcmgr.o: gcmgr.cc xaminegc.h gc.h gcmgr.h grobjdisplay.h XMWidget.h convert.h \
	dispgrob.h dispwind.h refreshctl.h
	$(CPP) $(XCCFLAGS) gcmgr.cc

defaultfile_lex.h: defaultfile.y windio.h
	$(YACC) -p defaultfile -b defaultfile -dl defaultfile.y
	mv defaultfile.tab.h defaultfile_lex.h

defaultfile_lex.o: windfile.l dispwind.h superpos.h defaultfile_lex.h windio.h
	$(LEX) windfile.l
	sed s/yy/defaultfile/g $(LEXOUT) >defaultfile_lex.cc
	cp defaultfile_lex.h windlex.h
	$(CPP) $(XCCFLAGS) -DDEFAULTFILE defaultfile_lex.cc
# DO NOT DELETE

XMCallback.o: /usr/include/errno.h /usr/include/features.h
XMCallback.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XMCallback.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
XMCallback.o: /usr/include/asm/errno.h /usr/include/stdio.h
XMCallback.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XMCallback.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
XMCallback.o: /usr/include/bits/types.h /usr/include/libio.h
XMCallback.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
XMCallback.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
XMCallback.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
XMCallback.o: /usr/include/time.h /usr/include/endian.h
XMCallback.o: /usr/include/bits/endian.h /usr/include/sys/select.h
XMCallback.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
XMCallback.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
XMCallback.o: /usr/X11/include/X11/Xfuncproto.h
XMCallback.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
XMCallback.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
XMCallback.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
XMCallback.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
XMCallback.o: /usr/X11/include/X11/RectObj.h
XMCallback.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
XMCallback.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
XMCallback.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
XMCallback.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
XMCallback.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
XMDialogs.o: helpmenu.h XMDialogs.h /usr/include/stdio.h
XMDialogs.o: /usr/include/features.h /usr/include/sys/cdefs.h
XMDialogs.o: /usr/include/gnu/stubs.h
XMDialogs.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XMDialogs.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
XMDialogs.o: /usr/include/bits/types.h /usr/include/libio.h
XMDialogs.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
XMDialogs.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/MessageB.h
XMDialogs.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
XMDialogs.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
XMDialogs.o: /usr/include/time.h /usr/include/endian.h
XMDialogs.o: /usr/include/bits/endian.h /usr/include/sys/select.h
XMDialogs.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
XMDialogs.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
XMDialogs.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
XMDialogs.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
XMDialogs.o: /usr/include/string.h /usr/X11/include/X11/Core.h
XMDialogs.o: /usr/X11/include/X11/Composite.h
XMDialogs.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
XMDialogs.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
XMDialogs.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
XMDialogs.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
XMDialogs.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
XMDialogs.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/SelectioB.h
XMDialogs.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
XMDialogs.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
XMDialogs.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
XMDialogs.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
XMDialogs.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
XMDialogs.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
XMDialogs.o: /usr/X11/include/Xm/Frame.h XMCallback.h errormsg.h
XMDialogs.o: /usr/X11/include/Xm/MwmUtil.h /usr/X11/include/X11/Xmd.h
XMDialogs.o: /usr/X11/include/Xm/DialogS.h /usr/include/assert.h
XMDialogs.o: /usr/include/ctype.h
XMManagers.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
XMManagers.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
XMManagers.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
XMManagers.o: /usr/include/features.h /usr/include/sys/cdefs.h
XMManagers.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
XMManagers.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XMManagers.o: /usr/include/time.h /usr/include/endian.h
XMManagers.o: /usr/include/bits/endian.h /usr/include/sys/select.h
XMManagers.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
XMManagers.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
XMManagers.o: /usr/X11/include/X11/Xfuncproto.h
XMManagers.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
XMManagers.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
XMManagers.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
XMManagers.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
XMManagers.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
XMManagers.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
XMManagers.o: /usr/X11/include/X11/ICE/ICElib.h
XMManagers.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
XMManagers.o: /usr/X11/include/Xm/XmStrDefs.h
XMManagers.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
XMManagers.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
XMManagers.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
XMManagers.o: /usr/X11/include/Xm/Frame.h XMWidget.h
XMMenus.o: /usr/include/stdio.h /usr/include/features.h
XMMenus.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XMMenus.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XMMenus.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
XMMenus.o: /usr/include/bits/types.h /usr/include/libio.h
XMMenus.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
XMMenus.o: /usr/X11/include/Xm/Separator.h /usr/X11/include/Xm/Xm.h
XMMenus.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
XMMenus.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
XMMenus.o: /usr/include/bits/endian.h /usr/include/sys/select.h
XMMenus.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
XMMenus.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
XMMenus.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
XMMenus.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
XMMenus.o: /usr/include/string.h /usr/X11/include/X11/Core.h
XMMenus.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
XMMenus.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
XMMenus.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
XMMenus.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
XMMenus.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
XMMenus.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/X11/StringDefs.h
XMMenus.o: /usr/X11/include/Xm/VendorS.h XMMenus.h XMWidget.h XMPushbutton.h
XMMenus.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
XMMenus.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
XMMenus.o: /usr/X11/include/Xm/RowColumn.h
XMShell.o: XMShell.h /usr/X11/include/Xm/Xm.h
XMShell.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
XMShell.o: /usr/include/sys/types.h /usr/include/features.h
XMShell.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XMShell.o: /usr/include/bits/types.h
XMShell.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XMShell.o: /usr/include/time.h /usr/include/endian.h
XMShell.o: /usr/include/bits/endian.h /usr/include/sys/select.h
XMShell.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
XMShell.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
XMShell.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
XMShell.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
XMShell.o: /usr/include/string.h /usr/X11/include/X11/Core.h
XMShell.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
XMShell.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
XMShell.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
XMShell.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
XMShell.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
XMShell.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/X11/StringDefs.h
XMShell.o: /usr/X11/include/Xm/VendorS.h XMWidget.h XMCallback.h
XMShell.o: /usr/include/stdio.h
XMShell.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
XMShell.o: /usr/include/libio.h /usr/include/_G_config.h
XMShell.o: /usr/include/bits/stdio_lim.h
XMWlist.o: /usr/include/stdio.h /usr/include/features.h
XMWlist.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XMWlist.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XMWlist.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
XMWlist.o: /usr/include/bits/types.h /usr/include/libio.h
XMWlist.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h XMWlist.h
XMWlist.o: /usr/include/errno.h /usr/include/bits/errno.h
XMWlist.o: /usr/include/linux/errno.h /usr/include/asm/errno.h XMWidget.h
XMWlist.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
XMWlist.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
XMWlist.o: /usr/include/bits/endian.h /usr/include/sys/select.h
XMWlist.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
XMWlist.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
XMWlist.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
XMWlist.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
XMWlist.o: /usr/include/string.h /usr/X11/include/X11/Core.h
XMWlist.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
XMWlist.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
XMWlist.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
XMWlist.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
XMWlist.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
XMWlist.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
XMWlist.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
XMWlist.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
XMWlist.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
XMWlist.o: /usr/X11/include/Xm/ArrowB.h
XMcallback.o: /usr/include/errno.h /usr/include/features.h
XMcallback.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
XMcallback.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
XMcallback.o: /usr/include/asm/errno.h /usr/include/stdio.h
XMcallback.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
XMcallback.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
XMcallback.o: /usr/include/bits/types.h /usr/include/libio.h
XMcallback.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
XMcallback.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
XMcallback.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
XMcallback.o: /usr/include/time.h /usr/include/endian.h
XMcallback.o: /usr/include/bits/endian.h /usr/include/sys/select.h
XMcallback.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
XMcallback.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
XMcallback.o: /usr/X11/include/X11/Xfuncproto.h
XMcallback.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
XMcallback.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
XMcallback.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
XMcallback.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
XMcallback.o: /usr/X11/include/X11/RectObj.h
XMcallback.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
XMcallback.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
XMcallback.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
XMcallback.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
XMcallback.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
Xamine.o: /usr/include/stdio.h /usr/include/features.h
Xamine.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Xamine.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Xamine.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
Xamine.o: /usr/include/bits/types.h /usr/include/libio.h
Xamine.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
Xamine.o: /usr/include/string.h
Xamine.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/new.h
Xamine.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/new
Xamine.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/exception
Xamine.o: /usr/X11/include/Xm/DrawingA.h /usr/X11/include/Xm/Xm.h
Xamine.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
Xamine.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
Xamine.o: /usr/include/bits/endian.h /usr/include/sys/select.h
Xamine.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
Xamine.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
Xamine.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
Xamine.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
Xamine.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
Xamine.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
Xamine.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
Xamine.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
Xamine.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
Xamine.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
Xamine.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
Xamine.o: /usr/X11/include/Xm/ScrolledW.h XMWidget.h XMPushbutton.h
Xamine.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
Xamine.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
Xamine.o: XMMenus.h /usr/X11/include/Xm/RowColumn.h XMDialogs.h
Xamine.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/SelectioB.h
Xamine.o: /usr/X11/include/Xm/FileSB.h XMManagers.h
Xamine.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
Xamine.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
Xamine.o: /usr/X11/include/Xm/Frame.h XMCallback.h dispshare.h dispwind.h
Xamine.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/assert.h
Xamine.o: superpos.h panemgr.h menusetup.h buttonsetup.h location.h XMLabel.h
Xamine.o: /usr/X11/include/Xm/Label.h pkgmgr.h colormgr.h dfltmgr.h gateio.h
Xamine.o: dispgrob.h XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
Xamine.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
Xamine.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
Xamine.o: /usr/include/getopt.h /usr/include/sys/time.h
Xamine.o: /usr/include/bits/time.h convert.h messages.h clientops.h
Xamine.o: ./Xamine.h clirequests.h prccheck.h spccheck.h trackcursor.h
Xwarning.o: /usr/include/stdio.h /usr/include/features.h
Xwarning.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Xwarning.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
Xwarning.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
Xwarning.o: /usr/include/bits/types.h /usr/include/libio.h
Xwarning.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h XMWidget.h
Xwarning.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
Xwarning.o: /usr/include/sys/types.h /usr/include/time.h
Xwarning.o: /usr/include/endian.h /usr/include/bits/endian.h
Xwarning.o: /usr/include/sys/select.h /usr/include/bits/select.h
Xwarning.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
Xwarning.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
Xwarning.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
Xwarning.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
Xwarning.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
Xwarning.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
Xwarning.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
Xwarning.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
Xwarning.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
Xwarning.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
Xwarning.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
Xwarning.o: /usr/X11/include/Xm/VendorS.h XMManagers.h
Xwarning.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
Xwarning.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
Xwarning.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h XMLabel.h
Xwarning.o: /usr/X11/include/Xm/Label.h XMPushbutton.h
Xwarning.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
Xwarning.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
acceptgates.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
acceptgates.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
acceptgates.o: /usr/include/features.h /usr/include/sys/cdefs.h
acceptgates.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
acceptgates.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
acceptgates.o: /usr/include/time.h /usr/include/endian.h
acceptgates.o: /usr/include/bits/endian.h /usr/include/sys/select.h
acceptgates.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
acceptgates.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
acceptgates.o: /usr/X11/include/X11/Xfuncproto.h
acceptgates.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
acceptgates.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
acceptgates.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
acceptgates.o: /usr/X11/include/X11/Constraint.h
acceptgates.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
acceptgates.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
acceptgates.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
acceptgates.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
acceptgates.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
acceptgates.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
acceptgates.o: XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
acceptgates.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
acceptgates.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
acceptgates.o: /usr/include/getopt.h /usr/include/sys/time.h
acceptgates.o: /usr/include/bits/time.h /usr/include/stdio.h
acceptgates.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
acceptgates.o: /usr/include/libio.h /usr/include/_G_config.h
acceptgates.o: /usr/include/bits/stdio_lim.h convert.h dispwind.h
acceptgates.o: /usr/include/stdlib.h /usr/include/alloca.h
acceptgates.o: /usr/include/assert.h superpos.h dispshare.h dispgrob.h
acceptgates.o: acceptgates.h sumregion.h XMList.h /usr/X11/include/Xm/List.h
acceptgates.o: /usr/X11/include/Xm/ScrolledW.h XMPushbutton.h
acceptgates.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
acceptgates.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
acceptgates.o: XMLabel.h /usr/X11/include/Xm/Label.h XMText.h
acceptgates.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
acceptgates.o: grobjinput.h XMManagers.h /usr/X11/include/Xm/BulletinB.h
acceptgates.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
acceptgates.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
acceptgates.o: /usr/X11/include/Xm/Frame.h XMDialogs.h
acceptgates.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/SelectioB.h
acceptgates.o: /usr/X11/include/Xm/FileSB.h XMCallback.h XMSeparators.h
acceptgates.o: /usr/X11/include/Xm/Separator.h grafinput.h helpmenu.h
acceptgates.o: refreshctl.h panemgr.h grobjdisplay.h colormgr.h chanplot.h
acceptgates.o: grobjmgr.h errormsg.h gateio.h messages.h clientops.h
acceptgates.o: ./Xamine.h
advprompt.o: /usr/include/stdlib.h /usr/include/features.h
advprompt.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
advprompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
advprompt.o: /usr/include/sys/types.h /usr/include/bits/types.h
advprompt.o: /usr/include/time.h /usr/include/endian.h
advprompt.o: /usr/include/bits/endian.h /usr/include/sys/select.h
advprompt.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
advprompt.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h XMDialogs.h
advprompt.o: /usr/include/stdio.h
advprompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
advprompt.o: /usr/include/libio.h /usr/include/_G_config.h
advprompt.o: /usr/include/bits/stdio_lim.h /usr/X11/include/X11/StringDefs.h
advprompt.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
advprompt.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
advprompt.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
advprompt.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
advprompt.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
advprompt.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
advprompt.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
advprompt.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
advprompt.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
advprompt.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
advprompt.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
advprompt.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/SelectioB.h
advprompt.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
advprompt.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
advprompt.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
advprompt.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
advprompt.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
advprompt.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
advprompt.o: /usr/X11/include/Xm/Frame.h XMCallback.h helpmenu.h advprompt.h
advprompt.o: spcdisplay.h dispwind.h /usr/include/assert.h superpos.h
advprompt.o: errormsg.h
attribdefault.o: /usr/include/ctype.h /usr/include/features.h
attribdefault.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
attribdefault.o: /usr/include/bits/types.h
attribdefault.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
attribdefault.o: /usr/include/endian.h /usr/include/bits/endian.h
attribdefault.o: /usr/include/stdlib.h /usr/include/sys/types.h
attribdefault.o: /usr/include/time.h /usr/include/sys/select.h
attribdefault.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
attribdefault.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
attribdefault.o: /usr/include/stdio.h
attribdefault.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
attribdefault.o: /usr/include/libio.h /usr/include/_G_config.h
attribdefault.o: /usr/include/bits/stdio_lim.h XMDialogs.h
attribdefault.o: /usr/X11/include/X11/StringDefs.h
attribdefault.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
attribdefault.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
attribdefault.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
attribdefault.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
attribdefault.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
attribdefault.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
attribdefault.o: /usr/X11/include/X11/Constraint.h
attribdefault.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
attribdefault.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
attribdefault.o: /usr/X11/include/X11/SM/SM.h
attribdefault.o: /usr/X11/include/X11/ICE/ICElib.h
attribdefault.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
attribdefault.o: /usr/X11/include/Xm/XmStrDefs.h
attribdefault.o: /usr/X11/include/Xm/VendorS.h
attribdefault.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
attribdefault.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
attribdefault.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
attribdefault.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
attribdefault.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
attribdefault.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
attribdefault.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
attribdefault.o: XMCallback.h XMLabel.h /usr/X11/include/Xm/Label.h XMText.h
attribdefault.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
attribdefault.o: helpmenu.h dispwind.h /usr/include/assert.h superpos.h
attribdefault.o: dfltmgr.h optionmenu.h XMScale.h /usr/X11/include/Xm/Scale.h
attribdefault.o: panemgr.h refreshctl.h
axes.o: /usr/include/math.h /usr/include/features.h /usr/include/sys/cdefs.h
axes.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
axes.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
axes.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
axes.o: /usr/include/assert.h XMWidget.h /usr/X11/include/X11/Intrinsic.h
axes.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
axes.o: /usr/include/bits/types.h
axes.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
axes.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
axes.o: /usr/include/sys/select.h /usr/include/bits/select.h
axes.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
axes.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
axes.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
axes.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
axes.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
axes.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
axes.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
axes.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
axes.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
axes.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
axes.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
axes.o: /usr/X11/include/Xm/VendorS.h XBatch.h /usr/X11/include/X11/Xos.h
axes.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h /usr/include/unistd.h
axes.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
axes.o: /usr/include/getopt.h /usr/include/sys/time.h
axes.o: /usr/include/bits/time.h /usr/include/stdio.h
axes.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
axes.o: /usr/include/libio.h /usr/include/_G_config.h
axes.o: /usr/include/bits/stdio_lim.h axes.h dispwind.h /usr/include/stdlib.h
axes.o: /usr/include/alloca.h superpos.h refreshctl.h panemgr.h XMManagers.h
axes.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
axes.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
axes.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h dispshare.h
axes.o: text.h gcmgr.h xaminegc.h gc.h
axisdefault.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
axisdefault.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
axisdefault.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
axisdefault.o: /usr/include/features.h /usr/include/sys/cdefs.h
axisdefault.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
axisdefault.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
axisdefault.o: /usr/include/time.h /usr/include/endian.h
axisdefault.o: /usr/include/bits/endian.h /usr/include/sys/select.h
axisdefault.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
axisdefault.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
axisdefault.o: /usr/X11/include/X11/Xfuncproto.h
axisdefault.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
axisdefault.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
axisdefault.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
axisdefault.o: /usr/X11/include/X11/Constraint.h
axisdefault.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
axisdefault.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
axisdefault.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
axisdefault.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
axisdefault.o: /usr/X11/include/Xm/XmStrDefs.h
axisdefault.o: /usr/X11/include/X11/StringDefs.h
axisdefault.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/CascadeB.h
axisdefault.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
axisdefault.o: XMWidget.h XMDialogs.h /usr/include/stdio.h
axisdefault.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
axisdefault.o: /usr/include/libio.h /usr/include/_G_config.h
axisdefault.o: /usr/include/bits/stdio_lim.h /usr/X11/include/Xm/MessageB.h
axisdefault.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
axisdefault.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
axisdefault.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
axisdefault.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
axisdefault.o: /usr/X11/include/Xm/Frame.h XMCallback.h helpmenu.h dispwind.h
axisdefault.o: /usr/include/stdlib.h /usr/include/alloca.h
axisdefault.o: /usr/include/assert.h superpos.h dfltmgr.h optionmenu.h
axisdefault.o: XMText.h /usr/X11/include/Xm/Text.h
axisdefault.o: /usr/X11/include/Xm/TextF.h XMLabel.h
axisdefault.o: /usr/X11/include/Xm/Label.h XMScale.h
axisdefault.o: /usr/X11/include/Xm/Scale.h panemgr.h refreshctl.h
buttondlg.o: /usr/include/string.h /usr/include/features.h
buttondlg.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
buttondlg.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h XMManagers.h
buttondlg.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Xm.h
buttondlg.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
buttondlg.o: /usr/include/sys/types.h /usr/include/bits/types.h
buttondlg.o: /usr/include/time.h /usr/include/endian.h
buttondlg.o: /usr/include/bits/endian.h /usr/include/sys/select.h
buttondlg.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
buttondlg.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
buttondlg.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
buttondlg.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
buttondlg.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
buttondlg.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
buttondlg.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
buttondlg.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
buttondlg.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
buttondlg.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
buttondlg.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
buttondlg.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
buttondlg.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
buttondlg.o: /usr/X11/include/Xm/Frame.h XMWidget.h XMPushbutton.h
buttondlg.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
buttondlg.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
buttondlg.o: XMShell.h XMCallback.h /usr/include/stdio.h
buttondlg.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
buttondlg.o: /usr/include/libio.h /usr/include/_G_config.h
buttondlg.o: /usr/include/bits/stdio_lim.h buttondlg.h clientops.h ./Xamine.h
buttondlg.o: /usr/X11/include/Xm/ScrolledW.h /usr/X11/include/Xm/MwmUtil.h
buttondlg.o: /usr/X11/include/X11/Xmd.h
buttonreq.o: XMCallback.h XMWidget.h /usr/X11/include/X11/Intrinsic.h
buttonreq.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
buttonreq.o: /usr/include/features.h /usr/include/sys/cdefs.h
buttonreq.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
buttonreq.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
buttonreq.o: /usr/include/time.h /usr/include/endian.h
buttonreq.o: /usr/include/bits/endian.h /usr/include/sys/select.h
buttonreq.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
buttonreq.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
buttonreq.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
buttonreq.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
buttonreq.o: /usr/include/string.h /usr/X11/include/X11/Core.h
buttonreq.o: /usr/X11/include/X11/Composite.h
buttonreq.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
buttonreq.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
buttonreq.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
buttonreq.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
buttonreq.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
buttonreq.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
buttonreq.o: /usr/X11/include/Xm/VendorS.h /usr/include/stdio.h
buttonreq.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
buttonreq.o: /usr/include/libio.h /usr/include/_G_config.h
buttonreq.o: /usr/include/bits/stdio_lim.h XMShell.h XMPushbutton.h
buttonreq.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
buttonreq.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
buttonreq.o: XMDialogs.h /usr/X11/include/Xm/MessageB.h
buttonreq.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
buttonreq.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
buttonreq.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
buttonreq.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
buttonreq.o: /usr/X11/include/Xm/Frame.h dispshare.h errormsg.h dispwind.h
buttonreq.o: /usr/include/stdlib.h /usr/include/alloca.h
buttonreq.o: /usr/include/assert.h superpos.h gateio.h dispgrob.h XBatch.h
buttonreq.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
buttonreq.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
buttonreq.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
buttonreq.o: /usr/include/getopt.h /usr/include/sys/time.h
buttonreq.o: /usr/include/bits/time.h convert.h messages.h clientops.h
buttonreq.o: ./Xamine.h buttonreq.h buttondlg.h panemgr.h spcchoice.h
buttonreq.o: ptlist.h sumregion.h XMList.h /usr/X11/include/Xm/List.h
buttonreq.o: /usr/X11/include/Xm/ScrolledW.h XMLabel.h
buttonreq.o: /usr/X11/include/Xm/Label.h XMText.h /usr/X11/include/Xm/Text.h
buttonreq.o: /usr/X11/include/Xm/TextF.h grobjinput.h XMSeparators.h
buttonreq.o: /usr/X11/include/Xm/Separator.h grafinput.h helpmenu.h
buttonreq.o: refreshctl.h
buttonsetup.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
buttonsetup.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
buttonsetup.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
buttonsetup.o: /usr/include/features.h /usr/include/sys/cdefs.h
buttonsetup.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
buttonsetup.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
buttonsetup.o: /usr/include/time.h /usr/include/endian.h
buttonsetup.o: /usr/include/bits/endian.h /usr/include/sys/select.h
buttonsetup.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
buttonsetup.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
buttonsetup.o: /usr/X11/include/X11/Xfuncproto.h
buttonsetup.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
buttonsetup.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
buttonsetup.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
buttonsetup.o: /usr/X11/include/X11/Constraint.h
buttonsetup.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
buttonsetup.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
buttonsetup.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
buttonsetup.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
buttonsetup.o: /usr/X11/include/Xm/XmStrDefs.h
buttonsetup.o: /usr/X11/include/X11/StringDefs.h
buttonsetup.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/Form.h
buttonsetup.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
buttonsetup.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
buttonsetup.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
buttonsetup.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
buttonsetup.o: /usr/X11/include/Xm/ArrowB.h buttonsetup.h location.h
buttonsetup.o: /usr/include/stdio.h
buttonsetup.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
buttonsetup.o: /usr/include/libio.h /usr/include/_G_config.h
buttonsetup.o: /usr/include/bits/stdio_lim.h XMLabel.h
buttonsetup.o: /usr/X11/include/Xm/Label.h geometrymenu.h panemgr.h
buttonsetup.o: dispwind.h /usr/include/stdlib.h /usr/include/alloca.h
buttonsetup.o: /usr/include/assert.h superpos.h specchoose.h XMDialogs.h
buttonsetup.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/SelectioB.h
buttonsetup.o: /usr/X11/include/Xm/FileSB.h XMCallback.h spcdisplay.h
buttonsetup.o: pkgmgr.h refreshctl.h expand.h info.h marker.h sumregion.h
buttonsetup.o: XMList.h /usr/X11/include/Xm/List.h
buttonsetup.o: /usr/X11/include/Xm/ScrolledW.h XMText.h
buttonsetup.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
buttonsetup.o: convert.h dispshare.h dispgrob.h XBatch.h
buttonsetup.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
buttonsetup.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
buttonsetup.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
buttonsetup.o: /usr/include/getopt.h /usr/include/sys/time.h
buttonsetup.o: /usr/include/bits/time.h grobjinput.h XMSeparators.h
buttonsetup.o: /usr/X11/include/Xm/Separator.h grafinput.h helpmenu.h
buttonsetup.o: guintegrate.h acceptgates.h trackcursor.h properties.h
chanplot.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
chanplot.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
chanplot.o: /usr/include/features.h /usr/include/sys/cdefs.h
chanplot.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
chanplot.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
chanplot.o: /usr/include/time.h /usr/include/endian.h
chanplot.o: /usr/include/bits/endian.h /usr/include/sys/select.h
chanplot.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
chanplot.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
chanplot.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
chanplot.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
chanplot.o: /usr/include/string.h /usr/X11/include/X11/Core.h
chanplot.o: /usr/X11/include/X11/Composite.h
chanplot.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
chanplot.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
chanplot.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
chanplot.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
chanplot.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
chanplot.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
chanplot.o: /usr/X11/include/Xm/VendorS.h refreshctl.h panemgr.h XMManagers.h
chanplot.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
chanplot.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
chanplot.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
chanplot.o: dispwind.h /usr/include/stdio.h
chanplot.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
chanplot.o: /usr/include/libio.h /usr/include/_G_config.h
chanplot.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
chanplot.o: /usr/include/alloca.h /usr/include/assert.h superpos.h axes.h
chanplot.o: scaling.h dispshare.h chanplot.h colormgr.h gc.h xaminegc.h
chanplot.o: gcmgr.h
checklist.o: /usr/include/stdio.h /usr/include/features.h
checklist.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
checklist.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
checklist.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
checklist.o: /usr/include/bits/types.h /usr/include/libio.h
checklist.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
checklist.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
checklist.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
checklist.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
checklist.o: /usr/include/time.h /usr/include/endian.h
checklist.o: /usr/include/bits/endian.h /usr/include/sys/select.h
checklist.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
checklist.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
checklist.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
checklist.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
checklist.o: /usr/include/string.h /usr/X11/include/X11/Core.h
checklist.o: /usr/X11/include/X11/Composite.h
checklist.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
checklist.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
checklist.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
checklist.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
checklist.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
checklist.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
checklist.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
checklist.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
checklist.o: /usr/X11/include/Xm/Frame.h XMWidget.h XMPushbutton.h
checklist.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
checklist.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
checklist.o: XMLabel.h /usr/X11/include/Xm/Label.h checklist.h
clirequests.o: /usr/include/assert.h /usr/include/features.h
clirequests.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
clirequests.o: acceptgates.h XMWidget.h /usr/X11/include/X11/Intrinsic.h
clirequests.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
clirequests.o: /usr/include/bits/types.h
clirequests.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
clirequests.o: /usr/include/time.h /usr/include/endian.h
clirequests.o: /usr/include/bits/endian.h /usr/include/sys/select.h
clirequests.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
clirequests.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
clirequests.o: /usr/X11/include/X11/Xfuncproto.h
clirequests.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
clirequests.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
clirequests.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
clirequests.o: /usr/X11/include/X11/Constraint.h
clirequests.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
clirequests.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
clirequests.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
clirequests.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
clirequests.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
clirequests.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
clirequests.o: convert.h dispwind.h /usr/include/stdio.h
clirequests.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
clirequests.o: /usr/include/libio.h /usr/include/_G_config.h
clirequests.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
clirequests.o: /usr/include/alloca.h superpos.h dispshare.h dispgrob.h
clirequests.o: XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
clirequests.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
clirequests.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
clirequests.o: /usr/include/getopt.h /usr/include/sys/time.h
clirequests.o: /usr/include/bits/time.h grobjdisplay.h refreshctl.h panemgr.h
clirequests.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
clirequests.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
clirequests.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
clirequests.o: /usr/X11/include/Xm/Frame.h errormsg.h clirequests.h gateio.h
clirequests.o: messages.h clientops.h ./Xamine.h grobjmgr.h buttonreq.h
colormgr.o: /usr/include/stdio.h /usr/include/features.h
colormgr.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
colormgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
colormgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
colormgr.o: /usr/include/bits/types.h /usr/include/libio.h
colormgr.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
colormgr.o: /usr/include/errno.h /usr/include/bits/errno.h
colormgr.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
colormgr.o: /usr/include/assert.h /usr/include/string.h
colormgr.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xlib.h
colormgr.o: /usr/include/sys/types.h /usr/include/time.h
colormgr.o: /usr/include/endian.h /usr/include/bits/endian.h
colormgr.o: /usr/include/sys/select.h /usr/include/bits/select.h
colormgr.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
colormgr.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
colormgr.o: /usr/X11/include/X11/Xutil.h /usr/include/stdlib.h
colormgr.o: /usr/include/alloca.h XMWidget.h /usr/X11/include/X11/Intrinsic.h
colormgr.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
colormgr.o: /usr/X11/include/X11/Composite.h
colormgr.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
colormgr.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
colormgr.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
colormgr.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
colormgr.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
colormgr.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
colormgr.o: /usr/X11/include/Xm/VendorS.h colormgr.h
colorset.o: /usr/include/stdio.h /usr/include/features.h
colorset.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
colorset.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
colorset.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
colorset.o: /usr/include/bits/types.h /usr/include/libio.h
colorset.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
colorset.o: /usr/include/stdlib.h /usr/include/sys/types.h
colorset.o: /usr/include/time.h /usr/include/endian.h
colorset.o: /usr/include/bits/endian.h /usr/include/sys/select.h
colorset.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
colorset.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
colorset.o: /usr/include/errno.h /usr/include/bits/errno.h
colorset.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
colorset.o: /usr/X11/include/Xm/Label.h /usr/X11/include/Xm/Xm.h
colorset.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
colorset.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
colorset.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
colorset.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
colorset.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
colorset.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
colorset.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
colorset.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
colorset.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
colorset.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
colorset.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
colorset.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/Scale.h
colorset.o: /usr/X11/include/Xm/Separator.h /usr/X11/include/Xm/Text.h
colorset.o: XMDialogs.h /usr/X11/include/Xm/MessageB.h
colorset.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
colorset.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
colorset.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
colorset.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
colorset.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
colorset.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
colorset.o: /usr/X11/include/Xm/Frame.h XMCallback.h XMMenus.h helpmenu.h
colorset.o: errormsg.h procede.h colormgr.h
compatspec.o: /usr/include/stdio.h /usr/include/features.h
compatspec.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
compatspec.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
compatspec.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
compatspec.o: /usr/include/bits/types.h /usr/include/libio.h
compatspec.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
compatspec.o: compatspec.h /usr/include/stdlib.h /usr/include/sys/types.h
compatspec.o: /usr/include/time.h /usr/include/endian.h
compatspec.o: /usr/include/bits/endian.h /usr/include/sys/select.h
compatspec.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
compatspec.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h XMManagers.h
compatspec.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Xm.h
compatspec.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
compatspec.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
compatspec.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
compatspec.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
compatspec.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
compatspec.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
compatspec.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
compatspec.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
compatspec.o: /usr/X11/include/X11/ICE/ICElib.h
compatspec.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
compatspec.o: /usr/X11/include/Xm/XmStrDefs.h
compatspec.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
compatspec.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
compatspec.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
compatspec.o: /usr/X11/include/Xm/Frame.h XMWidget.h XMList.h
compatspec.o: /usr/X11/include/Xm/List.h /usr/X11/include/Xm/ScrolledW.h
compatspec.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
compatspec.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
compatspec.o: /usr/X11/include/Xm/ArrowB.h XMLabel.h
compatspec.o: /usr/X11/include/Xm/Label.h dispshare.h panemgr.h dispwind.h
compatspec.o: /usr/include/assert.h superpos.h
convert.o: /usr/include/math.h /usr/include/features.h
convert.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
convert.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
convert.o: /usr/include/bits/mathcalls.h
convert.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h convert.h
convert.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
convert.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
convert.o: /usr/include/bits/types.h
convert.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
convert.o: /usr/include/time.h /usr/include/endian.h
convert.o: /usr/include/bits/endian.h /usr/include/sys/select.h
convert.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
convert.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
convert.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
convert.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
convert.o: /usr/include/string.h /usr/X11/include/X11/Core.h
convert.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
convert.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
convert.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
convert.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
convert.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
convert.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
convert.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
convert.o: dispwind.h /usr/include/stdio.h
convert.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
convert.o: /usr/include/libio.h /usr/include/_G_config.h
convert.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
convert.o: /usr/include/alloca.h /usr/include/assert.h superpos.h dispshare.h
convert.o: axes.h refreshctl.h panemgr.h XMManagers.h
convert.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
convert.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
convert.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
copier.o: /usr/include/stdio.h /usr/include/features.h
copier.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
copier.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
copier.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
copier.o: /usr/include/bits/types.h /usr/include/libio.h
copier.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h copier.h
copier.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/Xm.h
copier.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
copier.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
copier.o: /usr/include/bits/endian.h /usr/include/sys/select.h
copier.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
copier.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
copier.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
copier.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
copier.o: /usr/include/string.h /usr/X11/include/X11/Core.h
copier.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
copier.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
copier.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
copier.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
copier.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
copier.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/X11/StringDefs.h
copier.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/CascadeB.h
copier.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
copier.o: XMWidget.h XMManagers.h /usr/X11/include/Xm/BulletinB.h
copier.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
copier.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
copier.o: /usr/X11/include/Xm/Frame.h XMList.h /usr/X11/include/Xm/List.h
copier.o: /usr/X11/include/Xm/ScrolledW.h XMLabel.h
copier.o: /usr/X11/include/Xm/Label.h
copyatr.o: /usr/include/stdio.h /usr/include/features.h
copyatr.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
copyatr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
copyatr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
copyatr.o: /usr/include/bits/types.h /usr/include/libio.h
copyatr.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
copyatr.o: /usr/include/string.h dispwind.h /usr/include/stdlib.h
copyatr.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
copyatr.o: /usr/include/bits/endian.h /usr/include/sys/select.h
copyatr.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
copyatr.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
copyatr.o: /usr/include/assert.h superpos.h panemgr.h XMManagers.h
copyatr.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Xm.h
copyatr.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
copyatr.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
copyatr.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
copyatr.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
copyatr.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
copyatr.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
copyatr.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
copyatr.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
copyatr.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
copyatr.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/X11/StringDefs.h
copyatr.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/Form.h
copyatr.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
copyatr.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h XMWidget.h
copyatr.o: checklist.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
copyatr.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
copyatr.o: /usr/X11/include/Xm/ArrowB.h XMLabel.h /usr/X11/include/Xm/Label.h
copyatr.o: compatspec.h XMList.h /usr/X11/include/Xm/List.h
copyatr.o: /usr/X11/include/Xm/ScrolledW.h XMDialogs.h
copyatr.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/SelectioB.h
copyatr.o: /usr/X11/include/Xm/FileSB.h XMCallback.h errormsg.h helpmenu.h
copyatr.o: refreshctl.h
delgrob.o: /usr/include/stdio.h /usr/include/features.h
delgrob.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
delgrob.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
delgrob.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
delgrob.o: /usr/include/bits/types.h /usr/include/libio.h
delgrob.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
delgrob.o: /usr/include/string.h /usr/include/ctype.h /usr/include/endian.h
delgrob.o: /usr/include/bits/endian.h /usr/include/assert.h XMDialogs.h
delgrob.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/MessageB.h
delgrob.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
delgrob.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
delgrob.o: /usr/include/time.h /usr/include/sys/select.h
delgrob.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
delgrob.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
delgrob.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
delgrob.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
delgrob.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
delgrob.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
delgrob.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
delgrob.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
delgrob.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
delgrob.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
delgrob.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/SelectioB.h
delgrob.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
delgrob.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
delgrob.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
delgrob.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
delgrob.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
delgrob.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
delgrob.o: /usr/X11/include/Xm/Frame.h XMCallback.h dispgrob.h dispwind.h
delgrob.o: /usr/include/stdlib.h /usr/include/alloca.h superpos.h XBatch.h
delgrob.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
delgrob.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
delgrob.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
delgrob.o: /usr/include/getopt.h /usr/include/sys/time.h
delgrob.o: /usr/include/bits/time.h convert.h dispshare.h helpmenu.h
delgrob.o: delgrob.h grobjmgr.h panemgr.h errormsg.h refreshctl.h
dfltmgr.o: /usr/include/stdio.h /usr/include/features.h
dfltmgr.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
dfltmgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
dfltmgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
dfltmgr.o: /usr/include/bits/types.h /usr/include/libio.h
dfltmgr.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
dfltmgr.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
dfltmgr.o: /usr/include/endian.h /usr/include/bits/endian.h
dfltmgr.o: /usr/include/sys/select.h /usr/include/bits/select.h
dfltmgr.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
dfltmgr.o: /usr/include/alloca.h /usr/include/unistd.h
dfltmgr.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
dfltmgr.o: /usr/include/getopt.h panemgr.h XMManagers.h
dfltmgr.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Xm.h
dfltmgr.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
dfltmgr.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
dfltmgr.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
dfltmgr.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
dfltmgr.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
dfltmgr.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
dfltmgr.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
dfltmgr.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
dfltmgr.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
dfltmgr.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
dfltmgr.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
dfltmgr.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
dfltmgr.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
dfltmgr.o: /usr/X11/include/Xm/Frame.h XMWidget.h dispwind.h
dfltmgr.o: /usr/include/assert.h superpos.h grobjdisplay.h convert.h
dfltmgr.o: dispshare.h dispgrob.h XBatch.h /usr/X11/include/X11/Xos.h
dfltmgr.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
dfltmgr.o: /usr/include/sys/time.h /usr/include/bits/time.h refreshctl.h
dfltmgr.o: dfltmgr.h
dispgrob.o: /usr/include/stdio.h /usr/include/features.h
dispgrob.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
dispgrob.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
dispgrob.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
dispgrob.o: /usr/include/bits/types.h /usr/include/libio.h
dispgrob.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h dispgrob.h
dispgrob.o: /usr/include/assert.h /usr/include/string.h XMWidget.h
dispgrob.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
dispgrob.o: /usr/include/sys/types.h /usr/include/time.h
dispgrob.o: /usr/include/endian.h /usr/include/bits/endian.h
dispgrob.o: /usr/include/sys/select.h /usr/include/bits/select.h
dispgrob.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
dispgrob.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
dispgrob.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
dispgrob.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
dispgrob.o: /usr/X11/include/X11/Composite.h
dispgrob.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
dispgrob.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
dispgrob.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
dispgrob.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
dispgrob.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
dispgrob.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
dispgrob.o: /usr/X11/include/Xm/VendorS.h dispwind.h /usr/include/stdlib.h
dispgrob.o: /usr/include/alloca.h superpos.h XBatch.h
dispgrob.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
dispgrob.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
dispgrob.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
dispgrob.o: /usr/include/getopt.h /usr/include/sys/time.h
dispgrob.o: /usr/include/bits/time.h convert.h dispshare.h sumregion.h
dispgrob.o: XMList.h /usr/X11/include/Xm/List.h
dispgrob.o: /usr/X11/include/Xm/ScrolledW.h XMPushbutton.h
dispgrob.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
dispgrob.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
dispgrob.o: XMLabel.h /usr/X11/include/Xm/Label.h XMText.h
dispgrob.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
dispgrob.o: grobjinput.h XMManagers.h /usr/X11/include/Xm/BulletinB.h
dispgrob.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
dispgrob.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
dispgrob.o: /usr/X11/include/Xm/Frame.h XMDialogs.h
dispgrob.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/SelectioB.h
dispgrob.o: /usr/X11/include/Xm/FileSB.h XMCallback.h XMSeparators.h
dispgrob.o: /usr/X11/include/Xm/Separator.h grafinput.h helpmenu.h
dispgrob.o: refreshctl.h panemgr.h acceptgates.h grobjdisplay.h marker.h
dispgrob.o: gcmgr.h xaminegc.h gc.h chanplot.h
dispwind.o: /usr/include/stdio.h /usr/include/features.h
dispwind.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
dispwind.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
dispwind.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
dispwind.o: /usr/include/bits/types.h /usr/include/libio.h
dispwind.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
dispwind.o: /usr/include/stdlib.h /usr/include/sys/types.h
dispwind.o: /usr/include/time.h /usr/include/endian.h
dispwind.o: /usr/include/bits/endian.h /usr/include/sys/select.h
dispwind.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
dispwind.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h dispwind.h
dispwind.o: /usr/include/assert.h superpos.h errormsg.h XMWidget.h
dispwind.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
dispwind.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
dispwind.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
dispwind.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
dispwind.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
dispwind.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
dispwind.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
dispwind.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
dispwind.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
dispwind.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
dispwind.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
dispwind.o: /usr/X11/include/Xm/VendorS.h panemgr.h XMManagers.h
dispwind.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
dispwind.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
dispwind.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
errormsg.o: XMDialogs.h /usr/include/stdio.h /usr/include/features.h
errormsg.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
errormsg.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
errormsg.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
errormsg.o: /usr/include/bits/types.h /usr/include/libio.h
errormsg.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
errormsg.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/MessageB.h
errormsg.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
errormsg.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
errormsg.o: /usr/include/time.h /usr/include/endian.h
errormsg.o: /usr/include/bits/endian.h /usr/include/sys/select.h
errormsg.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
errormsg.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
errormsg.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
errormsg.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
errormsg.o: /usr/include/string.h /usr/X11/include/X11/Core.h
errormsg.o: /usr/X11/include/X11/Composite.h
errormsg.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
errormsg.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
errormsg.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
errormsg.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
errormsg.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
errormsg.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/SelectioB.h
errormsg.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
errormsg.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
errormsg.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
errormsg.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
errormsg.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
errormsg.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
errormsg.o: /usr/X11/include/Xm/Frame.h XMCallback.h
eventflags.o: /usr/include/assert.h /usr/include/features.h
eventflags.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h eventflags.h
exit.o: /usr/include/stdlib.h /usr/include/features.h
exit.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
exit.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
exit.o: /usr/include/sys/types.h /usr/include/bits/types.h
exit.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
exit.o: /usr/include/sys/select.h /usr/include/bits/select.h
exit.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
exit.o: /usr/include/alloca.h /usr/include/stdio.h
exit.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
exit.o: /usr/include/libio.h /usr/include/_G_config.h
exit.o: /usr/include/bits/stdio_lim.h XMDialogs.h
exit.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/MessageB.h
exit.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
exit.o: /usr/X11/include/X11/Xlib.h /usr/X11/include/X11/X.h
exit.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
exit.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
exit.o: /usr/include/string.h /usr/X11/include/X11/Core.h
exit.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
exit.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
exit.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
exit.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
exit.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
exit.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
exit.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
exit.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
exit.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
exit.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
exit.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
exit.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
exit.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h XMCallback.h
exit.o: panemgr.h dispwind.h /usr/include/assert.h superpos.h helpmenu.h
exit.o: exit.h winiomenu.h
expand.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
expand.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
expand.o: /usr/include/features.h /usr/include/sys/cdefs.h
expand.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
expand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
expand.o: /usr/include/time.h /usr/include/endian.h
expand.o: /usr/include/bits/endian.h /usr/include/sys/select.h
expand.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
expand.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
expand.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
expand.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
expand.o: /usr/include/string.h /usr/X11/include/X11/Core.h
expand.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
expand.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
expand.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
expand.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
expand.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
expand.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
expand.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
expand.o: XMDialogs.h /usr/include/stdio.h
expand.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
expand.o: /usr/include/libio.h /usr/include/_G_config.h
expand.o: /usr/include/bits/stdio_lim.h /usr/X11/include/Xm/MessageB.h
expand.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
expand.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
expand.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
expand.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
expand.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
expand.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
expand.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
expand.o: XMCallback.h errormsg.h panemgr.h dispwind.h /usr/include/stdlib.h
expand.o: /usr/include/alloca.h /usr/include/assert.h superpos.h helpmenu.h
expand.o: expand.h refreshctl.h exit.h select2.h XMLabel.h
expand.o: /usr/X11/include/Xm/Label.h XMText.h /usr/X11/include/Xm/Text.h
expand.o: /usr/X11/include/Xm/TextF.h grafinput.h dispshare.h convert.h
expand.o: colormgr.h gcmgr.h xaminegc.h gc.h
gateio.o: /usr/include/stdio.h /usr/include/features.h
gateio.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
gateio.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
gateio.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
gateio.o: /usr/include/bits/types.h /usr/include/libio.h
gateio.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
gateio.o: /usr/include/string.h /usr/include/stdlib.h
gateio.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
gateio.o: /usr/include/bits/endian.h /usr/include/sys/select.h
gateio.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
gateio.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h gateio.h
gateio.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
gateio.o: /usr/X11/include/X11/Xlib.h /usr/X11/include/X11/X.h
gateio.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
gateio.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
gateio.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
gateio.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
gateio.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
gateio.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
gateio.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
gateio.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
gateio.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
gateio.o: /usr/X11/include/Xm/VendorS.h dispgrob.h /usr/include/assert.h
gateio.o: dispwind.h superpos.h XBatch.h /usr/X11/include/X11/Xos.h
gateio.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
gateio.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
gateio.o: /usr/include/bits/confname.h /usr/include/getopt.h
gateio.o: /usr/include/sys/time.h /usr/include/bits/time.h convert.h
gateio.o: dispshare.h messages.h clientops.h ./Xamine.h
gc.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
gc.o: /usr/include/gnu/stubs.h
gc.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
gc.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
gc.o: /usr/include/bits/types.h /usr/include/libio.h /usr/include/_G_config.h
gc.o: /usr/include/bits/stdio_lim.h gc.h /usr/X11/include/X11/Xlib.h
gc.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
gc.o: /usr/include/bits/endian.h /usr/include/sys/select.h
gc.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
gc.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
gc.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
gcmgr.o: /usr/include/assert.h /usr/include/features.h
gcmgr.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h xaminegc.h
gcmgr.o: dispwind.h /usr/include/stdio.h
gcmgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
gcmgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
gcmgr.o: /usr/include/bits/types.h /usr/include/libio.h
gcmgr.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
gcmgr.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
gcmgr.o: /usr/include/endian.h /usr/include/bits/endian.h
gcmgr.o: /usr/include/sys/select.h /usr/include/bits/select.h
gcmgr.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
gcmgr.o: /usr/include/alloca.h superpos.h gc.h /usr/X11/include/X11/Xlib.h
gcmgr.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
gcmgr.o: /usr/X11/include/X11/Xosdefs.h XMWidget.h
gcmgr.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xutil.h
gcmgr.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
gcmgr.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
gcmgr.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
gcmgr.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
gcmgr.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
gcmgr.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
gcmgr.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
gcmgr.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
gcmgr.o: /usr/X11/include/Xm/VendorS.h grobjdisplay.h convert.h dispshare.h
gcmgr.o: dispgrob.h XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
gcmgr.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
gcmgr.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
gcmgr.o: /usr/include/getopt.h /usr/include/sys/time.h
gcmgr.o: /usr/include/bits/time.h refreshctl.h panemgr.h XMManagers.h
gcmgr.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
gcmgr.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
gcmgr.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
geometrymenu.o: /usr/include/stdio.h /usr/include/features.h
geometrymenu.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
geometrymenu.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
geometrymenu.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
geometrymenu.o: /usr/include/bits/types.h /usr/include/libio.h
geometrymenu.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
geometrymenu.o: /usr/include/errno.h /usr/include/bits/errno.h
geometrymenu.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
geometrymenu.o: /usr/include/stdlib.h /usr/include/sys/types.h
geometrymenu.o: /usr/include/time.h /usr/include/endian.h
geometrymenu.o: /usr/include/bits/endian.h /usr/include/sys/select.h
geometrymenu.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
geometrymenu.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h XMLabel.h
geometrymenu.o: /usr/X11/include/Xm/Label.h /usr/X11/include/Xm/Xm.h
geometrymenu.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
geometrymenu.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
geometrymenu.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
geometrymenu.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
geometrymenu.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
geometrymenu.o: /usr/X11/include/X11/Constraint.h
geometrymenu.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
geometrymenu.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
geometrymenu.o: /usr/X11/include/X11/SM/SM.h
geometrymenu.o: /usr/X11/include/X11/ICE/ICElib.h
geometrymenu.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
geometrymenu.o: /usr/X11/include/Xm/XmStrDefs.h
geometrymenu.o: /usr/X11/include/X11/StringDefs.h
geometrymenu.o: /usr/X11/include/Xm/VendorS.h XMWidget.h XMDialogs.h
geometrymenu.o: /usr/X11/include/Xm/MessageB.h
geometrymenu.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
geometrymenu.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
geometrymenu.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
geometrymenu.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
geometrymenu.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
geometrymenu.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
geometrymenu.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
geometrymenu.o: XMCallback.h XMSeparators.h /usr/X11/include/Xm/Separator.h
geometrymenu.o: helpmenu.h errormsg.h panemgr.h dispwind.h
geometrymenu.o: /usr/include/assert.h superpos.h menusetup.h XMMenus.h exit.h
grafinput.o: /usr/X11/include/X11/keysym.h /usr/X11/include/X11/keysymdef.h
grafinput.o: XMDialogs.h /usr/include/stdio.h /usr/include/features.h
grafinput.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
grafinput.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
grafinput.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
grafinput.o: /usr/include/bits/types.h /usr/include/libio.h
grafinput.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
grafinput.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/MessageB.h
grafinput.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
grafinput.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
grafinput.o: /usr/include/time.h /usr/include/endian.h
grafinput.o: /usr/include/bits/endian.h /usr/include/sys/select.h
grafinput.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
grafinput.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
grafinput.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
grafinput.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
grafinput.o: /usr/include/string.h /usr/X11/include/X11/Core.h
grafinput.o: /usr/X11/include/X11/Composite.h
grafinput.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
grafinput.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
grafinput.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
grafinput.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
grafinput.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
grafinput.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/SelectioB.h
grafinput.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
grafinput.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
grafinput.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
grafinput.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
grafinput.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
grafinput.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
grafinput.o: /usr/X11/include/Xm/Frame.h XMCallback.h grafinput.h helpmenu.h
grafinput.o: refreshctl.h panemgr.h dispwind.h /usr/include/stdlib.h
grafinput.o: /usr/include/alloca.h /usr/include/assert.h superpos.h convert.h
grafinput.o: dispshare.h
grobfile.o: /usr/include/stdio.h /usr/include/features.h
grobfile.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
grobfile.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
grobfile.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
grobfile.o: /usr/include/bits/types.h /usr/include/libio.h
grobfile.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
grobfile.o: /usr/include/time.h dispgrob.h /usr/include/assert.h
grobfile.o: /usr/include/string.h XMWidget.h /usr/X11/include/X11/Intrinsic.h
grobfile.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
grobfile.o: /usr/include/endian.h /usr/include/bits/endian.h
grobfile.o: /usr/include/sys/select.h /usr/include/bits/select.h
grobfile.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
grobfile.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
grobfile.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
grobfile.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
grobfile.o: /usr/X11/include/X11/Composite.h
grobfile.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
grobfile.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
grobfile.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
grobfile.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
grobfile.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
grobfile.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
grobfile.o: /usr/X11/include/Xm/VendorS.h dispwind.h /usr/include/stdlib.h
grobfile.o: /usr/include/alloca.h superpos.h XBatch.h
grobfile.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
grobfile.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
grobfile.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
grobfile.o: /usr/include/getopt.h /usr/include/sys/time.h
grobfile.o: /usr/include/bits/time.h convert.h dispshare.h panemgr.h
grobfile.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
grobfile.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
grobfile.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
grobfile.o: /usr/X11/include/Xm/Frame.h errormsg.h
grobjdb.o: /usr/include/stdio.h /usr/include/features.h
grobjdb.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
grobjdb.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
grobjdb.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
grobjdb.o: /usr/include/bits/types.h /usr/include/libio.h
grobjdb.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
grobjdb.o: /usr/include/errno.h /usr/include/bits/errno.h
grobjdb.o: /usr/include/linux/errno.h /usr/include/asm/errno.h dispgrob.h
grobjdb.o: /usr/include/assert.h /usr/include/string.h XMWidget.h
grobjdb.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
grobjdb.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
grobjdb.o: /usr/include/bits/endian.h /usr/include/sys/select.h
grobjdb.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
grobjdb.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
grobjdb.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
grobjdb.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
grobjdb.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
grobjdb.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
grobjdb.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
grobjdb.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
grobjdb.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
grobjdb.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
grobjdb.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
grobjdb.o: /usr/X11/include/Xm/VendorS.h dispwind.h /usr/include/stdlib.h
grobjdb.o: /usr/include/alloca.h superpos.h XBatch.h
grobjdb.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
grobjdb.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
grobjdb.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
grobjdb.o: /usr/include/getopt.h /usr/include/sys/time.h
grobjdb.o: /usr/include/bits/time.h convert.h dispshare.h
grobjdisplay.o: /usr/include/stdio.h /usr/include/features.h
grobjdisplay.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
grobjdisplay.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
grobjdisplay.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
grobjdisplay.o: /usr/include/bits/types.h /usr/include/libio.h
grobjdisplay.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
grobjdisplay.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
grobjdisplay.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
grobjdisplay.o: /usr/include/time.h /usr/include/endian.h
grobjdisplay.o: /usr/include/bits/endian.h /usr/include/sys/select.h
grobjdisplay.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
grobjdisplay.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
grobjdisplay.o: /usr/X11/include/X11/Xfuncproto.h
grobjdisplay.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
grobjdisplay.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
grobjdisplay.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
grobjdisplay.o: /usr/X11/include/X11/Constraint.h
grobjdisplay.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
grobjdisplay.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
grobjdisplay.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
grobjdisplay.o: /usr/X11/include/X11/SM/SM.h
grobjdisplay.o: /usr/X11/include/X11/ICE/ICElib.h
grobjdisplay.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
grobjdisplay.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
grobjdisplay.o: XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
grobjdisplay.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
grobjdisplay.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
grobjdisplay.o: /usr/include/getopt.h /usr/include/sys/time.h
grobjdisplay.o: /usr/include/bits/time.h panemgr.h XMManagers.h
grobjdisplay.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
grobjdisplay.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
grobjdisplay.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
grobjdisplay.o: dispwind.h /usr/include/stdlib.h /usr/include/alloca.h
grobjdisplay.o: /usr/include/assert.h superpos.h grobjdisplay.h convert.h
grobjdisplay.o: dispshare.h dispgrob.h refreshctl.h text.h errormsg.h
grobjdisplay.o: colormgr.h grobjmgr.h sumregion.h XMList.h
grobjdisplay.o: /usr/X11/include/Xm/List.h /usr/X11/include/Xm/ScrolledW.h
grobjdisplay.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
grobjdisplay.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
grobjdisplay.o: /usr/X11/include/Xm/ArrowB.h XMLabel.h
grobjdisplay.o: /usr/X11/include/Xm/Label.h XMText.h
grobjdisplay.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
grobjdisplay.o: grobjinput.h XMDialogs.h /usr/X11/include/Xm/MessageB.h
grobjdisplay.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
grobjdisplay.o: XMCallback.h XMSeparators.h /usr/X11/include/Xm/Separator.h
grobjdisplay.o: grafinput.h helpmenu.h chanplot.h acceptgates.h gc.h
grobjdisplay.o: xaminegc.h gcmgr.h
grobjinput.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
grobjinput.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
grobjinput.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
grobjinput.o: /usr/include/features.h /usr/include/sys/cdefs.h
grobjinput.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
grobjinput.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
grobjinput.o: /usr/include/time.h /usr/include/endian.h
grobjinput.o: /usr/include/bits/endian.h /usr/include/sys/select.h
grobjinput.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
grobjinput.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
grobjinput.o: /usr/X11/include/X11/Xfuncproto.h
grobjinput.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
grobjinput.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
grobjinput.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
grobjinput.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
grobjinput.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
grobjinput.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
grobjinput.o: /usr/X11/include/X11/ICE/ICElib.h
grobjinput.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
grobjinput.o: /usr/X11/include/Xm/XmStrDefs.h
grobjinput.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
grobjinput.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
grobjinput.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
grobjinput.o: /usr/X11/include/Xm/Frame.h XMWidget.h XMDialogs.h
grobjinput.o: /usr/include/stdio.h
grobjinput.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
grobjinput.o: /usr/include/libio.h /usr/include/_G_config.h
grobjinput.o: /usr/include/bits/stdio_lim.h /usr/X11/include/Xm/MessageB.h
grobjinput.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
grobjinput.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
grobjinput.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
grobjinput.o: /usr/X11/include/Xm/ArrowB.h XMCallback.h XMSeparators.h
grobjinput.o: /usr/X11/include/Xm/Separator.h grobjinput.h XMText.h
grobjinput.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
grobjinput.o: XMLabel.h /usr/X11/include/Xm/Label.h grafinput.h helpmenu.h
grobjinput.o: refreshctl.h panemgr.h dispwind.h /usr/include/stdlib.h
grobjinput.o: /usr/include/alloca.h /usr/include/assert.h superpos.h
grobjiomenu.o: /usr/include/stdio.h /usr/include/features.h
grobjiomenu.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
grobjiomenu.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
grobjiomenu.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
grobjiomenu.o: /usr/include/bits/types.h /usr/include/libio.h
grobjiomenu.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
grobjiomenu.o: /usr/include/errno.h /usr/include/bits/errno.h
grobjiomenu.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
grobjiomenu.o: /usr/X11/include/X11/StringDefs.h XMDialogs.h
grobjiomenu.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
grobjiomenu.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
grobjiomenu.o: /usr/include/sys/types.h /usr/include/time.h
grobjiomenu.o: /usr/include/endian.h /usr/include/bits/endian.h
grobjiomenu.o: /usr/include/sys/select.h /usr/include/bits/select.h
grobjiomenu.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
grobjiomenu.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
grobjiomenu.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
grobjiomenu.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
grobjiomenu.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
grobjiomenu.o: /usr/X11/include/X11/Constraint.h
grobjiomenu.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
grobjiomenu.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
grobjiomenu.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
grobjiomenu.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
grobjiomenu.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
grobjiomenu.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
grobjiomenu.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
grobjiomenu.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
grobjiomenu.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
grobjiomenu.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
grobjiomenu.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
grobjiomenu.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
grobjiomenu.o: XMCallback.h helpmenu.h grobjiomenu.h panemgr.h dispwind.h
grobjiomenu.o: /usr/include/stdlib.h /usr/include/alloca.h
grobjiomenu.o: /usr/include/assert.h superpos.h grobjmgr.h dispgrob.h
grobjiomenu.o: XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
grobjiomenu.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
grobjiomenu.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
grobjiomenu.o: /usr/include/getopt.h /usr/include/sys/time.h
grobjiomenu.o: /usr/include/bits/time.h convert.h dispshare.h refreshctl.h
grobjiomenu.o: errormsg.h winiomenu.h
grobjmgr.o: /usr/include/stdio.h /usr/include/features.h
grobjmgr.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
grobjmgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
grobjmgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
grobjmgr.o: /usr/include/bits/types.h /usr/include/libio.h
grobjmgr.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
grobjmgr.o: /usr/include/errno.h /usr/include/bits/errno.h
grobjmgr.o: /usr/include/linux/errno.h /usr/include/asm/errno.h dispgrob.h
grobjmgr.o: /usr/include/assert.h /usr/include/string.h XMWidget.h
grobjmgr.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
grobjmgr.o: /usr/include/sys/types.h /usr/include/time.h
grobjmgr.o: /usr/include/endian.h /usr/include/bits/endian.h
grobjmgr.o: /usr/include/sys/select.h /usr/include/bits/select.h
grobjmgr.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
grobjmgr.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
grobjmgr.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
grobjmgr.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
grobjmgr.o: /usr/X11/include/X11/Composite.h
grobjmgr.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
grobjmgr.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
grobjmgr.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
grobjmgr.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
grobjmgr.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
grobjmgr.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
grobjmgr.o: /usr/X11/include/Xm/VendorS.h dispwind.h /usr/include/stdlib.h
grobjmgr.o: /usr/include/alloca.h superpos.h XBatch.h
grobjmgr.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
grobjmgr.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
grobjmgr.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
grobjmgr.o: /usr/include/getopt.h /usr/include/sys/time.h
grobjmgr.o: /usr/include/bits/time.h convert.h dispshare.h grobjmgr.h
grobjtest.o: /usr/include/stdio.h /usr/include/features.h
grobjtest.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
grobjtest.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
grobjtest.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
grobjtest.o: /usr/include/bits/types.h /usr/include/libio.h
grobjtest.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
grobjtest.o: /usr/include/stdlib.h /usr/include/sys/types.h
grobjtest.o: /usr/include/time.h /usr/include/endian.h
grobjtest.o: /usr/include/bits/endian.h /usr/include/sys/select.h
grobjtest.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
grobjtest.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h dispgrob.h
grobjtest.o: /usr/include/assert.h /usr/include/string.h XMWidget.h
grobjtest.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
grobjtest.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
grobjtest.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
grobjtest.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
grobjtest.o: /usr/X11/include/X11/Composite.h
grobjtest.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
grobjtest.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
grobjtest.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
grobjtest.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
grobjtest.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
grobjtest.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
grobjtest.o: /usr/X11/include/Xm/VendorS.h dispwind.h superpos.h XBatch.h
grobjtest.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
grobjtest.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
grobjtest.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
grobjtest.o: /usr/include/getopt.h /usr/include/sys/time.h
grobjtest.o: /usr/include/bits/time.h convert.h dispshare.h
guintegrate.o: /usr/include/stdio.h /usr/include/features.h
guintegrate.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
guintegrate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
guintegrate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
guintegrate.o: /usr/include/bits/types.h /usr/include/libio.h
guintegrate.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
guintegrate.o: /usr/include/string.h XMDialogs.h
guintegrate.o: /usr/X11/include/X11/StringDefs.h
guintegrate.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
guintegrate.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
guintegrate.o: /usr/include/sys/types.h /usr/include/time.h
guintegrate.o: /usr/include/endian.h /usr/include/bits/endian.h
guintegrate.o: /usr/include/sys/select.h /usr/include/bits/select.h
guintegrate.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
guintegrate.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
guintegrate.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
guintegrate.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
guintegrate.o: /usr/X11/include/X11/Composite.h
guintegrate.o: /usr/X11/include/X11/Constraint.h
guintegrate.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
guintegrate.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
guintegrate.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
guintegrate.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
guintegrate.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
guintegrate.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
guintegrate.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
guintegrate.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
guintegrate.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
guintegrate.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
guintegrate.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
guintegrate.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
guintegrate.o: XMCallback.h XMText.h /usr/X11/include/Xm/Text.h
guintegrate.o: /usr/X11/include/Xm/TextF.h errormsg.h guintegrate.h
guintegrate.o: integrate.h dispgrob.h /usr/include/assert.h dispwind.h
guintegrate.o: /usr/include/stdlib.h /usr/include/alloca.h superpos.h
guintegrate.o: XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
guintegrate.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
guintegrate.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
guintegrate.o: /usr/include/getopt.h /usr/include/sys/time.h
guintegrate.o: /usr/include/bits/time.h convert.h dispshare.h logging.h
guintegrate.o: grobjmgr.h panemgr.h
helpmenu.o: /usr/include/stdio.h /usr/include/features.h
helpmenu.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
helpmenu.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
helpmenu.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
helpmenu.o: /usr/include/bits/types.h /usr/include/libio.h
helpmenu.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
helpmenu.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
helpmenu.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
helpmenu.o: /usr/include/time.h /usr/include/endian.h
helpmenu.o: /usr/include/bits/endian.h /usr/include/sys/select.h
helpmenu.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
helpmenu.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
helpmenu.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
helpmenu.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
helpmenu.o: /usr/include/string.h /usr/X11/include/X11/Core.h
helpmenu.o: /usr/X11/include/X11/Composite.h
helpmenu.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
helpmenu.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
helpmenu.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
helpmenu.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
helpmenu.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
helpmenu.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
helpmenu.o: XMDialogs.h /usr/X11/include/Xm/MessageB.h
helpmenu.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
helpmenu.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
helpmenu.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
helpmenu.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
helpmenu.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
helpmenu.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
helpmenu.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
helpmenu.o: XMCallback.h helpmenu.h
info.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
info.o: /usr/include/gnu/stubs.h
info.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
info.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
info.o: /usr/include/bits/types.h /usr/include/libio.h
info.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
info.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
info.o: /usr/include/endian.h /usr/include/bits/endian.h
info.o: /usr/include/sys/select.h /usr/include/bits/select.h
info.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
info.o: /usr/include/alloca.h /usr/include/string.h /usr/include/assert.h
info.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
info.o: /usr/X11/include/X11/Xlib.h /usr/X11/include/X11/X.h
info.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
info.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
info.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
info.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
info.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
info.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
info.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
info.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
info.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
info.o: /usr/X11/include/Xm/VendorS.h XMDialogs.h
info.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/SelectioB.h
info.o: /usr/X11/include/Xm/FileSB.h XMPushbutton.h
info.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
info.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
info.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
info.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
info.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
info.o: /usr/X11/include/Xm/Frame.h XMCallback.h XMText.h
info.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h info.h
info.o: logging.h panemgr.h dispwind.h superpos.h errormsg.h dispgrob.h
info.o: XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
info.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
info.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
info.o: /usr/include/getopt.h /usr/include/sys/time.h
info.o: /usr/include/bits/time.h convert.h dispshare.h grobjmgr.h
info.o: spcdisplay.h
integrate.o: /usr/include/math.h /usr/include/features.h
integrate.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
integrate.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
integrate.o: /usr/include/bits/mathcalls.h
integrate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h
integrate.o: /usr/include/string.h
integrate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h integrate.h
integrate.o: dispgrob.h /usr/include/stdio.h
integrate.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
integrate.o: /usr/include/bits/types.h /usr/include/libio.h
integrate.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
integrate.o: /usr/include/assert.h XMWidget.h
integrate.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
integrate.o: /usr/include/sys/types.h /usr/include/time.h
integrate.o: /usr/include/endian.h /usr/include/bits/endian.h
integrate.o: /usr/include/sys/select.h /usr/include/bits/select.h
integrate.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
integrate.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
integrate.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
integrate.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
integrate.o: /usr/X11/include/X11/Composite.h
integrate.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
integrate.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
integrate.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
integrate.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
integrate.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
integrate.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
integrate.o: /usr/X11/include/Xm/VendorS.h dispwind.h /usr/include/stdlib.h
integrate.o: /usr/include/alloca.h superpos.h XBatch.h
integrate.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
integrate.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
integrate.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
integrate.o: /usr/include/getopt.h /usr/include/sys/time.h
integrate.o: /usr/include/bits/time.h convert.h dispshare.h
lblfont.o: /usr/include/stdio.h /usr/include/features.h
lblfont.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
lblfont.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
lblfont.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
lblfont.o: /usr/include/bits/types.h /usr/include/libio.h
lblfont.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h XMDialogs.h
lblfont.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/MessageB.h
lblfont.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
lblfont.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
lblfont.o: /usr/include/time.h /usr/include/endian.h
lblfont.o: /usr/include/bits/endian.h /usr/include/sys/select.h
lblfont.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
lblfont.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
lblfont.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
lblfont.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
lblfont.o: /usr/include/string.h /usr/X11/include/X11/Core.h
lblfont.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
lblfont.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
lblfont.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
lblfont.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
lblfont.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
lblfont.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
lblfont.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
lblfont.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
lblfont.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
lblfont.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
lblfont.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
lblfont.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
lblfont.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
lblfont.o: XMCallback.h XMLabel.h /usr/X11/include/Xm/Label.h refreshctl.h
lblfont.o: panemgr.h dispwind.h /usr/include/stdlib.h /usr/include/alloca.h
lblfont.o: /usr/include/assert.h superpos.h grobjdisplay.h convert.h
lblfont.o: dispshare.h dispgrob.h XBatch.h /usr/X11/include/X11/Xos.h
lblfont.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
lblfont.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
lblfont.o: /usr/include/bits/confname.h /usr/include/getopt.h
lblfont.o: /usr/include/sys/time.h /usr/include/bits/time.h helpmenu.h
lblfont.o: lblfont.h dfltmgr.h text.h
location.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
location.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
location.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
location.o: /usr/include/features.h /usr/include/sys/cdefs.h
location.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
location.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
location.o: /usr/include/time.h /usr/include/endian.h
location.o: /usr/include/bits/endian.h /usr/include/sys/select.h
location.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
location.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
location.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
location.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
location.o: /usr/include/string.h /usr/X11/include/X11/Core.h
location.o: /usr/X11/include/X11/Composite.h
location.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
location.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
location.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
location.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
location.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
location.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
location.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
location.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
location.o: /usr/X11/include/Xm/Frame.h XMWidget.h XMLabel.h
location.o: /usr/X11/include/Xm/Label.h location.h /usr/include/stdio.h
location.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
location.o: /usr/include/libio.h /usr/include/_G_config.h
location.o: /usr/include/bits/stdio_lim.h
logging.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
logging.o: /usr/include/sys/types.h /usr/include/features.h
logging.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
logging.o: /usr/include/bits/types.h
logging.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
logging.o: /usr/include/time.h /usr/include/endian.h
logging.o: /usr/include/bits/endian.h /usr/include/sys/select.h
logging.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
logging.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
logging.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
logging.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
logging.o: /usr/include/string.h /usr/X11/include/X11/Core.h
logging.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
logging.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
logging.o: /usr/include/stdio.h
logging.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
logging.o: /usr/include/libio.h /usr/include/_G_config.h
logging.o: /usr/include/bits/stdio_lim.h /usr/include/errno.h
logging.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
logging.o: /usr/include/asm/errno.h errormsg.h XMWidget.h
logging.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
logging.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
logging.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
logging.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
logging.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
logging.o: XMDialogs.h /usr/X11/include/Xm/MessageB.h
logging.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
logging.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
logging.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
logging.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
logging.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
logging.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
logging.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
logging.o: XMCallback.h helpmenu.h logging.h
marker.o: /usr/include/errno.h /usr/include/features.h
marker.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
marker.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
marker.o: /usr/include/asm/errno.h /usr/include/string.h
marker.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h XMText.h
marker.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
marker.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
marker.o: /usr/include/bits/types.h /usr/include/time.h /usr/include/endian.h
marker.o: /usr/include/bits/endian.h /usr/include/sys/select.h
marker.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
marker.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
marker.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
marker.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
marker.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
marker.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
marker.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
marker.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
marker.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
marker.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
marker.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
marker.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/Text.h
marker.o: /usr/include/stdio.h
marker.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
marker.o: /usr/include/libio.h /usr/include/_G_config.h
marker.o: /usr/include/bits/stdio_lim.h /usr/X11/include/Xm/TextF.h XMLabel.h
marker.o: /usr/X11/include/Xm/Label.h XMPushbutton.h
marker.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
marker.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h XBatch.h
marker.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
marker.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
marker.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
marker.o: /usr/include/getopt.h /usr/include/sys/time.h
marker.o: /usr/include/bits/time.h errormsg.h marker.h grafinput.h
marker.o: XMDialogs.h /usr/X11/include/Xm/MessageB.h
marker.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
marker.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
marker.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
marker.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
marker.o: /usr/X11/include/Xm/Frame.h XMCallback.h helpmenu.h refreshctl.h
marker.o: panemgr.h dispwind.h /usr/include/stdlib.h /usr/include/alloca.h
marker.o: /usr/include/assert.h superpos.h grobjinput.h XMSeparators.h
marker.o: /usr/X11/include/Xm/Separator.h dispgrob.h convert.h dispshare.h
marker.o: colormgr.h grobjmgr.h grobjdisplay.h
menusetup.o: /usr/include/stdio.h /usr/include/features.h
menusetup.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
menusetup.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
menusetup.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
menusetup.o: /usr/include/bits/types.h /usr/include/libio.h
menusetup.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
menusetup.o: /usr/include/stdlib.h /usr/include/sys/types.h
menusetup.o: /usr/include/time.h /usr/include/endian.h
menusetup.o: /usr/include/bits/endian.h /usr/include/sys/select.h
menusetup.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
menusetup.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h properties.h
menusetup.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
menusetup.o: /usr/X11/include/X11/Xlib.h /usr/X11/include/X11/X.h
menusetup.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
menusetup.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
menusetup.o: /usr/include/string.h /usr/X11/include/X11/Core.h
menusetup.o: /usr/X11/include/X11/Composite.h
menusetup.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
menusetup.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
menusetup.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
menusetup.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
menusetup.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
menusetup.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
menusetup.o: /usr/X11/include/Xm/VendorS.h XMPushbutton.h
menusetup.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
menusetup.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
menusetup.o: menusetup.h XMMenus.h /usr/X11/include/Xm/RowColumn.h
menusetup.o: dispshare.h helpmenu.h XMDialogs.h
menusetup.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/SelectioB.h
menusetup.o: /usr/X11/include/Xm/FileSB.h XMManagers.h
menusetup.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
menusetup.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
menusetup.o: /usr/X11/include/Xm/Frame.h XMCallback.h exit.h winiomenu.h
menusetup.o: grobjiomenu.h geometrymenu.h toggles.h logging.h specchoose.h
menusetup.o: spcdisplay.h dispwind.h /usr/include/assert.h superpos.h
menusetup.o: advprompt.h panemgr.h pkgmgr.h optionmenu.h XMText.h
menusetup.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h XMLabel.h
menusetup.o: /usr/X11/include/Xm/Label.h XMScale.h
menusetup.o: /usr/X11/include/Xm/Scale.h printer.h refreshctl.h expand.h
menusetup.o: info.h marker.h sumregion.h XMList.h /usr/X11/include/Xm/List.h
menusetup.o: /usr/X11/include/Xm/ScrolledW.h convert.h dispgrob.h XBatch.h
menusetup.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
menusetup.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
menusetup.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
menusetup.o: /usr/include/getopt.h /usr/include/sys/time.h
menusetup.o: /usr/include/bits/time.h grobjinput.h XMSeparators.h
menusetup.o: /usr/X11/include/Xm/Separator.h grafinput.h guintegrate.h
menusetup.o: delgrob.h acceptgates.h trackcursor.h location.h errormsg.h
menusetup.o: superprompt.h objcopy.h copier.h lblfont.h copyatr.h
messages.o: /usr/include/stdio.h /usr/include/features.h
messages.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
messages.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
messages.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
messages.o: /usr/include/bits/types.h /usr/include/libio.h
messages.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
messages.o: /usr/include/assert.h /usr/include/string.h /usr/include/errno.h
messages.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
messages.o: /usr/include/asm/errno.h /usr/X11/include/Xm/Xm.h
messages.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
messages.o: /usr/include/sys/types.h /usr/include/time.h
messages.o: /usr/include/endian.h /usr/include/bits/endian.h
messages.o: /usr/include/sys/select.h /usr/include/bits/select.h
messages.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
messages.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
messages.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
messages.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
messages.o: /usr/X11/include/X11/Composite.h
messages.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
messages.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
messages.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
messages.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
messages.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
messages.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
messages.o: messages.h dispgrob.h XMWidget.h dispwind.h /usr/include/stdlib.h
messages.o: /usr/include/alloca.h superpos.h XBatch.h
messages.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
messages.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
messages.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
messages.o: /usr/include/getopt.h /usr/include/sys/time.h
messages.o: /usr/include/bits/time.h convert.h dispshare.h clientops.h
messages.o: ./Xamine.h
objcopy.o: /usr/include/stdio.h /usr/include/features.h
objcopy.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
objcopy.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
objcopy.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
objcopy.o: /usr/include/bits/types.h /usr/include/libio.h
objcopy.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
objcopy.o: /usr/include/string.h XMDialogs.h
objcopy.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/MessageB.h
objcopy.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
objcopy.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
objcopy.o: /usr/include/time.h /usr/include/endian.h
objcopy.o: /usr/include/bits/endian.h /usr/include/sys/select.h
objcopy.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
objcopy.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
objcopy.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
objcopy.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
objcopy.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
objcopy.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
objcopy.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
objcopy.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
objcopy.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
objcopy.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
objcopy.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/SelectioB.h
objcopy.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
objcopy.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
objcopy.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
objcopy.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
objcopy.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
objcopy.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
objcopy.o: /usr/X11/include/Xm/Frame.h XMCallback.h panemgr.h dispwind.h
objcopy.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/assert.h
objcopy.o: superpos.h dispshare.h objcopy.h copier.h XMList.h
objcopy.o: /usr/X11/include/Xm/List.h /usr/X11/include/Xm/ScrolledW.h
objcopy.o: XMLabel.h /usr/X11/include/Xm/Label.h dispgrob.h XBatch.h
objcopy.o: /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
objcopy.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
objcopy.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
objcopy.o: /usr/include/getopt.h /usr/include/sys/time.h
objcopy.o: /usr/include/bits/time.h convert.h grobjmgr.h gateio.h messages.h
objcopy.o: clientops.h ./Xamine.h helpmenu.h errormsg.h grobjdisplay.h
objcopy.o: refreshctl.h compatspec.h
panemgr.o: /usr/X11/include/X11/cursorfont.h /usr/X11/include/Xm/DrawingA.h
panemgr.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
panemgr.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
panemgr.o: /usr/include/features.h /usr/include/sys/cdefs.h
panemgr.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
panemgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
panemgr.o: /usr/include/time.h /usr/include/endian.h
panemgr.o: /usr/include/bits/endian.h /usr/include/sys/select.h
panemgr.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
panemgr.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
panemgr.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
panemgr.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
panemgr.o: /usr/include/string.h /usr/X11/include/X11/Core.h
panemgr.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
panemgr.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
panemgr.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
panemgr.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
panemgr.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
panemgr.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/X11/StringDefs.h
panemgr.o: /usr/X11/include/Xm/VendorS.h /usr/include/stdio.h
panemgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
panemgr.o: /usr/include/libio.h /usr/include/_G_config.h
panemgr.o: /usr/include/bits/stdio_lim.h XMManagers.h
panemgr.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
panemgr.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
panemgr.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h XMWidget.h
panemgr.o: menusetup.h XMMenus.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
panemgr.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
panemgr.o: /usr/X11/include/Xm/ArrowB.h panemgr.h dispwind.h
panemgr.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/assert.h
panemgr.o: superpos.h buttonsetup.h location.h XMLabel.h
panemgr.o: /usr/X11/include/Xm/Label.h pkgmgr.h dispshare.h refreshctl.h
panemgr.o: trackcursor.h colormgr.h
pkgmgr.o: XMWlist.h /usr/include/stdio.h /usr/include/features.h
pkgmgr.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
pkgmgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
pkgmgr.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
pkgmgr.o: /usr/include/bits/types.h /usr/include/libio.h
pkgmgr.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
pkgmgr.o: /usr/include/errno.h /usr/include/bits/errno.h
pkgmgr.o: /usr/include/linux/errno.h /usr/include/asm/errno.h XMWidget.h
pkgmgr.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
pkgmgr.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
pkgmgr.o: /usr/include/bits/endian.h /usr/include/sys/select.h
pkgmgr.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
pkgmgr.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
pkgmgr.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
pkgmgr.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
pkgmgr.o: /usr/include/string.h /usr/X11/include/X11/Core.h
pkgmgr.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
pkgmgr.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
pkgmgr.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
pkgmgr.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
pkgmgr.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
pkgmgr.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
pkgmgr.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
pkgmgr.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
pkgmgr.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
pkgmgr.o: /usr/X11/include/Xm/ArrowB.h pkgmgr.h
plot1d.o: /usr/include/stdio.h /usr/include/features.h
plot1d.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
plot1d.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
plot1d.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
plot1d.o: /usr/include/bits/types.h /usr/include/libio.h
plot1d.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
plot1d.o: /usr/include/math.h /usr/include/bits/huge_val.h
plot1d.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
plot1d.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h XMWidget.h
plot1d.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
plot1d.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
plot1d.o: /usr/include/bits/endian.h /usr/include/sys/select.h
plot1d.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
plot1d.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
plot1d.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
plot1d.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
plot1d.o: /usr/include/string.h /usr/X11/include/X11/Core.h
plot1d.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
plot1d.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
plot1d.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
plot1d.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
plot1d.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
plot1d.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
plot1d.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
plot1d.o: XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
plot1d.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
plot1d.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
plot1d.o: /usr/include/getopt.h /usr/include/sys/time.h
plot1d.o: /usr/include/bits/time.h scaling.h dispwind.h /usr/include/stdlib.h
plot1d.o: /usr/include/alloca.h /usr/include/assert.h superpos.h dispshare.h
plot1d.o: chanplot.h refreshctl.h panemgr.h XMManagers.h
plot1d.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
plot1d.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
plot1d.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
plot2d.o: /usr/include/stdio.h /usr/include/features.h
plot2d.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
plot2d.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
plot2d.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
plot2d.o: /usr/include/bits/types.h /usr/include/libio.h
plot2d.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
plot2d.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
plot2d.o: /usr/include/endian.h /usr/include/bits/endian.h
plot2d.o: /usr/include/sys/select.h /usr/include/bits/select.h
plot2d.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
plot2d.o: /usr/include/alloca.h /usr/include/math.h
plot2d.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
plot2d.o: /usr/include/bits/mathcalls.h
plot2d.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/float.h XMWidget.h
plot2d.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
plot2d.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
plot2d.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
plot2d.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
plot2d.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
plot2d.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
plot2d.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
plot2d.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
plot2d.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
plot2d.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
plot2d.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
plot2d.o: /usr/X11/include/Xm/VendorS.h XBatch.h /usr/X11/include/X11/Xos.h
plot2d.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
plot2d.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
plot2d.o: /usr/include/bits/confname.h /usr/include/getopt.h
plot2d.o: /usr/include/sys/time.h /usr/include/bits/time.h dispwind.h
plot2d.o: /usr/include/assert.h superpos.h dispshare.h chanplot.h
plot2d.o: refreshctl.h panemgr.h XMManagers.h /usr/X11/include/Xm/BulletinB.h
plot2d.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
plot2d.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
plot2d.o: /usr/X11/include/Xm/Frame.h scaling.h colormgr.h
printer.o: /usr/include/string.h /usr/include/features.h
printer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
printer.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
printer.o: /usr/include/stdio.h
printer.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
printer.o: /usr/include/bits/types.h /usr/include/libio.h
printer.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h XMDialogs.h
printer.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/MessageB.h
printer.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
printer.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
printer.o: /usr/include/time.h /usr/include/endian.h
printer.o: /usr/include/bits/endian.h /usr/include/sys/select.h
printer.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
printer.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
printer.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
printer.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
printer.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
printer.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
printer.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
printer.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
printer.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
printer.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
printer.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/SelectioB.h
printer.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
printer.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
printer.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
printer.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
printer.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
printer.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
printer.o: /usr/X11/include/Xm/Frame.h XMCallback.h XMText.h
printer.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h XMLabel.h
printer.o: /usr/X11/include/Xm/Label.h helpmenu.h printer.h
procede.o: XMDialogs.h /usr/include/stdio.h /usr/include/features.h
procede.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
procede.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
procede.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
procede.o: /usr/include/bits/types.h /usr/include/libio.h
procede.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
procede.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/MessageB.h
procede.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
procede.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
procede.o: /usr/include/time.h /usr/include/endian.h
procede.o: /usr/include/bits/endian.h /usr/include/sys/select.h
procede.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
procede.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
procede.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
procede.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
procede.o: /usr/include/string.h /usr/X11/include/X11/Core.h
procede.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
procede.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
procede.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
procede.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
procede.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
procede.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
procede.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
procede.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
procede.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
procede.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
procede.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
procede.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
procede.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
procede.o: XMCallback.h
properties.o: /usr/include/stdio.h /usr/include/features.h
properties.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
properties.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
properties.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
properties.o: /usr/include/bits/types.h /usr/include/libio.h
properties.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
properties.o: /usr/include/stdlib.h /usr/include/sys/types.h
properties.o: /usr/include/time.h /usr/include/endian.h
properties.o: /usr/include/bits/endian.h /usr/include/sys/select.h
properties.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
properties.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h XMWidget.h
properties.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
properties.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
properties.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
properties.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
properties.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
properties.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
properties.o: /usr/X11/include/X11/RectObj.h
properties.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
properties.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
properties.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
properties.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
properties.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
properties.o: XMDialogs.h /usr/X11/include/Xm/MessageB.h
properties.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
properties.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
properties.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
properties.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
properties.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
properties.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
properties.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
properties.o: XMCallback.h helpmenu.h errormsg.h properties.h spcdisplay.h
properties.o: dispwind.h /usr/include/assert.h superpos.h optionmenu.h
properties.o: XMText.h /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
properties.o: XMLabel.h /usr/X11/include/Xm/Label.h XMScale.h
properties.o: /usr/X11/include/Xm/Scale.h panemgr.h refreshctl.h colormgr.h
properties.o: exit.h
ptlist.o: panemgr.h XMManagers.h /usr/X11/include/Xm/BulletinB.h
ptlist.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
ptlist.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
ptlist.o: /usr/include/features.h /usr/include/sys/cdefs.h
ptlist.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
ptlist.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
ptlist.o: /usr/include/time.h /usr/include/endian.h
ptlist.o: /usr/include/bits/endian.h /usr/include/sys/select.h
ptlist.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
ptlist.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
ptlist.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
ptlist.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
ptlist.o: /usr/include/string.h /usr/X11/include/X11/Core.h
ptlist.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
ptlist.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
ptlist.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
ptlist.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
ptlist.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
ptlist.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/X11/StringDefs.h
ptlist.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/Form.h
ptlist.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
ptlist.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h XMWidget.h
ptlist.o: dispwind.h /usr/include/stdio.h
ptlist.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
ptlist.o: /usr/include/libio.h /usr/include/_G_config.h
ptlist.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
ptlist.o: /usr/include/alloca.h /usr/include/assert.h superpos.h errormsg.h
ptlist.o: dispgrob.h XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
ptlist.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
ptlist.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
ptlist.o: /usr/include/getopt.h /usr/include/sys/time.h
ptlist.o: /usr/include/bits/time.h convert.h dispshare.h ptlist.h sumregion.h
ptlist.o: XMList.h /usr/X11/include/Xm/List.h /usr/X11/include/Xm/ScrolledW.h
ptlist.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
ptlist.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
ptlist.o: /usr/X11/include/Xm/ArrowB.h XMLabel.h /usr/X11/include/Xm/Label.h
ptlist.o: XMText.h /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
ptlist.o: grobjinput.h XMDialogs.h /usr/X11/include/Xm/MessageB.h
ptlist.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
ptlist.o: XMCallback.h XMSeparators.h /usr/X11/include/Xm/Separator.h
ptlist.o: grafinput.h helpmenu.h refreshctl.h
queue.o: queue.h /usr/include/assert.h /usr/include/features.h
queue.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
reducedefault.o: /usr/include/stdio.h /usr/include/features.h
reducedefault.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
reducedefault.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
reducedefault.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
reducedefault.o: /usr/include/bits/types.h /usr/include/libio.h
reducedefault.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
reducedefault.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
reducedefault.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
reducedefault.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
reducedefault.o: /usr/include/time.h /usr/include/endian.h
reducedefault.o: /usr/include/bits/endian.h /usr/include/sys/select.h
reducedefault.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
reducedefault.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
reducedefault.o: /usr/X11/include/X11/Xfuncproto.h
reducedefault.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
reducedefault.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
reducedefault.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
reducedefault.o: /usr/X11/include/X11/Constraint.h
reducedefault.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
reducedefault.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
reducedefault.o: /usr/X11/include/X11/SM/SM.h
reducedefault.o: /usr/X11/include/X11/ICE/ICElib.h
reducedefault.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
reducedefault.o: /usr/X11/include/Xm/XmStrDefs.h
reducedefault.o: /usr/X11/include/X11/StringDefs.h
reducedefault.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/Form.h
reducedefault.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
reducedefault.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
reducedefault.o: XMWidget.h XMDialogs.h /usr/X11/include/Xm/MessageB.h
reducedefault.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
reducedefault.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
reducedefault.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
reducedefault.o: /usr/X11/include/Xm/ArrowB.h XMCallback.h dispwind.h
reducedefault.o: /usr/include/stdlib.h /usr/include/alloca.h
reducedefault.o: /usr/include/assert.h superpos.h optionmenu.h XMText.h
reducedefault.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
reducedefault.o: XMLabel.h /usr/X11/include/Xm/Label.h XMScale.h
reducedefault.o: /usr/X11/include/Xm/Scale.h dfltmgr.h helpmenu.h panemgr.h
reducedefault.o: refreshctl.h
refreshctl.o: /usr/include/stdio.h /usr/include/features.h
refreshctl.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
refreshctl.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
refreshctl.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
refreshctl.o: /usr/include/bits/types.h /usr/include/libio.h
refreshctl.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
refreshctl.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
refreshctl.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
refreshctl.o: /usr/include/time.h /usr/include/endian.h
refreshctl.o: /usr/include/bits/endian.h /usr/include/sys/select.h
refreshctl.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
refreshctl.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
refreshctl.o: /usr/X11/include/X11/Xfuncproto.h
refreshctl.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
refreshctl.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
refreshctl.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
refreshctl.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
refreshctl.o: /usr/X11/include/X11/RectObj.h
refreshctl.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
refreshctl.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
refreshctl.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
refreshctl.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
refreshctl.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
refreshctl.o: panemgr.h XMManagers.h /usr/X11/include/Xm/BulletinB.h
refreshctl.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
refreshctl.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
refreshctl.o: /usr/X11/include/Xm/Frame.h dispwind.h /usr/include/stdlib.h
refreshctl.o: /usr/include/alloca.h /usr/include/assert.h superpos.h
refreshctl.o: refreshctl.h axes.h scaling.h dispshare.h titles.h chanplot.h
refreshctl.o: dfltmgr.h colormgr.h spcdisplay.h grobjdisplay.h convert.h
refreshctl.o: dispgrob.h XBatch.h /usr/X11/include/X11/Xos.h
refreshctl.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
refreshctl.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
refreshctl.o: /usr/include/bits/confname.h /usr/include/getopt.h
refreshctl.o: /usr/include/sys/time.h /usr/include/bits/time.h pkgmgr.h
refreshctl.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
refreshctl.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
refreshctl.o: /usr/X11/include/Xm/ArrowB.h queue.h queue.cc gc.h xaminegc.h
refreshctl.o: gcmgr.h
refreshdefault.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
refreshdefault.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
refreshdefault.o: /usr/include/features.h /usr/include/sys/cdefs.h
refreshdefault.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
refreshdefault.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
refreshdefault.o: /usr/include/time.h /usr/include/endian.h
refreshdefault.o: /usr/include/bits/endian.h /usr/include/sys/select.h
refreshdefault.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
refreshdefault.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
refreshdefault.o: /usr/X11/include/X11/Xfuncproto.h
refreshdefault.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
refreshdefault.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
refreshdefault.o: /usr/X11/include/X11/Core.h
refreshdefault.o: /usr/X11/include/X11/Composite.h
refreshdefault.o: /usr/X11/include/X11/Constraint.h
refreshdefault.o: /usr/X11/include/X11/Object.h
refreshdefault.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
refreshdefault.o: /usr/X11/include/X11/SM/SMlib.h
refreshdefault.o: /usr/X11/include/X11/SM/SM.h
refreshdefault.o: /usr/X11/include/X11/ICE/ICElib.h
refreshdefault.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
refreshdefault.o: /usr/X11/include/Xm/XmStrDefs.h
refreshdefault.o: /usr/X11/include/X11/StringDefs.h
refreshdefault.o: /usr/X11/include/Xm/VendorS.h helpmenu.h XMDialogs.h
refreshdefault.o: /usr/include/stdio.h
refreshdefault.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
refreshdefault.o: /usr/include/libio.h /usr/include/_G_config.h
refreshdefault.o: /usr/include/bits/stdio_lim.h
refreshdefault.o: /usr/X11/include/Xm/MessageB.h
refreshdefault.o: /usr/X11/include/Xm/SelectioB.h
refreshdefault.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
refreshdefault.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
refreshdefault.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
refreshdefault.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
refreshdefault.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
refreshdefault.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
refreshdefault.o: /usr/X11/include/Xm/Frame.h XMCallback.h XMScale.h
refreshdefault.o: /usr/X11/include/Xm/Scale.h dfltmgr.h dispwind.h
refreshdefault.o: /usr/include/stdlib.h /usr/include/alloca.h
refreshdefault.o: /usr/include/assert.h superpos.h optionmenu.h XMText.h
refreshdefault.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
refreshdefault.o: XMLabel.h /usr/X11/include/Xm/Label.h panemgr.h
refreshdefault.o: refreshctl.h
rend1default.o: XMDialogs.h /usr/include/stdio.h /usr/include/features.h
rend1default.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
rend1default.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
rend1default.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
rend1default.o: /usr/include/bits/types.h /usr/include/libio.h
rend1default.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
rend1default.o: /usr/X11/include/X11/StringDefs.h
rend1default.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
rend1default.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
rend1default.o: /usr/include/sys/types.h /usr/include/time.h
rend1default.o: /usr/include/endian.h /usr/include/bits/endian.h
rend1default.o: /usr/include/sys/select.h /usr/include/bits/select.h
rend1default.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
rend1default.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
rend1default.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
rend1default.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
rend1default.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
rend1default.o: /usr/X11/include/X11/Constraint.h
rend1default.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
rend1default.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
rend1default.o: /usr/X11/include/X11/SM/SM.h
rend1default.o: /usr/X11/include/X11/ICE/ICElib.h
rend1default.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
rend1default.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
rend1default.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
rend1default.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
rend1default.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
rend1default.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
rend1default.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
rend1default.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
rend1default.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
rend1default.o: XMCallback.h optionmenu.h XMText.h /usr/X11/include/Xm/Text.h
rend1default.o: /usr/X11/include/Xm/TextF.h XMLabel.h
rend1default.o: /usr/X11/include/Xm/Label.h XMScale.h
rend1default.o: /usr/X11/include/Xm/Scale.h dispwind.h /usr/include/stdlib.h
rend1default.o: /usr/include/alloca.h /usr/include/assert.h superpos.h
rend1default.o: dfltmgr.h helpmenu.h refreshctl.h panemgr.h
rend2default.o: XMDialogs.h /usr/include/stdio.h /usr/include/features.h
rend2default.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
rend2default.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
rend2default.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
rend2default.o: /usr/include/bits/types.h /usr/include/libio.h
rend2default.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
rend2default.o: /usr/X11/include/X11/StringDefs.h
rend2default.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
rend2default.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
rend2default.o: /usr/include/sys/types.h /usr/include/time.h
rend2default.o: /usr/include/endian.h /usr/include/bits/endian.h
rend2default.o: /usr/include/sys/select.h /usr/include/bits/select.h
rend2default.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
rend2default.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
rend2default.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
rend2default.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
rend2default.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
rend2default.o: /usr/X11/include/X11/Constraint.h
rend2default.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
rend2default.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
rend2default.o: /usr/X11/include/X11/SM/SM.h
rend2default.o: /usr/X11/include/X11/ICE/ICElib.h
rend2default.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
rend2default.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
rend2default.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
rend2default.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
rend2default.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
rend2default.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
rend2default.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
rend2default.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
rend2default.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
rend2default.o: XMCallback.h optionmenu.h XMText.h /usr/X11/include/Xm/Text.h
rend2default.o: /usr/X11/include/Xm/TextF.h XMLabel.h
rend2default.o: /usr/X11/include/Xm/Label.h XMScale.h
rend2default.o: /usr/X11/include/Xm/Scale.h dispwind.h /usr/include/stdlib.h
rend2default.o: /usr/include/alloca.h /usr/include/assert.h superpos.h
rend2default.o: dfltmgr.h helpmenu.h colormgr.h refreshctl.h panemgr.h
scaling.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
scaling.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
scaling.o: /usr/include/features.h /usr/include/sys/cdefs.h
scaling.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
scaling.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
scaling.o: /usr/include/time.h /usr/include/endian.h
scaling.o: /usr/include/bits/endian.h /usr/include/sys/select.h
scaling.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
scaling.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
scaling.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
scaling.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
scaling.o: /usr/include/string.h /usr/X11/include/X11/Core.h
scaling.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
scaling.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
scaling.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
scaling.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
scaling.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
scaling.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
scaling.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
scaling.o: dispwind.h /usr/include/stdio.h
scaling.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
scaling.o: /usr/include/libio.h /usr/include/_G_config.h
scaling.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
scaling.o: /usr/include/alloca.h /usr/include/assert.h superpos.h scaling.h
scaling.o: dispshare.h axes.h refreshctl.h panemgr.h XMManagers.h
scaling.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
scaling.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
scaling.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
select2.o: /usr/include/stdio.h /usr/include/features.h
select2.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
select2.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
select2.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
select2.o: /usr/include/bits/types.h /usr/include/libio.h
select2.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h XMWidget.h
select2.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
select2.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
select2.o: /usr/include/bits/endian.h /usr/include/sys/select.h
select2.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
select2.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
select2.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
select2.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
select2.o: /usr/include/string.h /usr/X11/include/X11/Core.h
select2.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
select2.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
select2.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
select2.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
select2.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
select2.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
select2.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
select2.o: XMDialogs.h /usr/X11/include/Xm/MessageB.h
select2.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
select2.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
select2.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
select2.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
select2.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
select2.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
select2.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
select2.o: XMCallback.h XMLabel.h /usr/X11/include/Xm/Label.h XMText.h
select2.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h helpmenu.h
select2.o: select2.h
shared.o: /usr/include/stdlib.h /usr/include/features.h
shared.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
shared.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
shared.o: /usr/include/sys/types.h /usr/include/bits/types.h
shared.o: /usr/include/time.h /usr/include/endian.h
shared.o: /usr/include/bits/endian.h /usr/include/sys/select.h
shared.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
shared.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
shared.o: /usr/include/errno.h /usr/include/bits/errno.h
shared.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
shared.o: /usr/include/string.h /usr/include/stdio.h
shared.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
shared.o: /usr/include/libio.h /usr/include/_G_config.h
shared.o: /usr/include/bits/stdio_lim.h /usr/include/signal.h
shared.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
shared.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
shared.o: /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h
shared.o: dispshare.h
spccheck.o: spccheck.h panemgr.h XMManagers.h /usr/X11/include/Xm/BulletinB.h
spccheck.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
spccheck.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
spccheck.o: /usr/include/features.h /usr/include/sys/cdefs.h
spccheck.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
spccheck.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
spccheck.o: /usr/include/time.h /usr/include/endian.h
spccheck.o: /usr/include/bits/endian.h /usr/include/sys/select.h
spccheck.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
spccheck.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
spccheck.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
spccheck.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
spccheck.o: /usr/include/string.h /usr/X11/include/X11/Core.h
spccheck.o: /usr/X11/include/X11/Composite.h
spccheck.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
spccheck.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
spccheck.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
spccheck.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
spccheck.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
spccheck.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
spccheck.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
spccheck.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
spccheck.o: /usr/X11/include/Xm/Frame.h XMWidget.h dispwind.h
spccheck.o: /usr/include/stdio.h
spccheck.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
spccheck.o: /usr/include/libio.h /usr/include/_G_config.h
spccheck.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
spccheck.o: /usr/include/alloca.h /usr/include/assert.h superpos.h
spccheck.o: refreshctl.h dispshare.h
spcchoice.o: /usr/include/stdio.h /usr/include/features.h
spcchoice.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
spcchoice.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
spcchoice.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
spcchoice.o: /usr/include/bits/types.h /usr/include/libio.h
spcchoice.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
spcchoice.o: /usr/include/string.h /usr/X11/include/Xm/Xm.h
spcchoice.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
spcchoice.o: /usr/include/sys/types.h /usr/include/time.h
spcchoice.o: /usr/include/endian.h /usr/include/bits/endian.h
spcchoice.o: /usr/include/sys/select.h /usr/include/bits/select.h
spcchoice.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
spcchoice.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
spcchoice.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
spcchoice.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
spcchoice.o: /usr/X11/include/X11/Composite.h
spcchoice.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
spcchoice.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
spcchoice.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
spcchoice.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
spcchoice.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
spcchoice.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
spcchoice.o: /usr/include/assert.h errormsg.h XMWidget.h dispshare.h
spcchoice.o: spcchoice.h XMDialogs.h /usr/X11/include/Xm/MessageB.h
spcchoice.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
spcchoice.o: XMPushbutton.h /usr/X11/include/Xm/PushB.h
spcchoice.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
spcchoice.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
spcchoice.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
spcchoice.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
spcchoice.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
spcchoice.o: XMCallback.h
spccompatchoice.o: dispshare.h spcchoice.h XMDialogs.h /usr/include/stdio.h
spccompatchoice.o: /usr/include/features.h /usr/include/sys/cdefs.h
spccompatchoice.o: /usr/include/gnu/stubs.h
spccompatchoice.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
spccompatchoice.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
spccompatchoice.o: /usr/include/bits/types.h /usr/include/libio.h
spccompatchoice.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
spccompatchoice.o: /usr/X11/include/X11/StringDefs.h
spccompatchoice.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
spccompatchoice.o: /usr/X11/include/X11/Intrinsic.h
spccompatchoice.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
spccompatchoice.o: /usr/include/time.h /usr/include/endian.h
spccompatchoice.o: /usr/include/bits/endian.h /usr/include/sys/select.h
spccompatchoice.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
spccompatchoice.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
spccompatchoice.o: /usr/X11/include/X11/Xfuncproto.h
spccompatchoice.o: /usr/X11/include/X11/Xosdefs.h
spccompatchoice.o: /usr/X11/include/X11/Xutil.h
spccompatchoice.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
spccompatchoice.o: /usr/X11/include/X11/Core.h
spccompatchoice.o: /usr/X11/include/X11/Composite.h
spccompatchoice.o: /usr/X11/include/X11/Constraint.h
spccompatchoice.o: /usr/X11/include/X11/Object.h
spccompatchoice.o: /usr/X11/include/X11/RectObj.h
spccompatchoice.o: /usr/X11/include/X11/Shell.h
spccompatchoice.o: /usr/X11/include/X11/SM/SMlib.h
spccompatchoice.o: /usr/X11/include/X11/SM/SM.h
spccompatchoice.o: /usr/X11/include/X11/ICE/ICElib.h
spccompatchoice.o: /usr/X11/include/X11/ICE/ICE.h
spccompatchoice.o: /usr/X11/include/X11/Xatom.h
spccompatchoice.o: /usr/X11/include/Xm/XmStrDefs.h
spccompatchoice.o: /usr/X11/include/Xm/VendorS.h
spccompatchoice.o: /usr/X11/include/Xm/SelectioB.h
spccompatchoice.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
spccompatchoice.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
spccompatchoice.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
spccompatchoice.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
spccompatchoice.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
spccompatchoice.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
spccompatchoice.o: /usr/X11/include/Xm/Frame.h XMCallback.h dispwind.h
spccompatchoice.o: /usr/include/stdlib.h /usr/include/alloca.h
spccompatchoice.o: /usr/include/assert.h superpos.h panemgr.h
spcdisplay.o: /usr/include/stdio.h /usr/include/features.h
spcdisplay.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
spcdisplay.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
spcdisplay.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
spcdisplay.o: /usr/include/bits/types.h /usr/include/libio.h
spcdisplay.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
spcdisplay.o: errormsg.h XMWidget.h /usr/X11/include/X11/Intrinsic.h
spcdisplay.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
spcdisplay.o: /usr/include/time.h /usr/include/endian.h
spcdisplay.o: /usr/include/bits/endian.h /usr/include/sys/select.h
spcdisplay.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
spcdisplay.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
spcdisplay.o: /usr/X11/include/X11/Xfuncproto.h
spcdisplay.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
spcdisplay.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
spcdisplay.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
spcdisplay.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
spcdisplay.o: /usr/X11/include/X11/RectObj.h
spcdisplay.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
spcdisplay.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
spcdisplay.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
spcdisplay.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
spcdisplay.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
spcdisplay.o: dispshare.h spcdisplay.h dispwind.h /usr/include/stdlib.h
spcdisplay.o: /usr/include/alloca.h /usr/include/assert.h superpos.h
spcdisplay.o: panemgr.h XMManagers.h /usr/X11/include/Xm/BulletinB.h
spcdisplay.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
spcdisplay.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
spcdisplay.o: /usr/X11/include/Xm/Frame.h refreshctl.h dfltmgr.h exit.h
spcdisplay.o: pkgmgr.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
spcdisplay.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
spcdisplay.o: /usr/X11/include/Xm/ArrowB.h
spclimchoice.o: dispshare.h spcchoice.h XMDialogs.h /usr/include/stdio.h
spclimchoice.o: /usr/include/features.h /usr/include/sys/cdefs.h
spclimchoice.o: /usr/include/gnu/stubs.h
spclimchoice.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
spclimchoice.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
spclimchoice.o: /usr/include/bits/types.h /usr/include/libio.h
spclimchoice.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
spclimchoice.o: /usr/X11/include/X11/StringDefs.h
spclimchoice.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
spclimchoice.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
spclimchoice.o: /usr/include/sys/types.h /usr/include/time.h
spclimchoice.o: /usr/include/endian.h /usr/include/bits/endian.h
spclimchoice.o: /usr/include/sys/select.h /usr/include/bits/select.h
spclimchoice.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
spclimchoice.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
spclimchoice.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
spclimchoice.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
spclimchoice.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
spclimchoice.o: /usr/X11/include/X11/Constraint.h
spclimchoice.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
spclimchoice.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
spclimchoice.o: /usr/X11/include/X11/SM/SM.h
spclimchoice.o: /usr/X11/include/X11/ICE/ICElib.h
spclimchoice.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
spclimchoice.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
spclimchoice.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
spclimchoice.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
spclimchoice.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
spclimchoice.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
spclimchoice.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
spclimchoice.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
spclimchoice.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
spclimchoice.o: XMCallback.h
specchoose.o: /usr/include/stdio.h /usr/include/features.h
specchoose.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
specchoose.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
specchoose.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
specchoose.o: /usr/include/bits/types.h /usr/include/libio.h
specchoose.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
specchoose.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
specchoose.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
specchoose.o: /usr/include/time.h /usr/include/endian.h
specchoose.o: /usr/include/bits/endian.h /usr/include/sys/select.h
specchoose.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
specchoose.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
specchoose.o: /usr/X11/include/X11/Xfuncproto.h
specchoose.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
specchoose.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
specchoose.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
specchoose.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
specchoose.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
specchoose.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
specchoose.o: /usr/X11/include/X11/ICE/ICElib.h
specchoose.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
specchoose.o: /usr/X11/include/Xm/XmStrDefs.h
specchoose.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
specchoose.o: /usr/include/ctype.h /usr/include/stdlib.h
specchoose.o: /usr/include/alloca.h XMDialogs.h
specchoose.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/SelectioB.h
specchoose.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
specchoose.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
specchoose.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
specchoose.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
specchoose.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
specchoose.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
specchoose.o: /usr/X11/include/Xm/Frame.h XMCallback.h dispshare.h
specchoose.o: specchoose.h helpmenu.h errormsg.h spcdisplay.h dispwind.h
specchoose.o: /usr/include/assert.h superpos.h
spectra.o: /usr/include/stdio.h /usr/include/features.h
spectra.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
spectra.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
spectra.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
spectra.o: /usr/include/bits/types.h /usr/include/libio.h
spectra.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
spectra.o: /usr/include/ctype.h /usr/include/endian.h
spectra.o: /usr/include/bits/endian.h /usr/include/string.h
spectra.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
spectra.o: /usr/include/sys/select.h /usr/include/bits/select.h
spectra.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
spectra.o: /usr/include/alloca.h dispshare.h
sumregion.o: /usr/include/stdio.h /usr/include/features.h
sumregion.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
sumregion.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
sumregion.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
sumregion.o: /usr/include/bits/types.h /usr/include/libio.h
sumregion.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
sumregion.o: /usr/include/string.h /usr/include/errno.h
sumregion.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
sumregion.o: /usr/include/asm/errno.h XMWidget.h
sumregion.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
sumregion.o: /usr/include/sys/types.h /usr/include/time.h
sumregion.o: /usr/include/endian.h /usr/include/bits/endian.h
sumregion.o: /usr/include/sys/select.h /usr/include/bits/select.h
sumregion.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
sumregion.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
sumregion.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
sumregion.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
sumregion.o: /usr/X11/include/X11/Composite.h
sumregion.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
sumregion.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
sumregion.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
sumregion.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
sumregion.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
sumregion.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
sumregion.o: /usr/X11/include/Xm/VendorS.h XMPushbutton.h
sumregion.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
sumregion.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
sumregion.o: XMLabel.h /usr/X11/include/Xm/Label.h XMText.h
sumregion.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h XMList.h
sumregion.o: /usr/X11/include/Xm/List.h /usr/X11/include/Xm/ScrolledW.h
sumregion.o: XBatch.h /usr/X11/include/X11/Xos.h /usr/include/fcntl.h
sumregion.o: /usr/include/bits/fcntl.h /usr/include/unistd.h
sumregion.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
sumregion.o: /usr/include/getopt.h /usr/include/sys/time.h
sumregion.o: /usr/include/bits/time.h dispgrob.h /usr/include/assert.h
sumregion.o: dispwind.h /usr/include/stdlib.h /usr/include/alloca.h
sumregion.o: superpos.h convert.h dispshare.h grobjmgr.h grobjdisplay.h
sumregion.o: refreshctl.h panemgr.h XMManagers.h
sumregion.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
sumregion.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
sumregion.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
sumregion.o: sumregion.h grobjinput.h XMDialogs.h
sumregion.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/SelectioB.h
sumregion.o: /usr/X11/include/Xm/FileSB.h XMCallback.h XMSeparators.h
sumregion.o: /usr/X11/include/Xm/Separator.h grafinput.h helpmenu.h
sumregion.o: errormsg.h colormgr.h chanplot.h
superpos.o: /usr/include/assert.h /usr/include/features.h
superpos.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h superpos.h
superprompt.o: /usr/include/stdio.h /usr/include/features.h
superprompt.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
superprompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
superprompt.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
superprompt.o: /usr/include/bits/types.h /usr/include/libio.h
superprompt.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
superprompt.o: XMDialogs.h /usr/X11/include/X11/StringDefs.h
superprompt.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
superprompt.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
superprompt.o: /usr/include/sys/types.h /usr/include/time.h
superprompt.o: /usr/include/endian.h /usr/include/bits/endian.h
superprompt.o: /usr/include/sys/select.h /usr/include/bits/select.h
superprompt.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
superprompt.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
superprompt.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
superprompt.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
superprompt.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
superprompt.o: /usr/X11/include/X11/Constraint.h
superprompt.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
superprompt.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
superprompt.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
superprompt.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
superprompt.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
superprompt.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
superprompt.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
superprompt.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
superprompt.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
superprompt.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
superprompt.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
superprompt.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
superprompt.o: XMCallback.h superprompt.h dispwind.h /usr/include/stdlib.h
superprompt.o: /usr/include/alloca.h /usr/include/assert.h superpos.h
superprompt.o: spcdisplay.h dispshare.h helpmenu.h panemgr.h errormsg.h
superprompt.o: specchoose.h compatspec.h XMList.h /usr/X11/include/Xm/List.h
superprompt.o: /usr/X11/include/Xm/ScrolledW.h XMLabel.h
superprompt.o: /usr/X11/include/Xm/Label.h refreshctl.h menusetup.h XMMenus.h
superprompt.o: exit.h
text.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
text.o: /usr/include/gnu/stubs.h
text.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
text.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
text.o: /usr/include/bits/types.h /usr/include/libio.h
text.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
text.o: /usr/include/string.h /usr/X11/include/X11/Xlib.h
text.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
text.o: /usr/include/bits/endian.h /usr/include/sys/select.h
text.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
text.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
text.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
text.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xos.h
text.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h /usr/include/unistd.h
text.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
text.o: /usr/include/getopt.h /usr/include/sys/time.h
text.o: /usr/include/bits/time.h /usr/X11/include/X11/Xatom.h text.h
text.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Intrinsic.h
text.o: /usr/X11/include/X11/Xresource.h /usr/X11/include/X11/Core.h
text.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
text.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
text.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
text.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
text.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/Xm/XmStrDefs.h
text.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/VendorS.h
titledefault.o: XMDialogs.h /usr/include/stdio.h /usr/include/features.h
titledefault.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
titledefault.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
titledefault.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
titledefault.o: /usr/include/bits/types.h /usr/include/libio.h
titledefault.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
titledefault.o: /usr/X11/include/X11/StringDefs.h
titledefault.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
titledefault.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
titledefault.o: /usr/include/sys/types.h /usr/include/time.h
titledefault.o: /usr/include/endian.h /usr/include/bits/endian.h
titledefault.o: /usr/include/sys/select.h /usr/include/bits/select.h
titledefault.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
titledefault.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
titledefault.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
titledefault.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
titledefault.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
titledefault.o: /usr/X11/include/X11/Constraint.h
titledefault.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
titledefault.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
titledefault.o: /usr/X11/include/X11/SM/SM.h
titledefault.o: /usr/X11/include/X11/ICE/ICElib.h
titledefault.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
titledefault.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
titledefault.o: /usr/X11/include/Xm/SelectioB.h /usr/X11/include/Xm/FileSB.h
titledefault.o: XMWidget.h XMPushbutton.h /usr/X11/include/Xm/PushB.h
titledefault.o: /usr/X11/include/Xm/CascadeB.h /usr/X11/include/Xm/ToggleB.h
titledefault.o: /usr/X11/include/Xm/ArrowB.h XMManagers.h
titledefault.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
titledefault.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
titledefault.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
titledefault.o: XMCallback.h dispwind.h /usr/include/stdlib.h
titledefault.o: /usr/include/alloca.h /usr/include/assert.h superpos.h
titledefault.o: dfltmgr.h helpmenu.h optionmenu.h XMText.h
titledefault.o: /usr/X11/include/Xm/Text.h /usr/X11/include/Xm/TextF.h
titledefault.o: XMLabel.h /usr/X11/include/Xm/Label.h XMScale.h
titledefault.o: /usr/X11/include/Xm/Scale.h panemgr.h refreshctl.h
titles.o: /usr/include/stdio.h /usr/include/features.h
titles.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
titles.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
titles.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
titles.o: /usr/include/bits/types.h /usr/include/libio.h
titles.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
titles.o: /usr/include/string.h /usr/X11/include/X11/Xlib.h
titles.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
titles.o: /usr/include/bits/endian.h /usr/include/sys/select.h
titles.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
titles.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
titles.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
titles.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xos.h
titles.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
titles.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
titles.o: /usr/include/bits/confname.h /usr/include/getopt.h
titles.o: /usr/include/sys/time.h /usr/include/bits/time.h
titles.o: /usr/X11/include/X11/Xatom.h dispshare.h titles.h XMWidget.h
titles.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xresource.h
titles.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
titles.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
titles.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
titles.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
titles.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
titles.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
titles.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
titles.o: dispwind.h /usr/include/stdlib.h /usr/include/alloca.h
titles.o: /usr/include/assert.h superpos.h refreshctl.h panemgr.h
titles.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
titles.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
titles.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
titles.o: /usr/X11/include/Xm/Frame.h axes.h text.h chanplot.h
toggles.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/Xm.h
toggles.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
toggles.o: /usr/include/sys/types.h /usr/include/features.h
toggles.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
toggles.o: /usr/include/bits/types.h
toggles.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
toggles.o: /usr/include/time.h /usr/include/endian.h
toggles.o: /usr/include/bits/endian.h /usr/include/sys/select.h
toggles.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
toggles.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
toggles.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
toggles.o: /usr/X11/include/X11/Xutil.h /usr/X11/include/X11/Xresource.h
toggles.o: /usr/include/string.h /usr/X11/include/X11/Core.h
toggles.o: /usr/X11/include/X11/Composite.h /usr/X11/include/X11/Constraint.h
toggles.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
toggles.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
toggles.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
toggles.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
toggles.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/X11/StringDefs.h
toggles.o: /usr/X11/include/Xm/VendorS.h XMWidget.h toggles.h
trackcursor.o: XMWidget.h /usr/X11/include/X11/Intrinsic.h
trackcursor.o: /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
trackcursor.o: /usr/include/features.h /usr/include/sys/cdefs.h
trackcursor.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
trackcursor.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
trackcursor.o: /usr/include/time.h /usr/include/endian.h
trackcursor.o: /usr/include/bits/endian.h /usr/include/sys/select.h
trackcursor.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
trackcursor.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
trackcursor.o: /usr/X11/include/X11/Xfuncproto.h
trackcursor.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
trackcursor.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
trackcursor.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
trackcursor.o: /usr/X11/include/X11/Constraint.h
trackcursor.o: /usr/X11/include/X11/Object.h /usr/X11/include/X11/RectObj.h
trackcursor.o: /usr/X11/include/X11/StringDefs.h /usr/X11/include/Xm/Xm.h
trackcursor.o: /usr/X11/include/X11/Shell.h /usr/X11/include/X11/SM/SMlib.h
trackcursor.o: /usr/X11/include/X11/SM/SM.h /usr/X11/include/X11/ICE/ICElib.h
trackcursor.o: /usr/X11/include/X11/ICE/ICE.h /usr/X11/include/X11/Xatom.h
trackcursor.o: /usr/X11/include/Xm/XmStrDefs.h /usr/X11/include/Xm/VendorS.h
trackcursor.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
trackcursor.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
trackcursor.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
trackcursor.o: /usr/X11/include/Xm/Frame.h trackcursor.h location.h
trackcursor.o: /usr/include/stdio.h
trackcursor.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
trackcursor.o: /usr/include/libio.h /usr/include/_G_config.h
trackcursor.o: /usr/include/bits/stdio_lim.h XMLabel.h
trackcursor.o: /usr/X11/include/Xm/Label.h buttonsetup.h panemgr.h dispwind.h
trackcursor.o: /usr/include/stdlib.h /usr/include/alloca.h
trackcursor.o: /usr/include/assert.h superpos.h dispshare.h convert.h
trackcursor.o: spcdisplay.h
winiomenu.o: /usr/include/stdio.h /usr/include/features.h
winiomenu.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
winiomenu.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
winiomenu.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
winiomenu.o: /usr/include/bits/types.h /usr/include/libio.h
winiomenu.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
winiomenu.o: /usr/include/errno.h /usr/include/bits/errno.h
winiomenu.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
winiomenu.o: /usr/X11/include/X11/StringDefs.h XMDialogs.h
winiomenu.o: /usr/X11/include/Xm/MessageB.h /usr/X11/include/Xm/Xm.h
winiomenu.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xlib.h
winiomenu.o: /usr/include/sys/types.h /usr/include/time.h
winiomenu.o: /usr/include/endian.h /usr/include/bits/endian.h
winiomenu.o: /usr/include/sys/select.h /usr/include/bits/select.h
winiomenu.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
winiomenu.o: /usr/X11/include/X11/X.h /usr/X11/include/X11/Xfuncproto.h
winiomenu.o: /usr/X11/include/X11/Xosdefs.h /usr/X11/include/X11/Xutil.h
winiomenu.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
winiomenu.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
winiomenu.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
winiomenu.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/Shell.h
winiomenu.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
winiomenu.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
winiomenu.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
winiomenu.o: /usr/X11/include/Xm/VendorS.h /usr/X11/include/Xm/SelectioB.h
winiomenu.o: /usr/X11/include/Xm/FileSB.h XMWidget.h XMPushbutton.h
winiomenu.o: /usr/X11/include/Xm/PushB.h /usr/X11/include/Xm/CascadeB.h
winiomenu.o: /usr/X11/include/Xm/ToggleB.h /usr/X11/include/Xm/ArrowB.h
winiomenu.o: XMManagers.h /usr/X11/include/Xm/BulletinB.h
winiomenu.o: /usr/X11/include/Xm/Form.h /usr/X11/include/Xm/RowColumn.h
winiomenu.o: /usr/X11/include/Xm/PanedW.h /usr/X11/include/Xm/MainW.h
winiomenu.o: /usr/X11/include/Xm/Frame.h XMCallback.h helpmenu.h winiomenu.h
winiomenu.o: dispwind.h /usr/include/stdlib.h /usr/include/alloca.h
winiomenu.o: /usr/include/assert.h superpos.h panemgr.h exit.h refreshctl.h
winiomenu.o: errormsg.h buttonsetup.h location.h XMLabel.h
winiomenu.o: /usr/X11/include/Xm/Label.h menusetup.h XMMenus.h
wintest.o: dispshare.h dispwind.h /usr/include/stdio.h
wintest.o: /usr/include/features.h /usr/include/sys/cdefs.h
wintest.o: /usr/include/gnu/stubs.h
wintest.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
wintest.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
wintest.o: /usr/include/bits/types.h /usr/include/libio.h
wintest.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
wintest.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
wintest.o: /usr/include/endian.h /usr/include/bits/endian.h
wintest.o: /usr/include/sys/select.h /usr/include/bits/select.h
wintest.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
wintest.o: /usr/include/alloca.h /usr/include/assert.h superpos.h
xaminegc.o: gc.h /usr/X11/include/X11/Xlib.h /usr/include/sys/types.h
xaminegc.o: /usr/include/features.h /usr/include/sys/cdefs.h
xaminegc.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
xaminegc.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stddef.h
xaminegc.o: /usr/include/time.h /usr/include/endian.h
xaminegc.o: /usr/include/bits/endian.h /usr/include/sys/select.h
xaminegc.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
xaminegc.o: /usr/include/sys/sysmacros.h /usr/X11/include/X11/X.h
xaminegc.o: /usr/X11/include/X11/Xfuncproto.h /usr/X11/include/X11/Xosdefs.h
xaminegc.o: xaminegc.h dispwind.h /usr/include/stdio.h
xaminegc.o: /usr/lib/gcc-lib/i386-linux/2.95.2/include/stdarg.h
xaminegc.o: /usr/include/libio.h /usr/include/_G_config.h
xaminegc.o: /usr/include/bits/stdio_lim.h /usr/include/stdlib.h
xaminegc.o: /usr/include/alloca.h /usr/include/assert.h superpos.h XMWidget.h
xaminegc.o: /usr/X11/include/X11/Intrinsic.h /usr/X11/include/X11/Xutil.h
xaminegc.o: /usr/X11/include/X11/Xresource.h /usr/include/string.h
xaminegc.o: /usr/X11/include/X11/Core.h /usr/X11/include/X11/Composite.h
xaminegc.o: /usr/X11/include/X11/Constraint.h /usr/X11/include/X11/Object.h
xaminegc.o: /usr/X11/include/X11/RectObj.h /usr/X11/include/X11/StringDefs.h
xaminegc.o: /usr/X11/include/Xm/Xm.h /usr/X11/include/X11/Shell.h
xaminegc.o: /usr/X11/include/X11/SM/SMlib.h /usr/X11/include/X11/SM/SM.h
xaminegc.o: /usr/X11/include/X11/ICE/ICElib.h /usr/X11/include/X11/ICE/ICE.h
xaminegc.o: /usr/X11/include/X11/Xatom.h /usr/X11/include/Xm/XmStrDefs.h
xaminegc.o: /usr/X11/include/Xm/VendorS.h text.h /usr/X11/include/X11/Xos.h
xaminegc.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
xaminegc.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
xaminegc.o: /usr/include/bits/confname.h /usr/include/getopt.h
xaminegc.o: /usr/include/sys/time.h /usr/include/bits/time.h colormgr.h
xaminegc.o: chanplot.h refreshctl.h panemgr.h XMManagers.h
xaminegc.o: /usr/X11/include/Xm/BulletinB.h /usr/X11/include/Xm/Form.h
xaminegc.o: /usr/X11/include/Xm/RowColumn.h /usr/X11/include/Xm/PanedW.h
xaminegc.o: /usr/X11/include/Xm/MainW.h /usr/X11/include/Xm/Frame.h
