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
**   Xamine:
**     NSCL display program.
** Abstract:
**   superprompt.cc
**      This file contains code that requests for a suitable spectrum
**      to be superimposed on the currently selected spectrum.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/

/*
** Include files:
*/

#include <config.h>
#include <stdio.h>

#include "XMDialogs.h"
#include <Xm/SelectioB.h>
#include "superprompt.h"
#include "dispwind.h"
#include "superpos.h"
#include "spcdisplay.h"
#include "dispshare.h"

#include "helpmenu.h"
#include "panemgr.h"
#include "errormsg.h"
#include "specchoose.h"
#include "compatspec.h"
#include "refreshctl.h"
#include "menusetup.h"
#include "exit.h"
/*
** Local storage:
*/
#if 0
static Arg SuperPromptArgs[1];
#endif

static const char *help_text[] = {
  "  You are being prompted for a spectrum to superimpose on the currently\n",
  "selected spectrum.  The superimposed spectrum must:\n\n",
  "   1. Be another 1-d spectrum\n",
  "   2. Not already be superimposed on this spectrum\n",
  "   3. Have the same number of channels as the selected spectrum\n\n",
  "  You can select the spectrum by either double clicking on a spectrum\n",
  "name n the list box in the top part of the dialog, or by typing in either\n",
  "the spectrum number or spectrum name.\n\n",
  "    Once you have selected a spectrum, you can click on:\n\n",
  "      OK     - To accept the selection and dismiss this dialog\n",
  "      APPLY  - To accept the selection and leave the dialog up\n",
  "      CANCEL - To do nothing and leave the dialog up\n",
  "      HELP   - To display this help message\n",
  NULL
};
static Xamine_help_client_data help = { "Superposition_help", 
					  NULL, help_text };

/*
** Help text for the remove superposition help dialog:
*/
char const *remove_help[] = 
{ "This dialog allows you to remove a superimposed spectrum from \n",
  "the currently selected spectrum.  The spectra superimposed on the base\n",
  "spectrum are displayed in the list box.  Select the spectrum you want to\n",
  "remove from the superposition and click on:\n\n",
  "    OK     - Remove the spectrum and dismiss the dialog.\n",
  "    Apply  - Remove the spectrum and don't dismiss the dialog\n",
  "    Cancel - Dismiss this dialog without removing any superpositions\n",
  "    Help   - Display this help text\n",
  NULL
  };
/*
** Global References:
*/

extern spec_shared *xamine_shared;

/*
** Local class definitions:
*/
class SuperPrompt : public XMSelectionDialog
{
 private:
  int Match(XtPointer cd);            /* Common superposition code.  */
 public:
  SuperPrompt(char *name,
	      XMWidget &parent);     /* Construct the dialog. */
  ~SuperPrompt() {}                  /* Kill the dialog. */

  void SetList();	             /* Compute and set the list box. */
  Boolean OkAction(XtPointer cd);    /* Called for OK button        */
  Boolean ApplyAction(XtPointer cd); /* Called for Apply Action     */
  Boolean CancelAction(XtPointer cd)
    { return True; }                 /* Called for cancel action   */
  
};

class UnSuperPrompt : public XMSelector
{
private:
  int Match(XtPointer cd);
public:
  UnSuperPrompt(char *n, XMWidget &parent);		// Constructor.
  virtual ~UnSuperPrompt() {}	// Destructor.
  virtual void SetupList();	// Supply function to put entries in list.
  virtual Boolean Perform(XMWidget *wid, XtPointer cd, int reason, 
			  XmString value, int size); // Execute selection.
  virtual void ApplyCb(XMWidget *wid, XtPointer userd, XtPointer cd);
};


static XMWidget*   theParent = NULL;
static SuperPrompt *dialog = NULL;
static UnSuperPrompt *undialog = NULL;


/*
** Helper functions to call object callbacks from Motif callbacks.
*/
static void SuperPrompt_Ok(XMWidget *p, XtPointer obj, XtPointer cd)
{
  SuperPrompt *o = (SuperPrompt *)obj;
  if(!o->OkAction(cd)) {
    o->SetList();		/* Update the list. */
    o->Show();
  }
  else {
    o->Hide();
  }
}

static void SuperPrompt_Apply(XMWidget *p, XtPointer obj, XtPointer cd)
{
  SuperPrompt *o = (SuperPrompt *)obj;
  if(o->ApplyAction(cd)) o->Hide();
  else o->SetList();		/* Update the list.   */
}

static void SuperPrompt_Cancel(XMWidget *p, XtPointer obj, XtPointer cd)
{
  SuperPrompt *o = (SuperPrompt *)obj;
  if(o->CancelAction(cd)) o->Hide();
}

/*
** Functional Description:
**    IsCompatible:
**        Returns true if the spectrum number passed in is compatible
**        for superposition with the spectrum in the selected pane.
** Formal Parameters:
**     int specid:
**       Number of spectrum to check.
** Returns:
**   True   - Compatible.
**   False  - Incompatible.
*/
static Boolean IsCompatible(int specid)
{
  win_attributed *att = Xamine_GetSelectedDisplayAttributes();

  if(att == NULL) return False;

  int          target = att->spectrum();

  return Xamine_IsCompatible(target, specid);

}

/*
** Functional Description:
**   IsSuperimposed:
**     This function is a boolean which returns true if the input spectrum
**     is already displayed on a given pane.
** Formal Parameters:
**  win_attributed *att:
**     Pointer to the pane attributes block of the pane to check.
**  int specid:
**    Id of the spectrum we're hunting for.
** Returns:
**    True  - Spectrum is displayed or superimposed on the pane.
**    False - Spectrum is not displayed or superimposed on the pane.
*/
static Boolean IsSuperimposed(win_attributed * att, int specid)
{
  if(att->spectrum() == specid) return True; /* Specid is main spectrum. */

  /* If spectra are superimposed, then we need to produce an interator for
  ** the superposition list:
  */
  win_1d *a = (win_1d *)att;
  SuperpositionList &slist = a->GetSuperpositions();
  int cnt = slist.Count();
  if(cnt  > 0) {
    SuperpositionListIterator super(slist);
    while(!super.Last()) {
      Superposition &s = super.Next();
      if(s.Spectrum() == specid) return True; /* It's displayed. */
    }
  }
  return False;			/* Not in list.  */
}

/*
** Functional Description:
**   SuperPrompt::Match:
**     This function determines which spectrum has been chosen by the user.
** Formal Parameters:
**    XtPointer cd:
**      Pointer to the callback data which is really an 
**      XmSelectionBoxCallbackStruct describing the state of the selection
**      box at the time the selection has been confirmed.
** Returns:
**    0 - No spectrum matched, or invalid spectrum string.
**   >0 - Spectrum matched and compatible and this is its number.
**   <0 - Invalid match, spectrum matched but not compatible or current
**        spectrum is 2-d.
*/
int SuperPrompt::Match(XtPointer cd)
{
  XmSelectionBoxCallbackStruct *cbs = (XmSelectionBoxCallbackStruct *)cd;
  char *text;
  int  specid;

  /* Get the selection string as ASCIZ */


  if(!XmStringGetLtoR(cbs->value, const_cast<char*>(XmSTRING_DEFAULT_CHARSET),  &text)) {
    return 0;
  }

  /* Decode the spectrum string.  The spectrum string is either a pure number,
  ** a bracket enclosed number or a spectrum name.  We attempt to decode in that
  ** order.
  */

  if(sscanf(text, "%d", &specid) != 1) {      /* It was not a  number.  */
    if(sscanf(text, "[%05d]", &specid) != 1) { /* Not bracket enclosed.   */
      if((specid = Xamine_MatchSpecName(text)) < 0) {
	XtFree(text);
	return 0;
      }
    }
  }
  XtFree(text);

  /* The number of the spectrum is now in specid.  We must make sure this  */
  /* is a legal spectrum number, that the selected spectrum is 1d and that */
  /* the spectrum is compatible with it.                                   */

  if( (specid <= 0) || (specid > xamine_shared->getspectrumcount()))
    
    return 0;			/* bad spectrum number. */

  if(IsCompatible(specid))
    return specid;
  else
    return -1;    
}

/*
** Functional Description:
**   SuperPrompt::OkAction:
**     This method is called when the OK button is thwacked.
** Formal Parameters:
**   The callback data.
** Returns:
**   True if caller can dismiss the dialog.
*/
Boolean SuperPrompt::OkAction(XtPointer cd)
{
  int superid = Match(cd);
  win_attributed *att = Xamine_GetSelectedDisplayAttributes();
  if(att == NULL) {
    Xamine_error_msg(this, "Selected pane does not contain a 1-d spectrum");
    return True;
  }

  if(!IsCompatible(att->spectrum())) { /* Spectrum should be self compatible. */
    Xamine_error_msg(this, "Selected pane does not contain a 1-d spectrum");
    return True;
  }

  if(superid > 0) {
    if(IsSuperimposed(att, superid)) {
      Xamine_error_msg(this, "Spectrum is  already in the superposition list");
      return False;
    }
    else {
      Xamine_Superimpose(superid);
      Xamine_EnableUnsuperimpose();
      return True;
    }
  }
  if(superid < 0) {
    Xamine_error_msg(this,
     "The spectrum selected cannot be superimposed on the selected pane");
    return False;
  }
  if(superid == 0) {
    Xamine_error_msg(this,
		     "You have selected a nonexistent spectrum.");
    return False;
  }
  Xamine_error_msg(this,
     "BUG -- Xamine impossible code path reached in SuperPrompt::OkAction");
  return False;
}

/*
** Functional Description:
**   SuperPrompt::ApplyAction
**     This method is called when the OK button is thwacked.
** Formal Parameters:
**   The callback data.
** Returns:
**   True if caller can dismiss the dialog.
*/
Boolean SuperPrompt::ApplyAction(XtPointer cd)
{
  int superid = Match(cd);
  win_attributed *att = Xamine_GetSelectedDisplayAttributes();
  if(att == NULL) {
    Xamine_error_msg(this, "Selected pane does not contain a 1-d spectrum");
    return True;
  }

  if(!IsCompatible(att->spectrum())) { /* Spectrum should be self compatible. */
    Xamine_error_msg(this, "Selected pane does not contain a 1-d spectrum");
    return True;
  }

  if(superid > 0) {
    if(IsSuperimposed(att, superid)) {
      Xamine_error_msg(this, "Spectrum is already in the superposition list");
      return False;
    }
    else {
      Xamine_Superimpose(superid);
      Xamine_EnableUnsuperimpose();
      return False;
    }
  }
  if(superid < 0) {
    Xamine_error_msg(this,
     "The spectrum selected cannot be superimposed on the selected pane");
    return False;
  }
  if(superid == 0) {
    Xamine_error_msg(this,
		     "You have selected a nonexistent spectrum.");
    return False;
  }
  Xamine_error_msg(this,
   "BUG -- Control reached impossible point in SuperPrompt::ApplyAction");
  return True;

}


/*
** Functional Description:
**    SuperPrompt::SetList:
**       This is really the most important method.  It produces the list
**       of spectra which are elligible to be superimposed on the currently
**       selected window. If the current spectrum cannot be superimposed on
**      (it's a 2-d), then we emit a dialog and unmanage the prompter.
**      If there are no elligible spectra then again we emit an error
**      dialog and unmanage the dialog.
*/
void SuperPrompt::SetList()
{



  /* First get the spectrum ID of the current pane and make sure it's a 1-d */

  win_attributed *att = Xamine_GetSelectedDisplayAttributes();

  if(att == NULL) {
    Xamine_error_msg(theParent, "Selected pane does not contain a 1-d spectrum");
    Hide();
    return;
  }

  int target          = att->spectrum();
  spec_type tgttype   = Xamine_SpectrumType(target);
  if( (tgttype != onedlong) &&
      (tgttype != onedword)) {
    Xamine_error_msg(theParent, "Selected spectrum not 1-d");
    Hide();
    return;
  }

  /* Now we count the number of spectra that are 1-d's of conforming size 
  */
  int compatibles = 0;
  int i;
  for(i = 1; i <= xamine_shared->getspectrumcount(); i++) {
    if(IsCompatible(i))  compatibles++;
  }
  if(compatibles == 0) {
    Xamine_error_msg(theParent, 
         "There are no spectra that can be superimposed on the selected one");
    Hide();
    return;
  }
  /* Build the list of compatible spectrum names:    */
  /* we only put in the first 'compatibles' spectra, if more have popped into
  ** existence since the count, they just won't make the cut.
  */

  typedef spec_title *spec_titlep;
  spec_title name;
  spec_titlep *name_list = new spec_titlep[compatibles];
  for(i = 0; i < compatibles; i++) name_list[i] = NULL;

  /* Now we only allow spectra that are compatible and that have not already
  ** been superimposed on the list.  IsSuperimposed counts the displayed spec
  ** as always superimposed so it will not appear.
  */

  int slots_filled = 0;
  for(i = 1; 
      (i <= xamine_shared->getspectrumcount()) && (slots_filled < compatibles);
      i++) {
    if(IsCompatible(i) && !IsSuperimposed(att, i)) {
      name_list[slots_filled] = (spec_title *)new spec_title;
      xamine_shared->getname(name, i);
      name[sizeof(name)-10] = '\0';                  /* Force truncation */
      sprintf((char *)name_list[slots_filled], "[%05d]  %s", i, name);
      slots_filled++;
    }
  }

  /* At this time, slots_filled is the name count and 
  ** name_list is a list of name pointers.
  */
  if(slots_filled == 0) {
    Xamine_error_msg(theParent, "There are no compatible spectra to superimpose");
    Hide();
  }
  else {
    SetSelectionList((Cardinal)slots_filled, (char **)name_list);
  }

  /* Free storage.   */

  for(i = 0; i < slots_filled; i++) {
    delete [] name_list[i];	/* Delete an individual name... */
  }
  delete [] name_list;		/* Delete the list of pointers. */


}

/*
** Functional Description:
**   SuperPrompt::SuperPrompt:
**     This function is a constructor for the superposition dialog.
**     The dialog is just an ordinary list dialog with a few specialized
**     callback methods that get planted at dialog creation time.
** Formal Parameters;
**    char *name:
**      Name of the dialog.
**    XMWidget &parent:
**      Parent widget object.
*/
SuperPrompt::SuperPrompt(char *name, XMWidget &parent)
			:
       XMSelectionDialog(name, parent, "Spectrum: ")
{

  /* Plant the callbacks: */
  
  SetAttribute(XmNautoUnmanage, (XtArgVal)False);

  AddOkCallback(SuperPrompt_Ok, (XtPointer)this);

  AddApplyCallback(SuperPrompt_Apply, (XtPointer)this);
  AddCancelCallback(SuperPrompt_Cancel, (XtPointer)this);


  GetHelpButton()->AddCallback(Xamine_display_help, &help);


}

/*
** Functional Description:
**    Xamine_GetSuperposition:
**      This function produces a superposition prompt display (if necessary)
**      and manages it.  After this is done, the object itself takes over and
**      does all of the work.
** Formal Parameters:
**   XMWidget *parent:
**      Parent widget to use when initially creating the widget.
**   XtPointer u:
**      User data, currently ignored.
**   XtPointer c:
**      Push button call data (currently ignored).
*/
void Xamine_GetSuperposition(XMWidget *parent, XtPointer u, XtPointer c)
{
  /* If needed, create the dialog widget */

  if(dialog == NULL) {
    theParent = parent;
    dialog = new SuperPrompt(const_cast<char*>("Superpose"), *parent);
  }
  /* Compute the list of spectra and display the dialog:*/

  dialog->Show();		/* Do this first so that SetList can pop it */
  dialog->SetList();		/* down.  */


}

/*
** class methods for UnSuperPrompt:
*/
/*
** Functional Description:
**    UnSuperPrompt::UnSuperPrompt
**        Create an unsuperposition dialog.  We call parent class constructor
**        and setup the list.
*/
UnSuperPrompt::UnSuperPrompt(char *n, XMWidget &parent) :
  XMSelector(n, parent, const_cast<char*>("Remove Superposition of: "))
{
  RestrictChoices();		// Require choice from list.
  SetupList();
}
/*
** Functional Description:
**    This function sets up the superposition list box.
*/
void UnSuperPrompt::SetupList()
{
  win_attributed *atr = Xamine_GetSelectedDisplayAttributes();
  win_1d         *a1  = (win_1d *)atr;
  SuperpositionList list = a1->GetSuperpositions();
  SuperpositionListIterator iter(list);
  Superposition s;
  int          ns = list.Count(); // Get number of superpositions.

  if( ns > 0) {			// Pane has superpositions.
    /* Collect the names of the superimposed spectra: */
    
    char        *names[MAX_SUPERPOSITIONS];
    int i;
    for(i = 0; i < MAX_SUPERPOSITIONS; i++)
      names[i] = NULL;		// Null the pointers.
    i = 0;
    do {
      s       = iter.Next();	// Get a superposition.  
      int num = s.Spectrum();	// Get the spectrum number associated with it.
      names[i] = new spec_title;
      xamine_shared->getname(names[i], num);
      i++;
    } while(!iter.Last());
    
    /* set them in the little list window. */
    
    SetSelectionList(ns, (char **)names);
    for(i = 0; i < MAX_SUPERPOSITIONS; i++)
      if(names[i]) delete []names[i];
  }
  else {			// Pane has no superpositions...
    SetSelectionList(ns, (char **)NULL);
  }
  
}

/*
** Functional Description:
**   UnSuperPrompt::Match:
**     This function determines which spectrum has been chosen by the user.
** Formal Parameters:
**    XtPointer cd:
**      Pointer to the callback data which is really an 
**      XmSelectionBoxCallbackStruct describing the state of the selection
**      box at the time the selection has been confirmed.
** Returns:
**    0 - No spectrum matched, or invalid spectrum string.
**   >0 - Spectrum matched and and this is its number.
*/
int UnSuperPrompt::Match(XtPointer cd)
{
  XmSelectionBoxCallbackStruct *cbs = (XmSelectionBoxCallbackStruct *)cd;
  char *text;
  int  specid;

  /* Get the selection string as ASCIZ */


  if(!XmStringGetLtoR(cbs->value, const_cast<char*>(XmSTRING_DEFAULT_CHARSET),  &text)) {
    return 0;
  }

  /* Decode the spectrum string.  The spectrum string is either a pure number,
  ** a bracket enclosed number or a spectrum name.  We attempt to decode in that
  ** order.
  */

  if(sscanf(text, "%d", &specid) != 1) {      /* It was not a  number.  */
    if(sscanf(text, "[%3d]", &specid) != 1) { /* Not bracket enclosed.   */
      if((specid = Xamine_MatchSpecName(text)) < 0) {
	XtFree(text);
	return 0;
      }
    }
  }
  XtFree(text);

  /* The number of the spectrum is now in specid.  We must make sure this  */
  /* is a legal spectrum number, that the selected spectrum is 1d and that */
  /* the spectrum is compatible with it.                                   */

  if( (specid <= 0) || (specid > xamine_shared->getspectrumcount()))
    
    return 0;			/* bad spectrum number. */

  return specid;

}


/*
** Functional Description:
**   UnSuperPrompt::Perform:
**       This method is called in response to the OK or the Apply button.
**        We try to remove the selected superposition:
** Formal Paramters:
**   XMWidget *wid:
**     The button which triggered us.
**   XtPointer cd:
**      Call data for the button.
**   int reason:
**      Reason for the call could be:
**        XmCR_OK   - OK button hit and selection matches list.
**        XmCR_APPLY- Apply button hit and selection is in the list.
**        XmCR_NO_MATCH - eitehr Ok or Apply hit, but no match (remember
**                    the user can type in.
**   XmString value:
**      The selection that was chosen by the user.
**   int size:
**       # chars in the user's selection text.
**
** Returns:
**    TRUE - selection was successfully applied and OK can pop us down.
**    FALSE- Selection failed for some reason and OK should keep us up.
*/
Boolean UnSuperPrompt::Perform(XMWidget *wid, XtPointer cd, int reason,
			      XmString value, int size)
{
  if(reason == XmCR_NO_MATCH) {
    Xamine_error_msg(wid, "Please chose a selection from the list");
    return False;
  }
  else {
    /* Get the superposition list and a superposition iterator etc. so that
    ** we can compare the user's selection to the the selected superposition
    ** list.  Note that if the selected spectrum is not a 1d, then we just
    ** dismiss ourself without doing anything since the user cheated...
    ** and swapped spectra on us.... ditto if there's no match in the list.
    */
    win_attributed *wat = Xamine_GetSelectedDisplayAttributes();
    if(!wat->is1d()) {		// User cheated on us...
      this->UnManage();
      return True;
    }
    win_1d *w1 = (win_1d *)wat;	// Now we have access to the superpos list.
    SuperpositionList &list = w1->GetSuperpositions();
    /*
    **   Defend against a null superposition list.  Should not be 
    **   necessary at this point but we've burned ourselves before so
    **   what's the harm:
    */

    if(list.Count() == 0) {
      Xamine_error_msg(wid, "This spectrum has no superpositions");
      return True;
      this->UnManage();
    }

    SuperpositionListIterator iter(list); // Create the iterator.
    do {
      spec_title title;
      int        spno;
      XmString   XmStitle;
      Superposition s = iter.Next();
      spno = s.Spectrum();

      XmStitle = XmStringCreate(const_cast<char*>(xamine_shared->getname(title, spno)),
				const_cast<char*>(XmSTRING_DEFAULT_CHARSET));
      if(XmStringCompare(XmStitle, value)) { // Bingo, a match.
	XmStringFree(XmStitle);		     // Free string storage.
	w1->RemoveSuperposition(spno);	     // Delete superposition.
	SetupList();			     // This changes list.
	Xamine_ChangedWindows();
	Xamine_RedrawSelectedPane();	     // Schedule an update for pane.
	if(list.Count() == 0) {		     // Must disable us...
	  this->UnManage();		     // Disappear...
          Xamine_DisableUnsuperimpose();
        }
	return True;
      }
      XmStringFree(XmStitle);	// Free string storage.
    }while(!iter.Last());

    /* If we fell out of the loop, the user cheated on us:  */

    this->UnManage();
    return True;
 
  }
}

/*
** Functional Description:
**    UnSuperPrompt::ApplyCb:
**       This function is called when the Apply button is hit.
**       We do a bit of preprocessing for the Perform function.  In 
**       particular, if no valid selection has been made from the list,
**       we pass Perform XM_CR_NOMATCH instead of XM_CR_APPLY which got
**       passed to us.
** Formal Parameters:
**    XMWidget *wid:
**       Pointer to parent widget object of the button struck.
**    XtPointer userd:
**       User data which is not used
**    XtPointer cd:
**       Call data which is a pointer to a widget call record which includes
**       among other things the XmString of the selection.
*/
void UnSuperPrompt::ApplyCb(XMWidget *wid, XtPointer userd, XtPointer cd)
{
  XmSelectionBoxCallbackStruct *calldata = (XmSelectionBoxCallbackStruct *)cd;
  int m = Match(cd);
  int reason = m ? calldata->reason : XmCR_NO_MATCH;

  Perform(wid, userd,
	  reason, calldata->value, calldata->length);

}

/*
** Functional Description:
**   Xamine_UnSuperimpose:
**      Remove a superimposition.
** Formal Parameters:
**     The usual callback parameters which are all ignored.
*/
void Xamine_UnSuperimpose(XMWidget *wid, XtPointer u, XtPointer c)
{
  if(!undialog) {
    undialog = new UnSuperPrompt(const_cast<char*>("UnSuperimpose"), *wid);
  }
  undialog->SetupList();
  undialog->Manage();
}
