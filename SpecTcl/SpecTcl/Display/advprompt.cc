/*
** Facility:
**  Xamine:    NSCL display program.
** Abstract:
**   advprompt.cc - This file contains code to prompt for and set the
**                  advance value for the Display Spectrum + function.
**                  We require that the value be non zero.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo="@(#)advprompt.cc	8.1 6/23/95 \n";



/*
** Include files:
*/
#include <stdlib.h>

#include "XMDialogs.h"

#include "helpmenu.h"
#include "advprompt.h"
#include "spcdisplay.h"
#include "errormsg.h"
/*
** Below is the help definition for the dialog
*/

static char *help_text[] = {
  "   This dialog allows you to set the selection advance value.  This is\n",
  "number of panes by which the selection will advance on completing a\n",
  "successful `Display +'.  Type in a numeric value other than zero\n",
  "Positive values imply an advance that is to the right and down while\n",
  "negative values imply an advance that is to the left and up\n",
  "If you wanted to put in a zero value, consider just using the `Display'\n",
  "menu entry\n\n",
  "    The dialog buttons have the following meanings:\n\n",
  "   Ok      - Accept the value (Carriage return does the same thing)\n",
  "   Cancel  - Don't change the advance value\n",
  "   Help    - Display this help text\n",
  NULL
  };
static Xamine_help_client_data help =  {"Advance_help", NULL, help_text};

/*
** This is the prompt widget pointer:
*/

static XMPromptDialog *dialog = NULL;

/*
** Functional Description:
**   callback_handler:
**     This function called for the Ok or Apply (Not possible) cases.
**     We decode the value and set it.
**     If the value is zero, then we popup an error dialog letting them
**     know this is a problem.
** Formal Parameters:
**    XMWidget *w:
**      The invoking widget (the dialog).
**    XtPointer clidata:
**      The client data (unused).
**    XtPointer caldata:
**      The callback data.  This is used to fetch the value.
*/
void callback_handler(XMWidget *w, XtPointer clidata, XtPointer caldata)
{
  XmSelectionBoxCallbackStruct *why = (XmSelectionBoxCallbackStruct *)caldata;
  XMPromptDialog *dialog = (XMPromptDialog *)w;

  char *value_txt;
  int  value;

  switch(why->reason) {
  case XmCR_OK:
  case XmCR_APPLY:
    if(!XmStringGetLtoR(why->value, XmSTRING_DEFAULT_CHARSET, &value_txt)) {
      Xamine_error_msg(w,
		       "Unable to extract result string");
      return;
    }
    value = atoi(value_txt);
    if(value == 0) {
      dialog->Show();
      Xamine_error_msg(w,
		       "Please enter a number other than zero");
      return;
    }
    Xamine_SetAdvance(value);
    return;
  default:
      Xamine_error_msg(w,
	       "callback_handler - advance bug detected\nbad callback reason");
      return;
  }
}

/*
** Functional Description:
**   Xamine_PromptAdvance:
**     This function prompts for the advance value.  I've typed in to the
**     help text above what the advance value does so please refer to that
**     for more information.  From the Motif point of view, we put up a 
**     Prompt dialog and accept values which translate to nonzero numbers.
**  Formal Parameters:
**     XMWidget *w:
**       The pointer to a widget object which wound up invoking us, used as
**       the parent when creating the dialog the first time.
**     XtPointer client_data:
**       Client data passed in by the caller when the callback was added,
**       This field is ignored.
**     XtPointer callback_data:
**       Motif call back data structure associated with whoever invoked us,
**       also ignored.
*/

void Xamine_PromptAdvance(XMWidget *w, XtPointer client_data, 
			  XtPointer callback_data)
{
  char curvalue[20];
  
  /* If necessary, then create the dialog: */

  if(dialog == NULL) {
    dialog = new XMPromptDialog("Advance", *w, "Enter Advance Value:",
				callback_handler);
    dialog->GetHelpButton()->Enable();
    dialog->GetHelpButton()->AddCallback(Xamine_display_help, (XtPointer)&help);
    dialog->GetApplyButton()->UnManage();
    dialog->DefaultToOk();
  }

  /* Set up the instance specifics of the dialog */

  sprintf(curvalue, "%d", Xamine_GetAdvance());
  dialog->SetText(curvalue);
  dialog->Show();

}
