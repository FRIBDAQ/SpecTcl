#
#   Make file for the display program.
#
CCFLAGS= /define=('MOTIF_DEFINES',FLEXV2)/diagnostics-
/include=(sys$disk:[],dataq:[gnu.gcc.gxx_include])/debug/noop
XCCFLAGS= /DEFINE=('MOTIF_DEFINES',FLEXV2)/diagnostics-
/include=(sys$disk:[],dataq:[gnu.gcc.gxx_include])
fflags=/debug/noopt
CC      = CC/nowarn
CPP     = CXX
LD     =  LINK
YACC   = bison
LEX    = flex
LEXOUT = lexyy.c
VERSION= 1_0
#
#   The definition below is the default destination directory tree root for
#   the INSTALL Target.  It can be overriden on the command line using
#   DESTDIR="someotherdir"
#
DESTDIR = NSCL_LIBRARY:[XAMINE]
ETCDIR  = DAQ_LIB:

.FIRST
	mv    :== rename
	cp    :== copy
	echo  :== WRITE SYS$OUTPUT
	TOUCH :== APPEND/new_version NLA0: 
	@DATAQ:[GNU.BISON]INSTALL_BISON
	@DATAQ:[GNU.FLEX]INSTALL_FLEX
	copy client.h Xamine.h
	DEFINE XAMINE_DIR SYS$DISK:[]

SYSTEM : XAMINE_FILES WINDOW_FILE  GROBJ_FILE  DISP_SHARED \
	MOTIF_LIB COLORSET Xamine.exe CLIENT Xwarning.exe
	echo "System rebuild complete"

INSTALL : SYSTEM
	echo "VMS Installation is not automated... sorry"

WINDOW_FILE : windfile_parse.OBJ,windfile_lex.OBJ,dispwind.OBJ, -
	defaultfile_lex.obj
	touch WINDOW_FILE
	echo "Window file I/O subsystem built"

WINDOW_TEST : wintest.exe
	echo "Windows file test suite finished"
	touch WINDOW_TEST

XAMINE_FILES : menusetup.OBJ,exit.OBJ,helpmenu.OBJ,winiomenu.OBJ,grobjiomenu.OBJ,geometrymenu.OBJ,\
	errormsg.OBJ,procede.OBJ,logging.OBJ,toggles.OBJ,specchoose.OBJ,spectra.OBJ,\
	spcdisplay.OBJ,advprompt.OBJ,properties.OBJ,buttonsetup.OBJ,panemgr.OBJ,\
	pkgmgr.OBJ,dfltmgr.OBJ,axisdefault.OBJ,titledefault.OBJ,attribdefault.OBJ,\
	reducedefault.OBJ,rend1default.OBJ,rend2default.OBJ,printer.OBJ,refreshctl.OBJ,\
	axes.OBJ,scaling.OBJ,text.OBJ,titles.OBJ,chanplot.OBJ,plot1d.OBJ,plot2d.OBJ,\
	colormgr.OBJ,shared.OBJ,location.OBJ,convert.OBJ,trackcursor.OBJ,expand.OBJ,\
	select2.OBJ,info.OBJ,refreshdefault.OBJ,defaultfile_parse.OBJ,marker.OBJ,\
	grafinput.OBJ,grobjinput.OBJ,grobjmgr.OBJ,grobjdisplay.OBJ, sumregion.OBJ,\
	integrate.OBJ,guintegrate.OBJ,delgrob.OBJ,messages.OBJ,eventflags.OBJ,\
	gateio.OBJ,acceptgates.OBJ,clirequests.OBJ,spccheck.OBJ,prccheck.OBJ,superpos.OBJ,\
	superprompt.OBJ,objcopy.OBJ,lblfont.OBJ,checklist.OBJ,compatspec.OBJ,copyatr.OBJ,\
	buttonreq.OBJ,buttondlg.OBJ,spcchoice.OBJ,spccompatchoice.OBJ,spclimchoice.OBJ,\
	ptlist.obj, LEXSUPPORT.OBJ gc.obj xaminegc.obj gcmgr.obj
	echo "Xamine objects built"
	touch XAMINE_FILES

wintest.exe : wintest.cc WINDOW_FILE spectra.o
	$(CPP) $(XCCFLAGS) wintest.cc
	$(LD) wintest.OBJ,dispwind.OBJ,windfile_parse.OBJ,windfile_lex.OBJ,\
	spectra.OBJ,XWINDOWS.opt/opt

dispwind.OBJ : dispwind.cc dispwind.h superpos.h errormsg.h panemgr.h
	$(CPP) $(XCCFLAGS) dispwind.cc

windfile_parse.OBJ : windfile.y  dispwind.h superpos.h dispshare.h windio.h
	$(YACC)/name_prefix=windfile/file_prefix=windfile windfile.y
	mv windfile_tab.c windfile_parse.cc
	$(CPP) $(XCCFLAGS)  windfile_parse.cc

defaultfile_lex.h : defaultfile.y windio.h
	$(YACC)/name_prefix=defaultfile/file_prefix=defaultfile/defines defaultfile.y
	mv defaultfile_tab.h defaultfile_lex.h

defaultfile_parse.OBJ : defaultfile.y dispwind.h superpos.h dispshare.h dfltmgr.h windio.h
	$(YACC)/name_prefix=defaultfile/file_prefix=defaultfile defaultfile.y
	mv defaultfile_tab.c defaultfile_parse.cc
	$(CPP) $(XCCFLAGS) defaultfile_parse.cc

windfile_lex.h : windfile.y windio.h
	$(YACC)/name_prefix=windfile/file_prefix=windfile/defines  windfile.y
	mv windfile_tab.h windfile_lex.h

windfile_lex.OBJ : windfile.l dispwind.h superpos.h windfile_lex.h windio.h
	$(LEX) windfile.l
	EDIT/TPU/COMMAND=WIN.TPU/NODISPLAY $(LEXOUT) /output=WINDFILE_LEX.CC
	cp windfile_lex.h windlex.h
	$(CPP) $(XCCFLAGS) windfile_lex.cc

defaultfile_lex.OBJ : windfile.l dispwind.h superpos.h defaultfile_lex.h windio.h
	$(LEX) windfile.l
	EDIT/TPU/COMMAND=DFLT.TPU/NODISPLAY $(LEXOUT)  /output=DEFAULTFILE_LEX.CC
	cp defaultfile_lex.h windlex.h
	$(CPP) $(XCCFLAGS) defaultfile_lex.cc

GROBJ_FILE : dispgrob.OBJ,grobfile_lex.OBJ,grobfile_parse.OBJ,grobjdb.OBJ,\
	grobfile.obj
	echo "GROBJ_FILE rebuilt"
	touch GROBJ_FILE

dispgrob.OBJ : dispgrob.cc dispgrob.h XMWidget.h dispwind.h convert.h
	$(CPP) $(XCCFLAGS) dispgrob.cc

grobjdb.OBJ : grobjdb.cc dispgrob.h
	$(CPP) $(XCCFLAGS) grobjdb.cc

grobfile_lex.OBJ : grobfile.l dispgrob.h grobjio.h grobfile_lex.h 
	$(LEX) grobfile.l
	EDIT/TPU/COMMAND=GROBJ.TPU/NODISPLAY $(LEXOUT)  /output=GROBFILE_LEX.CC
	$(CPP) $(XCCFLAGS) grobfile_lex.cc

grobfile_parse.OBJ : grobfile.y dispgrob.h grobjio.h
	$(YACC)/name_prefix=grobjfile/file_prefix=grobfile grobfile.y
	mv grobfile_tab.c  grobfile_parse.cc
	$(CPP) $(XCCFLAGS) grobfile_parse.cc

grobfile_lex.h : grobfile.y grobjio.h
	$(YACC)/defines/name_prefix=grobjfile/file_prefix=grobfile grobfile.y
	mv grobfile_tab.h  grobfile_lex.h

grobfile.OBJ : grobfile.cc dispgrob.h grobjio.h panemgr.h errormsg.h
	$(CPP) $(XCCFLAGS) grobfile.cc

GROBJ_TEST : grobjtest.exe
	echo "Graphical object tests built"
	touch GROBJ_TEST

grobjtest.exe : grobjtest.cc GROBJ_FILE
	$(CPP) $(XCCFLAGS) grobjtest.cc
	$(LD) grobjtest.OBJ,dispgrob.OBJ,grobfile_lex.OBJ,\
		grobfile_parse.OBJ,grobjdb.OBJ,grobfile.OBJ,XWINDOWS.OPT/OPT


DISP_SHARED : spectra.obj
	echo "Spectrum access package rebuilt"
	touch DISP_SHARED

spectra.OBJ : spectra.cc dispshare.h
	$(CPP) $(XCCFLAGS) spectra.cc

MOTIF_LIB :  XMMenus.OBJ,XMDialogs.OBJ,XMManagers.OBJ,XMWlist.OBJ,\
	copier.OBJ,XMShell.OBJ,XMcallback.OBJ,
	touch MOTIF_LIB
	echo "MOTIF Library built"

XMcallback.OBJ : XMcallback.cc XMWidget.h
	$(CPP) $(XCCFLAGS) XMcallback.cc

XMMenus.OBJ : XMMenus.cc XMWidget.h XMPushbutton.h XMMenus.h
	$(CPP) $(XCCFLAGS) XMMenus.cc

Xamine.exe : Xamine.OBJ,\
	 MOTIF_LIB \
	 WINDOW_FILE \
	 XAMINE_FILES \
	GROBJ_FILE
	$(LD)/map/full Xamine, XMMenus,XMDialogs,\
	XMManagers,XMWlist, XMShell,XMcallback,\
	windfile_parse,windfile_lex,dispwind,\
	menusetup,exit,helpmenu,winiomenu,grobjiomenu,\
	geometrymenu,errormsg,logging,toggles,spectra,specchoose,\
	spcdisplay,advprompt,properties,panemgr,buttonsetup,pkgmgr,\
	dfltmgr,axisdefault,titledefault,attribdefault,reducedefault,\
	rend1default,rend2default,printer,refreshctl,axes,scaling,\
	text,titles,chanplot,plot1d, plot2d,colormgr,shared,\
	location,convert,trackcursor,expand,select2,info,\
	dispgrob,grobfile_lex,grobfile_parse,grobjdb,grobfile,\
	refreshdefault,defaultfile_parse,marker,grafinput,\
	grobjinput,grobjmgr,grobjdisplay,sumregion,integrate,\
	guintegrate,delgrob,messages,eventflags,\
	gateio,acceptgates,clirequests,\
	prccheck,spccheck,superpos,superprompt,objcopy,copier,\
	lblfont,checklist,compatspec,copyatr,\
	defaultfile_lex,buttonreq,buttondlg,spcchoice,\
	spccompatchoice,spclimchoice,ptlist,LEXSUPPORT, gc, gcmgr, xaminegc, \
	XWINDOWS.opt/opt

Xamine.OBJ : Xamine.cc XMPushbutton.h XMWidget.h  dispshare.h XMManagers.h \
	panemgr.h pkgmgr.h optionmenu.h colormgr.h \
	menusetup.h dfltmgr.h clirequests.h prccheck.h spccheck.h client.h
	$(CPP) $(XCCFLAGS) Xamine.cc

XMDialogs.OBJ : XMDialogs.cc XMDialogs.h XMWidget.h errormsg.h helpmenu.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) XMDialogs.cc


menusetup.OBJ : menusetup.cc XMMenus.h exit.h winiomenu.h grobjiomenu.h \
	geometrymenu.h toggles.h logging.h specchoose.h properties.h \
	menusetup.h dispshare.h helpmenu.h winiomenu.h spcdisplay.h \
	advprompt.h panemgr.h pkgmgr.h optionmenu.h printer.h refreshctl.h \
	expand.h info.h XMWidget.h XMPushbutton.h XMManagers.h XMDialogs.h \
	XMText.h XMLabel.h XMScale.h marker.h dispwind.h superpos.h sumregion.h \
	dispgrob.h convert.h guintegrate.h delgrob.h XMList.h grobjinput.h \
	acceptgates.h XMSeparators.h grafinput.h errormsg.h trackcursor.h \
	location.h superprompt.h objcopy.h lblfont.h copyatr.h objcopy.h \
	copier.h XMCallback.h XMCallback.cc
	$(CPP) $(XCCFLAGS) menusetup.cc

exit.OBJ : exit.cc XMDialogs.h panemgr.h helpmenu.h exit.h winiomenu.h \
	XMWidget.h dispwind.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) exit.cc

helpmenu.OBJ : helpmenu.cc helpmenu.h XMDialogs.h XMWidget.h XMPushbutton.h \
	XMManagers.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) helpmenu.cc


winiomenu.OBJ : winiomenu.cc XMDialogs.h helpmenu.h winiomenu.h panemgr.h \
	dispwind.h superpos.h exit.h refreshctl.h XMWidget.h XMPushbutton.h XMManagers.h \
	errormsg.h menusetup.h buttonsetup.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) winiomenu.cc


grobjiomenu.OBJ : grobjiomenu.cc grobjiomenu.h XMDialogs.h helpmenu.h \
	XMWidget.h XMPushbutton.h XMManagers.h panemgr.h grobjmgr.h \
	dispgrob.h errormsg.h winiomenu.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) grobjiomenu.cc


geometrymenu.OBJ : geometrymenu.cc XMLabel.h XMDialogs.h helpmenu.h \
	errormsg.h geometrymenu.h menusetup.h XMPushbutton.h XMManagers.h \
	XMSeparators.h exit.h XMWidget.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) geometrymenu.cc

errormsg.OBJ : errormsg.cc XMDialogs.h XMWidget.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) errormsg.cc

COLORSET : colorset.exe colorset.ico
	touch COLORSET
	echo "colorset - color map generation tool rebuilt."

colorset.exe : colorset.OBJ,MOTIF_LIB errormsg.OBJ,helpmenu.OBJ,procede.OBJ, \
	colormgr.obj
	$(LD) colorset.OBJ,errormsg.OBJ,helpmenu.OBJ,procede.OBJ,\
	XMDialogs.OBJ,XMMenus.OBJ,XMcallback.OBJ, colormgr.OBJ,XWINDOWS.opt/opt

colorset.OBJ : colorset.cc XMDialogs.h XMWidget.h XMPushbutton.h XMMenus.h \
	helpmenu.h errormsg.h procede.h colormgr.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) colorset.cc

procede.OBJ : procede.cc procede.h XMDialogs.h XMWidget.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) procede.cc

logging.OBJ : logging.cc logging.h XMDialogs.h helpmenu.h errormsg.h XMWidget.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) logging.cc

toggles.OBJ : toggles.cc toggles.h XMWidget.h
	$(CPP) $(XCCFLAGS) toggles.cc

specchoose.OBJ : specchoose.cc specchoose.h XMDialogs.h helpmenu.h errormsg.h \
	spcdisplay.h XMWidget.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) specchoose.cc

spcdisplay.OBJ : spcdisplay.cc spcdisplay.h dispwind.h superpos.h panemgr.h dfltmgr.h \
	      refreshctl.h dfltmgr.h exit.h pkgmgr.h superpos.h errormsg.h
	$(CPP) $(XCCFLAGS) spcdisplay.cc

advprompt.OBJ : advprompt.cc XMDialogs.h helpmenu.h advprompt.h spcdisplay.h \
	errormsg.h XMWidget.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS)  advprompt.cc

properties.OBJ : properties.cc XMWidget.h XMDialogs.h XMManagers.h helpmenu.h \
	errormsg.h properties.h spcdisplay.h dispwind.h superpos.h optionmenu.h \
	panemgr.h refreshctl.h colormgr.h exit.h XMText.h XMScale.h \
	XMPushbutton.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) properties.cc

XMManagers.OBJ : XMManagers.cc XMManagers.h XMWidget.h
	$(CPP) $(XCCFLAGS) XMManagers.cc

panemgr.OBJ : panemgr.cc XMManagers.h XMWidget.h panemgr.h menusetup.h \
	buttonsetup.h pkgmgr.h dispshare.h trackcursor.h colormgr.h superpos.h
	$(CPP) $(XCCFLAGS) panemgr.cc

buttonsetup.OBJ : buttonsetup.cc XMManagers.h XMPushbutton.h buttonsetup.h \
	geometrymenu.h panemgr.h specchoose.h spcdisplay.h pkgmgr.h \
	refreshctl.h location.h expand.h info.h marker.h sumregion.h \
	dispgrob.h convert.h XMWidget.h guintegrate.h XMList.h grafinput.h \
	trackcursor.h properties.h
	$(CPP) $(XCCFLAGS) buttonsetup.cc

XMWlist.OBJ : XMWlist.cc XMWlist.h XMWidget.h XMPushbutton.h
	$(CPP) $(XCCFLAGS) XMWlist.cc

pkgmgr.OBJ : pkgmgr.cc XMWlist.h pkgmgr.h XMWidget.h
	$(CPP) $(XCCFLAGS) pkgmgr.cc

dfltmgr.OBJ : dfltmgr.cc dfltmgr.h dispwind.h superpos.h XMWidget.h panemgr.h \
	refreshctl.h grobjdisplay.h
	$(CPP) $(XCCFLAGS) dfltmgr.cc

axisdefault.OBJ : axisdefault.cc  XMPushbutton.h XMDialogs.h XMWidget.h \
	helpmenu.h dispwind.h superpos.h dfltmgr.h optionmenu.h XMText.h XMScale.h \
	panemgr.h refreshctl.h	\
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) axisdefault.cc

titledefault.OBJ : titledefault.cc XMPushbutton.h  XMDialogs.h XMWidget.h \
	helpmenu.h dispwind.h superpos.h dfltmgr.h optionmenu.h XMText.h XMScale.h \
	refreshctl.h panemgr.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) titledefault.cc

attribdefault.OBJ : attribdefault.cc XMDialogs.h XMPushbutton.h XMLabel.h \
	XMText.h dispwind.h superpos.h dfltmgr.h optionmenu.h helpmenu.h XMScale.h \
	XMWidget.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) attribdefault.cc

reducedefault.OBJ : reducedefault.cc XMDialogs.h XMPushbutton.h XMManagers.h \
	dispwind.h superpos.h dfltmgr.h optionmenu.h helpmenu.h XMScale.h XMWidget.h \
	refreshctl.h panemgr.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) reducedefault.cc
rend1default.OBJ : rend1default.cc XMDialogs.h XMPushbutton.h XMManagers.h \
	optionmenu.h dfltmgr.h dispwind.h superpos.h helpmenu.h XMScale.h \
	refreshctl.h panemgr.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) rend1default.cc

rend2default.OBJ : rend2default.cc XMDialogs.h XMPushbutton.h XMManagers.h \
	optionmenu.h dfltmgr.h dispwind.h superpos.h helpmenu.h colormgr.h XMScale.h \
	XMWidget.h refreshctl.h panemgr.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) rend2default.cc

printer.OBJ : printer.cc XMDialogs.h XMManagers.h XMPushbutton.h XMText.h  \
	printer.h XMWidget.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) printer.cc

refreshctl.OBJ : refreshctl.cc refreshctl.h XMWidget.h panemgr.h axes.h titles.h \
	chanplot.h dfltmgr.h scaling.h dispshare.h grobjdisplay.h XMWidget.h \
	queue.h queue.cc
	$(CPP) $(XCCFLAGS) refreshctl.cc

axes.OBJ : axes.cc axes.h XMWidget.h dispwind.h superpos.h dispshare.h \
	XBatch.h text.h refreshctl.h
	$(CPP) $(XCCFLAGS) axes.cc

scaling.OBJ : scaling.cc scaling.h XMWidget.h dispwind.h superpos.h scaling.h dispshare.h \
	axes.h
	$(CPP) $(XCCFLAGS) scaling.cc

text.OBJ : text.cc text.h
	$(CPP) $(XCCFLAGS) text.cc

titles.OBJ : titles.cc dispshare.h titles.h axes.h panemgr.h text.h refreshctl.h
	$(CPP) $(XCCFLAGS) titles.cc

chanplot.OBJ : chanplot.cc refreshctl.h dispwind.h superpos.h axes.h scaling.h chanplot.h \
	panemgr.h XMWidget.h colormgr.h superpos.h
	$(CPP) $(XCCFLAGS) chanplot.cc

plot1d.OBJ : plot1d.cc XMWidget.h XBatch.h chanplot.h panemgr.h  \
	dispshare.h dispwind.h scaling.h
	$(CPP) $(XCCFLAGS) plot1d.cc

plot2d.OBJ : plot2d.cc XMWidget.h XBatch.h dispwind.h superpos.h dispshare.h chanplot.h \
	scaling.h colormgr.h refreshctl.h
	$(CPP) $(XCCFLAGS)  plot2d.cc

colormgr.OBJ : colormgr.cc XMWidget.h colormgr.h
	$(CPP) $(XCCFLAGS) colormgr.cc

CLIENT : libXamine.olb Xaminetest.exe demo.exe
	echo "Client software and tests built"
	touch CLIENT

libXamine.olb : client.OBJ,aedxamine.OBJ,clientgates.OBJ,prccheck.OBJ,allocator.OBJ,\
	specalloc.OBJ,clientbuttons.obj
	library/create libXamine client.OBJ,aedxamine.OBJ,clientgates.OBJ,prccheck.OBJ,\
	allocator.OBJ,specalloc.OBJ,clientbuttons.obj

Xaminetest.exe : Xaminetest.OBJ,libXamine.olb
	$(LD) Xaminetest.OBJ,client.OBJ,libXamine/lib,XWINDOWS.OPT/opt

Xaminetest.OBJ : Xaminetest.c client.h
	$(CC) $(CCFLAGS) Xaminetest.c

client.OBJ : client.c client.h clientgates.h allocator.h
	$(CC) $(CCFLAGS) client.c

shared.OBJ : shared.cc dispshare.h
	$(CPP) $(CCFLAGS) shared.cc

aedxamine.OBJ : aedxamine.for aedshare.for xamineshare.for
	fortran$(FFLAGS) aedxamine.for

demo.exe : demo.OBJ,libXamine.olb
	link demo,libXamine/lib,daq_lib:libqtree/lib,libfile/lib,libcvt/lib,-
	sys$disk:[]Xamine.opt/opt

demo.OBJ : demo.for aedshare.for aedpublic.for
	fortran$(FFLAGS) demo.for


location.OBJ : location.cc XMManagers.h XMLabel.h location.h XMWidget.h
	$(CPP) $(XCCFLAGS) location.cc

convert.OBJ : convert.cc convert.h XMWidget.h dispwind.h superpos.h axes.h
	$(CPP) $(XCCFLAGS) convert.cc

trackcursor.OBJ : trackcursor.cc XMWidget.h trackcursor.h buttonsetup.h \
	location.h panemgr.h dispwind.h superpos.h dispshare.h convert.h
	$(CPP) $(XCCFLAGS) trackcursor.cc

expand.OBJ : expand.cc XMWidget.h XMDialogs.h panemgr.h helpmenu.h expand.h \
	refreshctl.h exit.h errormsg.h select2.h dispshare.h panemgr.h \
	convert.h colormgr.h grafinput.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) expand.cc

select2.OBJ : select2.cc XMDialogs.h XMLabel.h XMText.h XMPushbutton.h \
	helpmenu.h select2.h XMWidget.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) select2.cc

info.OBJ : info.cc XMWidget.h XMDialogs.h XMText.h \
	info.h logging.h dispshare.h panemgr.h errormsg.h dispwind.h superpos.h \
	dispgrob.h grobjmgr.h \
	XMCallback.cc XMCallback.h
	$(CPP) $(XCCFLAGS) info.cc

refreshdefault.OBJ : refreshdefault.cc XMScale.h dfltmgr.h optionmenu.h  \
	helpmenu.h dispwind.h superpos.h XMWidget.h panemgr.h refreshctl.h
	$(CPP) $(XCCFLAGS) refreshdefault.cc

marker.OBJ : marker.cc marker.h XMWidget.h XMText.h XMLabel.h XMPushbutton.h \
	XBatch.h errormsg.h grafinput.h grobjinput.h dispgrob.h panemgr.h \
	dispshare.h colormgr.h convert.h grobjmgr.h grobjdisplay.h
	$(CPP) $(XCCFLAGS) marker.cc

grafinput.OBJ : grafinput.cc XMDialogs.h grafinput.h helpmenu.h panemgr.h \
	refreshctl.h convert.h dispshare.h XMWidget.h \
	XMCallback.h XMCallback.cc
	$(CPP) $(XCCFLAGS) grafinput.cc

grobjinput.OBJ : grobjinput.cc XMManagers.h XMDialogs.h XMSeparators.h \
	grobjinput.h XMWidget.h
	$(CPP) $(XCCFLAGS) grobjinput.cc

grobjmgr.OBJ : grobjmgr.cc dispgrob.h grobjmgr.h
	$(CPP) $(XCCFLAGS) grobjmgr.cc

grobjdisplay.OBJ : grobjdisplay.cc XMWidget.h panemgr.h grobjdisplay.h \
	dispwind.h superpos.h text.h dispgrob.h dispshare.h errormsg.h convert.h \
	colormgr.h grobjmgr.h XBatch.h sumregion.h chanplot.h acceptgates.h \
	XMList.h refreshctl.h
	$(CPP) $(XCCFLAGS) grobjdisplay.cc

sumregion.OBJ : sumregion.cc XMWidget.h XMPushbutton.h XMLabel.h XMText.h \
	XMList.h dispgrob.h dispwind.h superpos.h dispshare.h grobjmgr.h sumregion.h \
	panemgr.h convert.h errormsg.h grobjinput.h colormgr.h chanplot.h \
	grobjdisplay.h XBatch.h
	$(CPP) $(XCCFLAGS) sumregion.cc

integrate.OBJ : integrate.cc integrate.h dispshare.h dispgrob.h
	$(CPP) $(XCCFLAGS) integrate.cc

guintegrate.OBJ : guintegrate.cc XMDialogs.h XMText.h guintegrate.h integrate.h \
	logging.h dispshare.h dispgrob.h grobjmgr.h XMWidget.h panemgr.h \
	errormsg.h
	$(CPP) $(XCCFLAGS) guintegrate.cc

delgrob.OBJ : delgrob.cc XMWidget.h XMDialogs.h dispgrob.h dispwind.h superpos.h \
	helpmenu.h delgrob.h grobjmgr.h panemgr.h errormsg.h
	$(CPP) $(XCCFLAGS) delgrob.cc

messages.OBJ : messages.cc eventflags.h XMWidget.h messages.h dispgrob.h \
	dispwind.h superpos.h clientops.h client.h
	$(CPP) $(XCCFLAGS)   messages.cc

eventflags.OBJ : eventflags.cc eventflags.h
	$(CPP) $(XCCFLAGS) eventflags.cc

gateio.OBJ : gateio.cc XMWidget.h messages.h gateio.h dispgrob.h clientops.h
	$(CPP) $(XCCFLAGS) gateio.cc

acceptgates.OBJ : acceptgates.cc XMWidget.h XBatch.h convert.h dispgrob.h \
	acceptgates.h sumregion.h clientops.h
	$(CPP) $(XCCFLAGS) acceptgates.cc

clirequests.OBJ : clirequests.cc dispshare.h XMWidget.h errormsg.h panemgr.h \
	dispgrob.h clirequests.h gateio.h grobjmgr.h acceptgates.h \
	convert.h grobjdisplay.h buttonreq.h clientops.h
	$(CPP) $(XCCFLAGS) clirequests.cc

clientgates.OBJ : clientgates.c clientgates.h client.h messages.h clientops.h
	$(CC) $(CCFLAGS) clientgates.c


spccheck.OBJ : spccheck.cc spccheck.h panemgr.h dispwind.h superpos.h refreshctl.h \
	dispshare.h
	$(CPP) $(XCCFLAGS) spccheck.cc

prccheck.OBJ : prccheck.c prccheck.h
	$(CC) $(XCCFLAGS) prccheck.c

superpos.OBJ : superpos.cc superpos.h
	$(CPP) $(XCCFLAGS) superpos.cc

superprompt.OBJ : superprompt.cc XMDialogs.h superprompt.h dispwind.h superpos.h \
	spcdisplay.h dispshare.h helpmenu.h panemgr.h errormsg.h specchoose.h \
	compatspec.h menusetup.h
	$(CPP) $(XCCFLAGS) superprompt.cc

allocator.OBJ : allocator.c allocator.h
	$(CC) $(XCCFLAGS) allocator.c

specalloc.OBJ : specalloc.c client.h
	$(CC) $(XCCFLAGS) specalloc.c

copier.OBJ : copier.cc copier.h XMPushbutton.h XMManagers.h XMList.h XMLabel.h
	$(CPP) $(XCCFLAGS) copier.cc

objcopy.OBJ : objcopy.cc XMDialogs.h panemgr.h dispshare.h objcopy.h dispgrob.h \
	   grobjmgr.h gateio.h helpmenu.h compatspec.h copier.h clirequests.h
	$(CPP) $(XCCFLAGS) objcopy.cc

lblfont.OBJ : lblfont.cc XMDialogs.h XMPushbutton.h XMLabel.h refreshctl.h \
	grobjdisplay.h helpmenu.h lblfont.h dfltmgr.h text.h
	$(CPP) $(XCCFLAGS) lblfont.cc

checklist.OBJ : checklist.cc XMManagers.h XMPushbutton.h XMLabel.h checklist.h
	$(CPP) $(XCCFLAGS) checklist.cc

compatspec.OBJ : compatspec.cc compatspec.h dispshare.h XMManagers.h XMList.h \
	XMPushbutton.h panemgr.h
	$(CPP) $(XCCFLAGS) compatspec.cc

copyatr.OBJ : copyatr.cc dispwind.h panemgr.h checklist.h compatspec.h \
	superpos.h XMDialogs.h XMManagers.h XMCallback.h compatspec.h \
	errormsg.h helpmenu.h refreshctl.h XMCallback.cc
	$(CPP) $(XCCFLAGS) copyatr.cc

clientbuttons.OBJ : clientbuttons.c client.h clientops.h XMWidget.h
	$(CC) $(XCCFLAGS) clientbuttons.c 

buttondlg.OBJ : buttondlg.cc XMManagers.h XMPushbutton.h XMShell.h buttondlg.h \
	XMCallback.h
	$(CPP) $(XCCFLAGS) buttondlg.cc

buttonreq.OBJ : buttonreq.cc XMCallback.h XMShell.h XMPushbutton.h errormsg.h \
	clientops.h buttonreq.h messages.h gateio.h buttondlg.h panemgr.h \
	spcchoice.h client.h ptlist.h XMCallback.cc
	$(CPP) $(XCCFLAGS) buttonreq.cc

XMShell.OBJ : XMShell.cc XMShell.h
	$(CPP) $(XCCFLAGS) XMShell.cc

spcchoice.OBJ : spcchoice.cc errormsg.h dispshare.h spcchoice.h
	$(CPP) $(XCCFLAGS) spcchoice.cc

spccompatchoice.OBJ :  spccompatchoice.cc dispshare.h spcchoice.h dispwind.h \
	panemgr.h
	$(CPP) $(XCCFLAGS) spccompatchoice.cc

spclimchoice.OBJ : spclimchoice.cc dispshare.h spcchoice.h
	$(CPP) $(XCCFLAGS) spclimchoice.cc

ptlist.OBJ : ptlist.cc errormsg.h dispgrob.h ptlist.h panemgr.h XMWidget.h \
	sumregion.h
	$(CPP) $(XCCFLAGS) ptlist.cc

Xwarning.exe :	Xwarning.OBJ,MOTIF_LIB
	$(LD)  Xwarning.OBJ, XMMenus.OBJ,\
	XMDialogs.OBJ,XMManagers.OBJ,XMWlist.OBJ,copier.OBJ,XMShell.OBJ,\
	errormsg.OBJ,helpmenu.OBJ,XMcallback.OBJ,\
	XWINDOWS.OPT/opt

Xwarning.OBJ :	Xwarning.cc XMWidget.h XMManagers.h XMLabel.h XMPushbutton.h
	$(CPP) $(XCCFLAGS) Xwarning.cc

LEXSUPPORT.OBJ : LEXSUPPORT.C
	$(CC) $(XCCFLAGS) LEXSUPPORT


gc.obj : gc.cc gc.h
	$(CPP) $(XCCFLAGS) gc.cc

xaminegc.obj : xaminegc.cc gc.h xaminegc.h text.h colormgr.h
	$(CPP) $(XCCFLAGS) xaminegc.cc

gcmgr.obj : gcmgr.cc xaminegc.h gc.h gcmgr.h grobjdisplay.h XMWidget.h \
	convert.h dispgrob.h dispwind.h refreshctl.h
	$(CPP) $(XCCFLAGS) gcmgr.cc

