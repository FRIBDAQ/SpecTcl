/*
** Facility:
**   Xamine NSCL display program user interface.
** Abstract:
**   geometrymenu.cc  -- This file contains code to handle the window/geometry
**                       menu entry.  This includes functions to
**                           create/manage a prompting dialog which asks for
**                             the window dimensions.
**                           code to handle the various action buttons
**                             including Accept, Apply, Cancel and Help.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**  @(#)geometrymenu.cc	8.1 6/23/95 
*/
#if 0
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
#endif


/*
** Include files:
*/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "XMLabel.h"
#include "XMDialogs.h"
#include "XMPushbutton.h"
#include "XMManagers.h"
#include "XMSeparators.h"

#include "helpmenu.h"
#include "errormsg.h"
#include "panemgr.h"
#include "menusetup.h"
#include "exit.h"
/*
** Constant definitions:
*/

/* Set limits on window shapes: */

#define MAX_HORIZONTAL   10
#define MAX_VERTICAL     10
#define MAX_REPLY        100 /* Maximum characters in reply */
/*
** Class definitions
*/
class XMPromptGeometry : public XMCustomDialog {
 protected:
  XMLabel *rowlabel, *collabel;
  XMRowColumn  *row_manager, *column_manager;
  XMVerticalSeparator *bar;
  XMToggleButton *rows[10], *columns[10];
 public:
  /*  Constructors and destructors: */

  XMPromptGeometry(char *name, XMWidget *parent, char *title);
  ~XMPromptGeometry() {
    delete rowlabel;
    delete collabel;
    delete row_manager;
    delete column_manager;
    delete bar;
    for(int i = 0;i < 9; i++) {
      delete rows[i];
      delete columns[i];
    }
  }


  /* Manipulators:  */

  void Rows(int r);		/* Set number of rows. */
  int  Rows();			/* Return number of rows. */

  void Columns(int c);		/* Set number of columns */
  int  Columns();		/* Return number of columns. */

  void Manage() {		/* Make visible */
    row_manager->Manage();
    column_manager->Manage();
    XMCustomDialog::Manage();
  }
  void UnManage() {		/* Make invisible. */
    row_manager->UnManage();
    column_manager->UnManage();
    XMCustomDialog::UnManage();
  }
};


/*
** Local definitions:
*/
static char *geometry_help_text[] = {
  "  This dialog is prompting you to enter the number of horizontal and\n",
  "vertical boxes that will comprise the display.  Once this geometry is\n",
  "accepted, you will be able to display one or more spectra into each box.\n",
  "\n   To fill in the dialog you should set the desired number of rows and\n",
  "columns in the upper half of the dialog box.  Then choose either Ok or\n",
  "Apply.\n",
  "\n\nSummary of button meanings:\n\n",
  "     Ok       - The display is subdivided and the dialog vanishes\n",
  "     Apply    - The display is subdivided and the dialog remains available\n",
  "     Cancel   - The dialog vanishes and no action is taken\n",
  "     Help     - This help text is displayed\n",
  NULL};


static XMPromptGeometry *geometry_prompt = NULL;
static Xamine_help_client_data geometry_help = { "Geometry_help",
						   NULL,
						   geometry_help_text };

/*
** Method Description:
**  XMPromptGeometry::XMPromptGeometry:
**    This method instantiates a XMPromptGeometry object.  This kind of object
**    prompts the user for the window geometry.  It is derived from
**    XMCustomDialog.  The work area consists of two Radio boxes which
**    are oriented vertically.  The Radio boxes are labelled Rows and Columns
**    and are separated by a vertical separator.  Each radio box is filled with
**    10 toggles representing the number of rows or columns requested.
** Parameters:
**   char *name:
**    Name of the dialog widget shell.
**   XMWidget *parent:
**    Parent of the dialog subtree.
**   char *title:
**    Title of the widget.
*/
XMPromptGeometry::XMPromptGeometry(char *name, XMWidget *parent, char *title) :
       XMCustomDialog(name, *parent, title)
{
  Arg chars[10];		/* Characteristics. */

  /* Make the row label: */

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(chars[1], XmNtopAttachment, XmATTACH_FORM);
  rowlabel = new XMLabel("Rows", *work_area, "Rows", chars, 2);

  /* Add the Row manager row/column widget: */

  XtSetArg(chars[1], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[2], XmNtopWidget,     rowlabel->getid());
  XtSetArg(chars[3], XmNbottomAttachment, XmATTACH_FORM);
  XtSetArg(chars[4], XmNorientation,   XmVERTICAL);
  XtSetArg(chars[5], XmNradioAlwaysOne, True);
  XtSetArg(chars[6], XmNradioBehavior,  True);
  row_manager = new XMRowColumn("Row_manager", *work_area, chars, 7);

  /* Put toggles in the row manager: */

  for(int r = 0; r < 10; r++) {
    char label[10];
    sprintf(label, "%d", r+1); /* Label of toggle. */
    rows[r] = new XMToggleButton(label, *row_manager);
  }

  /* Put in the vertical separator: */

  XtSetArg(chars[0], XmNleftAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[1], XmNleftWidget,     row_manager->getid());
  XtSetArg(chars[2], XmNtopAttachment,  XmATTACH_FORM);
  XtSetArg(chars[3], XmNbottomAttachment, XmATTACH_FORM);
  bar = new XMVerticalSeparator("VBAR", *work_area, chars, 4);

  /* Put in the column label: */

  XtSetArg(chars[1], XmNleftWidget,   bar->getid());
  XtSetArg(chars[3], XmNrightAttachment, XmATTACH_FORM);
  collabel = new XMLabel("Columns", *work_area, "Columns", chars, 4);

  /* Put in the column manager: */

  XtSetArg(chars[2], XmNtopAttachment, XmATTACH_WIDGET);
  XtSetArg(chars[4], XmNbottomAttachment, XmATTACH_FORM);
  XtSetArg(chars[5], XmNorientation, XmVERTICAL);
  XtSetArg(chars[6], XmNradioAlwaysOne, True);
  XtSetArg(chars[7], XmNradioBehavior, True);
  XtSetArg(chars[8], XmNtopWidget,     collabel->getid());
  column_manager = new XMRowColumn("Column_Manager", *work_area, chars, 9);

  /* fill in the column toggles: */

  for(int cols = 0; cols < 10; cols++) {
    char label[10];
    sprintf(label, "%d", cols+1);
    columns[cols] = new XMToggleButton(label, *column_manager);
  }

}

/*
** Method Description:
**    XMPromptGeometry::Rows:
**      These overloaded methods allow the client to query or set the
**      state of the row toggles.
** Formal Parameters:
**   int nrows:
**      Number of rows to set in the toggles (for settings method).
** Returns:
**    Current state of the toggles (for readings method)
*/
int XMPromptGeometry::Rows()
{
  for(int i = 0; i < 10; i++) {
    if(rows[i]->GetState())  return i + 1;
  }
  fprintf(stderr,"XMPromptGeometry::Rows(?) -- none set, returning 1\n");
  return 1;
}
void XMPromptGeometry::Rows(int nrows)
{
  for(int i = 0; i < 10; i++)  {
    if((i+1) == nrows) 
      rows[i]->SetState(True);
    else
      rows[i]->SetState(False);
  }
}

/*
** Functional Description:
**   XMPromptGeometry::Columns:
**     These overloaded methods allow the client to query or set the
**     state of the column toggles.
** Formal Parameters:
**    int cols:
**      For the settings routine, this specifies the new state.
** Returns:
**   int - For the readings routine, this specifies the new state.
*/
int XMPromptGeometry::Columns()
{
  for(int i = 0; i < 10; i++) {
    if(columns[i]->GetState()) return i + 1;
  }
  fprintf(stderr,"XMPromptGeometry::Columns(?) -- none set, returning 1\n");
  return 1;
}
void XMPromptGeometry::Columns(int cols)
{
  for(int i = 0; i < 10; i++ ) {
    if((i+1) == cols)
      columns[i]->SetState(True);
    else
      columns[i]->SetState(False);
  }
}

/*
 ** Functional Description:
 **    SetWindows    - This function is called in response to either the
 **                    Apply or the Ok button being pressed.  It is
 **                    decodes the text values and validates them.
 **                    If they are invalid, an error dialog is displayed
 **                    and no action is taken (the prompt dialog remains
 **                    visible).  If the values are good, then application
 **                    specific code is called to perform the appropriate 
 **                    subdivision.  If the Ok button was pressed, then
 **                    the dialog is unmanaged.
 ** Formal Parameters:
 **   XMWidget *dialog:
 **     The Button which was pressed in the dialog (we're doing custom
 **     dialog handling
 **   XtPointer client_data:
 **     The pointer to the dialog object.
 **  XtPointer callback_data:
 **      Ignored.
 **
 */
static void SetWindows(XMWidget *w, XtPointer client_data, 
		       XtPointer callback_data)
{
  
  XMPushButton *btn = (XMPushButton *)w;
  XMPromptGeometry *dialog = (XMPromptGeometry *)client_data;
  Boolean unmanage = False;
  
  int reason;
  
  
  /* Determine why we were called */
  
  if(btn == dialog->ok()) reason = XmCR_OK;
  if(btn == dialog->apply()) reason = XmCR_APPLY;
  if(btn == dialog->cancel()) reason = XmCR_CANCEL;

  switch(reason) {
  case XmCR_OK:			/* OK then unmanage after application */
    unmanage = True;
  case XmCR_APPLY:		/* Apply then just set the geometry. */
    Xamine_SetPaneGeometry(dialog->Columns(), dialog->Rows());
    Xamine_ChangedWindows();	/* Indicate a change was made. */
    break;
  case XmCR_CANCEL:		/* Cancel then unmanage without apply */
    unmanage = True;
    break;
  }


  /* If required, get rid of the dialog: */

  if(unmanage) dialog->UnManage();
  
}

/*
** Functional Description:
**  Xamine_request_geometry  - This function builds and instance of the
**                             XMPrompt2Strings class.  That class is
**                             a specialization of the XMPromptDialog.
**                             The prompt dialog requests the number
**                             of Vertical and horizontal panes.
** Formal Parameters:
**   XMWidget *w:
**     Pointer to the button widget which invoked this function.
**   XtPointer cd, cb:
**     Client and Callback data both of which are ignored for this function.
**/
void Xamine_request_geometry(XMWidget *w, XtPointer cd, XtPointer cb)
{
  /* If the geometry prompt dialog doesn't exist yet, then instantiate it */
  /* Otherwise it just needs to be managed:                               */

  if(!geometry_prompt) {
    geometry_prompt = new XMPromptGeometry("Geometry_Prompt",
					   w,
					   "Pane Geometry");
    geometry_prompt->AddDoCallback(SetWindows, geometry_prompt);
    geometry_prompt->AddCancelCallback(SetWindows, geometry_prompt);
    geometry_prompt->AddHelpCallback(Xamine_display_help, &geometry_help);
  }

  /* Set the initial toggle state and manage the dialog before returning */

  geometry_prompt->Rows(Xamine_Panerows());
  geometry_prompt->Columns(Xamine_Panecols());
  geometry_prompt->Manage();

}

/*
** Functional Description:
**   Xamine_reset_geometry:
**     This function resets the geometry of the Xamine display system to the
**     original size.
** Formal Parameters:
**     XMWidget *w:
**        Widget of button that resulted in our callback.
**     XtPointer cd:
**        Client call data (unused)
**     XtPointer cb:
**        Button callback data (unused)
*/
void Xamine_reset_geometry(XMWidget *w, XtPointer cd, XtPointer cb)
{
  Xamine_ResetShellSize(w);
}

/*
** Functional Description:
**   Xamine_minimize_geometry:
**     This function sets the Xamine window size to the minimum allowed size.
**     This is intended to allow users to put Xamine off into a small corner
**     somewhere out of the way and yet barely be able to see the spectra.
** Formal Parameter:
**    XMWidget *w:
**      Pointer to the widget which invoked us.
**    XtPointer ud:
**      User call data (ignored).
**    XTPointer cd:
**      Call description data (also ignored).
*/
void Xamine_minimize_geometry(XMWidget *w, XtPointer ud, XtPointer cd)
{
  Xamine_SetShellSize(XAMINE_PANES_MINXPIXELS, XAMINE_PANES_MINYPIXELS);
}



