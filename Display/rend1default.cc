/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**   rend1default.cc  - This file contains code which is responsible for
**                      setting the default rendition of a 1d spectrum.
**                      The prompting is done via a dialog that is a 
**                      specialization of the XMCustomDialog widget class.
**                      The customization puts up a dialog which contains
**                      a radio button box that allows the user to choose
**                      between the appropriate set of renditions.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo="@(#)rend1default.cc	8.1 6/23/95 ";


/*
** Include files required:
*/
#include "XMDialogs.h"
#include "XMPushbutton.h"
#include "XMManagers.h"

#include "optionmenu.h"
#include "dfltmgr.h"
#include "dispwind.h"
#include "helpmenu.h"
#include "refreshctl.h"
#include "panemgr.h" 
/*
** Specialized classes
*/

class Rend1dDialog : public XMCustomDialog, 
                     public Rend1dForm {
  public:
    Rend1dDialog(char *name, XMWidget *parent, char *title) :
      XMCustomDialog(name, *parent, title),
      Rend1dForm(name, *work_area) {
	Apply->Label("Apply To All");
        Rend1dForm::Manage();
      }
    ~Rend1dDialog() { }

    void Manage() {
        XMCustomDialog::Manage(); 
    }

    void UnManage() {
      XMCustomDialog::UnManage();
    }
};
/*
* local data
*/
static Rend1dDialog *dialog  = NULL;

static char *help_text[] = 
{
  "  This dialog is prompting you to select the default rendition for 1-d\n",
  "histograms.  The following 1-d renditions are currently supported by\n",
  "Xamine:\n\n",
  "   Histogram      - A connected bar chart\n",
  "   Lines          - Straight lines connect the tops of the channels\n",
  "   Points         - Each channel is shown as a single point\n\n",
  "  After you have selected the desired default rendition, you should\n",
  "click on one of the buttons in the lower half of the dialog box\n\n",
  "    Ok      - Will accept your rendition selection and dismiss the box\n",
  "    Apply to All \n",
  "            - Same as ok but also applies the default to all panes\n",
  "    Cancel  - Will dismiss the box without making any changes\n",
  "    Help    - Will display this message",
  NULL
  };

static Xamine_help_client_data help = {"Rend1d_default", NULL, help_text};

/*
** Method Description:
**  Rend1dForm::Rend1dForm:
**    Constructor for the Rend1dDialog class.  This method sets up the 
**    work area of the dialog box.  We create a row/column widget and cover
**    up the entire work area with it.  The widget is set up as a radio box
**    and we populate it with one toggle for each of the allowable
**    1-d renditions.  In addition, we unmanage the Apply button making it
**    disappear.
** Formal Parameters:
**    char *name:
**      Name of the dialog widget... this will be applied to the dialog widget
**      shell.
**    XMForm &work_area:
**      Parent widget form object.
*/
Rend1dForm::Rend1dForm(char *name, XMForm &work_area)
{
  /* Create and set up the work area row/column manager: */
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

  Histogram = new XMToggleButton("Histogram", *radio_box);
  Lines     = new XMToggleButton("Lines",     *radio_box);
  Points    = new XMToggleButton("Points",    *radio_box);


}

/*
** Method Description:
**    Rend1dForm::State:
**      Get or set the state of the dialog widget.  This is overloaded to
**      both a setting and a reading function.
** Formal parameters:
**    rendition_1d newstate:
**       new state when setting.
** Returns:
**   rendition_1d  - current state when reading.
*/
rendition_1d Rend1dForm::State()
{
  if(Histogram->GetState()) return histogram;
  if(Lines->GetState()) return lines;
  if(Points->GetState()) return points;

  fprintf(stderr, "Error detected in Rend1dDialog::State(?) -- bad rendition\n");
  fprintf(stderr, "Defaulting to histogram and attempting to continue\n");
  return histogram;

}
void Rend1dForm::State(rendition_1d newstate)
{
  /* radio boxes don't take care of the on-only property when set from the */
  /* back therefore the first thing we do is clear all the buttons:        */

  Histogram->SetState(False);
  Lines->SetState(False);
  Points->SetState(False);


  /* The switch statement below is responsible for setting one of the
  ** toggles 
  */
  switch(newstate) {
  case histogram:
    Histogram->SetState(True);
    break;
  case lines:
    Lines->SetState(True);
    break;
  case points:
    Points->SetState(True);
    break;
  default:
    fprintf(stderr, "Rend1dDialog::State(!), invalid newstate\n");
    fprintf(stderr, "  Defaulting to Histogram and attempting to continue\n");
    Histogram->SetState(True);
    break;
  }
}

/*
** Functional Description:
**   ApplyDefaultsEverywhere:
**     This function applies the default 1-d rendition to all 1-d spectra
**     that are currently displayed.
** Formal Parameters:
**   rendition_1d rend:
**     The new 1-d rendition.
*/
static void ApplyDefaultsEverywhere(rendition_1d rend)
{
  int rows = Xamine_Panerows();
  int cols = Xamine_Panecols();

  for(int r = 0; r < rows; r++) {
    for(int c = 0;c < cols; c++) {
      win_attributed *a = Xamine_GetDisplayAttributes(r,c);
      if(a != NULL) {
	if( a->is1d()) {
	  win_1d *a1 = (win_1d *)a;
	  a1->setrend(rend);
	  Xamine_RedrawPane(c,r);
	}
      }
    }
  }
}

/*
** Functional Description:
**   ActionCallback:
**      This function is called when either the Ok or Cancel button has been
**      pressed on a Rend1dDialog object.  If the Ok button was pressed, the
**      default 1d rendition is updated from the toggle box.  Regardless,
**      the dialog is unmanaged.
** Formal Parameters:
**   XMWidget *w:
**     Pointer to the button object that was pressed.  This is used to tell
**     the difference between OK and CANCEL.  We need to operate that way since
**     this is a custom dialog and therefore we don't get the nice dialog
**     callback structures as the call data.
**   XtPointer user_d:
**     This by convention is a pointer to the dialog object itself.
**   XtPointer call_d:
**     This is the callback data structure which in this case is ignored.
*/
static void ActionCallback(XMWidget *w, XtPointer user_d, XtPointer call_d)
{
  XMPushButton *btn  = (XMPushButton *)w;
  Rend1dDialog *dlg  = (Rend1dDialog *)user_d;

  /*  If Ok was pressed, then we save the current state as the defaults */

  if((btn == dlg->ok()) || (btn == dlg->apply())) {
    Xamine_SetDefault1DRendition(dlg->State());
    Xamine_SaveDefaultProperties();
  }
  if(btn == dlg->apply()) {
    ApplyDefaultsEverywhere(dlg->State());
  }

  /* Now unmanage the dialog to make it invisible */

  dlg->UnManage();
}

/*
** Functional Description:
**   Xamine_Set1dDefaultRendition:
**     This function prompts for the default 1-d spectrum rendition.
**     It does so by instantiating a Rend1dDialog object.
** Formal Parameters:
**   XMWidget *w:
**      Used as the parent widget for the dialog, this is the widget which
**      triggered this callback.
**  All other Motif callback parameters are ignored.
*/
void Xamine_Set1dDefaultRendition(XMWidget *w, 
				  XtPointer user_d, XtPointer call_d)
{

  /* If necessary create an instance of a Rend1dDialog and set up the 
  ** callbacks:
  */
  if(dialog == NULL) {
    dialog = new Rend1dDialog("Rend1d_defaults", w, "1-D Default Renditions");
    dialog->AddOkCallback(ActionCallback, dialog);
    dialog->AddApplyCallback(ActionCallback, dialog);
    dialog->AddCancelCallback(ActionCallback, dialog);
    dialog->AddHelpCallback(Xamine_display_help, &help);
  }
  /* Set the current state of the dialog in the toggles */
  win_1d defaults;

  Xamine_Construct1dDefaultProperties(&defaults);

  dialog->State(defaults.getrend());

  /* Pop up the dialog: */
  dialog->Manage();
}
