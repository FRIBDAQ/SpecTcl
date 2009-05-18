/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**    Xamine  - NSCL display program.
** Abstract:
**    properties.cc   - This file contains code which interacts at the user
**                      interface level to change the properties of the 
**                      selected pane.
** Author:
**    Ron Fox
**    NSCl
**    Michigan State University
**    East Lansing, MI 48824-1321
*/


/*
** External include files:
*/
#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#include "XMWidget.h"
#include "XMDialogs.h"
#include "XMManagers.h"
#include "XMPushbutton.h"
#include "helpmenu.h"
#include "errormsg.h"
#include "properties.h"
#include "spcdisplay.h"
#include "dispwind.h"
#include "dispshare.h"
#include "optionmenu.h"
#include "panemgr.h"
#include "refreshctl.h"
#include "colormgr.h"
#include "exit.h"
/*
** Local Storage:
*/

/* help_text contains the list of help text strings that are
** displayed in the help dialog triggered by the help button
*/

extern volatile spec_shared *xamine_shared;

static const char *help_text[]  = {
  "  This dialog allows you to change the properties of the currently \n",
  "selected spectrum.  When the dialog is first presented, it shows the\n",
  "current settings for the selected spectrum.  Modify them as you see fit\n",
  "then click on OK to make the changes.\n\n",
  "  The Buttons at the bottom of the dialog have the following meanings:\n\n",
  "     OK     - Accept the changes you have made to the spectrum properties\n",
  "              the spectrum will be updated using the new properties\n",
  "     CANCEL - Make no changes and don't update the spectrum\n",
  "     HELP   - Display this help message\n",
  NULL
};
/*
** When help is passed as user data to Xamine_display_help
** that software will display a help dialog (information dialog).
** containing the help_text
*/
static struct Xamine_help_client_data help = { "Properties_Help",
					       NULL,
					       help_text
				           };


/*
** Custom classes:
*/
/*
** PropertyForm is a class which takes a parent form and produces a suitable
** array of subforms for displaying prompters for spectrum display property
** values.  By building a separate class, we can get the entire PropertyDialog
** built via the constructors of it's subclasses (essentially) using 
** inheritance rather than containment so that the subclass methods are
** transparently accessible to clients of the class.
*/
class PropertyForm {
 protected:
  XMForm *left_form;		/* Form for left half of the box. */
  XMForm *right_form;		/* Form for right half of the box.  */

  XMFrame *axis_frame;		/* There's a box around each prompter. */
  XMForm *axis_labels;		/* Prompt for axis/ticks/labels visibility */

  XMFrame *spectrum_frame;	/* Prompt for spectrum title visibility */
  XMForm *spectrum_titles;

  XMFrame *reduction_frame;	/* Prompt for spectrum reduction method. */
  XMForm *reduction;

  XMFrame *attributes_frame;	/* Prompt for spectrum attributes */
  XMForm *attributes;

  XMFrame *rendition_frame;	/* Prompt for rendition. */
  XMForm  *rendition_form;
  XMForm  *rendition_1;	/* 1d or 2d is visible, the other */
  XMForm  *rendition_2;	/* is unmanaged.                  */
 
  XMFrame *refresh_frame;
  XMForm  *refresh_form;

 public:
  PropertyForm(XMForm &parent);
  virtual ~PropertyForm() {
    delete rendition_2;
    delete rendition_1;
    delete rendition_form;
    delete rendition_frame;
    delete attributes;		/* Destroy all of the widgets we created. */
    delete attributes_frame;
    delete spectrum_titles;
    delete spectrum_frame;
    delete reduction_frame;
    delete reduction;
    delete axis_labels;
    delete axis_frame;
    delete refresh_form;
    delete refresh_frame;
  }
  void Oned(Boolean is1d) {
    rendition_form->UnManage();
    if(is1d) {
      rendition_2->UnManage();
      rendition_form->SetTopAttachment(*rendition_1, XmATTACH_FORM);
      rendition_form->SetLeftAttachment(*rendition_1, XmATTACH_FORM);
      rendition_form->SetRightAttachment(*rendition_1, XmATTACH_FORM);
      rendition_form->SetBottomAttachment(*rendition_1, XmATTACH_FORM);
      rendition_form->SetTopAttachment(*rendition_2, XmATTACH_NONE);
      rendition_form->SetLeftAttachment(*rendition_2, XmATTACH_NONE);
      rendition_form->SetRightAttachment(*rendition_2, XmATTACH_NONE);
      rendition_form->SetBottomAttachment(*rendition_2, XmATTACH_NONE);
      rendition_1->Manage();
    }
    else {
      rendition_1->UnManage();
      rendition_form->SetTopAttachment(*rendition_2, XmATTACH_FORM);
      rendition_form->SetLeftAttachment(*rendition_2, XmATTACH_FORM);
      rendition_form->SetRightAttachment(*rendition_2, XmATTACH_FORM);
      rendition_form->SetBottomAttachment(*rendition_2, XmATTACH_FORM);
      rendition_form->SetTopAttachment(*rendition_1, XmATTACH_NONE);
      rendition_form->SetLeftAttachment(*rendition_1, XmATTACH_NONE);
      rendition_form->SetRightAttachment(*rendition_1, XmATTACH_NONE);
      rendition_form->SetBottomAttachment(*rendition_1, XmATTACH_NONE);
      rendition_2->Manage();
    }
    rendition_form->Manage();
  }
  void Manage() {
    axis_labels->Manage();
    axis_frame->Manage();
    spectrum_titles->Manage();
    spectrum_frame->Manage();
    reduction->Manage();
    reduction_frame->Manage();
    attributes->Manage();
    attributes_frame->Manage();
    rendition_form->Manage();
    rendition_frame->Manage();
    refresh_form->Manage();
    refresh_frame->Manage();
    left_form->Manage();
    right_form->Manage();
  }
  void UnManage() {
    left_form->UnManage();
    right_form->UnManage();
    rendition_form->UnManage();
    rendition_frame->UnManage();
    attributes->UnManage();
    attributes_frame->UnManage();
    reduction->UnManage();
    reduction_frame->UnManage();
    spectrum_titles->UnManage();
    spectrum_frame->UnManage();
    axis_labels->UnManage();
    axis_frame->UnManage();
    refresh_form->UnManage();
    refresh_frame->UnManage();
  }
};


class PropertyDialog : public XMCustomDialog,
                       public PropertyForm,
                       public AxisForm,
                       public AttributeForm,
                       public ReduceForm,
                       public Rend1dForm,
                       public Rend2dForm,
                       public TitleForm,
                       public RefreshForm
{
 public:
  PropertyDialog(const char *name, XMWidget &parent, const char *title) :
    XMCustomDialog(name, parent, title),
    PropertyForm(*work_area),
    AxisForm(const_cast<char*>("Axis_Prompt"), *axis_labels),
    AttributeForm(const_cast<char*>("Attribute_Prompt"), *attributes),
    ReduceForm(const_cast<char*>("Reduction_Prompt"), *reduction),
    Rend1dForm(const_cast<char*>("Rendition_1d"), *rendition_1),
    Rend2dForm(const_cast<char*>("Rendition_2d"), *rendition_2),
    TitleForm(const_cast<char*>("Titles"), *spectrum_titles),
    RefreshForm(const_cast<char*>("Refresh"), *refresh_form) {
      SetColor(Xamine_ColorDisplay());
      PropertyForm::Manage();
      Apply->UnManage();
    }
  ~PropertyDialog() {
  }
  void Manage() {
    TitleForm::Manage();
    ReduceForm::Manage(); 
    PropertyForm::Manage(); 
    work_area->Manage();	/* Re-manage work area. */
    XMCustomDialog::Manage();
  }
  void UnManage() {
    PropertyForm::UnManage();
    ReduceForm::UnManage();
    TitleForm::UnManage(); 
    work_area->UnManage();	/* Force re-management of work area. */
    XMCustomDialog::UnManage();
  }

};
/*
** Below we cache the prompting widget since it probably will take
** a long time to get going the first time around.
*/

static PropertyDialog *dialog = NULL;

/*
** Method Description:
**   PropertyForm::PropertyForm:
**     The constructor method for the property form.  We create, layout
**     and manage a property prompter form.  The parent form is not managed.
**     All forms except children of the rendition_form are managed.
**     Which subform is actually managed intot the rendition_form depends
**     on the dimensionality of the spectrum.  Prior to managing the form as
**     a whole, the client should invoke the Oned method to select an 
**     appropriate 1-d prompt.
*/
 PropertyForm::PropertyForm(XMForm &parent)
{

  /* First we set up the left and right half forms.   */

  left_form = new XMForm("Left_form", parent);
  right_form= new XMForm("Right_Form", parent);

  /* Create the axis label box.  It's in the top part of the left form. */

  axis_frame  = new XMFrame("Axis_Frame", *left_form);
  axis_labels = new XMForm("Axis_Form",   *axis_frame);
  left_form->SetTopAttachment(*axis_frame,    XmATTACH_FORM);
  left_form->SetLeftAttachment(*axis_frame,   XmATTACH_FORM);
  left_form->SetRightAttachment(*axis_frame,  XmATTACH_FORM);
  left_form->SetBottomAttachment(*axis_frame, XmATTACH_POSITION);
  left_form->SetBottomPosition(*axis_frame,   25);

  /* Create the spectrum titles frame and form, It's at the top of the
  ** right form.
  */

  spectrum_frame = new XMFrame("Spectrum_Frame", *right_form);
  spectrum_titles= new XMForm("Spectrum_form", *spectrum_frame);
  right_form->SetTopAttachment(*spectrum_frame,    XmATTACH_FORM);
  right_form->SetLeftAttachment(*spectrum_frame,   XmATTACH_FORM);
  right_form->SetRightAttachment(*spectrum_frame,  XmATTACH_FORM);
  right_form->SetBottomAttachment(*spectrum_frame, XmATTACH_POSITION);
  right_form->SetBottomPosition(*spectrum_frame,   40);


  /* Create the attributes prompter.  It's attached to axis_frame on the
  ** top and the form on the left, right and bottom.
  ** It's a child of the left form:
  */

  attributes_frame = new XMFrame("Att_Frame", *left_form);
  attributes       = new XMForm("Att_Form",  *attributes_frame);
  left_form->SetTopAttachment(*attributes_frame,      XmATTACH_POSITION);
  left_form->SetTopPosition(*attributes_frame,         26);
  left_form->SetLeftAttachment(*attributes_frame,     XmATTACH_FORM);
  left_form->SetRightAttachment(*attributes_frame,    XmATTACH_FORM);
  left_form->SetBottomAttachment(*attributes_frame,   XmATTACH_FORM);



  /*  Create the reduction form.  This is attached on the top to the titles,
  **  to the form on the left and right and is in the right form.
  */

  reduction_frame = new XMFrame("Reduction_frame", *right_form);
  reduction       = new XMForm("Reduction_form",   *reduction_frame);
  right_form->SetTopAttachment(*reduction_frame,     XmATTACH_POSITION);
  right_form->SetTopPosition(*reduction_frame,        41);
  right_form->SetLeftAttachment(*reduction_frame,    XmATTACH_FORM);
  right_form->SetRightAttachment(*reduction_frame,   XmATTACH_FORM);
  right_form->SetBottomAttachment(*reduction_frame,   XmATTACH_POSITION);
  right_form->SetBottomPosition(*reduction_frame,    75);

  /* Create the rendition form... it is also created with two subforms.
  ** The subforms, rendition_1 and rendition_2 are both attached to the
  ** edgest of the super form.  Selectively managing one or the other via
  ** the Oned method allows the same dialog to collect information for both
  ** 1-d and 2-d spectrum renditions depending on which is appropriate.
  **   The rendition form is in the right_widget attached on the top to
  ** the reduction_frame and everywhere else to the form.
  */

  rendition_frame   = new XMFrame("Rendition_Frame",*right_form);
  rendition_form    = new XMForm("Rendition_Form",  *rendition_frame);


  rendition_1      = new XMForm("Rendition_1d", *rendition_form);
  rendition_form->SetTopAttachment(*rendition_1, XmATTACH_NONE);
  rendition_form->SetLeftAttachment(*rendition_1, XmATTACH_NONE);
  rendition_form->SetRightAttachment(*rendition_1, XmATTACH_NONE);
  rendition_form->SetBottomAttachment(*rendition_1, XmATTACH_NONE);

  rendition_2      = new XMForm("Rendition_2", *rendition_form);
  rendition_form->SetTopAttachment(*rendition_2, XmATTACH_NONE);
  rendition_form->SetLeftAttachment(*rendition_2, XmATTACH_NONE);
  rendition_form->SetBottomAttachment(*rendition_2, XmATTACH_NONE);
  rendition_form->SetRightAttachment(*rendition_2, XmATTACH_NONE);

  right_form->SetTopAttachment(*rendition_frame,      XmATTACH_WIDGET);
  right_form->SetTopWidget(*rendition_frame,         *reduction_frame);
  right_form->SetLeftAttachment(*rendition_frame,     XmATTACH_FORM);
  right_form->SetRightAttachment(*rendition_frame,    XmATTACH_FORM);
  right_form->SetBottomAttachment(*rendition_frame,   XmATTACH_FORM);

  /* Create the referesh form, it will extend along the whole of the
  ** top of the main form:
  */

  refresh_frame = new XMFrame("Refresh_frame", parent);
  refresh_form  = new XMForm("Refresh_from",  *refresh_frame);
  parent.SetLeftAttachment(*refresh_frame,     XmATTACH_FORM);
  parent.SetTopAttachment(*refresh_frame,      XmATTACH_FORM);
  parent.SetRightAttachment(*refresh_frame,    XmATTACH_FORM);

  /* Now paste the left and right forms below the slider frame: */

  parent.SetTopAttachment(*left_form,          XmATTACH_WIDGET);
  parent.SetTopWidget(*left_form,             *refresh_frame);
  parent.SetLeftAttachment(*left_form,         XmATTACH_FORM);
  parent.SetBottomAttachment(*left_form,       XmATTACH_FORM);
  
  parent.SetTopAttachment(*right_form,         XmATTACH_WIDGET);
  parent.SetTopWidget(*right_form,            *refresh_frame);
  parent.SetLeftAttachment(*right_form,        XmATTACH_WIDGET);
  parent.SetLeftWidget(*right_form,           *left_form);
  parent.SetBottomAttachment(*right_form,      XmATTACH_FORM);
  parent.SetRightAttachment(*right_form,       XmATTACH_FORM);

}

/*
** Functional Description:
**  UpdateProperties:
**    This function reads the set of properties from a PropertyDialog and
**    updates the properties of the selected spectrum to match them.
**    It is the mechanism for applying changes in the PropertyDialog to the
**    spectrum pane properties.
** Formal Parameters:
**   PropertyDialog *dialog:
**     The property dialog to load into the pane properties.
*/
static void UpdateProperties(PropertyDialog *dlg)
{
  win_attributed *a = Xamine_GetSelectedDisplayAttributes();

  if(a == NULL) {		/* Spectrum disappeared on us... */
    dlg->UnManage();
    return;
  }

  /* This stufff here preserves compatibility with a version that built
  ** up the new attributes block in separate storage cells and then
  ** copied them in.... we can't do this, but must modify the property
  ** block in place since it contains superpositions and other things not
  ** accessible from the properties menu.
  */


  win_attributed *att = a;


  /* Set the dimensionality independent attributes first using the generic
  ** win_attributed attributes.
  */

       /* Axis labeling attributes: */

  att->set_axes(dlg->ShowAxes(),
		dlg->ShowTicks(),
		dlg->ShowLabels());

       /* Fill properties from the attributes form: */

  att->setflip(dlg->Flipped());
  att->setlog(dlg->LogScale());
  if(dlg->AutoScale()) att->autoscale();
  else        
    att->setfs((unsigned int)atoi(dlg->FullScale()));
  att->setfloor((unsigned int)atoi(dlg->LowLevel()));
  if(!dlg->LowLevelEnabled()) att->nofloor();
  att->setceiling((unsigned int)atoi(dlg->HiLevel()));
  if(!dlg->HiLevelEnabled()) att->noceiling();

       /* Set reduction mode: */

  att->setreduce(dlg->ReduceForm::State());

      /* Set titling stuff: */

  att->set_titles(dlg->ShowName(),
		  dlg->ShowNumber(),
		  dlg->ShowDesc(),
		  dlg->ShowMaxPeak(),
		  dlg->ShowUpdate(),
		  dlg->ShowObjects());

  /* Now we need to set the rendition depending on the spectrum type: */

  if(att->is1d()) {
    win_1d *a1 = (win_1d *)att;
    a1->setrend(dlg->Rend1dForm::State());
  }
  else {
    win_2d *a2 = (win_2d *)att;
    a2->setrend(dlg->Rend2dForm::State());
  }
  att->update_interval(dlg->RefreshForm::Value());
  if(att->update_interval() == 0) att->noautoupdate();
  if(att->autoupdate_enabled()) {
    Xamine_ScheduleTimedUpdate(Xamine_GetSelectedDrawingArea(),
			       Xamine_PaneSelectedcol(), 
			       Xamine_PaneSelectedrow(), 
			       att->update_interval());
  }

  /* Trigger the update. */

  Xamine_RedrawSelectedPane();

}

/*
** Functional Description:
**  ActionCallback:
**    This function is called when one of the action area buttons is clicked
**    other than the Help button. Action depends on which button was clicked.
** Formal Parameters:
**  XMWidget *w:
**    Widget of the button clicked -- could be Ok, Apply, or Cancel.
**  XtPointer userd:
**    User data... in this case a pointer to the PropertyDialog which controls
**    this mess.
**  XtPointer calld:
**    In this case a pointer to an XmPushButtonCallbackStruct which is
**    relatively useless and hence ignored.
*/
static void ActionCallback(XMWidget *w, XtPointer userd, XtPointer calld)
{
  PropertyDialog *dlg = (PropertyDialog *)userd;

  /* We assume the HELP Button is handled by some other callback. */

  /* If the cancel button was hit, we just unmanage the widget and return: */

  if(w == dlg->cancel()) {
    dlg->popDown();
    return;
  }

  /* If the Apply or OK button was hit, we must update the properties of
  ** the selected spectrum, and schedule an update with the new properties
  */
  if( (w == dlg->ok()) || (w == dlg->apply())) {
    UpdateProperties(dlg);	/* Update the properties of the spectrum. */
    Xamine_RedrawSelectedPane(); /* Redraw the pane with new properties */
    Xamine_ChangedWindows();	/* Mark windows changed. */
  }

  /* If the OK button was hit, then we unmanage the dialog. */

  if((w == dlg->ok())) {
    dlg->popDown();
  }
  
}


/*
** Functional Description:
**   Xamine_SetSelectedProperties:
**     This function requests new properties for the current spectrum using
**     the properties dialog. (Motif callback).
** Formal Parameters:
**   XMWidget *w:
**      Widget of menu button which invoked this routine... parents the dialog
**   XtPointer cli:
**      Client data (ignored).
**   XtPointer call:
**      Call Data (ignored)
*/
void Xamine_SetSelectedProperties(XMWidget *w, XtPointer cli, XtPointer call)
{
  /* If the dialog has not yet been created, then we create it here
  ** and setup the callbacks.. We also set the dialog to be Application
  ** modal.  This is mainly done to prevent Motif from reacting to
  ** mouse hits that might change the selected spectrum while the
  ** user is filling in this dialog.
  */

  if(!dialog) {
    dialog = new PropertyDialog("Property_dialog", *w, 
				"Set Spectrum Properties");
    dialog->AddDoCallback(ActionCallback, (XtPointer)dialog);
    dialog->AddCancelCallback(ActionCallback, (XtPointer)dialog);
    dialog->AddHelpCallback(Xamine_display_help, &help);
    dialog->AddCallback(XtNdestroyCallback, NullPointer, (XtPointer)&dialog);
    dialog->AddCallback(XtNpopdownCallback, NullPointer, (XtPointer)&dialog);


    /* BUGBUGBUG   */
    /* It would be really nice if the below worked. but evidently it doesn't
    ** I'd like to make this dialog fully application modal because I don't
    ** want the user changing spectra on me in the middle of dealing with
    ** this dialog.  I thought that this should work following the resource
    ** values and inheritances in Motif, but it doesn't, evidiently because
    ** the imediate parent of the work area is a paned window and not the
    ** direct dialog shell.... Naturally, in their infinite unwisdom,
    ** there is no inheritance from XmBulletinBoard to XmPanedWindow which
    ** in turn means that I can't set the modality of the immediate child
    ** of the dialog shell (which is evidently what I have to do) to create
    ** a modal widget without also rewriting the custom dialog class.
    ** One option for later is to place the Paned window inside a parent
    ** Bulletin board or Form (Which does inherit from BulletinBoard and
    ** then set the modality on that guy... therefore I'll leave this code
    ** in here for now.
    */

    dialog->SetModal(XmDIALOG_FULL_APPLICATION_MODAL);

  }
  /* Once the dialog is created, we stock it with the attributes of the
  ** current spectrum.  This includes calling Oned to make the appropriate
  ** rendition dialog visible.
  */
  win_attributed *att = Xamine_GetSelectedDisplayAttributes();
  if(att == NULL)  return;	/* Should never happen, but let's be sure. */

  char value[20];

  dialog->ShowAxes(att->showaxes());
  dialog->ShowTicks(att->showticks()); /* Set axis prompts to current values */
  dialog->ShowLabels(att->labelaxes());

  dialog->Flipped(att->isflipped());
  dialog->LogScale(att->islog());
  dialog->AutoScale(!att->manuallyscaled());
  dialog->LowLevelEnabled(att->hasfloor());
  dialog->HiLevelEnabled(att->hasceiling()); /* Set Attributes to current */
  sprintf(value, "%d", att->getfsval());     /* values. */
  dialog->FullScale(value);
  sprintf(value, "%d", att->getfloor());
  dialog->LowLevel(value);
  sprintf(value, "%d", att->getceiling());
  dialog->HiLevel(value);
  dialog->AdjustGhosting();

  dialog->ReduceForm::State(att->getreduction()); /* Make reduction mode current */

  dialog->ShowName(att->showname());
  dialog->ShowNumber(att->shownum());
  dialog->ShowDesc(att->showdescrip());
  dialog->ShowMaxPeak(att->showpeak());
  dialog->ShowUpdate(att->showupdt());
  dialog->ShowObjects(att->showlbl());

  dialog->RefreshForm::Value(att->update_interval());

  dialog->Rend2dForm::UnManage();
  dialog->Rend1dForm::UnManage();
  if(att->is1d()) {		/* If the spectrum is 1-d... */
    win_1d *a = (win_1d *)att;
    dialog->Rend1dForm::State(a->getrend()); /* Set the rendition state and */
    dialog->Oned(True);		/* Enable the 1-d part of the dialog. */
    dialog->Rend1dForm::Manage();
  }
  else {			/* If the spectrum is 2-d... */
    win_2d *a = (win_2d *)att;
    dialog->Rend2dForm::State(a->getrend()); /* Set rendition state and */
    dialog->Oned(False);	/* Select the 2-d part of the dialog.   */
    dialog->Rend2dForm::Manage();
  }

  /* 
  ** Manage the dialog and return to the main loop to process events.
  */
  dialog->Manage();
}

/*
** Functional Description:
**   Xamine_ToggleZoomState:
**     Called when the button box zoom toggle button is struck.
**     Our action is to set the log/lin state of the selected pane's
**     spectrum to the state of the toggle button and schedule a refresh.
** Formal Parameters:
**    XMWidget *wid:
**      The toggle button widget handle.
**    XtPointer cd, ud:
**      Ignored/unused Xm callback arguments.
*/
void Xamine_ToggleZoomState(XMWidget *wid, XtPointer cd, XtPointer ud)
{
  XMToggleButton *tb = (XMToggleButton *)wid;
  win_attributed *at = Xamine_GetSelectedDisplayAttributes();

  if(at != NULL) {
    at->setlog(tb->GetState());
    Xamine_RedrawSelectedPane();
  }  
}

void Xamine_ToggleUserMapping(XMWidget* wid, XtPointer cd, XtPointer ud) 
{
  XMToggleButton* tb = (XMToggleButton*)wid;
  win_attributed* at = Xamine_GetSelectedDisplayAttributes();
  if(at != NULL) {
    if(at->is1d()) {
      win_1d* a1 = (win_1d*)at;
      a1->setmapped(tb->GetState());
    }
    else {
      win_2d* a2 = (win_2d*)at;
      a2->setmapped(tb->GetState());
    }
    Xamine_RedrawSelectedPane();
  }
}




