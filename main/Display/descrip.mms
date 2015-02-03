.FIRST
	SETUP XAMINE
	@DATAQ:[GNU.FLEX]INSTALL_FLEX
	@DATAQ:[GNU.BISON]INSTALL_BISON
	echo :== write sys$output
	mv   :== rename
	touch:==APPEND/new_version nla0: 
#
#   Make file for the display program.
#
CCFLAGS= /diagnostics/DEFINE=('MOTIF_DEFINES')/debug/noopt
CC      = CC
CPP     = CXX
LD     = LINK
YACC   = bison
LEX    = flex
LEXOUT = lexyy.c
F77    = FORTRAN
F77FLAGS = /CHECK=NOOVERFLOW/debug/noopt/diagnostics
ECHO   = WRITE SYS$OUTPUT 
DESTDIR= XAMINE_DIR:

SYSTEM : XAMINE_FILES WINDOW_FILE  GROBJ_FILE  DISP_SHARED \
	MOTIF_LIB COLORSET Xamine.EXE CLIENT
	$(ECHO) "System rebuild complete"

INSTALL : XAMINE.EXE COLORSET.EXE CLIENT.OLB client.h AEDSHARE.FOR \
	aedpublic.for XAMINESHARE.FOR xamine.opt
	COPY $(MMS$SOURCE_LIST) $(DESTDIR)/LOG
	rename $(DESTDIR)client.h $(DESTDIR)Xamine.h
	ECHO "Xamine installed"

WINDOW_FILE : windfile_parse.obj windfile_lex.obj dispwind.obj
	touch WINDOW_FILE
	$(ECHO) "Window file I/O subsystem built"

WINDOW_TEST : wintest.exe
	$(ECHO) "Windows file test suite finished"

wintest.exe : wintest.cc WINDOW_FILE spectra.obj
	$(CPP) $(CCFLAGS) wintest.cc
	$(LD) wintest wintest.obj dispwind.obj windfile_parse.obj windfile_lex.obj \
	         spectra.obj \

dispwind.obj : dispwind.cc dispwind.h
	$(CPP) $(CCFLAGS) dispwind.cc

windfile_parse.obj : windfile.y  dispwind.h dispshare.h
	$(YACC)/name_prefix=windfile windfile.y
	mv windfile_tab.c windfile_parse.cc
	$(CPP) $(CCFLAGS)  windfile_parse.cc

defaultfile_parse.obj : defaultfile.y dispwind.h dispshare.h dfltmgr.h
 	$(YACC)/name_prefix=defaultfile defaultfile.y
 	mv defaultfile_tab.c defaultfile_parse.cc
 	$(CPP) $(CCFLAGS) defaultfile_parse.cc


windfile_lex.h : windfile.y
	$(YACC)/defines/name_prefix=windfile  windfile.y
	mv windfile_tab.h windfile_lex.h

windfile_lex.obj : windfile.l dispwind.h windfile_lex.h
	$(LEX) windfile.l
	mv $(LEXOUT) windfile_lex.cc
	$(CPP) /include=(gnu_gxx_include:[000000]) $(CCFLAGS) windfile_lex.cc

GROBJ_FILE : dispgrob.obj grobfile_lex.obj grobfile_parse.obj grobjdb.obj grobfile.obj
	touch GROBJ_FILE
	$(ECHO) "GROBJ_FILE rebuilt"

dispgrob.obj : dispgrob.cc dispgrob.h
	$(CPP) $(CCFLAGS) dispgrob.cc

grobjdb.obj : grobjdb.cc dispgrob.h
	$(CPP) $(CCFLAGS) grobjdb.cc

grobfile_lex.obj : grobfile.l dispgrob.h grobfile_lex.h
	$(LEX) grobfile.l
	mv $(LEXOUT) grobfile_lex.cc
	$(CPP) /include=(gnu_gxx_include:[000000]) $(CCFLAGS) grobfile_lex.cc

grobfile_parse.obj : grobfile.y dispgrob.h
	$(YACC)/name_prefix=grobjfile grobfile.y
	mv grobfile_tab.c  grobfile_parse.cc
	$(CPP) $(CCFLAGS) grobfile_parse.cc

grobfile_lex.h : grobfile.y
	$(YACC) /defines/name_prefix=grobjfile grobfile.y
	mv grobfile_tab.h  grobfile_lex.h

grobfile.obj : grobfile.cc dispgrob.h
	$(CPP) $(CCFLAGS) grobfile.cc

GROBJ_TEST : grobjtest.exe
	$(ECHO) "Graphical object tests built"

grobjtest.exe : grobjtest.cc GROBJ_FILE
	$(CPP) $(CCFLAGS) grobjtest.cc
	$(LD) grobjtest grobjtest.obj dispgrob.obj grobfile_lex.obj \
		grobfile_parse.obj grobjdb.obj grobfile.obj


DISP_SHARED : spectra.obj
	$(ECHO) "Spectrum access package rebuilt"

spectra.obj : spectra.cc dispshare.h
	$(CPP) $(CCFLAGS) spectra.cc

MOTIF_LIB_OBJS = XmCallback.obj XMMenus.obj XMDialogs.obj XMManagers.obj \
	XMWlist.obj
MOTIF_LIB : XMcallback.obj, XMMenus.obj, XMDialogs.obj, XMManagers.obj, \
	 XMWlist.obj 
	touch motif_lib.
	$(ECHO) "MOTIF Library built"

XMcallback.obj : XMcallback.cc XMWidget.h
	$(CPP) $(CCFLAGS) XMcallback.cc

XMMenus.obj : XMMenus.cc XMWidget.h XMPushbutton.h XMMenus.h
	$(CPP) $(CCFLAGS) XMMenus.cc

Xamine.exe : Xamine.obj \
	 MOTIF_LIB \
	 WINDOW_FILE \
	 XAMINE_FILES \
	 GROBJ_FILE
	$(LD)  Xamine,XMcallback,XMMenus,XMDialogs,\
	XMManagers,XMWlist, \
	windfile_parse,windfile_lex,dispwind,\
	menusetup,exit,helpmenu,winiomenu,grobjiomenu,\
	geometrymenu,errormsg,logging,toggles,spectra,\
	specchoose,\
	spcdisplay,advprompt,properties,panemgr,\
	buttonsetup,pkgmgr,\
	dfltmgr,axisdefault,titledefault,attribdefault,\
	reducedefault,\
	rend1default,rend2default,printer,refreshctl,\
	axes,scaling,\
	text,titles,chanplot,plot1d,plot2d,colormgr,\
	shared, \
	location,convert,trackcursor,expand,select2,\
	info,dispgrob,grobfile_lex,grobfile_parse,grobjdb,\
	grobfile, refreshdefault, defaultfile_parse, \
	marker, grafinput, grobjinput, grobjmgr, grobjdisplay, \
	sumregion, integrate, guintegrate, delgrob, \
	messages, eventflags, gateio, acceptgates, clirequests, \
	spccheck, prccheck, superpos, superprompt, \
	XWINDOWS.opt/opt

Xamine.obj : Xamine.cc XMPushbutton.h XMWidget.h  dispshare.h XMManagers.h \
	panemgr.h pkgmgr.h optionmenu.h colormgr.h \
	menusetup.h dfltmgr.h gateio.h clirequests.h
	$(CPP) $(CCFLAGS) Xamine.cc

XMDialogs.obj : XMDialogs.cc XMDialogs.h
	$(CPP) $(CCFLAGS) XMDialogs.cc

XAMINE_FILES : menusetup.obj exit.obj helpmenu.obj winiomenu.obj grobjiomenu.obj geometrymenu.obj \
	errormsg.obj procede.obj logging.obj toggles.obj specchoose.obj spectra.obj \
	spcdisplay.obj advprompt.obj properties.obj buttonsetup.obj panemgr.obj \
	pkgmgr.obj dfltmgr.obj axisdefault.obj titledefault.obj attribdefault.obj \
	reducedefault.obj rend1default.obj rend2default.obj printer.obj refreshctl.obj \
	axes.obj scaling.obj text.obj titles.obj chanplot.obj plot1d.obj plot2d.obj \
	colormgr.obj shared.obj location.obj convert.obj trackcursor.obj \
	expand.obj select2.obj info.obj refreshdefault.obj \
	defaultfile_parse.obj marker.obj grafinput.obj grobjinput.obj \
	grobjmgr.obj grobjdisplay.obj sumregion.obj integrate.obj \
	guintegrate.obj delgrob.obj \
	messages.obj eventflags.obj gateio.obj \
	acceptgates.obj clirequests.obj spccheck.obj prccheck.obj \
	superpos.obj superprompt.obj
	$(ECHO) "Xamine objects built"
	TOUCH XAMINE_FILES.

menusetup.obj : menusetup.cc XMMenus.h exit.h winiomenu.h grobjiomenu.h \
	geometrymenu.h toggles.h logging.h specchoose.h properties.h \
	menusetup.h dispshare.h helpmenu.h winiomenu.h spcdisplay.h \
	advprompt.h panemgr.h pkgmgr.h optionmenu.h printer.h refreshctl.h \
	expand.h info.h
	$(CPP) $(CCFLAGS) menusetup.cc

exit.obj : exit.cc XMDialogs.h panemgr.h helpmenu.h exit.h winiomenu.h
	$(CPP) $(CCFLAGS) exit.cc

helpmenu.obj : helpmenu.cc helpmenu.h XMDialogs.h
	$(CPP) $(CCFLAGS) helpmenu.cc


winiomenu.obj : winiomenu.cc XMDialogs.h helpmenu.h winiomenu.h panemgr.h \
	dispwind.h exit.h refreshctl.h
	$(CPP) $(CCFLAGS) winiomenu.cc


grobjiomenu.obj : grobjiomenu.cc grobjiomenu.h XMDialogs.h helpmenu.h
	$(CPP) $(CCFLAGS) grobjiomenu.cc


geometrymenu.obj : geometrymenu.cc XMLabel.h XMDialogs.h helpmenu.h \
	errormsg.h geometrymenu.h menusetup.h XMPushbutton.h XMManagers.h \
	XMSeparators.h exit.h
	$(CPP) $(CCFLAGS) geometrymenu.cc

errormsg.obj : errormsg.cc XMDialogs.h XMWidget.h
	$(CPP) $(CCFLAGS) errormsg.cc

COLORSET : colorset.exe colorset.ico
	$(ECHO) "colorset - color map generation tool rebuilt."

colorset.exe : colorset.obj $(MOTIF_LIB_OBS) errormsg.obj helpmenu.obj \
	procede.obj colormgr.obj
	$(LD)  colorset, errormsg, helpmenu, procede, colormgr, \
	XMDialogs, XMMenus, XMcallback, XWINDOWS.opt/opt

colorset.obj : colorset.cc XMDialogs.h XMWidget.h XMPushbutton.h XMMenus.h \
	helpmenu.h errormsg.h procede.h
	$(CPP) $(CCFLAGS) colorset.cc

procede.obj : procede.cc procede.h XMDialogs.h
	$(CPP) $(CCFLAGS) procede.cc

logging.obj : logging.cc logging.h XMDialogs.h helpmenu.h errormsg.h
	$(CPP) $(CCFLAGS) logging.cc

toggles.obj : toggles.cc toggles.h XMWidget.h
	$(CPP) $(CCFLAGS) toggles.cc

specchoose.obj : specchoose.cc specchoose.h XMDialogs.h helpmenu.h errormsg.h \
	spcdisplay.h
	$(CPP) $(CCFLAGS) specchoose.cc

spcdisplay.obj : spcdisplay.cc spcdisplay.h dispwind.h panemgr.h dfltmgr.h \
	      refreshctl.h dfltmgr.h exit.h
	$(CPP) $(CCFLAGS) spcdisplay.cc

advprompt.obj : advprompt.cc XMDialogs.h helpmenu.h advprompt.h spcdisplay.h \
	errormsg.h
	$(CPP) $(CCFLAGS)  advprompt.cc

properties.obj : properties.cc XMWidget.h XMDialogs.h XMManagers.h helpmenu.h \
	errormsg.h properties.h spcdisplay.h dispwind.h optionmenu.h \
	panemgr.h refreshctl.h colormgr.h exit.h
	$(CPP) $(CCFLAGS) properties.cc

XMManagers.obj : XMManagers.cc XMManagers.h XMWidget.h
	$(CPP) $(CCFLAGS) XMManagers.cc

panemgr.obj : panemgr.cc XMManagers.h XMWidget.h panemgr.h menusetup.h \
	buttonsetup.h pkgmgr.h dispshare.h trackcursor.h
	$(CPP) $(CCFLAGS) panemgr.cc

buttonsetup.obj : buttonsetup.cc XMManagers.h XMPushbutton.h buttonsetup.h \
	geometrymenu.h panemgr.h specchoose.h spcdisplay.h pkgmgr.h \
	refreshctl.h location.h expand.h info.h
	$(CPP) $(CCFLAGS) buttonsetup.cc

XMWlist.obj : XMWlist.cc XMWlist.h XMWidget.h XMPushbutton.h
	$(CPP) $(CCFLAGS) XMWlist.cc

pkgmgr.obj : pkgmgr.cc XMWlist.h pkgmgr.h
	$(CPP) $(CCFLAGS) pkgmgr.cc

dfltmgr.obj : dfltmgr.cc dfltmgr.h dispwind.h colormgr.h XMWidget.h
	$(CPP) $(CCFLAGS) dfltmgr.cc

axisdefault.obj : axisdefault.cc  XMPushbutton.h XMDialogs.h \
		helpmenu.h dispwind.h dfltmgr.h optionmenu.h
	$(CPP) $(CCFLAGS) axisdefault.cc

titledefault.obj : titledefault.cc XMPushbutton.h  XMDialogs.h \
	helpmenu.h dispwind.h dfltmgr.h optionmenu.h
	$(CPP) $(CCFLAGS) titledefault.cc

attribdefault.obj : attribdefault.cc XMDialogs.h XMPushbutton.h XMLabel.h \
	XMText.h dispwind.h dfltmgr.h optionmenu.h helpmenu.h
	$(CPP) $(CCFLAGS) attribdefault.cc

reducedefault.obj : reducedefault.cc XMDialogs.h XMPushbutton.h XMManagers.h \
	dispwind.h dfltmgr.h optionmenu.h helpmenu.h
	$(CPP) $(CCFLAGS) reducedefault.cc

rend1default.obj : rend1default.cc XMDialogs.h XMPushbutton.h XMManagers.h \
	optionmenu.h dfltmgr.h dispwind.h helpmenu.h
	$(CPP) $(CCFLAGS) rend1default.cc

rend2default.obj : rend2default.cc XMDialogs.h XMPushbutton.h XMManagers.h \
	optionmenu.h dfltmgr.h dispwind.h helpmenu.h colormgr.h
	$(CPP) $(CCFLAGS) rend2default.cc

printer.obj : printer.cc XMDialogs.h XMManagers.h XMPushbutton.h XMText.h  \
	printer.h
	$(CPP) $(CCFLAGS) printer.cc

refreshctl.obj : refreshctl.cc refreshctl.h XMWidget.h panemgr.h axes.h titles.h \
	chanplot.h spcdisplay.h queue.h
	$(CPP) $(CCFLAGS) refreshctl.cc

axes.obj : axes.cc axes.h XMWidget.h dispwind.h dispshare.h XBatch.h text.h
	$(CPP) $(CCFLAGS) axes.cc

scaling.obj : scaling.cc scaling.h XMWidget.h dispwind.h scaling.h dispshare.h \
	axes.h
	$(CPP) $(CCFLAGS) scaling.cc

text.obj : text.cc text.h
	$(CPP) $(CCFLAGS) text.cc

titles.obj : titles.cc dispshare.h titles.h axes.h panemgr.h text.h
	$(CPP) $(CCFLAGS) titles.cc

chanplot.obj : chanplot.cc refreshctl.h dispwind.h axes.h scaling.h chanplot.h \
	panemgr.h XMWidget.h
	$(CPP) $(CCFLAGS) chanplot.cc

plot1d.obj : plot1d.cc XMWidget.h XBatch.h chanplot.h panemgr.h  \
	dispshare.h dispwind.h
	$(CPP) $(CCFLAGS) plot1d.cc

plot2d.obj : plot2d.cc XMWidget.h XBatch.h dispwind.h dispshare.h chanplot.h \
	scaling.h colormgr.h
	$(CPP) $(CCFLAGS)  plot2d.cc

colormgr.obj : colormgr.cc XMWidget.h colormgr.h
	$(CPP) $(CCFLAGS) colormgr.cc

shared.obj : shared.cc dispshare.h
	$(CPP) $(CCFLAGS) shared.cc


CLIENT :  client.olb Xaminetest.exe aedxamine.obj 
	echo "Client software and tests built"

client.olb : client.obj aedxamine.obj clientgates.obj prccheck.obj \
	allocator.obj specalloc.obj
	library/create client.olb client,aedxamine,clientgates,prccheck,\
		allocator,specalloc

client.obj : client.c client.h
	$(CC) $(CCFLAGS) client.c


Xaminetest.exe : Xaminetest.obj client.olb
	$(LD) Xaminetest.obj, client/lib, sys$library:crtlib/opt

Xaminetest.obj : Xaminetest.c client.h
	$(CC) $(CCFLAGS) Xaminetest.c


aedxamine.obj : aedxamine.for xamineshare.for
	$(F77) $(F77FLAGS) aedxamine

DEMO.EXE : DEMO.OBJ client.olb
	$(LD) demo,client/lib,daq_lib:libqtree/lib,libfile/lib,libcvt/lib, -
		sys$disk:[]xamine/opt

demo.obj : demo.for
	$(F77) $(F77FLAGS) demo

location.obj : location.cc XMManagers.h XMLabel.h location.h
	$(CPP) $(CCFLAGS) location.cc

convert.obj : convert.cc convert.h XMWidget.h dispwind.h axes.h
	$(CPP) $(CCFLAGS) convert.cc

trackcursor.obj : trackcursor.cc XMWidget.h trackcursor.h buttonsetup.h \
	location.h panemgr.h dispwind.h dispshare.h convert.h
	$(CPP) $(CCFLAGS) trackcursor.cc

expand.obj : expand.cc XMWidget.h XMDialogs.h panemgr.h helpmenu.h expand.h \
	refreshctl.h exit.h errormsg.h select2.h dispshare.h panemgr.h \
   	convert.h colormgr.h
	   	$(CPP) $(CCFLAGS) expand.cc
   
select2.obj : select2.cc XMDialogs.h XMLabel.h XMText.h XMPushbutton.h \
 	helpmenu.h select2.h XMWidget.h 
	  	$(CPP) $(CCFLAGS) select2.cc
   
info.obj : info.cc XMWidget.h XMDialogs.h XMText.h \
	  	info.h logging.h dispshare.h panemgr.h errormsg.h dispwind.h dispgrob.h
	  	$(CPP) $(CCFLAGS) info.cc

refreshdefault.obj : refreshdefault.cc XMScale.h dfltmgr.h optionmenu.h  \
	helpmenu.h dispwind.h
	$(CPP) $(CCFLAGS) refreshdefault.cc

clientgates.obj : clientgates.c clientgates.h client.h 
	$(CC) $(CCFLAGS) clientgates

messages.obj : messages.cc messages.h eventflags.h
	$(CPP) $(CCFLAGS) messages.cc

eventflags.obj : eventflags.cc eventflags.h
	$(CPP) $(CCFLAGS) eventflags.cc

gateio.obj : gateio.cc XMWidget.h messages.h gateio.h
	$(CPP) $(CCFLAGS) gateio.cc

acceptgates.obj : acceptgates.cc XMWidget.h XBatch.h convert.h dispgrob.h \
	acceptgates.h sumregion.h grobjdisplay.h panemgr.h XMDIalogs.h \
	grafinput.h grobjinput.h colormgr.h chanplot.h dispshare.h grobjmgr.h \
	errormsg.h gateio.h
	$(CPP) $(CCFLAGS) acceptgates.cc


marker.obj : marker.cc marker.h XMWidget.h XMText.h XMLabel.h XMPushbutton.h \
	XBatch.h errormsg.h grafinput.h grobjinput.h dispgrob.h panemgr.h \
	dispshare.h colormgr.h convert.h grobjmgr.h
	$(CPP) $(CCFLAGS) marker.cc

grafinput.obj : grafinput.cc XMDialogs.h grafinput.h helpmenu.h panemgr.h \
	refreshctl.h convert.h dispshare.h XMWidget.h
	$(CPP) $(CCFLAGS) grafinput.cc

grobjinput.obj : grobjinput.cc XMManagers.h XMDialogs.h XMSeparators.h \
	grobjinput.h XMWidget.h
	$(CPP) $(CCFLAGS) grobjinput.cc

grobjmgr.obj : grobjmgr.cc dispgrob.h grobjmgr.h
	$(CPP) $(CCFLAGS) grobjmgr.cc

grobjdisplay.obj : grobjdisplay.cc XMWidget.h panemgr.h grobjdisplay.h \
	dispwind.h text.h dispgrob.h dispshare.h errormsg.h convert.h \
	colormgr.h grobjmgr.h XBatch.h sumregion.h chanplot.h acceptgates.h \
	XMList.h
	$(CPP) $(CCFLAGS) grobjdisplay.cc

sumregion.obj : sumregion.cc XMWidget.h XMPushbutton.h XMLabel.h XMText.h \
	XMList.h dispgrob.h dispwind.h dispshare.h grobjmgr.h sumregion.h \
	panemgr.h convert.h errormsg.h grobjinput.h colormgr.h chanplot.h \
	grobjdisplay.h XBatch.h
	$(CPP) $(CCFLAGS) sumregion.cc

integrate.obj : integrate.cc integrate.h dispshare.h dispgrob.h
	$(CPP) $(CCFLAGS) integrate.cc

guintegrate.obj : guintegrate.cc XMDialogs.h XMText.h guintegrate.h integrate.h \
	logging.h dispshare.h dispgrob.h grobjmgr.h XMWidget.h panemgr.h \
	errormsg.h
	$(CPP) $(CCFLAGS) guintegrate.cc

delgrob.obj : delgrob.cc XMWidget.h XMDialogs.h dispgrob.h dispwind.h \
	helpmenu.h delgrob.h grobjmgr.h panemgr.h errormsg.h
	$(CPP) $(CCFLAGS) delgrob.cc

clirequests.obj : clirequests.cc dispshare.h XMWidget.h errormsg.h \
	panemgr.h dispgrob.h clirequests.h gateio.h grobjmgr.h refreshctl.h
	$(CPP) $(CCFLAGS) clirequests.cc

spccheck.obj : spccheck.cc spccheck.h panemgr.h dispwind.h refreshctl.h \
	dispshare.h
	$(CPP)$(CCFLAGS) spccheck.cc

prcheck.obj : prccheck.c prccheck.h
	$(CC)$(CCFLAGS) prccheck.c


superpos.obj : superpos.cc superpos.h
	$(CPP) $(CCFLAGS) superpos.cc

superprompt.obj : superprompt.cc XMDialogs.h superprompt.h dispwind.h \
	superpos.h spcdisplay.h dispshare.h helpmenu.h panemgr.h errormsg.h \
	specchoose.h
	$(CPP) $(CCFLAGS) superprompt.cc

specalloc.obj : specalloc.c client.h
	$(CC) $(CCFLAGS) specalloc.c

allocator.obj : allocator.c allocator.h
	$(CC) $(CCFLAGS) allocator.c

