/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**   titledefault.cc  - This file contains code that prompts for and obtains
**                      the default title labelling for spectra.  This is
**                      accomplished by creating a specialized dialog class
**                      from the generic XMCustomDialog class.  The dialog
**                      consists of a vertical row of toggles in the
**                      work area each one representing a title attribute
**                      which may or may not be displayed.
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
#include "XMPushbutton.h"
#include "dispwind.h"
#include "dfltmgr.h"
#include "helpmenu.h"
#include "optionmenu.h"
#include "panemgr.h"
#include "refreshctl.h"

/*
** Specialized class:
*/
class TitleDialog : public XMCustomDialog, public TitleForm
{
 public:
  TitleDialog(char *name, XMWidget &parent, char *title) :
    XMCustomDialog(name, parent, title),
    TitleForm(name, *work_area) {
      Apply->Label("Apply To All");
    }
  ~TitleDialog() {}
  void Manage() { TitleForm::Manage();
                  XMCustomDialog::Manage();
                }
  void UnManage() { TitleForm::UnManage();
                    XMCustomDialog::UnManage();
                  }
     
};
/*
** Local static data:
*/
static TitleDialog *dialog = NULL; /* Pointer to the dialog. */

static char *help_text[] = {
  "This dialog is prompting you for new settings which describe how much\n",
  "title information to place on a spectrum by default.  The upper half\n",
  "of the box allows you to enable or disable the display of:\n\n",
  "      Show Spectrum Name       - The title of the spectrum.\n",
  "      Show Spectrum Number     - The number of the spectrum\n",
  "      Show Spectrum Description- The description of the spectrum e.g.\n",
  "                                 number of channels, dimensions and \n",
  "                                 bytes per channel\n",
  "      Show Maximum Peak Info   - The height and location of the highest \n",
  "                                 channel excluding the endpoints.\n",
  "      Show Last Update time    - The date and time the plot was last\n",
  "                                 updated.\n",
  "      Show Object Labels       - Labels on the graphical objects\n\n",
  "   After you use the mouse to select the titling options you want, the\n",
  "three buttons in the lower half of the box control what is done with\n",
  "these selections as follows:\n\n",
  "     Ok        - Updates the defaults from the settings you have selected\n",
  "     Apply To All \n",
  "               - Same as OK but also applies the defaults to all panes\n",
  "     Cancel    - Leaves the defaults unchanged\n",
  "     Help      - Displays this message\n",
  "\nNOTE:\n",
  "     Changing these settings does not affect any spectra that are already\n",
  "displayed.",
  NULL
  };

static Xamine_help_client_data help = { "Title_help", NULL, help_text };


/*
** Method Description:
**    TitleForm::TitleForm:
**       This method constructs the title default prompt in a form.
**       The work area form is a vertical column of toggle button widgets
**       with labels describing the options that they control.
*/
TitleForm::TitleForm(char *name, XMForm &work_area)
{
  /*  First instantiate the toggle buttons in the work_area and bind them */
  /*  to positions in the form.  All buttons are bound to the left and right */
  /*  sides of the form.  The top binding forms a chain with the top button */
  /*  bound to the form and all other buttons bound to the widget above.    */
  /*  The bottom button is also bound at the bottom to the form bottom     */

  showname = new XMToggleButton("Showname", work_area);
  showname->Label("Show Spectrum Title");
  work_area.SetTopAttachment(*showname, XmATTACH_FORM);
  work_area.SetLeftAttachment(*showname, XmATTACH_FORM);
  work_area.SetRightAttachment(*showname, XmATTACH_FORM);

  shownumber = new XMToggleButton("Shownumber", work_area);
  shownumber->Label("Show Spectrum Number");
  work_area.SetTopAttachment(*shownumber, XmATTACH_WIDGET);
  work_area.SetTopWidget(*shownumber, *showname);
  work_area.SetLeftAttachment(*shownumber, XmATTACH_FORM);
  work_area.SetRightAttachment(*shownumber, XmATTACH_FORM);

  showdesc = new XMToggleButton("Showdesc", work_area);
  showdesc->Label("Show Spectrum Description");
  work_area.SetTopAttachment(*showdesc, XmATTACH_WIDGET);
  work_area.SetTopWidget(*showdesc, *shownumber);
  work_area.SetLeftAttachment(*showdesc, XmATTACH_FORM);
  work_area.SetRightAttachment(*showdesc, XmATTACH_FORM);

  showmaxpeak = new XMToggleButton("Showmaxpeak", work_area);
  showmaxpeak->Label("Show Maximum Peak Info.");
  work_area.SetTopAttachment(*showmaxpeak, XmATTACH_WIDGET);
  work_area.SetTopWidget(*showmaxpeak, *showdesc);
  work_area.SetLeftAttachment(*showmaxpeak, XmATTACH_FORM);
  work_area.SetRightAttachment(*showmaxpeak, XmATTACH_FORM);

  showupdate = new XMToggleButton("Showupdate", work_area);
  showupdate->Label("Show Last Update Time");
  work_area.SetTopAttachment(*showupdate, XmATTACH_WIDGET);
  work_area.SetTopWidget(*showupdate, *showmaxpeak);
  work_area.SetLeftAttachment(*showupdate, XmATTACH_FORM);
  work_area.SetRightAttachment(*showupdate, XmATTACH_FORM);

  showobjects = new XMToggleButton("Showobjects", work_area);
  showobjects->Label("Show Graphical Object Labels");
  work_area.SetTopAttachment(*showobjects, XmATTACH_WIDGET);
  work_area.SetTopWidget(*showobjects, *showupdate);
  work_area.SetLeftAttachment(*showobjects, XmATTACH_FORM);
  work_area.SetRightAttachment(*showobjects, XmATTACH_FORM);
     /* Remove the next line if adding more toggles to the form. */
  work_area.SetBottomAttachment(*showobjects, XmATTACH_FORM);
  

}

/*
** Functional Description:
**   ApplyDefaultsEverywhere:
**     This local function applies the title defaults to all visible spectra.
**     An update is triggered for each visible pane.
** Formal Parameters:
**   int name:
**     New state of name flag.
**   int spnum:
**     New State of Spectrum number flag.
**   int desc:
**     New state of description flag.
**   int peak:
**     New state of peak flag.
**   int upd:
**     New State of show time flag.
**   int objs:
**     New state of object lables on flag.
*/
static void ApplyDefaultsEverywhere(int name, int spnum, int desc, int peak,
				    int upd, int objs)
{
  int rows = Xamine_Panerows();
  int cols = Xamine_Panecols();

  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      win_attributed *a = Xamine_GetDisplayAttributes(r,c);
      if(a != NULL) {
	a->set_titles(name, spnum, desc, peak, upd, objs);
	Xamine_RedrawPane(c,r);
      }
    }
  }
}

/*
** Functional Description:
**   ActionCallback:
**     This function is attached to the callbacks for both the
**     Ok and Cancel Buttons.  The action on the Ok Button is to update
**     the defaults from the values of the toggles.  After either Ok or
**     Cancel, the dialog is unmanaged and cached for later use.
** Formal Parameters:
**    XMWidget *w:
**      The XMPushButton which was pressed to invoke us.  Since this is a
**      custom dialog, unfortunately this is not the dialog widget itself.
**    XtPointer user_d:
**      User data.  In this case, by convention it is a pointer to the
**      Dialog widget.
**    XtPointer client_d:
**      Since this is a custom dialog widget, the data passed is just a simple
**      XmPushButtonCallbackStruct data structure which is ignored.
*/
static void ActionCallback(XMWidget *w, XtPointer user_d, XtPointer client_d)
{
  TitleDialog *dlg = (TitleDialog *)user_d;
  XMPushButton *pb = (XMPushButton *)w;

  if((pb == dlg->ok()) || (pb == dlg->apply())) {     /* Ok or apply */
    win_attributed *defaults = Xamine_GetDefaultGenericAttributes();
    
    defaults->set_titles(dlg->ShowName(),
			dlg->ShowNumber(),
			dlg->ShowDesc(),
			dlg->ShowMaxPeak(),
			dlg->ShowUpdate(),
			dlg->ShowObjects()
			);
    Xamine_SaveDefaultProperties();
    
  }
  if(pb == dlg->apply()) {
    ApplyDefaultsEverywhere(dlg->ShowName(),
			    dlg->ShowNumber(),
			    dlg->ShowDesc(),
			    dlg->ShowMaxPeak(),
			    dlg->ShowUpdate(),
			    dlg->ShowObjects());
  }

  dlg->UnManage();
}

/*
** Functional Description:
**     Xamine_SetDefaultTitling:
**        This function prompts for a new set of title defaults.
**        The first invocation creates the dialog widget and install the
**        callbacks.  Subsequent invocations just manage the cached instantiated
**        widget.
** Formal Parameters:
**     All Ignored.
*/
void Xamine_SetDefaultTitling(XMWidget *w, XtPointer user, XtPointer call)
{
  if(dialog == NULL) {
    dialog = new TitleDialog("TitleDefaults", *w, "Title Defaults");
    dialog->AddOkCallback(ActionCallback, dialog);
    dialog->AddApplyCallback(ActionCallback, dialog);
    dialog->AddCancelCallback(ActionCallback, dialog);
    dialog->AddHelpCallback(Xamine_display_help, &help);
  }

  /* Grab the settings from the defaults and set the toggle buttons */

  win_attributed *dflts  = Xamine_GetDefaultGenericAttributes();

  dialog->ShowName(dflts->showname());
  dialog->ShowNumber(dflts->shownum());
  dialog->ShowDesc(dflts->showdescrip());
  dialog->ShowMaxPeak(dflts->showpeak());
  dialog->ShowUpdate(dflts->showupdt());
  dialog->ShowObjects(dflts->showlbl());

  dialog->Manage();
}
