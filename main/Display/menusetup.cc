/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/* Facility:
**   Xamine user interface code.
** Abstract:
**   menusetup.cc    - This function sets up the menu tree for the 
**                     Xamine menubar.  It also manages the children so that
**                     the menu will be realized when the parent widget is
**                     realized.  It is assumed that the caller is using the
**                      XM object set.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

/*
** Include files required:
*/

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include "properties.h"
#include "menusetup.h"
#include "dispshare.h"
#include "XMMenus.h"
#include "helpmenu.h"
#include "exit.h"
#include "winiomenu.h"
#include "grobjiomenu.h"
#include "geometrymenu.h"
#include "toggles.h"
#include "logging.h"
#include "specchoose.h"
#include "spcdisplay.h"
#include "advprompt.h"
#include "panemgr.h"
#include "pkgmgr.h"
#include "optionmenu.h"
#include "printer.h"
#include "refreshctl.h"
#include "expand.h"
#include "info.h"
#include "marker.h"
#include "sumregion.h"
#include "guintegrate.h"
#include "delgrob.h"
#include "acceptgates.h"
#include "trackcursor.h"
#include "errormsg.h"
#include "superprompt.h"
#include "objcopy.h"
#include "lblfont.h"
#include "copyatr.h"
#include "Tab.h"
#include "wysiwygPrint.h"

extern Boolean Xamine_hideGates;


/*
** External references: 
*/
extern spec_shared xamine_shared;

/*
** Module static data:
*/
XMToggleButton *zoom_toggle;
static XMPushButton *unsuper_button;

static const float mult = XAMINE_SCALE_MULTIPLIER;
static const float sdiv  = 1.0/XAMINE_SCALE_MULTIPLIER;

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Xamine_EnableUnsuperimpose
**	    Enables the superimpose button.
**	Xamine_DisableUnsuperimpose
**	    Disable the superimpose button.
**
**
**--
*/

void Xamine_EnableUnsuperimpose()
{
    unsuper_button->Enable();
}
void Xamine_DisableUnsuperimpose()
{
    unsuper_button->Disable();
}

/*
** Functional Description:
**   UnImplemented:
**      This function is invoked whenever the user chooses an unimplemented
**      menu selection.  We pop up a dialog indicating that the function
**      is not yet implemented.
** Formal Parameters:
**    XMWidget *who:
**       Widget pointer of the button that triggered our call.
**    XtPointer ud:
**       User data (not used).
**    XtPointer cd:
**       Callback Data (not used).
*/
static void UnImplemented(XMWidget *who, XtPointer ud, XtPointer cd)
{
  Xamine_error_msg(who, "This function is not yet implemented");
}

/*
** Functional Description:
**   Xamine_SetZoomToggleButtonState - Sets the state of the zoom toggle buton.
**                                     this may be necessary if a geometry
**                                     request is made or if a windows file is
**                                     read in.
** Formal Parameters:
**   Boolean state:
**     New state.
*/
void Xamine_SetZoomToggleButtonState(Boolean state)
{
   if(state) {
     zoom_toggle->Set();
   }
   else {
     zoom_toggle->UnSet();
   }
}

/*
** Functional Description:
**   Xamine_setup_menus   - This function sets up the menu bar of the
**                          Xamine display program.
** Formal Parameters:
**   XMWidget *parent     - The object pointer of the parent of the menu bar
*/
XMMenuBar *Xamine_setup_menus(XMWidget *parent)
{
  XMMenuBar *bar;
  XMPulldown *pd;		/* Current pulldown menu */
  XMPulldown *sub;		/* Current Submenu pulldown */
  XMPushButton *b;
  XMToggleButton *tb;

  /* Make sure the menu bar got made: */

  bar =  new XMMenuBar("XamineMenuBar", *parent, 10);
  if(bar == NULL) {
    fprintf(stderr, "Xamine -- Unable to create main menu bar");
    exit(-1);
  }

  /* Now setup the file menu:   */

  pd = bar->AddPulldown("File", 10);	/* Add the file pulldown menu */
  tb = pd->AddMenuToggleButton("Logging");   /* Enable/disable loggin      */
  tb->SetAccelerator("Meta<Key>l", "Alt+L");
  tb->AddCallback(Xamine_SetToggle, (XtPointer)&Xamine_logging);
  Xamine_AddtoBasePackage(tb);

     /* Save/Restore: */

  pd->AddSeparator();
  sub = pd->AddSubmenu("Save",5);	        /* Save objects/windows */
  b =  sub->AddMenuButton("SVWindows");
  b->Label("Save Windows...");
  b->AddCallback(Xamine_Open_window_file,
		 (XtPointer)&Xamine_Open_win_write);
  Xamine_AddtoBasePackage(b);

  b =  sub->AddMenuButton("SVGrobs");
  b->Label("Save Graphical Objects... ");
  b->AddCallback(Xamine_Open_grobj_file,
		 (XtPointer)&Xamine_Open_grobj_write);
  Xamine_AddtoBasePackage(b);


  sub = pd->AddSubmenu("Restore",5);
  b = sub->AddMenuButton("RSWindows");
  b->Label("Restore Windows...");
  b->AddCallback(Xamine_Open_window_file, 
		 (XtPointer)&Xamine_Open_win_read);
  Xamine_AddtoBasePackage(b);

  b = sub->AddMenuButton("RSGrobs");
  b->Label("Restore Graphical Objects...");
  b->AddCallback(Xamine_Open_grobj_file,
		 (XtPointer)&Xamine_Open_grobj_read);
  Xamine_AddtoBasePackage(b);

  /* Print spectra: */

  pd->AddSeparator();
#ifdef HAVE_GRI
  b = pd->AddMenuButton("Print...",Xamine_PrintSpectrumDialog);
#else
  b = pd->AddMenuButton("Print...",Xamine_NoGriDialog);
#endif
  b->SetAccelerator("Meta<Key>P", "Alt+P");
  Xamine_AddtoSpectrumSelectedPackage(b);

  /* WYSIWYG printing:  */
   
  b = pd->AddMenuButton("WSYWIG Print...", Xamine_printCapture);

  /* Exit */

  pd->AddSeparator();
  b = pd->AddMenuButton("Exit", Xamine_confirm_exit);
  b->SetAccelerator("Ctrl<Key>X", "Ctrl+X");
  Xamine_AddtoBasePackage(b);
			
  /* Window Menu */

  pd = bar->AddPulldown("Window", 20);
  b = pd->AddMenuButton("Info");
  b->SetAccelerator("Meta<Key>Q","Alt+Q");
  b->AddCallback(Xamine_DisplayInfo);
  Xamine_AddtoSpectrumSelectedPackage(b);

  b = pd->AddMenuButton("WMGeom");
  b->Label("Geometry ...");
  b->SetAccelerator("Meta<Key>G", "Alt+G");
  b->AddCallback(Xamine_request_geometry);
  Xamine_AddtoBasePackage(b);

  pd->AddSeparator();
  b = pd->AddMenuButton("WMRdconfig");
  b->Label("Read Configuration...");
  b->SetAccelerator("Meta<Key>R","Alt+R");
  b->AddCallback(Xamine_Open_window_file, 
		 (XtPointer)&Xamine_Open_win_read);
  Xamine_AddtoBasePackage(b);


  b = pd->AddMenuButton("WMWrconfig");
  b->Label("Write Configuration ...");
  b->SetAccelerator("Meta<Key>W", "Alt+W");
  b->AddCallback(Xamine_Open_window_file,
		 (XtPointer)&Xamine_Open_win_write);
  Xamine_AddtoBasePackage(b);



  pd->AddSeparator();
  b = pd->AddMenuButton("Reset");
  b->AddCallback(Xamine_reset_geometry);
  Xamine_AddtoBasePackage(b);


  tb = pd->AddMenuToggleButton("Zoom");
  tb->AddCallback(Xamine_ToggleZoom);
  tb->SetAccelerator("Meta<Key>Z", "Alt+Z");
  Xamine_AddtoBasePackage(tb);
  zoom_toggle = tb;		/* Save the zoom toggle button widget. */

  b = pd->AddMenuButton("WMZoomPlus");
  b->Label("Zoom +");
  b->AddCallback(Xamine_ZoomNext);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("Minimize");
  b->AddCallback(Xamine_minimize_geometry);
  Xamine_AddtoBasePackage(b);


  /* The following are added to allow for accelerators for scale up and down */

  pd->AddSeparator();
 
  b = pd->AddMenuButton("SCUP");
  b->Label("Scale Down");
  b->SetAccelerator("Meta<Key>minus", "Alt+ -");
  b->AddCallback(Xamine_MultiplySelectedScaleCb, (XtPointer)&mult);
  Xamine_AddtoSpectrumSelectedPackage(b);

  b = pd->AddMenuButton("SCDN");
  b->Label("Scale Up");
  b->SetAccelerator("Meta<Key>plus", "Alt+ +");
  b->AddCallback(Xamine_MultiplySelectedScaleCb, (XtPointer)&sdiv);
  Xamine_AddtoSpectrumSelectedPackage(b);

  /* Spectra Menu: */


  static Xamine_ChooserCbData  DisplayCb = { True,
					      Xamine_Display };
  static Xamine_ChooserCbData  PlusCb   = { False,
					     Xamine_DisplayAdvance };



  

  pd = bar->AddPulldown("Spectra", 20);
  tb = pd->AddMenuToggleButton("Hide Gates");
  tb->AddCallback(Xamine_ToggleAndRefresh, (XtPointer)(&Xamine_hideGates));
  Xamine_AddtoBasePackage(tb);

  b = pd->AddMenuButton("SMDisplay");
  b->Label("Display Spectrum ...");
  b->SetAccelerator("Meta<Key>D","Alt+D");
  b->AddCallback(Xamine_ChooseSpectrum, (XtPointer)&DisplayCb);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("SMDisplayplus");
  b->SetAccelerator("Meta Shift<Key>D", "Alt+Shift+D");
  b->Label("Display Spectrum + ...");
  b->AddCallback(Xamine_ChooseSpectrum, (XtPointer)&PlusCb);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("Clear");
  b->SetAccelerator("Ctrl Meta<Key>Z", "Ctrl+Alt+Z");
  b->AddCallback(Xamine_ClearSelectedPane);
  Xamine_AddtoSpectrumSelectedPackage(b);

  b = pd->AddMenuButton("SMAdvance");
  b->Label("Set Advance ...");
  b->SetAccelerator("Ctrl<Key>A", "Control+A");
  b->AddCallback(Xamine_PromptAdvance);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("SMSuper");
  b->Label("Superimpose ...");
  b->SetAccelerator("Meta<Key>S","Alt+S");
  b->AddCallback(Xamine_GetSuperposition);
  Xamine_Addto1dSelectedPackage(b);

  b = pd->AddMenuButton("SMUnSuper");
  b->Label("UnSuperimpose...");
  b->AddCallback(Xamine_UnSuperimpose);
  unsuper_button = b;
  Xamine_DisableUnsuperimpose();

  pd->AddSeparator();

  b = pd->AddMenuButton("SMUpdall");
  b->Label("Update All");
  b->SetAccelerator("Ctrl<Key>U", "Ctrl+U");
  b->AddCallback(Xamine_UpdateAll);
  Xamine_AddtoSpectrumPresentPackage(b);

  b = pd->AddMenuButton("SMUpdsel");
  b->Label("Update Selected");
  b->SetAccelerator("Meta<Key>U", "Alt+U");
  b->AddCallback(Xamine_Update);
  Xamine_AddtoSpectrumSelectedPackage(b);

  pd->AddSeparator();

  b = pd->AddMenuButton("Properties");
  b->Label("Properties...");
  b->SetAccelerator("Meta<Key>O","Alt+O");
  b->AddCallback(Xamine_SetSelectedProperties);
  Xamine_AddtoSpectrumSelectedPackage(b);

  b = pd->AddMenuButton("CopyProp");
  b->Label("Copy Properties...");
  b->SetAccelerator("Meta<Key>C", "Alt+C");
  b->AddCallback(Xamine_CopyPaneAttributes);
  Xamine_AddtoSpectrumSelectedPackage(b);

  pd->AddSeparator();

  b = pd->AddMenuButton("Expand");
  b->AddCallback(Xamine_Expand);
  b->SetAccelerator("Meta<Key>E", "Alt+E");
  Xamine_AddtoSpectrumSelectedPackage(b);

  b = pd->AddMenuButton("UnExpand");
  b->AddCallback(Xamine_UnExpand);
  b->SetAccelerator("Ctrl<Key>E", "Ctrl+E");
  Xamine_AddtoSpectrumSelectedPackage(b);
  pd->AddSeparator();

  tb = pd->AddMenuToggleButton("Project");
  tb->Label("Project ...");
  tb->AddCallback(UnImplemented);
  Xamine_Addto2dSelectedPackage(tb);
  pd->AddSeparator();

/*
  b = pd->AddMenuButton("SMCopy");
  b->Label("Copy ...");
  b->SetAccelerator("Meta<Key>C", "Alt+C");
  b->AddCallback(UnImplemented);
  Xamine_AddtoSpectrumSelectedPackage(b);
*/

  /* Option menu: */

  pd = bar->AddPulldown("Options", 15);

  tb = pd->AddMenuToggleButton("ShowSbar");
  tb->SetAccelerator("Ctrl<Key>S", "Ctrl+S");
  tb->Label("Show Status Bar");
  tb->AddCallback(Xamine_ToggleStatusBar);
  tb->Set();

  b = pd->AddMenuButton("OMCursor");
  b->Label("Cursor Type ...");
  b->AddCallback(UnImplemented);
  pd->AddSeparator();
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("Updtime");
  b->Label("Update Rate...");
  b->SetAccelerator("Meta Shift<Key>U", "Alt+Shift+U");
  b->AddCallback(Xamine_SetDefaultRefresh);

  b = pd->AddMenuButton("Axis");
  b->Label("Axis Labels...");
  b->SetAccelerator("Meta Shift<Key>L", "Alt+Shift+L");
  b->AddCallback(Xamine_SetDefaultAxis);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("Title");
  b->Label("Spectrum Titles...");
  b->SetAccelerator("Meta Shift<Key>T", "Alt+Shift+T");
  b->AddCallback(Xamine_SetDefaultTitling);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("Attributes");
  b->Label("Spectrum Attributes...");
  b->SetAccelerator("Meta Shift<Key>A", "Alt+Shift+A");
  b->AddCallback(Xamine_SetDefaultAttributes);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("Reduction");
  b->Label("Spectrum Reduction...");
  b->SetAccelerator("Meta Shift<Key>R", "Alt+Shift+R");
  b->AddCallback(Xamine_SetDefaultReduction);
  Xamine_AddtoBasePackage(b);

  sub = pd->AddSubmenu("Rendition",5);
  b = sub->AddMenuButton("OMRS1d");
  b->Label("1-D...");
  b->SetAccelerator("Meta Shift<Key>1", "Alt+Shift+1");
  b->AddCallback(Xamine_Set1dDefaultRendition);
  Xamine_AddtoBasePackage(b);

  b = sub->AddMenuButton("OMRS2d");
  b->Label("2-D...");
  b->SetAccelerator("Meta Shift<Key>2", "Alt+Shift+2");
  b->AddCallback(Xamine_Set2dDefaultRendition);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("grobjlblsize");
  b->Label("Object Label Size..");
  b->SetAccelerator("Meta Shift<Key>G", "Alt+Shift+G");
  b->AddCallback(Xamine_SetGrobjLblFont);
  Xamine_AddtoBasePackage(b);

  pd->AddSeparator();
  b = pd->AddMenuButton("OMPrsetup");
  b->Label("Print Setup ...");
  b->SetAccelerator("Meta Shift<Key>P", "Alt+Shift+P");
  b->AddCallback(Xamine_SetupPrinter);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("OMLsetup");
  b->Label("Logfile setup ...");
  b->SetAccelerator("Meta Shift<Key>F", "Alt+Shift+F");
  b->AddCallback(Xamine_SetupLogFile);
  Xamine_AddtoBasePackage(b);

  /* Graphical Objects Menu: */

  pd = bar->AddPulldown("Graph_objects", 10);
  b = pd->AddMenuButton("GMRd");
  b->Label("Read Objects... ");
  b->SetAccelerator("Ctrl<Key>R", "Ctrl+R");
  b->AddCallback(Xamine_Open_grobj_file,
		 (XtPointer)&Xamine_Open_grobj_read);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("GMWrt");
  b->Label("Write Objects... ");
  b->SetAccelerator("Ctrl<Key>W", "Ctrl+W");
  b->AddCallback(Xamine_Open_grobj_file,
		 (XtPointer)&Xamine_Open_grobj_write);
  Xamine_AddtoBasePackage(b);

  pd->AddSeparator();

  sub = pd->AddSubmenu("Add", 5);
  b = sub->AddMenuButton("GMASSum");
  b->Label("Summing Region ...");
  b->SetAccelerator("Ctrl<Key>L","Ctrl+L");
  Xamine_AddtoSpectrumSelectedPackage(b);
  b->AddCallback(Xamine_AcceptSummingRegion);

  b = sub->AddMenuButton("Marker");
  b->SetAccelerator("Ctrl<Key>M", "Ctrl+M");
  b->AddCallback(Xamine_AddMarker);
  Xamine_AddtoSpectrumSelectedPackage(b);

  b = sub->AddMenuButton("Cut");
  b->SetAccelerator("Ctrl<Key>G", "Ctrl+G");
  b->AddCallback(Xamine_AcceptGate, (XtPointer)cut_1d);
  Xamine_Addto1dSelectedPackage(b);

  b = sub->AddMenuButton("Contour");
  b->SetAccelerator("Alt Shift<Key>C", "Alt+Shift+C");
  b->AddCallback(Xamine_AcceptGate, (XtPointer)contour_2d);
  Xamine_Addto2dSelectedPackage(b);

  b = sub->AddMenuButton("Band");
  b->AddCallback(Xamine_AcceptGate, (XtPointer)band);
  b->SetAccelerator("Ctrl<Key>B", "Ctrl+B");
  Xamine_Addto2dSelectedPackage(b);
  pd->AddSeparator();

  b = pd->AddMenuButton("Integrate");
  b->SetAccelerator("Meta<Key>I", "Alt+I");
  Xamine_AddtoSpectrumSelectedPackage(b);
  b->AddCallback(Xamine_Integrate);

  b = pd->AddMenuButton("Delete");
  b->SetAccelerator("Meta Shift<Key>Z", "Alt+Shift+Z");
  Xamine_AddtoSpectrumSelectedPackage(b);
  b->AddCallback(Xamine_DeleteObject);
  

  b = pd->AddMenuButton("GMCopy");
  b->Label("Copy Object ...");
  b->SetAccelerator("Meta Shift<Key>O", "Alt+Shift+O");
  b->AddCallback(Xamine_CopyObject);
  Xamine_AddtoSpectrumSelectedPackage(b);

  /*  Help Menu: */

  pd = bar->AddHelpPulldown("Help", 2);
  b = pd->AddMenuButton("HMAbout");
  b->Label("About ...");
  b->SetAccelerator("Meta<Key>V", "Alt+V");
  b->AddCallback(Xamine_display_help, (XtPointer)&Xamine_About);
  Xamine_AddtoBasePackage(b);

  b = pd->AddMenuButton("Help");
  b->Label("Help ...");
  b->SetAccelerator("Meta<Key>slash", "Alt+?");
  b->AddCallback(Xamine_display_help, (XtPointer)&Xamine_Help_Overview);
  Xamine_AddtoBasePackage(b);

  /*  Manage the menu bar and return it's widget ID to the caller */


  bar->Manage();
  return bar;
}
