/*
** Facility:
**   Xamine - NSCL display Program.
** Abstract:
**   rend2default.cc  - This file contains code which sets the default
**                      rendering of 2-d spectra from user input.
**                      User input is sollicited via a specialized
**                      version of an XMCustomDialog widget.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files:
*/
#include "XMDialogs.h"
#include "XMManagers.h"
#include "XMPushbutton.h"

#include "optionmenu.h"
#include "dfltmgr.h"
#include "dispwind.h"
#include "helpmenu.h"
#include "colormgr.h"
#include "refreshctl.h"
#include "panemgr.h"
/*
** Specialized classes.
*/
class Rend2dDialog : public XMCustomDialog, public Rend2dForm {
 public:
  Rend2dDialog(char *name, XMWidget *parent, char *title) :
    XMCustomDialog(name, *parent, title),
    Rend2dForm(name, *work_area)
      {
	Apply->Label("Apply To All");
        Rend2dForm::Manage();
      }
  ~Rend2dDialog() {}

  void Manage()    { XMCustomDialog::Manage();   }
  void UnManage()  { XMCustomDialog::UnManage(); }
};
/*
** Static local variables:
*/
static Rend2dDialog *dialog = NULL;	/* Dialog handle. */

static char *help_text[] = {
  "  This dialog prompts you for the default 2-d histogram rendition\n",
  "The following renditions are supported:\n\n",
  "     Color    - Each channel is represented by a Color which indicates\n",
  "                the number of counts in the channel \n",
  "     Scatter  - Each channel is represented by a small box.  Pixels are\n",
  "                placed randomly with a density that's proportional to the\n",
  "                number of counts in the channel\n",
  "     Boxes    - Each channel is represented by a small box.  A square is\n",
  "                drawn in the box with a side length proportional to the\n",
  "                number of counts in the channel\n\n",
  "   Select the rendition you prefer from the top part of the dialog. Once\n",
  "you are ready to dismiss the dialog, click on one of the buttons int the\n",
  "bottom part of the box.  The buttons have the following meanings:\n\n",
  "     Ok       - Updates the default rendition and dismisses the dialog\n",
  "     Apply To All\n",
  "              - Same as Ok but also applies the defaults to all panes\n",
  "     Cancel   - Dismisses the dialog without making any changes\n",
  "     Help     - Displays this message and leaves the dialog in place\n",
  NULL
};

static Xamine_help_client_data help = { "Rend2d_help", NULL, help_text };

/*
** Method Description:
**    Rend2dForm::Rend2dForm:
**      This is the constructor method for a form which will prompt
**      for a new default 2-d rendition.  The  work area will be
**      composed of a Radio box with toggles for each of the possible 
**      2d renditions.  
** Formal Parameters:
**   char *name:
**     Name to be associated with the dialog shell widget that parents and
**     manages the dialog.
**   XMForm &work_area:
**     The form which parents the widgets we create.
*/
 Rend2dForm::Rend2dForm(char *name, XMForm &work_area)
{
  /* Create the radio box's manager widget and set it up. */

  radio_box = new XMRowColumn("Radio_Box", work_area);
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

  Color = new XMToggleButton("Color", *radio_box);
  Color->Label("Intensity to Color");

  Scatter = new XMToggleButton("Scatter", *radio_box);
  Scatter->Label("Intensity to Scatter density");

  Boxes = new XMToggleButton("Boxes", *radio_box);
  Boxes->Label("Intensity to Box size");



}

/*
** Method Description:
**   Rend2dForm::State:
**     Gets or sets the state of the toggle buttons. Note that this is really
**     two overloaded methods.
** Formal Parameters:
**   rendition_2d newstate:
**     The new rendition state to represent (for modify only).
** Returns:
**     The current rendition state represented by the toggles
**     (for reading only).
*/
rendition_2d Rend2dForm::State() /* Get state */
{
  if(Color->GetState()) return color;
  if(Scatter->GetState()) return scatter;
  if(Boxes->GetState()) return boxes;

  fprintf(stderr, "Rend2dDialog::State(?) -- Invalid state, defaulting\n");
  fprintf(stderr,"    to Scatter plots and attempting to continue\n");
  return scatter;
}

void Rend2dForm::State(rendition_2d newstate)
{
  /* First clear all toggles: */

  Color->SetState(False);
  Scatter->SetState(False);
  Boxes->SetState(False);

  /* The switch below sets the state appropriately */

  switch(newstate) {
  default:
    fprintf(stderr, "Rend2dDialog::State(!) -- Invalid state, defaulting to\n");
    fprintf(stderr, "   Scatter and attempting to continue\n");
  case scatter:
    Scatter->SetState(True);
    break;
  case color:
    Color->SetState(True);
    break;
  case  boxes:
    Boxes->SetState(True);
    break;
  }
}

/*
** Functional Description:
**   ApplyDefaultsEverywhere:
**     This function applies the default 2-d rendition to all 2-d spectra
**     that are currently displayed.
** Formal Parameters:
**   rendition_2d rend:
**     The new 2-d rendition.
*/
static void ApplyDefaultsEverywhere(rendition_2d rend)
{
  int rows = Xamine_Panerows();
  int cols = Xamine_Panecols();

  for(int r = 0; r < rows; r++) {
    for(int c = 0;c < cols; c++) {
      win_attributed *a = Xamine_GetDisplayAttributes(r,c);
      if(a != NULL) {
	if( !a->is1d()) {
	  win_2d *a2 = (win_2d *)a;
	  a2->setrend(rend);
	  Xamine_RedrawPane(c,r);
	}
      }
    }
  }
}


/*
** Functional Description:
**   ActionCallback:
**     This function is called in response to an action area callback
**     These include presses of the Cancel and the Ok buttons.  Help callbacks
**     are handled by Xamine_display_help(), the default help text display
**     function.
** Formal Parameters:
**    XMWidget *w:
**      The widget object which triggered the callback.  Since were' using
**      custom dialogs, this is the button widget.
**    XtPointer user_d:
**      user data, in this case a pointer to the dialog object.
**    XtPointer call_d:
**      XmPushbuttonCallbackStruct * to callback reason data.
*/
static void ActionCallback(XMWidget *w, XtPointer user_d, XtPointer call_d)
{
  XMPushButton  *b  = (XMPushButton *)w;
  Rend2dDialog  *d  = (Rend2dDialog *)user_d;

  /* If the Ok button was pressed the update the default rendition: */

  if( (b == d->ok()) || (b == d->apply())) {
    Xamine_SetDefault2DRendition(d->State());
    Xamine_SaveDefaultProperties();
  }
  if(b == d->apply()) {
    ApplyDefaultsEverywhere(d->State());
  }

  /* Make the dialog disappear: */

  d->UnManage();
}

/*
** Functional Description:
**   Xamine_Set2dDefaultRendition:
**     Prompts for the default rendition for 2-d spectra.
** Formal Parameters:
**    XMWidget *w:
**      Widget invoking us which will be used as the parent for the
**      dialog widget.
**   Remaining arguments are ignored.
*/
void Xamine_Set2dDefaultRendition(XMWidget *w, XtPointer p, XtPointer q)
{
  if(dialog == NULL) {		/* Need to instantiate dialog.  */
    dialog = new Rend2dDialog("Rend_2d", w, "Select 2d default rendition");

    dialog->AddOkCallback(ActionCallback, dialog);
    dialog->AddApplyCallback(ActionCallback, dialog);
    dialog->AddCancelCallback(ActionCallback, dialog);
    dialog->AddHelpCallback(Xamine_display_help, &help);
  }
  /* Now that the dialog exists, just set the state and manage it: */

  win_2d props;

  Xamine_Construct2dDefaultProperties(&props);

  dialog->State(props.getrend());
  dialog->SetColor(Xamine_ColorDisplay());
  dialog->Manage();
}
