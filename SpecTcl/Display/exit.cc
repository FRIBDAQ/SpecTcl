/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**   exit.cc - This function contains the code necessary to exit the program.
**             This includes the function Xamine_confirm_exit which ensures
**             the user really wants to exit.
** Author:
**  Ron Fox
**  NSCL
**  Michigan State University
**  East Lansing, MI 48824-1321
** Version information:
**    @(#)exit.cc	8.1 6/23/95 
*/
static char *sccsinfo="@(#)exit.cc	8.1 6/23/95 ";

#ifdef unix
#pragma implementation "XMCallback.h"
#endif

/*
** Include files:
*/
#include <stdlib.h>
#include <stdio.h>

#include "XMDialogs.h"
#include "panemgr.h"
#include "helpmenu.h"
#include "exit.h"
#include "winiomenu.h"
#include <string>
/*
** External definitions:
*/

extern "C" {
  void exit(int);
}

/*
** Local definitions:
*/

static XMInformationDialog *exit_help_dialog = NULL; /* The help dialog. */
static XMQuestionDialog *dlg = NULL;

static char *msg =
  "If you exit, all changes will be lost.\nAre you sure you want to exit?";


static char *Exit_help[] 
  = { "If you click on the button labelled \"Yes\" Xamine will exit\n",
      "If you click on the button labelled \"No\" Xamine will continue\n\n",
      "If you have graphical objects you have not yet saved, or if you\n"
      "have not yet saved the current window configuration and if you want\n"
      "to save these, then click the \"No\" button and use the \"File\" menu\n",
      "to save these\n",
	NULL
	};

static Boolean windows_changed = False;
static Boolean grobjs_changed  = False;

void do_exit()
{
  exit(0);
}

/*
** Functional Description:
**   exit_display_help:   This function displays the exit help.  The
**                        Exit information dialog is created if needed:
** Formal Parameters:
**   We don't pay any attention to any of them:
*/
void exit_display_help(XMWidget *help_gadget, XtPointer client_data,
		       XtPointer callback_data)
{
  if(!exit_help_dialog)
    exit_help_dialog = Xamine_help("Exit_help", (XMWidget *)dlg, 
				   (char **)Exit_help);
  else
    exit_help_dialog->Manage();
}


/*
** Functional Description:
**   killdlg   - Static local function attached to the cancel button to
**               delete the exit dialog.
** Formal Parameters:
**   XMWidget *w:
**     Pointer to a question dialog which needs to be removed from the display
**   XtPointer cd, cbs:
**     Unused parameters that callbacks get.
*/
static void killdlg(XMWidget *w, XtPointer cd, XtPointer cbs)
{
  XMQuestionDialog *q = (XMQuestionDialog *)w;

  q->UnManage();
}

/*
** Functional Description:
**   Xamine_confirm_exit    - This function is attached as the callback
**                            to the Exit menu entry.  We put up the
**                            "are you sure" Dialog and attach the exit()
**                            function to the OK button.
** Formal Parameters:
**    XMWidget *w:
**      Pointer to the widget of the button which invoked us.
**    XtPointer client_data:
**      Client data (unused)
**    XtPointer cbs:
**      Callback reason structure.
**  Returns:
**    Nothing
**  Side-effects:
**    Creates a question dialog
*/
void Xamine_confirm_exit(XMWidget *w, XtPointer client_data, XtPointer cbs)
{
  Boolean changed = (windows_changed || grobjs_changed);

  if(!changed) do_exit();	/* No confirmation if no changes. */
  if(!dlg) {
    dlg = new XMQuestionDialog("Confirm_exit",
			       *w,
			       msg,
			       (void (*)(XMWidget *,
					 XtPointer,
					 XtPointer))do_exit);
    dlg->DefaultToCancel();
    dlg->AddCancelCallback(killdlg);
    dlg->GetHelpButton()->AddCallback(exit_display_help, (XtPointer)NULL);
    dlg->GetHelpButton()->Enable();
  }
  else {
    dlg->Show();
  }
}

/*
** Functional Description:
**   Xamine_ChangedWindows:
**     This function indicates to the exit procedure that something
**     worth saving has changed in the XAMINE windows.  If Xamine_SavedWindows
**     is not called subsequently, then Exit will force a confirmation prompt
*/
void Xamine_ChangedWindows()
{
  if(!windows_changed) {
    string title("Xamine -- ");
    title += Xamine_GetLastWindowFile();
    title += " [Modified]";
    XMWidget *mgr = Xamine_Getpanemgr();
    SetWindowLabel(mgr->getid(), (char*)title.c_str());
  }
  windows_changed = True;
}
/*
** Functional Description:
**  Xamine_ChangedGrobjs:
**   This function indicates to the exit procedure that something worth 
**   saving has changed in Xamine's graphical object database.
**   If Xamine_SavedGrobjs is not subsequently called, then
**   exit will force a confirmation prompt.
*/
void Xamine_ChangedGrobjs()
{
  if(!grobjs_changed) {
    string title("Xamine -- ");
    title += Xamine_GetLastWindowFile();
    title += " [Modified]";

    XMWidget *mgr = Xamine_Getpanemgr();
    SetWindowLabel(mgr->getid(), (char*)title.c_str());
  }
  grobjs_changed = True;
}
/*
** Functional Description:
**   Xamine_SavedWindows:
**     Indicates that the window state of Xamine has been saved.
*/
void Xamine_SavedWindows()
{
  windows_changed = False;
}
/*
** Functional Description:
**   Xamine_SavedGrobjs:
**     Indicates that the graphical object state of Xamine has been saved.
*/
void Xamine_SavedGrobjs()
{
  grobjs_changed = False;
}
