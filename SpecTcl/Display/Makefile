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
CYGWINLIB=-L/usr/local/LessTif/Motif2.0/lib -lSM -lICE -lfl
LinuxLIB = -lfl
OSF1Lib  = -lfl
HPUXLIB  = -lfl

#
#  MOTIFINCLUDES may need to be overridden. This is correct for Linux.
#


CCFLAGS= -c -w -I. -DXTFUNCPROTO $($(OS)MOTIFINCLUDES) $(HOMEDEF) -D$(OS)
XCCFLAGS=-c -w -I. -DXTFUNCPROTO -DXAMINE $($(OS)MOTIFINCLUDES) $(HOMEDEF) -D$(OS)
CC      = $($(OS)CC)
CPP     = $($(OS)CPP)
LD     =  $($(OS)LD) -L/usr/lesstif/lib/Xm-2.0/.libs
YACC   = bison
LEX    = flex
LEXOUT = lex.yy.c

#
#  The F77 compiler variable is not part of the sysdefs.sh nor sysdefs.bash
#  shell scripts because most people will not want to build the Fortran
#  interface, and most people will not have neither the Intel Fortran
#  Compiler, nor the Compaq Fortran Compiler.  Thus, F77 is ``echo'' by
#  default, which causes nothing to be built.  Uncomment the appropriate
#  line for your operating system below and the AEDOBJS definition, if you
#  wish to build the Fortran interface.
#
F77    = echo

#
#  For OSF1:
#
#F77    = f77 -check bounds

#
#  For Linux:
#  Using the Intel Fortran Compiler.  You must source in the IFC shell
#  variables script, which can be done for the 5.0 compiler by running:
#  . /opt/intel/compiler50/ia32/bin/ifcvars.sh
#
#  For the 6.0 compiler:
#  . /opt/intel/compiler60/ia32/bin/ifcvars.sh
#
#F77    = ifc -w90 -w95

#
#  Fortran interface objects.  By default this is nothing.
#
AEDOBJS = 

#
#  Uncommenting the following line causes the Fortran inferface to be built.
#
#AEDOBJS = aedxamine.o

.DEFAULT:
	echo Would sccs get $<

system: XAMINE_FILES WINDOW_FILE  GROBJ_FILE  DISP_SHARED \
	MOTIF_LIB COLORSET Xamine CLIENT
	echo "System rebuild complete"

depend:
	makedepend  -Y. *.cc


clean:
	rm -rf *.o *.a Xamine *_lex.cc *_parse.cc *_lex.h

install:
	install -m 0664    aedpublic.for $(DESTDIR)/Include/aedpublic.for
	install -m 0664    aedshare.for xamineshare.for $(DESTDIR)/Include
	install -m 0664    client.h $(DESTDIR)/Include/Xamine.h
	install -m 0664    clientops.h $(DESTDIR)/Include/clientops.h
	install -m 0664    clientgates.h $(DESTDIR)/Include/clientgates.h
	install -m 0775    Xamine $(DESTDIR)/Bin/Xamine2_0
	install -m 0775     colorset $(DESTDIR)/Bin/colorset
	install -m 0664    libXamine.a $(DESTDIR)/Lib
	install -m 0664    *.ctbl $(DESTDIR)/Etc
	install -m 0664    *.ico  $(DESTDIR)/Etc
	echo "Xamine installation complete"

WINDOW_FILE: windfile_parse.o windfile_lex.o dispwind.o defaultfile_lex.o
	touch WINDOW_FILE
	echo "Window file I/O subsystem built"

WINDOW_TEST: wintest
	echo "Windows file test suite finished"
	touch WINDOW_TEST

XAMINE_FILES: menusetup.o exit.o helpmenu.o winiomenu.o grobjiomenu.o \
	geometrymenu.o \
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
	ptlist.o lexsupport.o xaminegc.o gcmgr.o copier.o gc.o Tab.o griprint.o
	echo "Xamine objects built"
	touch XAMINE_FILES

wintest: wintest.cc WINDOW_FILE spectra.o
	$(CPP) $(XCCFLAGS) wintest.cc
	$(LD) -o wintest wintest.o dispwind.o windfile_parse.o windfile_lex.o \
	spectra.o \
	-y  $($(OS)LIB) 


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
		grobfile_parse.o grobjdb.o grobfile.o -ly $($(OS)LIB)


DISP_SHARED: spectra.o
	echo "Spectrum access package rebuilt"
	touch DISP_SHARED

spectra.o: spectra.cc dispshare.h
	$(CPP) $(XCCFLAGS) spectra.cc

MOTIF_LIB: XMCallback.o XMMenus.o XMDialogs.o XMManagers.o XMWlist.o \
	XMShell.o copier.o
	touch MOTIF_LIB
	echo "MOTIF Library built"

XMCallback.o: XMCallback.cc XMWidget.h
	$(CPP) $(XCCFLAGS) XMCallback.cc

XMMenus.o: XMMenus.cc XMWidget.h XMPushbutton.h XMMenus.h
	$(CPP) $(XCCFLAGS) XMMenus.cc

Tab.o: Tab.c Tab.h TabP.h
	$(CPP) -g $(XCCFLAGS) Tab.c

Xamine: Xamine.o \
	 MOTIF_LIB \
	 WINDOW_FILE \
	 XAMINE_FILES \
	GROBJ_FILE
	$(LD) -o Xamine Xamine.o XMCallback.o XMMenus.o XMDialogs.o \
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
	spccompatchoice.o spclimchoice.o ptlist.o lexsupport.o gc.o xaminegc.o\
	gcmgr.o Tab.o griprint.o \
	 -lXm -lXt -lX11 -lm $($(OS)LIB)


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
	XMText.h XMLabel.h XMScale.h XMTab.h marker.h dispwind.h superpos.h sumregion.h \
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
	XMDialogs.o XMMenus.o XMCallback.o colormgr.o -lXm -lXt -lX11 \
	$($(OS)LIB)

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
	$(CPP) $(XCCFLAGS) advprompt.cc

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
	printer.h XMWidget.h XMTab.h
	$(CPP) $(XCCFLAGS) printer.cc

griprint.o: griprint.cc printer.h griprint.h
	$(CPP) $(XCCFLAGS) -Wall griprint.cc

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
	$(CPP) $(XCCFLAGS) plot2d.cc

colormgr.o: colormgr.cc XMWidget.h colormgr.h
	$(CPP) $(XCCFLAGS) colormgr.cc

CLIENT: libXamine.a Xaminetest
	echo "Client software and tests built"
	touch CLIENT

libXamine.a: client.o  clientgates.o prccheck.o allocator.o \
	specalloc.o clientbuttons.o $(AEDOBJS)
	rm -f libXamine.a 
	ar cq libXamine.a $(AEDOBJS) client.o  clientgates.o prccheck.o \
	allocator.o specalloc.o clientbuttons.o
	ranlib libXamine.a

Xaminetest: Xaminetest.o libXamine.a
	$(LD) -L. -o Xaminetest Xaminetest.o client.o -lXamine  -lm \
	$($(OS)LIB)

Xaminetest.o: Xaminetest.c client.h
	$(CC) $(CCFLAGS) Xaminetest.c

client.o: client.c client.h clientgates.h allocator.h
	$(CC) $(CCFLAGS) client.c

shared.o: shared.cc dispshare.h
	$(CPP) $(CCFLAGS) shared.cc

aedxamine.o: aedxamine.for aedshare.for xamineshare.for
	$($(OS)F77) -c aedxamine.for

demo: demo.o libXamine.a
#	$(F77) -L. -L/daq/lib -o demo demo.o -lXamine -lc -lqtree -lfile -lcvt
	$(F77) -L. -L$(INSTDIR)/Lib -o demo demo.o -lXamine
#	 -lXamine  -lqtree -lfile -lcvt

demo.o: demo.for aedshare.for aedpublic.for
	$(F77) -c demo.for


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
	$(CPP) $(XCCFLAGS) messages.cc

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
	$(CC) $(CCFLAGS) clientgates.c


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
	errormsg.o helpmenu.o XMCallback.o \
	-lXm -lXt -lX11 $($(OS)LIB)

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

acceptgates.o: XMWidget.h XBatch.h convert.h dispwind.h superpos.h
acceptgates.o: dispshare.h dispgrob.h acceptgates.h sumregion.h XMList.h
acceptgates.o: XMPushbutton.h XMLabel.h XMText.h grobjinput.h XMManagers.h
acceptgates.o: XMDialogs.h XMCallback.h XMSeparators.h grafinput.h helpmenu.h
acceptgates.o: refreshctl.h panemgr.h grobjdisplay.h colormgr.h chanplot.h
acceptgates.o: grobjmgr.h errormsg.h gateio.h messages.h clientops.h
acceptgates.o: ./Xamine.h
advprompt.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
advprompt.o: helpmenu.h advprompt.h spcdisplay.h dispwind.h superpos.h
advprompt.o: errormsg.h
attribdefault.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h
attribdefault.o: XMCallback.h XMLabel.h XMText.h helpmenu.h dispwind.h
attribdefault.o: superpos.h dfltmgr.h optionmenu.h XMScale.h panemgr.h
attribdefault.o: refreshctl.h
axes.o: XMWidget.h XBatch.h axes.h dispwind.h superpos.h refreshctl.h
axes.o: panemgr.h XMManagers.h dispshare.h text.h gcmgr.h xaminegc.h gc.h
axisdefault.o: XMPushbutton.h XMWidget.h XMDialogs.h XMManagers.h
axisdefault.o: XMCallback.h helpmenu.h dispwind.h superpos.h dfltmgr.h
axisdefault.o: optionmenu.h XMText.h XMLabel.h XMScale.h panemgr.h
axisdefault.o: refreshctl.h
buttondlg.o: XMManagers.h XMWidget.h XMPushbutton.h XMShell.h XMCallback.h
buttondlg.o: buttondlg.h clientops.h ./Xamine.h
buttonreq.o: XMShell.h XMWidget.h XMCallback.h XMPushbutton.h XMDialogs.h
buttonreq.o: XMManagers.h dispshare.h errormsg.h dispwind.h superpos.h
buttonreq.o: gateio.h dispgrob.h XBatch.h convert.h messages.h clientops.h
buttonreq.o: ./Xamine.h buttonreq.h buttondlg.h panemgr.h spcchoice.h
buttonreq.o: ptlist.h sumregion.h XMList.h XMLabel.h XMText.h grobjinput.h
buttonreq.o: XMSeparators.h grafinput.h helpmenu.h refreshctl.h
buttonsetup.o: XMManagers.h XMWidget.h XMPushbutton.h buttonsetup.h
buttonsetup.o: location.h XMLabel.h geometrymenu.h panemgr.h dispwind.h
buttonsetup.o: superpos.h specchoose.h XMDialogs.h XMCallback.h spcdisplay.h
buttonsetup.o: pkgmgr.h refreshctl.h expand.h info.h marker.h sumregion.h
buttonsetup.o: XMList.h XMText.h convert.h dispshare.h dispgrob.h XBatch.h
buttonsetup.o: grobjinput.h XMSeparators.h grafinput.h helpmenu.h
buttonsetup.o: guintegrate.h acceptgates.h trackcursor.h properties.h
chanplot.o: XMWidget.h refreshctl.h panemgr.h XMManagers.h dispwind.h
chanplot.o: superpos.h axes.h scaling.h dispshare.h chanplot.h colormgr.h
chanplot.o: gc.h xaminegc.h gcmgr.h
checklist.o: XMManagers.h XMWidget.h XMPushbutton.h XMLabel.h checklist.h
clirequests.o: acceptgates.h XMWidget.h convert.h dispwind.h superpos.h
clirequests.o: dispshare.h dispgrob.h XBatch.h grobjdisplay.h refreshctl.h
clirequests.o: panemgr.h XMManagers.h errormsg.h clirequests.h gateio.h
clirequests.o: messages.h clientops.h ./Xamine.h grobjmgr.h buttonreq.h
colormgr.o: XMWidget.h colormgr.h
colorset.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
colorset.o: XMMenus.h helpmenu.h errormsg.h procede.h colormgr.h
compatspec.o: compatspec.h XMManagers.h XMWidget.h XMList.h XMPushbutton.h
compatspec.o: XMLabel.h dispshare.h panemgr.h dispwind.h superpos.h
convert.o: convert.h XMWidget.h dispwind.h superpos.h dispshare.h axes.h
convert.o: refreshctl.h panemgr.h XMManagers.h
copier.o: copier.h XMPushbutton.h XMWidget.h XMManagers.h XMList.h XMLabel.h
copyatr.o: dispwind.h superpos.h panemgr.h XMManagers.h XMWidget.h
copyatr.o: checklist.h XMPushbutton.h XMLabel.h compatspec.h XMList.h
copyatr.o: XMDialogs.h XMCallback.h errormsg.h helpmenu.h refreshctl.h
delgrob.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
delgrob.o: dispgrob.h dispwind.h superpos.h XBatch.h convert.h dispshare.h
delgrob.o: helpmenu.h delgrob.h grobjmgr.h panemgr.h errormsg.h refreshctl.h
dfltmgr.o: panemgr.h XMManagers.h XMWidget.h dispwind.h superpos.h
dfltmgr.o: grobjdisplay.h convert.h dispshare.h dispgrob.h XBatch.h
dfltmgr.o: refreshctl.h dfltmgr.h
dispgrob.o: dispgrob.h XMWidget.h dispwind.h superpos.h XBatch.h convert.h
dispgrob.o: dispshare.h sumregion.h XMList.h XMPushbutton.h XMLabel.h
dispgrob.o: XMText.h grobjinput.h XMManagers.h XMDialogs.h XMCallback.h
dispgrob.o: XMSeparators.h grafinput.h helpmenu.h refreshctl.h panemgr.h
dispgrob.o: acceptgates.h grobjdisplay.h marker.h gcmgr.h xaminegc.h gc.h
dispgrob.o: chanplot.h
dispwind.o: dispwind.h superpos.h errormsg.h XMWidget.h panemgr.h
dispwind.o: XMManagers.h dispshare.h
errormsg.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
eventflags.o: eventflags.h
exit.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
exit.o: panemgr.h dispwind.h superpos.h helpmenu.h exit.h winiomenu.h
expand.o: XMWidget.h XMDialogs.h XMPushbutton.h XMManagers.h XMCallback.h
expand.o: errormsg.h panemgr.h dispwind.h superpos.h helpmenu.h expand.h
expand.o: refreshctl.h exit.h select2.h XMLabel.h XMText.h grafinput.h
expand.o: dispshare.h convert.h colormgr.h gcmgr.h xaminegc.h gc.h
gateio.o: gateio.h XMWidget.h dispgrob.h dispwind.h superpos.h XBatch.h
gateio.o: convert.h dispshare.h messages.h clientops.h ./Xamine.h
gc.o: gc.h
gcmgr.o: xaminegc.h dispwind.h superpos.h gc.h XMWidget.h grobjdisplay.h
gcmgr.o: convert.h dispshare.h dispgrob.h XBatch.h refreshctl.h panemgr.h
gcmgr.o: XMManagers.h
geometrymenu.o: XMLabel.h XMWidget.h XMDialogs.h XMPushbutton.h XMManagers.h
geometrymenu.o: XMCallback.h XMSeparators.h helpmenu.h errormsg.h panemgr.h
geometrymenu.o: dispwind.h superpos.h menusetup.h XMMenus.h exit.h
grafinput.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
grafinput.o: grafinput.h helpmenu.h refreshctl.h panemgr.h dispwind.h
grafinput.o: superpos.h convert.h dispshare.h
griprint.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
griprint.o: XMText.h XMLabel.h XMTab.h TabP.h Tab.h XMList.h panemgr.h
griprint.o: dispwind.h superpos.h helpmenu.h printer.h dispgrob.h XBatch.h
griprint.o: convert.h dispshare.h axes.h refreshctl.h grobjmgr.h griprint.h
griprint.o: colormgr.h
grobfile.o: dispgrob.h XMWidget.h dispwind.h superpos.h XBatch.h convert.h
grobfile.o: dispshare.h panemgr.h XMManagers.h errormsg.h
grobjdb.o: dispgrob.h XMWidget.h dispwind.h superpos.h XBatch.h convert.h
grobjdb.o: dispshare.h
grobjdisplay.o: XMWidget.h XBatch.h panemgr.h XMManagers.h dispwind.h
grobjdisplay.o: superpos.h grobjdisplay.h convert.h dispshare.h dispgrob.h
grobjdisplay.o: refreshctl.h text.h errormsg.h colormgr.h grobjmgr.h
grobjdisplay.o: sumregion.h XMList.h XMPushbutton.h XMLabel.h XMText.h
grobjdisplay.o: grobjinput.h XMDialogs.h XMCallback.h XMSeparators.h
grobjdisplay.o: grafinput.h helpmenu.h chanplot.h acceptgates.h gc.h
grobjdisplay.o: xaminegc.h gcmgr.h
grobjinput.o: XMManagers.h XMWidget.h XMDialogs.h XMPushbutton.h XMCallback.h
grobjinput.o: XMSeparators.h grobjinput.h XMText.h XMLabel.h grafinput.h
grobjinput.o: helpmenu.h refreshctl.h panemgr.h dispwind.h superpos.h
grobjiomenu.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h
grobjiomenu.o: XMCallback.h helpmenu.h grobjiomenu.h panemgr.h dispwind.h
grobjiomenu.o: superpos.h grobjmgr.h dispgrob.h XBatch.h convert.h
grobjiomenu.o: dispshare.h refreshctl.h errormsg.h winiomenu.h
grobjmgr.o: dispgrob.h XMWidget.h dispwind.h superpos.h XBatch.h convert.h
grobjmgr.o: dispshare.h grobjmgr.h
grobjtest.o: dispgrob.h XMWidget.h dispwind.h superpos.h XBatch.h convert.h
grobjtest.o: dispshare.h
guintegrate.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h
guintegrate.o: XMCallback.h XMText.h errormsg.h guintegrate.h integrate.h
guintegrate.o: dispgrob.h dispwind.h superpos.h XBatch.h convert.h
guintegrate.o: dispshare.h logging.h grobjmgr.h panemgr.h
helpmenu.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
info.o: XMWidget.h XMDialogs.h XMPushbutton.h XMManagers.h XMCallback.h
info.o: XMText.h info.h logging.h panemgr.h dispwind.h superpos.h errormsg.h
info.o: dispgrob.h XBatch.h convert.h dispshare.h grobjmgr.h spcdisplay.h
integrate.o: integrate.h dispgrob.h XMWidget.h dispwind.h superpos.h XBatch.h
integrate.o: convert.h dispshare.h
lblfont.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
lblfont.o: XMLabel.h refreshctl.h panemgr.h dispwind.h superpos.h
lblfont.o: grobjdisplay.h convert.h dispshare.h dispgrob.h XBatch.h
lblfont.o: helpmenu.h lblfont.h dfltmgr.h text.h
location.o: XMManagers.h XMWidget.h XMLabel.h location.h
logging.o: errormsg.h XMWidget.h XMDialogs.h XMPushbutton.h XMManagers.h
logging.o: XMCallback.h helpmenu.h logging.h
marker.o: XMText.h XMWidget.h XMLabel.h XMPushbutton.h XBatch.h errormsg.h
marker.o: marker.h grafinput.h XMDialogs.h XMManagers.h XMCallback.h
marker.o: helpmenu.h refreshctl.h panemgr.h dispwind.h superpos.h
marker.o: grobjinput.h XMSeparators.h dispgrob.h convert.h dispshare.h
marker.o: colormgr.h grobjmgr.h grobjdisplay.h
menusetup.o: properties.h XMWidget.h XMPushbutton.h menusetup.h XMMenus.h
menusetup.o: dispshare.h helpmenu.h XMDialogs.h XMManagers.h XMCallback.h
menusetup.o: exit.h winiomenu.h grobjiomenu.h geometrymenu.h toggles.h
menusetup.o: logging.h specchoose.h spcdisplay.h dispwind.h superpos.h
menusetup.o: advprompt.h panemgr.h pkgmgr.h optionmenu.h XMText.h XMLabel.h
menusetup.o: XMScale.h printer.h dispgrob.h XBatch.h convert.h XMList.h
menusetup.o: refreshctl.h expand.h info.h marker.h sumregion.h grobjinput.h
menusetup.o: XMSeparators.h grafinput.h guintegrate.h delgrob.h acceptgates.h
menusetup.o: trackcursor.h location.h errormsg.h superprompt.h objcopy.h
menusetup.o: copier.h lblfont.h copyatr.h Tab.h
messages.o: messages.h dispgrob.h XMWidget.h dispwind.h superpos.h XBatch.h
messages.o: convert.h dispshare.h clientops.h ./Xamine.h
objcopy.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
objcopy.o: panemgr.h dispwind.h superpos.h dispshare.h objcopy.h copier.h
objcopy.o: XMList.h XMLabel.h dispgrob.h XBatch.h convert.h grobjmgr.h
objcopy.o: gateio.h messages.h clientops.h ./Xamine.h helpmenu.h errormsg.h
objcopy.o: grobjdisplay.h refreshctl.h compatspec.h
panemgr.o: XMManagers.h XMWidget.h menusetup.h XMMenus.h XMPushbutton.h
panemgr.o: panemgr.h dispwind.h superpos.h buttonsetup.h location.h XMLabel.h
panemgr.o: pkgmgr.h dispshare.h refreshctl.h trackcursor.h colormgr.h
pkgmgr.o: XMWlist.h XMWidget.h XMPushbutton.h pkgmgr.h
plot1d.o: XMWidget.h XBatch.h scaling.h dispwind.h superpos.h dispshare.h
plot1d.o: chanplot.h refreshctl.h panemgr.h XMManagers.h
plot2d.o: XMWidget.h XBatch.h dispwind.h superpos.h dispshare.h chanplot.h
plot2d.o: refreshctl.h panemgr.h XMManagers.h scaling.h colormgr.h
printer.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
printer.o: XMText.h XMLabel.h XMTab.h TabP.h Tab.h XMList.h panemgr.h
printer.o: dispwind.h superpos.h helpmenu.h printer.h dispgrob.h XBatch.h
printer.o: convert.h dispshare.h axes.h refreshctl.h grobjmgr.h griprint.h
procede.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
properties.o: XMWidget.h XMDialogs.h XMPushbutton.h XMManagers.h XMCallback.h
properties.o: helpmenu.h errormsg.h properties.h spcdisplay.h dispwind.h
properties.o: superpos.h optionmenu.h XMText.h XMLabel.h XMScale.h panemgr.h
properties.o: refreshctl.h colormgr.h exit.h
ptlist.o: panemgr.h XMManagers.h XMWidget.h dispwind.h superpos.h errormsg.h
ptlist.o: dispgrob.h XBatch.h convert.h dispshare.h ptlist.h sumregion.h
ptlist.o: XMList.h XMPushbutton.h XMLabel.h XMText.h grobjinput.h XMDialogs.h
ptlist.o: XMCallback.h XMSeparators.h grafinput.h helpmenu.h refreshctl.h
queue.o: queue.h
reducedefault.o: XMManagers.h XMWidget.h XMDialogs.h XMPushbutton.h
reducedefault.o: XMCallback.h dispwind.h superpos.h optionmenu.h XMText.h
reducedefault.o: XMLabel.h XMScale.h dfltmgr.h helpmenu.h panemgr.h
reducedefault.o: refreshctl.h
refreshctl.o: XMWidget.h panemgr.h XMManagers.h dispwind.h superpos.h
refreshctl.o: refreshctl.h axes.h scaling.h dispshare.h titles.h chanplot.h
refreshctl.o: dfltmgr.h colormgr.h spcdisplay.h grobjdisplay.h convert.h
refreshctl.o: dispgrob.h XBatch.h pkgmgr.h XMPushbutton.h queue.h queue.cc
refreshctl.o: gc.h xaminegc.h gcmgr.h
refreshdefault.o: helpmenu.h XMDialogs.h XMWidget.h XMPushbutton.h
refreshdefault.o: XMManagers.h XMCallback.h XMScale.h dfltmgr.h dispwind.h
refreshdefault.o: superpos.h optionmenu.h XMText.h XMLabel.h panemgr.h
refreshdefault.o: refreshctl.h
rend1default.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h
rend1default.o: XMCallback.h optionmenu.h XMText.h XMLabel.h XMScale.h
rend1default.o: dispwind.h superpos.h dfltmgr.h helpmenu.h refreshctl.h
rend1default.o: panemgr.h
rend2default.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h
rend2default.o: XMCallback.h optionmenu.h XMText.h XMLabel.h XMScale.h
rend2default.o: dispwind.h superpos.h dfltmgr.h helpmenu.h colormgr.h
rend2default.o: refreshctl.h panemgr.h
scaling.o: XMWidget.h dispwind.h superpos.h scaling.h dispshare.h axes.h
scaling.o: refreshctl.h panemgr.h XMManagers.h
select2.o: XMWidget.h XMDialogs.h XMPushbutton.h XMManagers.h XMCallback.h
select2.o: XMLabel.h XMText.h helpmenu.h select2.h
shared.o: dispshare.h
spccheck.o: spccheck.h panemgr.h XMManagers.h XMWidget.h dispwind.h
spccheck.o: superpos.h refreshctl.h dispshare.h
spcchoice.o: errormsg.h XMWidget.h dispshare.h spcchoice.h XMDialogs.h
spcchoice.o: XMPushbutton.h XMManagers.h XMCallback.h
spccompatchoice.o: dispshare.h spcchoice.h XMDialogs.h XMWidget.h
spccompatchoice.o: XMPushbutton.h XMManagers.h XMCallback.h dispwind.h
spccompatchoice.o: superpos.h panemgr.h
spcdisplay.o: errormsg.h XMWidget.h dispshare.h spcdisplay.h dispwind.h
spcdisplay.o: superpos.h panemgr.h XMManagers.h refreshctl.h dfltmgr.h exit.h
spcdisplay.o: pkgmgr.h XMPushbutton.h
spclimchoice.o: dispshare.h spcchoice.h XMDialogs.h XMWidget.h XMPushbutton.h
spclimchoice.o: XMManagers.h XMCallback.h
specchoose.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
specchoose.o: dispshare.h specchoose.h helpmenu.h errormsg.h spcdisplay.h
specchoose.o: dispwind.h superpos.h
spectra.o: dispshare.h
sumregion.o: XMWidget.h XMPushbutton.h XMLabel.h XMText.h XMList.h XBatch.h
sumregion.o: dispgrob.h dispwind.h superpos.h convert.h dispshare.h
sumregion.o: grobjmgr.h grobjdisplay.h refreshctl.h panemgr.h XMManagers.h
sumregion.o: sumregion.h grobjinput.h XMDialogs.h XMCallback.h XMSeparators.h
sumregion.o: grafinput.h helpmenu.h errormsg.h colormgr.h chanplot.h
superpos.o: superpos.h
superprompt.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h
superprompt.o: XMCallback.h superprompt.h dispwind.h superpos.h spcdisplay.h
superprompt.o: dispshare.h helpmenu.h panemgr.h errormsg.h specchoose.h
superprompt.o: compatspec.h XMList.h XMLabel.h refreshctl.h menusetup.h
superprompt.o: XMMenus.h exit.h
text.o: text.h
titledefault.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h
titledefault.o: XMCallback.h dispwind.h superpos.h dfltmgr.h helpmenu.h
titledefault.o: optionmenu.h XMText.h XMLabel.h XMScale.h panemgr.h
titledefault.o: refreshctl.h
titles.o: dispshare.h titles.h XMWidget.h dispwind.h superpos.h refreshctl.h
titles.o: panemgr.h XMManagers.h axes.h text.h chanplot.h
toggles.o: XMWidget.h toggles.h
trackcursor.o: XMWidget.h XMManagers.h trackcursor.h location.h XMLabel.h
trackcursor.o: buttonsetup.h panemgr.h dispwind.h superpos.h dispshare.h
trackcursor.o: convert.h spcdisplay.h
winiomenu.o: XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h XMCallback.h
winiomenu.o: helpmenu.h winiomenu.h dispwind.h superpos.h panemgr.h exit.h
winiomenu.o: refreshctl.h errormsg.h buttonsetup.h location.h XMLabel.h
winiomenu.o: menusetup.h XMMenus.h
wintest.o: dispshare.h dispwind.h superpos.h
Xamine.o: XMWidget.h XMPushbutton.h XMMenus.h XMDialogs.h XMManagers.h
Xamine.o: XMCallback.h dispshare.h dispwind.h superpos.h panemgr.h
Xamine.o: menusetup.h buttonsetup.h location.h XMLabel.h pkgmgr.h colormgr.h
Xamine.o: dfltmgr.h gateio.h dispgrob.h XBatch.h convert.h messages.h
Xamine.o: clientops.h ./Xamine.h clirequests.h prccheck.h spccheck.h
Xamine.o: trackcursor.h
xaminegc.o: gc.h xaminegc.h dispwind.h superpos.h XMWidget.h text.h
xaminegc.o: colormgr.h chanplot.h refreshctl.h panemgr.h XMManagers.h
XMCallback.o: XMWidget.h
XMDialogs.o: helpmenu.h XMDialogs.h XMWidget.h XMPushbutton.h XMManagers.h
XMDialogs.o: XMCallback.h errormsg.h
XMManagers.o: XMManagers.h XMWidget.h
XMMenus.o: XMMenus.h XMWidget.h XMPushbutton.h
XMShell.o: XMShell.h XMWidget.h XMCallback.h
XMWlist.o: XMWlist.h XMWidget.h XMPushbutton.h
Xwarning.o: XMWidget.h XMManagers.h XMLabel.h XMPushbutton.h
