/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   buttonsetup.cc  - This file contains code which sets up and manages
**                     the button bar.  The button bar is a set of buttons
**                     which allow the user to quickly select the most
**                     common operations without requiring interaction
**                     with the menus.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
static char *sccsinfo = "@(#)buttonsetup.cc	8.2 10/3/95 ";


/*
**  Include files: 
*/
#include "XMManagers.h"
#include "XMPushbutton.h"
#include "buttonsetup.h"
#include "geometrymenu.h"
#include "panemgr.h"
#include "specchoose.h"
#include "spcdisplay.h"
#include "pkgmgr.h"
#include "refreshctl.h"
#include "location.h"
#include "expand.h"
#include "info.h"
#include "marker.h"
#include "sumregion.h"
#include "guintegrate.h"
#include "acceptgates.h"
#include "trackcursor.h"
#include "properties.h"

/*
** Module static storage:
*/
static XMToggleButton *zoom_button;
static XMToggleButton *log_button;

/* 
** Functional Description:
**   Xamine_SetButtonBarLog:
**     Sets the state of the button bar log toggle button.
** Formal Parameters:
**    Boolean state:
**      New stateu of the button.
*/
void Xamine_SetButtonBarLog(Boolean state)
{
  if(state) log_button->Set();
  else      log_button->UnSet();
}

/*
** Functional Description:
**   Xamine_SetButtonBarZoom:
**     Sets the state of the button bar zoom button.
** Formal Parameters:
**   Boolean state:
**    New state of the button.
*/
void Xamine_SetButtonBarZoom(Boolean state)
{
  if(state) zoom_button->Set();
  else      zoom_button->UnSet();
}
/*
** Functional Description:
**   Xamine_GetButtonBarLog:
**     Gets the state of the log button:
** Returns:
**    True if set, False if not.
*/
Boolean Xamine_GetButtonBarLog()
{
  return log_button->GetState();
}

/*
** Functional Description:
**   Xamine_SetupButtonBar:
**      This function creates the initial button bar and fills it in.
**      The button bar is managed by a row column widget.  Each subsection
**      within the button bar is a Form widget bounded by a frame.
** Formal Parameters:
**   XMWidget *button_bar:
**      The managing scrolled window widget.
*/
void Xamine_SetupButtonBar(XMWidget *button_bar)
{


  XMRowColumn *manager = new XMRowColumn("Button_rc", *button_bar);

  /* Set the characteristics of the manager: */

  manager->SetOrientation(XmHORIZONTAL);
  manager->SetRowColumns(1);
  manager->SetPacking(XmPACK_COLUMN);

  /* Produce the manager frames: */

  XMFrame *windows, *spectra, *grobs;
  Arg frame_attribs;

  XtSetArg(frame_attribs, XmNshadowType, XmSHADOW_ETCHED_IN);


  windows = new XMFrame("Windows_f", *manager, &frame_attribs, 1);
  spectra = new XMFrame("Spectra_f", *manager, &frame_attribs, 1);
  grobs   = new XMFrame("Grobj_f",   *manager, &frame_attribs, 1);

  /* Produce the forms: */

  XMForm *windows_rc, *spectra_rc, *grobs_rc;

  windows_rc = new XMForm("Windows_rc", *windows);
  spectra_rc = new XMForm("Spectra_rc", *spectra);
  grobs_rc   = new XMForm("Grobj_rc",   *grobs);


  /* Create the buttons in windows category */

  XMPushButton *pb;
  XMWidget *pbt1, *pbt2;
  XMToggleButton *tb;
  XMWidget *last;

  windows_rc->SetAttribute(XmNallowOverlap, (XtArgVal)False);
  windows_rc->SetFractionBase(45);

  pb = new XMPushButton("Geometry", *windows_rc);
  pb->AddCallback(Xamine_request_geometry);
  windows_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  windows_rc->SetLeftPosition(*pb, 1);
  windows_rc->SetTopAttachment(*pb, XmATTACH_POSITION);
  windows_rc->SetTopPosition(*pb, 5);
  Xamine_AddtoBasePackage(pb);
  pbt1 = pb;

  tb = new XMToggleButton("Zoom",   *windows_rc);
  tb->AddCallback(Xamine_ToggleZoom);
  zoom_button = tb;
  windows_rc->SetLeftAttachment(*tb, XmATTACH_POSITION);
  windows_rc->SetLeftPosition(*tb, 30);
  windows_rc->SetTopAttachment(*tb, XmATTACH_POSITION);
  windows_rc->SetTopPosition(*tb, 5);
  windows_rc->SetRightAttachment(*tb, XmATTACH_FORM);
  Xamine_AddtoBasePackage(zoom_button);
  pbt2 = tb;

  static  Xamine_ChooserCbData DisplayCb = { True,
					     Xamine_Display
					    };
  static Xamine_ChooserCbData PlusCb    = { False,
					    Xamine_DisplayAdvance
					    };

  pb = new XMPushButton("Display", *windows_rc);
  pb->AddCallback(Xamine_ChooseSpectrum, (XtPointer)&DisplayCb);
  windows_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  windows_rc->SetLeftPosition(*pb, 1);
  windows_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  windows_rc->SetTopWidget(*pb, *pbt1);
  Xamine_AddtoBasePackage(pb);
  pbt1 = pb;

  pb = new XMPushButton("Displayp", *windows_rc);
  pb->Label("Display +");
  pb->AddCallback(Xamine_ChooseSpectrum, (XtPointer)&PlusCb);
  windows_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  windows_rc->SetLeftPosition(*pb, 1);
  windows_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  windows_rc->SetTopWidget(*pb, *pbt1);
  windows_rc->SetBottomAttachment(*pb, XmATTACH_FORM);
  Xamine_AddtoBasePackage(pb);
  pbt1 = pb;

  

  /* Create the buttons in the spectrum category: */

  spectra_rc->SetAttribute(XmNallowOverlap, (XtArgVal)False);
  spectra_rc->SetFractionBase(45);


  pb = new XMPushButton("Updall",  *spectra_rc);
  pb->Label("Update All");
  pb->AddCallback(Xamine_UpdateAll);
  spectra_rc->SetTopAttachment(*pb, XmATTACH_POSITION);
  spectra_rc->SetTopPosition(*pb, 5);
  windows_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  spectra_rc->SetLeftPosition(*pb, 1);
  Xamine_AddtoSpectrumPresentPackage(pb);
  pbt1 = pb;

  pb = new XMPushButton("Expand",  *spectra_rc);
  pb->AddCallback(Xamine_Expand);
  spectra_rc->SetTopAttachment(*pb, XmATTACH_POSITION);
  spectra_rc->SetTopPosition(*pb, 5);
  spectra_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  spectra_rc->SetLeftPosition(*pb, 30);
  Xamine_AddtoSpectrumSelectedPackage(pb);
  pbt2 = pb;

  pb = new XMPushButton("Updsel",  *spectra_rc);
  pb->Label("Update Selected");
  pb->AddCallback(Xamine_Update);
  spectra_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  spectra_rc->SetTopWidget(*pb, *pbt1);
  spectra_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  spectra_rc->SetLeftPosition(*pb, 1);
  Xamine_AddtoSpectrumSelectedPackage(pb);
  pbt1 = pb;

  pb = new XMPushButton("UnExpand", *spectra_rc);
  pb->AddCallback(Xamine_UnExpand);
  spectra_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  spectra_rc->SetTopWidget(*pb, *pbt2);
  spectra_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  spectra_rc->SetLeftPosition(*pb, 30);
  Xamine_AddtoSpectrumSelectedPackage(pb);
  pbt2 = pb;

  pb = new XMPushButton("Info",    *spectra_rc);
  pb->AddCallback(Xamine_DisplayInfo);
  spectra_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  spectra_rc->SetTopWidget(*pb, *pbt1);
  spectra_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  spectra_rc->SetLeftPosition(*pb, 1);
  spectra_rc->SetBottomAttachment(*pb, XmATTACH_FORM);
  Xamine_AddtoSpectrumSelectedPackage(pb);
  last = pb;

  static float mult =     XAMINE_SCALE_MULTIPLIER;
  static float div  = 1.0/XAMINE_SCALE_MULTIPLIER;
  pb = new XMPushButton("+", *spectra_rc);
  pb->AddCallback(Xamine_MultiplySelectedScaleCb, 
		  (XtPointer)&div);
  spectra_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  spectra_rc->SetTopWidget(*pb, *pbt1);
  spectra_rc->SetLeftAttachment(*pb, XmATTACH_WIDGET);
  spectra_rc->SetLeftWidget(*pb, *last);
  spectra_rc->SetBottomAttachment(*pb, XmATTACH_FORM);
  Xamine_AddtoSpectrumSelectedPackage(pb);
  last = pb;

  pb = new XMPushButton("-", *spectra_rc);
  pb->AddCallback(Xamine_MultiplySelectedScaleCb, 
		  (XtPointer)&mult);
  spectra_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  spectra_rc->SetTopWidget(*pb, *pbt1);
  spectra_rc->SetLeftAttachment(*pb, XmATTACH_WIDGET);
  spectra_rc->SetLeftWidget(*pb, *last);
  spectra_rc->SetBottomAttachment(*pb, XmATTACH_FORM);
  Xamine_AddtoSpectrumSelectedPackage(pb);
  last = pb;


  log_button = new XMToggleButton("Log", *spectra_rc);
  log_button->AddCallback(Xamine_ToggleZoomState);
  spectra_rc->SetTopAttachment(*log_button, XmATTACH_WIDGET);
  spectra_rc->SetTopWidget(*log_button, *pbt2);
  spectra_rc->SetLeftAttachment(*log_button,XmATTACH_WIDGET);
  spectra_rc->SetLeftWidget(*log_button, *last);
  spectra_rc->SetRightAttachment(*log_button,  XmATTACH_NONE);
  spectra_rc->SetBottomAttachment(*log_button, XmATTACH_FORM);
  Xamine_AddtoSpectrumSelectedPackage(log_button);

  /* Create the objects in the graphical objects category. */

  grobs_rc->SetAttribute(XmNallowOverlap, (XtArgVal)False);
  grobs_rc->SetFractionBase(45);
 

  pb = new XMPushButton("Marker",  *grobs_rc);
  grobs_rc->SetTopAttachment(*pb, XmATTACH_POSITION);
  grobs_rc->SetTopPosition(*pb,   5);
  grobs_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  grobs_rc->SetLeftPosition(*pb, 1);
  pb->AddCallback(Xamine_AddMarker);
  pbt1 = pb;
  Xamine_AddtoSpectrumSelectedPackage(pb);

  pb = new XMPushButton("Cut",     *grobs_rc);
  pb->AddCallback(Xamine_AcceptGate, (XtPointer)cut_1d);
  grobs_rc->SetTopAttachment(*pb, XmATTACH_POSITION);
  grobs_rc->SetTopPosition(*pb, 5);
  grobs_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  grobs_rc->SetLeftPosition(*pb, 30);
  pbt2 = pb;
  Xamine_Addto1dSelectedPackage(pb);

  pb = new XMPushButton("Sreg",    *grobs_rc);
  pb->Label("Summing Region");
  pb->AddCallback(Xamine_AcceptSummingRegion);
  grobs_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  grobs_rc->SetLeftPosition(*pb, 1);
  grobs_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  grobs_rc->SetTopWidget(*pb, *pbt1);
  pbt1 = pb;
  Xamine_AddtoSpectrumSelectedPackage(pb);

  pb = new XMPushButton("Band",    *grobs_rc);
  pb->AddCallback(Xamine_AcceptGate, (XtPointer)band);
  grobs_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  grobs_rc->SetLeftPosition(*pb, 30);
  grobs_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  grobs_rc->SetTopWidget(*pb, *pbt2);
  pbt2 = pb;
  Xamine_Addto2dSelectedPackage(pb);

  pb = new XMPushButton("Integrate", *grobs_rc, Xamine_Integrate);
  grobs_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  grobs_rc->SetLeftPosition(*pb, 1);
  grobs_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  grobs_rc->SetTopWidget(*pb, *pbt1);
  grobs_rc->SetBottomAttachment(*pb, XmATTACH_FORM);
  pbt1  = pb;
  Xamine_AddtoSpectrumSelectedPackage(pb);

  pb = new XMPushButton("Contour", *grobs_rc);
  pb->AddCallback(Xamine_AcceptGate, (XtPointer)contour_2d);
  grobs_rc->SetLeftAttachment(*pb, XmATTACH_POSITION);
  grobs_rc->SetLeftPosition(*pb, 30);
  grobs_rc->SetTopAttachment(*pb, XmATTACH_WIDGET);
  grobs_rc->SetTopWidget(*pb, *pbt2);
  grobs_rc->SetBottomAttachment(*pb, XmATTACH_FORM);
  pbt2 = pb;
  Xamine_Addto2dSelectedPackage(pb);

  /* Manage the forms and frames: */


  windows_rc->Manage();
  windows->Manage();

  spectra_rc->Manage();
  spectra->Manage();

  grobs_rc->Manage();
  grobs->Manage();

  manager->Manage();
  button_bar->Manage();

}

