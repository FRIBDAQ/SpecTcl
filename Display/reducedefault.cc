/*
** Facility:
**  Xamine -- NSCL Display program.
** Abstract:
**   reducedefault.cc -- This file contains code to prompt for and accept
**                       the default display reduction method.  The display
**                       reduction method determines how many channels get
**                       mapped to few pixels of display real-estate.
**                       The prompt is a specialized version of the
**                       Custom Dialog class.  The work area form is filled
**                       with a row column widget that implements a radio
**                       box containing buttons for each of the possible
**                       options.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo="@(#)reducedefault.cc	8.2 7/10/95 ";


/*
** Include files required
*/
#include <stdio.h>

#include "XMManagers.h"
#include "XMDialogs.h"
#include "XMPushbutton.h"

#include "dispwind.h"
#include "optionmenu.h"
#include "dfltmgr.h"
#include "dispwind.h"
#include "helpmenu.h"
#include "panemgr.h"
#include "refreshctl.h"
/*
** Specialized classes
*/

class ReduceDialog : public XMCustomDialog, public ReduceForm {
 public:
  ReduceDialog(char *name, XMWidget *parent, char *title) :
    XMCustomDialog(name, *parent, title),
    ReduceForm(name, *work_area) {
      Apply->Label("Apply To All");
      ReduceForm::Manage();
    }
  ~ReduceDialog() {}

  void Manage() {
    XMCustomDialog::Manage();
    }
  void UnManage() {
    XMCustomDialog::UnManage();
  }
};

/*
** Module static data
*/
static ReduceDialog *dialog  = NULL;
static char *help_text[] = {
  "   This dialog is prompting for the default spectrum reduction mechanism.\n",
  "The spectrum reduction mechanism determines how to render spectra when\n",
  "there are insufficient screen pixels in a pane to display all channels\n",
  "in the display region.  The following reduction mechanisms are defined\n\n",
  "      Sampled    - Each displayed channel represents a region of spectrum\n",
  "                   channels.  The displayed channel is the middle of the\n",
  "                   region\n",
  "      Averaged   - As for Sampled, but the displayed channel is the \n",
  "                   average of the region\n",
  "      Summed     - As for Sampled except the displayed channel is the sum\n",
  "                   over the region\n\n",
  "   After you have chosen the desired default spectrum reduction mechanism,\n",
  "click on one of the buttons in the bottom region of the dialog.\n",
  "These buttons have the following meanings:\n\n",
  "         Ok       - Accepts the changed sample mechanism, Dismisses the\n",
  "                    dialog. \n",
  "         Apply to All\n",
  "                  - Same as Ok, but also applies the default to all panes\n",
  "         Cancel   - Dismisses the dialog without making any changes\n",
  "         Help     - Displays this message\n",
  NULL };

static Xamine_help_client_data help = { "Reduce_help", NULL, help_text };

/*
** Method description:
**   ReduceForm::ReduceForm:
**     Constructs the form that prompts for the reduction mode.  The
**     work area is filled in with a radio box.  This is done by filling
**     The work area form with a row/column manager which implements the
**     radio behavior of the toggles that we put in.  The toggle buttons
**     we add are labelled:
**        Reduce by Sampling  Indicating sampling reduction.
**        Reduce by Averaging Indicating Averaged reduction.
**        Reduce by Summing   Indicating Summed sampling.
** Formal Parameters:
**    char *name:
**      Name of the dialog shell at the top of the dialog widget tree.
**    XMForm &work_area:
**      Form that we're filling in..
**    char *title:
*/
ReduceForm::ReduceForm(char *name, XMForm &work_area)
{
  /* Create the row column widget and set radio box behavior: */

  radio_box = new XMRowColumn("Reduce_radio", work_area);
  radio_box->RadioMenu();
  radio_box->RadioForceOne();
  radio_box->SetOrientation(XmVERTICAL);
  radio_box->SetPacking(XmPACK_COLUMN);
  radio_box->SetRowColumns(1);

  /* Set the radio box extent to cover the entire work area: */

  work_area.SetTopAttachment(*radio_box, XmATTACH_FORM);
  work_area.SetLeftAttachment(*radio_box, XmATTACH_FORM);
  work_area.SetRightAttachment(*radio_box, XmATTACH_FORM);
  work_area.SetBottomAttachment(*radio_box, XmATTACH_FORM);

  /*  Next populate the radio box with toggle buttons: */

  Sampled = new XMToggleButton("Sample", *radio_box);
  Sampled->Label("Reduce by Sampling");

  Summed = new XMToggleButton("Sum", *radio_box);
  Summed->Label("Reduce by Summing");
  
  Averaged = new XMToggleButton("Averaged", *radio_box);
  Averaged->Label("Reduce by Averaging");

}

/*
** Method Description:
**    ReduceForm::State:
**      This method is overloaded to set or get the state of the radio box.
** Formal Parameters:
**   reduction_mode mode:
**      The new mode to set for the set function.
*/
reduction_mode ReduceForm::State() /* Get the state. */
{
  /* Find out which button is set and return the corresponding state: */

  if(Sampled->GetState()) return sampled;
  if(Summed->GetState()) return summed;
  if(Averaged->GetState()) return averaged;

  fprintf(stderr, "BUG in ReduceDialog::State(?)... invalid reduction state\n");
  fprintf(stderr, "Returning 'sampled' and trying to continue\n");
  return sampled;
}
void ReduceForm::State(reduction_mode newmode)
{
  /* It seems that the radio boxes don't enfoce radio behavior when the */
  /* buttons are set from the 'back'.  Therefore clear all buttons:     */

  Sampled->SetState(False);
  Summed->SetState(False);
  Averaged->SetState(False);

  /* Then figure out who to set   */


  switch(newmode) {
  case sampled:
    Sampled->SetState(True);
    break;
  case summed:
    Summed->SetState(True);
    break;
  case averaged:
    Averaged->SetState(True);
    break;
  default:
    fprintf(stderr, "Invalid mode passed to ReduceDialog::State(!)\n");
    fprintf(stderr,"Assuming 'sampled' and trying to continue\n");
    Sampled->SetState(True);
    break;
  }
}

/*
** Functional Description:
**   ApplyDefaultsEverywhere:
**     This function applies the default reduction to all displayed spectra.
**     Each displayed pane is also refreshed so that the new defaults become
**     visible.
** Formal Parameters:
**    reduction_mode: reduce:
**     The new reduction mode.
*/
static void ApplyDefaultsEverywhere(reduction_mode reduce)
{
  int rows = Xamine_Panerows();
  int cols = Xamine_Panecols();

  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      win_attributed *a = Xamine_GetDisplayAttributes(r,c);
      if(a != NULL ) {
	a->setreduce(reduce);
	Xamine_RedrawPane(c,r);
      }
    }
  }
}

/*
** Functional Descrpition:
**    ActionCallback:
**      This function is called in response to the Ok Or Cancel button
**      (Help is mapped to Xamine_display_help).  If Ok was pressed,
**      then the defaults are updated.  Regardless, the dialog is UnManaged
**      making it invisible.
** Formal Parameters:
**    XMWidget *w:
**      The widget belonging to the button that was pressed.  Since this is
**      a custom dialog we don't get the overal dialog widget.
**    XtPointer user_d:
**      user data:  By convention points to the dialog object.
**    XtPointer call_d:
**       Callback data which is ignored in this case.
*/
static void ActionCallback(XMWidget *w, XtPointer user_d, XtPointer call_d)
{
  XMPushButton *btn = (XMPushButton *)w;
  ReduceDialog *dlg = (ReduceDialog *)user_d;

  /*
  **  If the button widget is the Ok widget, then we must update the defaults
  **  from the toggles.
  */
  if( (btn == dlg->ok()) || (btn == dlg->apply())) {
    win_attributed *dflts = Xamine_GetDefaultGenericAttributes();
    dflts->setreduce(dlg->State());
    Xamine_SaveDefaultProperties();

  }
  if(btn == dlg->apply()) {
    ApplyDefaultsEverywhere(dlg->State());
  }
  /* Regardless of the which button was pressed, we need to 
  ** UnManage the widget:
  */
  dlg->UnManage();
}

/*
** Functional Description:
**   Xamine_SetDefaultReduction:
**     This function is called when the Spectrum Reductions... menu selection
**     is picked.  A ReduceDialog is instantiated if necessary and displayed.
** Formal Parameters:
**   XMWidget *w:
**     The widget that woke us up.
**   XtPointer user_d:
**     Unused user callback data.
**   XtPointer call_d:
**     Unused Xt callback data.
*/
void Xamine_SetDefaultReduction(XMWidget *w, 
				XtPointer user_d, XtPointer call_d)
{
  win_attributed *dflt = Xamine_GetDefaultGenericAttributes();

  if(dialog == NULL) {		/* Instantiate the dialog and set callbacks */

    dialog = new ReduceDialog("Reduce_Prompt", w, "Reduction Method");
    dialog->AddOkCallback(ActionCallback, dialog);
    dialog->AddApplyCallback(ActionCallback, dialog);
    dialog->AddCancelCallback(ActionCallback, dialog);
    dialog->AddHelpCallback(Xamine_display_help, &help);

  }
  /* Set the dialog state to reflect the current default reduction: */

  dialog->State(dflt->getreduction());
  dialog->Manage();

}


