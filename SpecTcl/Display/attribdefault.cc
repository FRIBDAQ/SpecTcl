/*
** Facility:
**   Xamine : NSCL display program.
** Abstract:
**   attribdefault.cc:
**     This file contains code to prompt for the default spectrum attributes.
**     These attributes include axis orientation, counts axistype and
**     scaling methodology.  The prompting dialog is done via a specialization
**     of the XMCustomDialog widget class.  The work area of the custom dialog
**     will consist of a column of toggle buttons and in some cases 
**     associated type-in text widgets which allow the user to type in
**     numeric values.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files required
*/
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "XMDialogs.h"
#include "XMPushbutton.h"
#include "XMLabel.h"
#include "XMText.h"

#include "helpmenu.h"
#include "dispwind.h"
#include "dfltmgr.h"
#include "optionmenu.h"
#include "panemgr.h"
#include "refreshctl.h"

/*
** Specialized widget class:
*/

class AttributeDialog : public XMCustomDialog, public AttributeForm {
 public:
  AttributeDialog(char *name, XMWidget &parent, char *title) :
    XMCustomDialog(name, parent, title),
    AttributeForm(name, *work_area) {
      Apply->Label("Apply To All");
    }
  ~AttributeDialog() {}

};
/*
** Module static storage
*/
static AttributeDialog *dialog = NULL;	/* Caches pointer to the dialog. */

static char *help_text[] = {
  "   This dialog is prompting you to set the default attributes for spectra\n",
  "default attributes affect some aspects of how spectra placed in panes\n",
  "will look. Default attributes do not affect the appearance of spectra\n",
  "which already are displayed.  These can only be affected by the \n",
  "'Properties' menu entry on the 'Spectra' menu.\n",
  "    The properties you can adjust are:\n\n",
  "      Flip Axes        - When true, X axis is vertical and Y horizontal\n",
  "      Log Scale        - When true, the counts axis is a log scale\n",
  "      Auto Scale       - When true, Xamine chooses the full scale for\n",
  "                         a spectrum after each update.\n",
  "      Full Scale       - If Auto Scale is not selected, then you can set\n",
  "                         the default full scale by typing in this box\n",
  "      Low Level Cutoff - If set enables the low level cutoff\n",
  "      Low Level Value  - The value of the low level cutoff\n",
  "      High Level Cutoff- If set enables the high level cutoff\n",
  "      High Level Value - The value of the high level cutoff\n\n",
  "  Once you have set the properties in the top half of the window, the\n",
  "Buttons in the bottom half of the window have the following meanings:\n\n",
  "      OK               - Updates the defaults and dismisses the dialog.\n",
  "      Cancel           - Dismisses the dialog without changing anything.\n",
  "      Apply To All     - Same as Ok, but also applies defaults to all\n",
  "                         panes containing spectra\n",
  "      Help             - Displays this message\n",
  NULL
  };

static Xamine_help_client_data help = {"Attribute_Help", NULL, help_text};



/*
** Method Description:
**   AttributeForm::AttributeForm
**     This constructor for the special purpose class AttributeForm.
**     The constructor fills in the work_area form.   The form is filled in
**     to look like a single column of options.
** Formal Parameters:
**   char *name:
**     Name of the widget.
**   XMWidget *parent:
**     Parent of the dialog.
*/
AttributeForm::AttributeForm(char *name, XMForm &work_area)
{
  /* Create and layout all the widgets as a top to bottom chain.  */
  /* Note that for the text/label pairs, we chain the left to the right */

  flipped = new XMToggleButton("Flipped", work_area);
  flipped->Label("Flip X and Y Axes");
  work_area.SetTopAttachment(*flipped, XmATTACH_FORM);
  work_area.SetLeftAttachment(*flipped, XmATTACH_FORM);
  work_area.SetRightAttachment(*flipped, XmATTACH_FORM);

  logscale = new XMToggleButton("LogScale", work_area);
  logscale->Label("Logarithmic Counts Scale");
  work_area.SetTopAttachment(*logscale, XmATTACH_WIDGET);
  work_area.SetTopWidget(*logscale, *flipped);
  work_area.SetLeftAttachment(*logscale, XmATTACH_FORM);
  work_area.SetRightAttachment(*logscale, XmATTACH_FORM);

  autoscale = new XMToggleButton("AutoScale", work_area);
  work_area.SetTopAttachment(*autoscale, XmATTACH_WIDGET);
  work_area.SetTopWidget(*autoscale, *logscale);
  work_area.SetLeftAttachment(*autoscale, XmATTACH_FORM);
  work_area.SetRightAttachment(*autoscale, XmATTACH_FORM);

  fullscale = new XMText("FullScale", work_area, 1, 10);
  fullscale_label = new XMLabel("FullScaleL", work_area, "Full Scale Value");
  work_area.SetTopAttachment(*fullscale, XmATTACH_WIDGET);
  work_area.SetTopWidget(*fullscale, *autoscale);
  work_area.SetTopAttachment(*fullscale_label, XmATTACH_WIDGET);
  work_area.SetTopWidget(*fullscale_label, *autoscale);
  work_area.SetLeftAttachment(*fullscale, XmATTACH_FORM);
  work_area.SetLeftAttachment(*fullscale_label, XmATTACH_WIDGET);
  work_area.SetLeftWidget(*fullscale_label, *fullscale);
  work_area.SetRightAttachment(*fullscale_label, XmATTACH_FORM);

  lowlevel_enable = new XMToggleButton("LowLevelEnable", work_area);
  lowlevel_enable->Label("Enable Low Level Cutoff");
  work_area.SetTopAttachment(*lowlevel_enable, XmATTACH_WIDGET);
  work_area.SetTopWidget(*lowlevel_enable, *fullscale);
  work_area.SetLeftAttachment(*lowlevel_enable, XmATTACH_FORM);
  work_area.SetRightAttachment(*lowlevel_enable, XmATTACH_FORM);

  lowlevel = new XMText("LowLevel", work_area, 1,10);
  lowlevel_label = new XMLabel("LowLevelL", work_area, 
			       "Low Level Cutoff Value");
  work_area.SetTopAttachment(*lowlevel, XmATTACH_WIDGET);
  work_area.SetTopWidget(*lowlevel, *lowlevel_enable);
  work_area.SetTopAttachment(*lowlevel_label, XmATTACH_WIDGET);
  work_area.SetTopWidget(*lowlevel_label, *lowlevel_enable);
  work_area.SetLeftAttachment(*lowlevel, XmATTACH_FORM);
  work_area.SetLeftAttachment(*lowlevel_label, XmATTACH_WIDGET);
  work_area.SetLeftWidget(*lowlevel_label, *lowlevel);
  work_area.SetRightAttachment(*lowlevel_label, XmATTACH_FORM);

  hilevel_enable = new XMToggleButton("HiLevelEnable", work_area);
  hilevel_enable->Label("Enable High Level Cutoff");
  work_area.SetTopAttachment(*hilevel_enable, XmATTACH_WIDGET);
  work_area.SetTopWidget(*hilevel_enable, *lowlevel);
  work_area.SetLeftAttachment(*hilevel_enable, XmATTACH_FORM);
  work_area.SetRightAttachment(*hilevel_enable, XmATTACH_FORM);

  hilevel = new XMText("HighLevel", work_area, 1,10);
  hilevel_label = new XMLabel("HighLevelL", work_area,
			      "High Level Cutoff Value");
  work_area.SetTopAttachment(*hilevel, XmATTACH_WIDGET);
  work_area.SetTopWidget(*hilevel, *hilevel_enable);
  work_area.SetTopAttachment(*hilevel_label, XmATTACH_WIDGET);
  work_area.SetTopWidget(*hilevel_label, *hilevel_enable);
  work_area.SetLeftAttachment(*hilevel, XmATTACH_FORM);
  work_area.SetBottomAttachment(*hilevel, XmATTACH_FORM);
  work_area.SetLeftAttachment(*hilevel_label, XmATTACH_WIDGET);
  work_area.SetLeftWidget(*hilevel_label, *hilevel);
  work_area.SetRightAttachment(*hilevel_label, XmATTACH_FORM);
  work_area.SetBottomAttachment(*hilevel_label, XmATTACH_FORM);

  /* Set the behavior callbacks: */

  AddAutoScaleCallback(Xamine_AttribEnableCallback, (XtPointer)this);
  AddLowLevelEnableCallback(Xamine_AttribEnableCallback, (XtPointer)this);
  AddHiLevelEnableCallback(Xamine_AttribEnableCallback, (XtPointer)this);

  /* Set text validation callbacks: */

  AddFullScaleValidationCallback(Xamine_EnsureNumerics, (XtPointer)this);
  AddLowLevelValidationCallback(Xamine_EnsureNumerics,  (XtPointer)this);
  AddHiLevelValidationCallback(Xamine_EnsureNumerics,   (XtPointer)this);

}

/*
** Functional Description:
**   EnsureNumerics:
**     This function is a text modify verify callback which ensures that
**     all characters that are typed in a text widget must be numeric.
**   Formal Parameters:
**      XMText *w:
**        The text widget being modified.
**      XtPointer user_d:
**        user data, not used.
**      XtPointer call_d:
**        actually a pointer to an XmTextVerifyCallbackStruct describing the
**        text modification that's underway.  The modification is checked for
**        an XmCR_MODIFYING_TEXT_VALUE and if it is such an event, if the
**        text pointer is good, then if the text pointer is not all numeric
**        the modification is vetoed.
*/
void Xamine_EnsureNumerics(XMWidget *w, XtPointer user_d, XtPointer call_d)
{
  XmTextVerifyCallbackStruct *cbd = (XmTextVerifyCallbackStruct *)call_d;
  char *text;
  int  length;

  if(cbd->reason != XmCR_MODIFYING_TEXT_VALUE) 
    return;			/* Return if not modifying the text value. */

  if(cbd->text->ptr == NULL)	/* Motif book claims this is deletion */
    return;

  /* Get the particulars of the proposed text. */

  text = cbd->text->ptr;
  length= cbd->text->length;

  /* Assume everything works fine then scan the text and fail if there are */
  /* any illegal characters.                                               */

  cbd->doit = True;
  for(int i = 0; i < length; i++)
    if(!isdigit(text[i])) {
      cbd->doit = False;
      return;
    }
}

/*
** Functional Description:
**   Xamine_AttribEnableCallback:
**     This callback is invoked when one of the toggle buttons with enable
**     or disable function is pressed.   It will call the AdjustGhosting
**     method of the associated form to ensure that the associated entry
**     region sensitivities track the toggle state.
** Formal Parameters:
**     XMWidget *w:
**        The toggle button widget which called us.
**     XtPointer user_d:
**        User data, in this case a pointer to the form widget object that
**        the toggle belongs to.
**     XtPointer call_d:
**        Callback data ignored in this case.
*/
void Xamine_AttribEnableCallback(XMWidget *w, XtPointer user_d, 
					XtPointer call_d)
{
  AttributeForm *dlg = (AttributeForm *)user_d;

  dlg->AdjustGhosting();

}

/*
** Functional Description:
**   ApplyDefaultsEverywhere:
**     This local function applies the default spectrum attributes
**     to all displayed spectra triggering updates on them as we fly by.
** Formal Parameters:
**    win_attributed *default:
**       The default attribute set.
*/
static void ApplyDefaultsEverywhere(win_attributed *def)
{
  int rows = Xamine_Panerows();
  int cols = Xamine_Panecols();

  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      win_attributed *a = Xamine_GetDisplayAttributes(r,c);

      if(a != NULL ) {		/* Set the flip state. */
	a->setflip(def->isflipped());
	a->setlog(def->islog()); /* Set the Log counts state. */

	if(def->manuallyscaled()) 	/* Set scaling state */
	  a->setfs(def->getfs());
	else 
	  a->autoscale();

	if(def->hasfloor()) /* Set floor state/value.  */
	  a->setfloor(def->getfloor());
	else
	  a->nofloor();

	if(def->hasceiling()) /* Set ceiling state/value. */
	  a->setceiling(def->getceiling());
	else
	  a->noceiling();

	Xamine_RedrawPane(c,r);
      }
    }
  }
}

/*
** Functional Description:
**   ActionCallback:
**     This callback is invoked when either the OK or Cancel buttons are
**     selected from the AttributeDialog action area.  If the Ok button was
**     pressed, then we update the default values from the widget state.
**     In either case, we unmanage the dialog before returning causing it
**     to be dismissed but cached for later use.
** Formal Parameters:
**    XMWidget *w:
**      The pushbutton widget which caused us to fire.  We use it to figure
**      out if this is a cancel or an Ok operation.  This is because since this
**      is a custom dialog, we don't get a nice convenient callback structure
**      with a dialog specific reason, but just the callback structure which
**      corresponds to the pushbutton that was pressed.
**    XtPointer user_d:
**      User data, in this case a pointer to the controlling dialog object.
**    XtPointer call_d:
**      Unused callback data.
*/
static void ActionCallback(XMWidget *w, XtPointer user_d, XtPointer call_d)
{
  AttributeDialog *dlg = (AttributeDialog *)user_d;
  XMPushButton    *btn = (XMPushButton *)w;

  win_attributed *dflt = Xamine_GetDefaultGenericAttributes();

  if((btn == dlg->ok()) ||
     (btn == dlg->apply())) {	/* It was an OK operation. */

    dflt->setflip(dlg->Flipped()); /* Flip mode */
    dflt->setlog(dlg->LogScale()); /* Axis scale mode */


    if(dlg->AutoScale()) {	/* Full scale mode and value if manual. */
      dflt->autoscale();
    }
    else {
      char *txt;
      txt = dlg->FullScale();
      dflt->setfs(atoi(txt));
      XtFree(txt);
    }

    if(dlg->LowLevelEnabled()) { /* Low level cutoff and value: */
      char *txt;
      txt = dlg->LowLevel();
      dflt->setfloor(atoi(txt));
      XtFree(txt);
    }
    else
      dflt->nofloor();

    if(dlg->HiLevelEnabled()) {	/* High level cutoff and value */
      char *txt;
      txt = dlg->HiLevel();
      dflt->setceiling(atoi(txt));
      XtFree(txt);
    }
    else
      dflt->noceiling();

    Xamine_SaveDefaultProperties();
  }
  if(btn == dlg->apply()) {
    ApplyDefaultsEverywhere(dflt);
  }

  /* Regardless of why we were called, we unmanage the dialog */

  dlg->UnManage();
}

/*
** Functional Description:
**   Xamine_SetDefaultAttributes:
**     This function prompts for the set of default spectrum display 
**     attributes.  The dialog static variable is used to cache an
**     AttributeDialog object which is recycled from use to use.
**     If this is the first call, the object is instantiated and the
**     callbacks set up.
**     On all subsequent calls, the object is loaded with initial information
**     from the default database and the managed into visibility.
** Formal Parameters:
**   XMWidget *w:
**     The widget which invoked us.  Used as the dialog's parent.
**  All other parameters are ignored but part of the standard Xt callback
**  mechanism.  
*/
void Xamine_SetDefaultAttributes(XMWidget *w, XtPointer user_d, XtPointer cd)
{
  if(dialog == NULL) {		/* Instantiate the dialog and set callbacks */
    dialog = new AttributeDialog("Attribute_Prompt", *w, "Default Attributes");

    /* Handle the buttons in the action area. */

    dialog->AddOkCallback(ActionCallback, dialog);
    dialog->AddApplyCallback(ActionCallback, dialog);
    dialog->AddCancelCallback(ActionCallback, dialog);
    dialog->AddHelpCallback(Xamine_display_help, &help);

  }

  /* Load the data from the default database into the dialog work area */

  win_attributed *dflt = Xamine_GetDefaultGenericAttributes();
  char number[20];

  dialog->Flipped(dflt->isflipped());
  dialog->LogScale(dflt->islog());

  dialog->AutoScale( (dflt->getfs() == -1));
  sprintf(number, "%d", dflt->getfsval());
  dialog->FullScale(number);

  dialog->LowLevelEnabled(dflt->hasfloor());
  sprintf(number, "%d", dflt->getfloor());
  dialog->LowLevel(number);

  dialog->HiLevelEnabled(dflt->hasceiling());
  sprintf(number, "%d", dflt->getceiling());
  dialog->HiLevel(number);

  /* Manage the dialog to make it visible to the user */

  dialog->AdjustGhosting();
  dialog->Manage();
}
