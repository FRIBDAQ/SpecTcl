/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   axisdefault.cc -- This file prompts for and obtains new default axis
**                     properties.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo="@(#)axisdefault.cc	8.1 6/23/95 ";


/*
** Include files required:
*/

#include "XMPushbutton.h"
#include "XMDialogs.h"
#include "helpmenu.h"
#include "dispwind.h"
#include "dfltmgr.h"
#include "optionmenu.h"
#include "panemgr.h"
#include "refreshctl.h"

/*
** Specialized class definitions:
*/

class AxisDialog : public XMCustomDialog, public AxisForm {
 public:
  AxisDialog(char *name, XMWidget &parent, char *title) : 
    XMCustomDialog(name, parent, title),
    AxisForm(name, *work_area)
  {
    Apply->Label("Apply To All");
  }
  ~AxisDialog() 
    { }

};
/*
** Local data
*/
static AxisDialog *dialog = NULL; /* Dialog widget */

static char *(help_text[]) = {
  "  This dialog is prompting you for the default axis labelling\n",
  "attributes.  The attributes you can set are as follows:\n\n",
  "     Show Axes       - Enables and disables the display of axes\n",
  "     Show Tick Marks - Enables and disables the display of tick marks\n",
  "                       on the axes\n",
  "     Show Axis Labels- Enables and disables the display of labels \n",
  "                       on the axis showing channel and count values\n\n",
  "   Using the mouse, click on the toggle buttons in the top\n",
  "half to set up your selections.  Once you are done, click one of the\n",
  "buttons in the lower half.  These buttons function as follows:\n\n",
  "    Ok      - Accept the new axis label configuration described by the\n",
  "              top half of the form and dismiss the dialog.\n",
  "    Apply To All\n",
  "            - Sets the default and applies it to all panes which are\n",
  "              now displaying spectra. Then dismisses the dialog.\n",
  "    Cancel  - Dismiss the dialog without making any changes.\n",
  "    Help    - Pop up this message\n", 
  NULL
  };
static Xamine_help_client_data help = { "Axis_help", NULL, help_text };


/*
** Method Description:
**    AxisForm::AxisDialog:
**       The constructor for the Axis Form. class.  We layout the form
**       which prompts for the axis type.
** Formal Parameters:
**    char *name:
**       Name to be attached to the pop-up DialogShell widget.
**    XMForm &parent:
**       Form widget which contains the axis prompt region..
*/
AxisForm::AxisForm(char *name, XMForm &parent) 
{
  /* The show axes toggle is attached to the form except for the bottom which
  ** floats free
  */

  shwaxes = new XMToggleButton("Show_Axes", parent);
  shwaxes->Label("Show Axes");
  parent.SetTopAttachment(*shwaxes, XmATTACH_FORM);
  parent.SetLeftAttachment(*shwaxes, XmATTACH_FORM);
  parent.SetRightAttachment(*shwaxes, XmATTACH_FORM);
  parent.SetBottomAttachment(*shwaxes, XmATTACH_NONE);

  /*
  ** The show ticks toggle is attached to the form on the left and right sides
  ** and, free on the bottom and attached to the showaxes widget on the top
  */

  shwticks = new XMToggleButton("Show_Ticks", parent);
  shwticks->Label("Show Tick Marks");
  parent.SetTopAttachment(*shwticks, XmATTACH_WIDGET);
  parent.SetTopWidget(*shwticks, *shwaxes);
  parent.SetLeftAttachment(*shwticks, XmATTACH_FORM);
  parent.SetRightAttachment(*shwticks, XmATTACH_FORM);
  parent.SetBottomAttachment(*shwticks, XmATTACH_NONE);

  /* The showlabels toggle is attached at the top to the showticks widget,
  ** Left and right to the form and bottom to the form as well 
  */

  shwlabels = new XMToggleButton("Show_Labels", parent);
  shwlabels->Label("Show Axis Labels");
  parent.SetTopAttachment(*shwlabels, XmATTACH_WIDGET);
  parent.SetTopWidget(*shwlabels, *shwticks);
  parent.SetLeftAttachment(*shwlabels, XmATTACH_FORM);
  parent.SetRightAttachment(*shwlabels, XmATTACH_FORM);
  parent.SetBottomAttachment(*shwlabels, XmATTACH_FORM);

  shwaxes->AddCallback(Xamine_ShowAxisCallback, (XtPointer)this);
}

/*
** Functional Description:
**   ApplyDefaultsEverywhere:
**     This local function applies the axis defaults to all displayed 
**     spectra and triggers an update.
** Formal Parameters:
**    int axes:
**       State of display axes flag.
**    int ticks:
**       State of display ticks flag.
**    int labels:
**       State of display labels flag.
*/
static void ApplyDefaultsEverywhere(int axes, int ticks, int labels)
{
  int rows = Xamine_Panerows();
  int cols = Xamine_Panecols();
 
  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      win_attributed *a = Xamine_GetDisplayAttributes(r,c);
      if(a != NULL) {
	a->set_axes(axes, ticks, labels);
	Xamine_RedrawPane(c,r);
      }
    }
  }
}

/*
** Functional Description:
**    Xamine_ShowAxisCallback:
**      This function is a callback for the showaxis toggle button.
**      The intent is to be sure that ghosting of the other buttons on the
**      work area are appropriately adjusted depending on state changes from
**      that button.
** Formal Parameters:
**   XMWidget *w:
**     The widget initiating the callback, in this case, the 
**     showaxes toggle button.
**   XtPointer user_d:
**     User data, in this case expected to be a pointer to our object.
**     object which controls the toggle.
**   XtPointer call_d:
**     Pointer to callback data, in this case a pointer to an
**     XmToggleButtonCallbackStruct describing the event that caused the
**     callback to be initiated.
*/
void Xamine_ShowAxisCallback(XMWidget *w, XtPointer user_d, XtPointer call_d)
{
  AxisForm     *dlg = (AxisForm *)user_d;

  /*
  ** Action is just to adjust the ghosting, we assume the state has been
  ** changed in the initiating button by now.
  */

  dlg->AdjustEnables();

}

/*
** Functional Description:
**   ActionAreaCallback:
**     This function is intended to field user presses of either the
**     Ok or the Cancel button.  If the cancel button was pressed, then
**     we just un-manage the dialog.  If the Ok Button was pressed, then
**     we update the state of the axis part of the defaults database and
**     then un-manage the dialog.  By using toggles and ghosting, illegal
**     states are made impossible and hence we don't do any error checking.
** Formal Parameters:
**    XMWidget *b:
**      The widget corresponding to the pushbutton that caused the callback.
**    XtPointer user_d:
**      Callback's user data, in this case a pointer to the dialog object
**    XtPointer call_d:
**      Callback's call data, in this case XmPushbuttonCallbackStruct.
*/
static void ActionAreaCallback(XMWidget *b, XtPointer user_d, XtPointer call_d)
{
  XMPushButton *btn = (XMPushButton *)b;
  AxisDialog *dlg = (AxisDialog *)user_d;

  Boolean newaxes          = dlg->ShowAxes();
  Boolean newticks         = dlg->ShowTicks();
  Boolean newlbls          = dlg->ShowLabels();

  /* If the OK button was struck then update the default information */

  if( (btn == dlg->ok()) ||
      (btn == dlg->apply())) {
    win_attributed *defaults = Xamine_GetDefaultGenericAttributes();

    defaults->set_axes(newaxes, newticks, newlbls);
    Xamine_SaveDefaultProperties();
  }
  /* If apply button, the apply the defaults to all spectra. */

  if(btn == dlg->apply()) {
    ApplyDefaultsEverywhere(newaxes, newticks, newlbls);
  }

  /* Regardless, unmanage the dialog:                      */

  dlg->UnManage();

}

/*
** Functional Description:
**    Xamine_SetDefaultAxis:
**      This menu callback function is called when the user wants to update
**      the default axis attributes.  It displays an AxisDialog form which
**      allows the user to fill in the desired attributes.
** Formal Parameters:
**    XMWidget *w:
**      Menu entry which led to our invocation (ignored).
**    XtPointer user_d:
**      user data ignored.
**    XtPointer call:
**      call data ignored.
*/
void Xamine_SetDefaultAxis(XMWidget *w, XtPointer user, XtPointer call)
{
  /*  If the widget has not yet been created, then we must create it */
  /*  and set the callbacks.                                         */
  if(dialog == NULL) {
    dialog = new AxisDialog( "Axis_defaults", *w, "Axis Label Defaults");
    dialog->AddOkCallback(ActionAreaCallback, dialog);
    dialog->AddApplyCallback(ActionAreaCallback,  dialog);
    dialog->AddCancelCallback(ActionAreaCallback, dialog);
    dialog->AddHelpCallback(Xamine_display_help, &help);
  }
  /* Now set the initial state of the toggle buttons and manage the dialog */
  /* widget.                                                               */

  win_attributed *defaults = Xamine_GetDefaultGenericAttributes();

  dialog->ShowAxes(defaults->showaxes());
  dialog->ShowTicks(defaults->showticks());
  dialog->ShowLabels(defaults->labelaxes());

  dialog->Manage();
}
