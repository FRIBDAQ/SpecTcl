/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   This file contains the code to implement:
**     The Copy_Multiselect GUI class,
**     The Copy_Object      GUI class,
**     The CopyObjectDialog GUI class,
**     The Xamine code to copy graphical objects and gates from one spectrum
**     to another compatible spectrum.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
static char *sccsinfo="@(#)objcopy.cc	8.1 6/23/95 6/23/95";

/*
** Include files:
*/
#include <stdio.h>
#include <string.h>
#include "XMDialogs.h"
#include "panemgr.h"
#include "dispshare.h"
#include "objcopy.h"
#include "dispgrob.h"
#include "grobjmgr.h"
#include "gateio.h"
#include "helpmenu.h"
#include "errormsg.h"
#include "grobjdisplay.h"
#include "compatspec.h"
/*
** Local class definitions:
*/

/*
** Form to copy a graphical object. 
*/
static void sync(XMWidget *w)
{
  XSync(XtDisplay(w->getid()), 0);
}
class Copy_Object : public Copy_Multiselect
{
 public:
  Copy_Object(XMForm *top, XMForm *bottom) :
    Copy_Multiselect(top, bottom, "Objects", "Spectra") 
                         {
			   UpdateLeft();
			   UpdateRight();
			 }
  virtual ~Copy_Object() {}
  virtual void UpdateLeft();
  virtual void UpdateRight();
  virtual int CopyItem(char *from , char *to, Copier_Arrow_Direction dir);
  void Manage()    { 
                     Copy_Multiselect::Manage(); 

		   }
  void UnManage()  { Copy_Multiselect::UnManage(); }
 protected:
};

/*
** Class to implement the copy object dialog.
*/

class CopyObjectDialog : XMCustomDialog
{
 public:
  /* Instantiators and destructors */

  CopyObjectDialog(XMWidget *parent);
  ~CopyObjectDialog() {
    delete copier;
    delete top;
    delete bottom;
    if(help.dialog != NULL) delete help.dialog;
  }

  /* Manipulators: */

  void Manage() {
    copier->Manage();
    top->Manage();
    XMCustomDialog::Manage();
    copier->Manage();
    top->Manage();
    bottom->Manage();
    XMCustomDialog::Manage();
    copier->UpdateLeft();
    copier->UpdateRight();
  }
  void UnManage() {
    copier->UnManage();
    bottom->UnManage();
    top->UnManage();
    XMCustomDialog::UnManage();
  }

  /* Action response functions: */

  void Dismiss() { UnManage(); }
  void DisplayHelp() {
    Xamine_display_help(WorkArea(), (XtPointer)(&help), (XtPointer)this);
  }

 protected:
  Copy_Object *copier;
  XMForm      *top, *bottom;
  Xamine_help_client_data help;
};
/*
** Static data declarations:
*/
char *objtype[] = {
                    "Generic",
		    "Cut",
		    "Sum",
		    "Band",
		    "Contour",
		    "Marker",
		    "Sum",
		    "Marker"
                  };
static char *help_text[] = {
  "This dialog is prompting you to copy objects from the selected spectrum\n",
  "to any compatible spectrum.  To copy an object or set of objects, select\n",
  "the objects you want to copy in the left list, and selecte the set of\n",
  " destination spectra in the right list and press th arrow button\n",
  "At any time pressing:\n",
  "Select All (left)       - Will Select all objects\n",
  "Select All (right)      - Will Select all spectra\n",
  "Update                  - Will refresh the lists to show new spectra and\n",
  "                          objects\n",
  "->                      - Copies all selected objects to all selected spectra\n",
  "Dismiss                 - Makes the dialog disappear\n",
  "Help                    - Displays this help pane\n",
  NULL
  };

static CopyObjectDialog *dialog = (CopyObjectDialog *)NULL;
/*
** External References:
*/

extern spec_shared *xamine_shared;

/*
** The pages below implement the methods of the generic multiselect copy
** class.  This object consists of a copier with 3 extr buttons.
*/

/*
** Functional Description:
**   UpdateRelay:
**     This function is attached to the update button and is used to relay
**     the update button hit to the UpdateLeft and UpdateRight virtual functions
** Formal Parameters:
**    XMWidget *b:
**      The button widget.
**    XtPointer ud:
**      Pointer to the object instance.
**    XtPointer cd:
**      Pushbutton client data.
*/
static void UpdateRelay(XMWidget *b, XtPointer ud, XtPointer cd)
{
  Copy_Multiselect *us = (Copy_Multiselect *)ud;

  us->UpdateLeft();
  us->UpdateRight();

}

/*
** Functional Description:
**  SelectRelay:
**    This function is attached to the two select buttons.  It is used to
**    process the two select all buttons for the associated object.
** Formal Parameters:
**    XMWidget *b:
**      The button pressed.
**   XtPointer ud:
**      Pointer to the object.
**   XtPointer cd:
**     call data for pushbutton activate callbacks.
*/
static void SelectRelay(XMWidget *b, XtPointer ud, XtPointer cd)
{
  Copy_Multiselect *us = (Copy_Multiselect *)ud;
  XMScrolledList *l;

  if(strcmp(b->getname(), "SelRight") == 0) {
    l = us->GetRightListWidget();
  }
  else if(strcmp(b->getname(), "SelLeft") == 0) {
    l = us->GetLeftListWidget();
  }
  else {
    fprintf(stderr, "SelectRelay -- inappropriate button widget called us\n");
    return;
  }
  int nitems;

  l->GetAttribute(XmNitemCount, &nitems);

  l->SetAttribute(XmNselectionPolicy, XmMULTIPLE_SELECT);
  while(nitems > 0) {
    Widget id = l->getid();

    XmListSelectPos(id, nitems, False);
    nitems--;
  }
  l->SetAttribute(XmNselectionPolicy, XmEXTENDED_SELECT);
}
/*
** Functional Description:
**  Copy_Multiselect::Copy_Multiselect
**    This function instantiates a multiselectable copy form.
**    This involves instantiating a copier and creating a set of additional
**    buttons to handle the extended functions.  The buttons will have
**    callback forwarding functions attached to them.
** Formal Parameters:
**    XMForm *top:
**      Form that will parent the copier.
**    XMForm *bottom:
**      Form that parents the additional buttons.
**    char *left_lbl, *right_lbl:
**      Pointers to names for the left and right slection columns.
*/
Copy_Multiselect::Copy_Multiselect(XMForm *top, XMForm *bottom, 
				   char *left_lbl, char *right_lbl) :
     Copier_Form(top, left_lbl, right_lbl)
{

  /* Instantiate the widgets... */

  SelectLeft = new XMPushButton("SelLeft", *bottom, SelectRelay, this);
  SelectLeft->Label("Select All");

  SelectRight = new XMPushButton("SelRight", *bottom, SelectRelay, this);
  SelectRight->Label("Select All");

  Update     = new XMPushButton("Update", *bottom, UpdateRelay, this);

  /* Set up the form layout so that the buttons are properly distributed. */

  bottom->SetFractionBase(8);
  bottom->SetLeftAttachment(*SelectLeft, XmATTACH_POSITION);
  bottom->SetLeftPosition(*SelectLeft, 1);
  bottom->SetRightAttachment(*SelectLeft, XmATTACH_NONE);
  bottom->SetTopAttachment(*SelectLeft, XmATTACH_FORM);
  bottom->SetBottomAttachment(*SelectLeft, XmATTACH_FORM);

  bottom->SetLeftAttachment(*Update, XmATTACH_POSITION);
  bottom->SetLeftPosition(*Update, 3);
  bottom->SetRightAttachment(*Update, XmATTACH_NONE);
  bottom->SetTopAttachment(*Update, XmATTACH_FORM);
  bottom->SetBottomAttachment(*Update, XmATTACH_FORM);

  bottom->SetLeftAttachment(*SelectRight, XmATTACH_POSITION);
  bottom->SetLeftPosition(*SelectRight,   5);
  bottom->SetTopAttachment(*SelectRight,  XmATTACH_FORM);
  bottom->SetRightAttachment(*SelectRight, XmATTACH_NONE);
  bottom->SetBottomAttachment(*SelectRight, XmATTACH_FORM);

  t = top;
  b = bottom;

  /* Allow multiple selections: */

  left_l->SetAttribute(XmNselectionPolicy, XmEXTENDED_SELECT);
  right_l->SetAttribute(XmNselectionPolicy, XmEXTENDED_SELECT);
}

/*
** The functions and methods in the next pages implement pieces of the
** CopyObject class.  That class specializes the Copy_MultiSelect to
** copy graphical objects to spectra.  When these pages are done, we have
** a complete graphical user interface element that can be included into
** dialogs to do the copy function.
*/
/*
** Functional Description:
**   IsSelected:
**     Returns True if an input string is a member of a selection list.
** Formal Parameters:
**    char *tryme:
**       String to check for selection membership.
**    char **selectlist:
**       List of selected items.
**    int selsize:
**       Number of items in the selection list.
*/
Boolean IsSelected(char *tryme, char **selectlist, int selsize)
{
  for(int i = 0; i < selsize; i++) {
    if(strcmp(tryme, *selectlist++) == 0) return True;
  }
  return False;
}

/*
** Functional Description:
**   Copy_Object::UpdateLeft:
**    Updates the list of items that are on the left side list widget.
**    This implementation first saves the set of selections that have
**    been maintained and then begins to update the list.  Whenever a match
**    is found between an element to go into the list and an old selection
**    the element is placed in the list and then selected. 
*/
void Copy_Object::UpdateLeft()
{
  /* Get the current selection list */

  char **selection_list;
  int    nsels = GetLeftSelectedList(&selection_list);
  int    nobj, ngate;
  int    i, pos, specid;

  /* Clear the list to start. */

  left_l->ClearItems();

  /* If the selected pane is empty or corresponds to an undefined spectrum,
  ** leave things blank.
  */

  win_attributed *att = Xamine_GetSelectedDisplayAttributes();
  if(att == NULL) goto cleanup;	/* Nothing on a nonexi<stent spectrum. */

  specid = att->spectrum();
  if(xamine_shared->gettype(specid) == undefined) goto cleanup;


  /* Get the set of graphical objects and gates defined on the spectrum */


  grobj_generic *objects[GROBJ_MAXOBJECTS];
  grobj_generic *gates[GROBJ_MAXOBJECTS];

  nobj = Xamine_GetSpectrumObjects(specid, objects, GROBJ_MAXOBJECTS,
				   True);
  ngate= Xamine_GetSpectrumGates(specid, gates, GROBJ_MAXOBJECTS, True);

  /* Enter the graphical objects one by one into the list followed by
  ** the gates.  If the gate is in the selection list, then select it too:
  */
  pos =1;

  grobj_name name;
  for(i = 0; i < nobj; i++) {
    char tname[100];		/* Presented name. */
    sprintf(tname, "[%03d] %s", objects[i]->getid(), objects[i]->getname(name));
    left_l->AddItem(tname, pos);
    if(IsSelected(name, selection_list, nsels))
      XmListSelectPos(left_l->getid(), pos, False);
    pos++;
  }

  for(i = 0; i < ngate; i++) {
    grobj_name name;
    char       tname[100];	/* presented name. */
    sprintf(tname, "[%03d] %s", gates[i]->getid(), gates[i]->getname(name));
    left_l->AddItem(tname, pos);
    if(IsSelected(tname, selection_list, nsels))
      XmListSelectPos(left_l->getid(), pos, False);
    pos++;
  }


  /* Clean up all allocated resources */
 cleanup:                          /* This was a mistake...lists maintained
				      by the widget */
  return;
}

/*
** Functional Description:
**   CopyObject::UpdateRight:
**     This function updates the right hand side list widget.  The right
**     side list widget is considered to be a list of spectra which are
**     compatible with the selected one.  Compatible is defined as
**     having the same dimensionality and number of channels on all axes.
*/
void Copy_Object::UpdateRight()
{


  /* Get the prior selection list: */

  char **sellist;
  int    nselected = Copier_Form::GetRightSelectedList(&sellist);
  int i, pos, ych, xch, specid;
  Boolean is1d;

  /* Clear the old list so that if all the checks below fail the target list
  ** will be null.
  */

  right_l->ClearItems();

  /* compatibility requires that there be a selected spectrum. */


  /* If the selected pane is empty or corresponds to an undefined spectrum,
  ** leave things blank.
  */

  win_attributed *att = Xamine_GetSelectedDisplayAttributes();
  if(att == NULL) goto cleanup;	/* Nothing on a nonexistent spectrum. */

  specid = att->spectrum();
  if(xamine_shared->gettype(specid) == undefined) goto cleanup;


  /*
  ** Now fetch the criteria for compatibility
  */

  is1d = (xamine_shared->gettype(specid) == onedlong) ||
                 (xamine_shared->gettype(specid) == onedword);
  xch      = xamine_shared->getxdim(specid);
  ych      = xamine_shared->getydim(specid);

  /* Now loop over all the defined spectra, adding them to the list and
  ** selecting them if they are in the selection list.
  */

  pos = 1;
  for(i = 0; i < DISPLAY_MAXSPEC; i++) {
    if(Xamine_IsCompatible(specid, i)) {
      spec_title name;
      char      tname[100];
      sprintf(tname, "[%03d] %s", i, xamine_shared->getname(name, i));
      right_l->AddItem(tname, pos);
      if(IsSelected(tname, sellist, nselected))
	XmListSelectPos(right_l->getid(), pos, False);
      pos++;
    }
  }       
  
  /* Clean up the mess we have: */  /* Mistake... Xm handles the lists */

 cleanup:
  return;
}

/*
** Functional Description:
**    Copy_Object::CopyItem:
**     This method initiates a copy of a graphical object to another
**     spectrum.  We don't copy if the to spectrum is the selected spectrum
**     as this could just be the poor guy doing a select all and then copying.
** Formal Parameters:
**   char *from:
**      Source (graphical object name from the list box).
**   char *to:
**      Destination (target spectrum name from the list box).
**   Copier_Arrow_Direction dir:
**      Must be right... the arrow direction.
*/
int Copy_Object::CopyItem(char *from, char *to, Copier_Arrow_Direction dir)
{
  char msg[1000];


  /* Note that since they're not allowed to type-in, we're assured that the
  ** text we got has the [nnn] form so we can get e.g. the spectrum id from that
  */
  /* BUGBUGBUG  -- This might change and later releases should not take
  **               advantage of that.
  */
  /* Get the destination spectrum... make sure it exists and is still
  ** compatible with the source which must also still exist.
  */

  int dst_spec;			/* Destination spectrum. */
  sscanf(to, "[%03d]", &dst_spec);
  if(xamine_shared->gettype(dst_spec) == undefined) {
    sprintf(msg, "Destination spectrum '%s' is no longer defined",
	    to);
    Xamine_error_msg(right_l, 
		     msg);
    return True;
  }
  win_attributed *at;
  int src_spec;
  at = Xamine_GetSelectedDisplayAttributes();
  if(at == NULL) {
    Xamine_error_msg(left_l,
		     "Source spectrum pane no longer has a spectrum");
    UnManage();
    return False;
  }
  src_spec = at->spectrum();
  if(xamine_shared->gettype(dst_spec) == undefined) {
    spec_title name;
    sprintf(msg, "Source spectrum '%s' is no longer defined",
	    xamine_shared->getname(name, dst_spec));
    Xamine_error_msg(left_l,
		     msg);
    UnManage();
    return False;
  }

  if(!Xamine_IsCompatible(src_spec, dst_spec)) {
    spec_title srcname, dstname;
    sprintf(msg, "'%s' and '%s' are no longer compatible spectra",
	    xamine_shared->getname(srcname, src_spec),
	    xamine_shared->getname(dstname, dst_spec));
    Xamine_error_msg(left_l, msg);
    UnManage();
    return True;		/* Could be multiple destinations. */
  }
  if(src_spec == dst_spec) {
    return True;
  }

  /* Get the source graphical object:
   ** BUGBUGBUG -- what're you going to do if there's a grobj with the same
   **              name and id as a gate?
  */
  
  int objid;
  Boolean isgate = False;
  sscanf(from, "[%03d]", &objid);
  grobj_database *odb = Xamine_GetObjectDatabase(); /* Hunt first in grobj db. */
  grobj_generic *obj = odb->find(objid, src_spec);
  if(obj != NULL) {		/* Verify that it's the same as src... */
    char tname[100];		/* Object title string. */
    grobj_name name;
    sprintf(tname, "[%03d] %s", obj->getid(), obj->getname(name));
    if(strcmp(from, tname) != 0)
      obj = (grobj_generic *)NULL; /* Wrong object. */
  }
  if(obj == NULL) {		/* If not in grobj db, look in gates: */
    odb = Xamine_GetGateDatabase();
    obj = odb->find(objid, src_spec);
    if(obj == NULL) {
      sprintf(msg, "Object '%s' no longer exists",
	      from);
      Xamine_error_msg(left_l, msg);
      return True;		/* Keep trying if multiple objects select<ed. */
    }
    isgate = True;
  }
  /* At this point the source object is pointed to by obj, and isgate is true
  ** if the object is actually a gate dst_spec is the destination spectrum id.
  */
  grobj_generic *object = obj->clone(); /* Make a new object... */

  if(isgate) {			/* If it's a gate, then didle the id and */
				/* pass it to the client.                */
    /* Reformulate the gate... */

    grobj_name    newname;


    object->setid(Xamine_GetNextGateId()); /* Fake up a new gate. */
    object->setspectrum(dst_spec); /* Set spectrum as destination spectrum */
    sprintf(newname, "%s%03d Copy of %03d from spectrum %03d",
	    objtype[object->type()], object->getid(), 
	     obj->getid(), obj->getspectrum());
    object->setname(newname);
    
    /* Send the gate to the client.  */


    /* Empty the Xt event queue so that we can process any pending user 
    ** gate echoing.
    */
    
    XtAppContext ourctx = XtWidgetToApplicationContext(left_l->getid());
    while(XtAppPending(ourctx)) 
      XtAppProcessEvent(ourctx, XtIMAll);

    Xamine_ReportGate(object);


    /* Empty the Xt event queue so that we can process any pending user 
    ** gate echoing.
    */
    
    while(XtAppPending(ourctx)) 
      XtAppProcessEvent(ourctx, XtIMAll);

  }
  else {			/* If an object we must accept and draw it. */
    
				/* pass it to the client.                */
    /* Reformulate the gate... */

    grobj_name    newname;


    object->setid(Xamine_GetNextObjectId()); /* Fake up a new object. */
    object->setspectrum(dst_spec); /* Set spectrum as destination spectrum */
    sprintf(newname, "%s%03d Copy of %03d from spectrum %03d",
	    objtype[object->type()], object->getid(), 
	    obj->getid(), obj->getspectrum());
    object->setname(newname);

    grobj_generic *o = Xamine_EnterGrobj(object);

    Xamine_PlotObject(o, True);	/* Draw the object. */
  }

    
  
  /* Free the object...           */
  
  delete object;
  return True;
}

/*
**  Call relays for the Dismiss button and the Help button:
*/

static void 
CopyObjectDlg_Dismiss_relay(XMWidget *wid, XtPointer cli, XtPointer cd)
{
  CopyObjectDialog *dlg = (CopyObjectDialog *)cli;
  dlg->Dismiss();
}
static void 
CopyObjectDlg_Help_relay(XMWidget *wid, XtPointer cli, XtPointer cd)
{
  CopyObjectDialog *dlg = (CopyObjectDialog *)cli;
  dlg->DisplayHelp();
}

/*
** Functional Description:
**   CopyObjectDialog::CopyObjectDialog:
**      This method instantiates a copy object dialog.  The copy object dialog
**     places a Copy_Object input form into a custom dialog.  The Copy_Object
**     form is allowed to function autonomously while we just handle the
**     dismiss button, and the help button as well as miscellaneous
**     management operations our client throws at us.
**
** Formal Parameters:
**    XMWidget *parent:
**       The parent widget for this dialog.
*/
CopyObjectDialog::CopyObjectDialog(XMWidget *parent) :
       XMCustomDialog("Object_CopierDLG", *parent, "Copy Objects")
{

  /* First set up the dialog box action area: */

  AddDoCallback(CopyObjectDlg_Dismiss_relay, this);
  AddHelpCallback(CopyObjectDlg_Help_relay, this);
  cancel()->UnManage();		/* Don't display the cancel button. */
  ok()->Label("Dismiss");	/* The OK button should be labelled Dismiss */
  apply()->UnManage();		/* Don't display the apply button either. */

  /* Now set up the work area to be a Copy_Object dialog: */
  XMForm *wa  = WorkArea();

  top    = new XMForm("TopForm", *wa);
  bottom = new XMForm("BottomForm", *wa);
  


  wa->SetLeftAttachment(*top, XmATTACH_FORM);
  wa->SetTopAttachment(*top, XmATTACH_FORM);
  wa->SetRightAttachment(*top, XmATTACH_FORM);
  wa->SetBottomAttachment(*top, XmATTACH_POSITION);
  wa->SetBottomPosition(*top, 85);

  wa->SetLeftAttachment(*bottom, XmATTACH_FORM);
  wa->SetTopAttachment(*bottom, XmATTACH_WIDGET);
  wa->SetTopWidget(*bottom, *top);
  wa->SetRightAttachment(*bottom, XmATTACH_FORM);
  wa->SetBottomAttachment(*bottom, XmATTACH_FORM);

  copier = new Copy_Object(top, bottom);
  /*
  ** Set up help text:
  */

  help.name   = "Copy_Dialog_help";
  help.dialog = (XMInformationDialog *)NULL;
  help.text   = help_text;
  top->Manage();
  bottom->Manage();
  wa->Manage();

}

/*
** Functional Description:
**   Xamine_CopyObject:
**     Displays and starts operation of the Xamine copy object dialog.
**     If the cached dialog pointer, dialog is NULL then the dialog
**     is created.  If non null, then the dialog is created.  Otherwise,
**     just managed.
** Formal Parameters:
**   XMWidget *button:
**     The buton pressed which caused us to be called.
**   XtPOinter user_d:
**      User data for the push callback (Ignored).
**   XtPointer call_d:
**      Call data for the push callback (ignored).
*/
void Xamine_CopyObject(XMWidget *button, XtPointer user_d, XtPointer call_d)
{
  if(!dialog) {
    dialog = new CopyObjectDialog(button);
  }
  dialog->Manage();
}
