/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   refresehdefault.cc:
**     This file contains implementation code for a dialog which prompts for
**     the default refresh interval for spectra displayed in Xamine panes.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1312
*/
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
static char *sccsinfo = "@(#)refreshdefault.cc	8.1 6/23/95 ";

/*
** Include files:
*/
#include <Xm/Xm.h>
#include "helpmenu.h"
#include "XMDialogs.h"
#include "XMScale.h"
#include "dfltmgr.h"
#include "optionmenu.h"
#include "dispwind.h"
#include "panemgr.h"
#include "refreshctl.h"

/*
** Constant definitions:
*/
#define MAXTIME    600		/* Maximum time on slider in seconds */

/*
** Local Data:
*/

/*  The data items below are help text and structures to drive the dialog's
**  context sensitive help.
*/

static char *help_text[] = {
  "  This dialog is prompting for the default spectrum refresh interval.\n",
  "Non-zero refresh intervals cause Xamine to periodically re-draw a pane\n",
  "with current data.  Be careful when setting the default refresh value\n",
  "to something other than zero since refreshing the display can be\n",
  "computationally expensive, a non-zero but low value can cause Xamine\n",
  "to spend all of the computer's cycles refreshing panes leaving nothing\n",
  "available for other work such as sorting data into the histograms that\n",
  "Xamine displays.\n\n",
  "   If the refresh interval is zero, then panes are only updated when you\n",
  "manually request updates with the Update and Update Selected buttons\n\n",
  "   Use the mouse to move the slider until the desired update rate is\n",
  "selected.  If you hold down the left button, the slider will follow the\n",
  "mouse.  If you click the left button when the mouse is not on the slider's\n",
  "cursor, then the slider moves a predetermined amount towards the mouse\n",
  "If you click the middle button when the mouse is not on the slider's\n",
  "cursor, then the mouse will move to the cursor.\n\n",
  "   The butons in the action area of the dialog have the following meanings:",
  "\n\n",
  "      Ok       - Accepts the current value of the slider as the new\n",
  "                 default refresh rate and dismisses the dialog.\n",
  "      Apply To All\n",
  "               - Same as Ok but also applies the change to all panes\n",
  "      Cancel   - Dismisses the dialog and makes no changes.\n",
  "      Help     - Displays this help dialog\n",
  NULL
};

static Xamine_help_client_data help =  { "Refresh_rate",
					 NULL,
					 help_text
					 };
					   
/*
** Below we build up a class to accept the refresh time.  It is descended
** from the CustomDialog class and the RefreshForm class.
*/

class RefreshDialog : public XMCustomDialog, public RefreshForm {
   public:
      RefreshDialog(char *name, XMWidget *parent) :
	XMCustomDialog(name, *parent, "Default Refresh Rate"),
        RefreshForm(name, *work_area) { Apply->Label("Apply To All"); }
      ~RefreshDialog() {}
};

static RefreshDialog *dialog = NULL; /* Dialog pointer */


/*
** RefreshForm::RefreshForm:
**    This method instantiates a RefreshForm.  A refresh form consists
**    of a Scale widget with the label:  Refresh Interval in Seconds.
**    The range runs from 0 to 3600 seconds.
** Formal Parameters:
**    char *name:
**      Name of the widget.
**    XMForm &parent:
**      Parent of the widget... assumed to be a form widget.
*/
RefreshForm::RefreshForm(char *name, XMForm &parent) {

  /* Instantiate a scale widget. */

  interval = new XMScale("RefreshScale", parent);
  interval->SetRange(MAXTIME);	/* Set the range 0-3600. */

  /* Set the scale's label */

  XmString label = XmStringCreateLtoR("Refresh Interval In Seconds",
				      XmSTRING_DEFAULT_CHARSET);
  interval->SetAttribute(XmNtitleString, label);
  interval->SetAttribute(XmNorientation, XmHORIZONTAL);
  interval->SetAttribute(XmNprocessingDirection, XmMAX_ON_RIGHT);
  interval->SetAttribute(XmNshowValue, True);
  XmStringFree(label);


  /* Set up the scale to fill the manager.  */

  parent.SetTopAttachment(*interval,    XmATTACH_FORM);
  parent.SetLeftAttachment(*interval,   XmATTACH_FORM);
  parent.SetBottomAttachment(*interval, XmATTACH_FORM);
  parent.SetRightAttachment(*interval,  XmATTACH_FORM);

}

/*
** Functional Description:
**   ApplyDefaultsEverywhere
**     This local function applies the default refresh value to all
**     spectra that are currently displayed.  Since we are setting the
**     update interval, all we do is:
**     1) Cancel any timed update that is in place on the pane.
**     2) Set the new update value.
**     3) If the update value is > 0 then we also 
**        schedule the next update.
** Formal Parameters:
**   int interval:
**      Number of seconds between updates requested (0 if no auto-refresh).
*/
static void ApplyDefaultsEverywhere(int interval)
{
  int rows = Xamine_Panerows();
  int cols = Xamine_Panecols();

  Xamine_CancelUpdateTimers();	/* Kill all updates. */

  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      win_attributed *a = Xamine_GetDisplayAttributes(r,c);
      if(a != NULL) {
	a->update_interval(interval);
	Xamine_ScheduleTimedUpdate(Xamine_GetDrawingArea(r, c),
				   c, r, interval);
      }
    }
  }
}

/*
** Functional Description:
**   SetRefresh:
**     This local function is called whenever either the cancel or the ok
**     button is pressed.  
**  Formal Parameters:
**    XMWidget *button:
**      Widget object which caused us to be called. We need this to determine
**      exactly what happened to us.
**    XtPointer user_d, call_d:
**      Ignored Motif++ callback arguments.
*/
static void SetRefresh(XMWidget *button, XtPointer user_d, XtPointer call_d)
{
  if( (button == dialog->ok()) ||
      (button == dialog->apply())) {	/* If Ok was pressed, update the value */
    win_attributed *at = Xamine_GetDefaultGenericAttributes();
    at->update_interval(dialog->Value());
    Xamine_SaveDefaultProperties();
  }
  if(button == dialog->apply()) {
    ApplyDefaultsEverywhere(dialog->Value());
  }
  /* Regardless, dismiss the widget */

  dialog->XMCustomDialog::UnManage();
}

/*
** Functional Description:
**   Xamine_SetDefaultRefresh:
**     This function is called when the user wants to modify the
**     default refresh interval.  We pop up a RefreshDialog and let them
**     play with the scale widget appropriately.
** Formal Parameters:
**   XMWidget *wid:
**     The widget that invoked us (We use this to parent the dialog)
**   XtPointer ud, cd:
**     User and call data (ignored).
*/
void Xamine_SetDefaultRefresh(XMWidget *wid, XtPointer ud, XtPointer cd)
{
  /*
  ** If necessary, we must create the dialog the first time:
  */
  if(!dialog) {
    dialog = new RefreshDialog("RefreshDefault", wid);
    dialog->AddOkCallback(SetRefresh); /* Add the completion callback. */
    dialog->AddApplyCallback(SetRefresh);
    dialog->AddCancelCallback(SetRefresh);
    dialog->AddHelpCallback(Xamine_display_help, &help);
  }
  /* Set the current slider value to the current default value: */

  win_attributed *dflt = Xamine_GetDefaultGenericAttributes();
  int def = dflt->update_interval();
  if(def < 0) def = 0;
  if(def > MAXTIME) def = MAXTIME;

  dialog->Value(def);

  /* Make the dialog visible */

  dialog->XMCustomDialog::Manage();
}
