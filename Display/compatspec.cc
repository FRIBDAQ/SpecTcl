/*
** Facility:
**   Xamine - NSCL display program X11/Motif support.
** Abstract:
**   compatspec.cc:
**    Contains methods for the compatible spectrum list composite widget.
**    We also have a utiliity function to test the compatibility of two
**    spectra.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State Unversity
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo = "@(#)compatspec.cc	8.1 6/23/95 ";


/*
** Include files:
*/
#include <stdio.h>

#include "compatspec.h"
#include "dispshare.h"
#include "panemgr.h"
/*
** User defined types:
*/

typedef char *cstring;

/*
** External references: 
*/
extern spec_shared *xamine_shared;


/*
** Functional Description:
**   Xamine_IsCompatible:
**    This local boolean function determines if two spectra are compatible
**    for the purposes of copying gates.  Compatibility is defined as
**    the same dimensionality and same number of channels on all axes.
** Formal Parameters:
**    int spec1,spec2:
**      The two spectra to check for compatibility
** Returns:
**   True  - compatible
**   False - incompatible
*/
Boolean Xamine_IsCompatible( int spec1, int spec2)
{
  spec_type t1 = xamine_shared->gettype(spec1);
  spec_type t2 = xamine_shared->gettype(spec2);

  switch(t1) {
  case onedlong:
  case onedword:
    if((t2 == onedlong) || (t2 == onedword)) {
      return (xamine_shared->getxdim(spec1) == xamine_shared->getxdim(spec2));
    }
    else {
      return False;
    }
    break;
  case twodword:
  case twodbyte:
    if((t2 == twodword) || (t2 == twodbyte)) {
      return ((xamine_shared->getxdim(spec1) == xamine_shared->getxdim(spec2)) 
	      &&
	      (xamine_shared->getydim(spec1) == xamine_shared->getydim(spec2))
	      );
    }
    else {
      return False;
    }
    break;
  default:
    return False;
    break;
  }
}

/*
** Functional Description:
**   SelAll_relay:
**      Called when the select all button is pushed.  We relay to the 
**      SelectAll method.
** Formal Parameters:
**   XMWidget *button:
**     The push buton widget that called us.
**   XtPointer cd:
**     Client data. In this case the object that's called us.
**   XtPointer ud:
**     call data which is unused but is a push buton callback data struct.
*/
void SelAll_relay(XMWidget *button, XtPointer cd, XtPointer ud) 
{
  CompatibleSpectrumList *o = (CompatibleSpectrumList *)cd;
  o->SelectAll();
}

/*
** Functional Description:
**   CompatibleSpectrumList::CompatibleSpectrumList:
**    This function is called to instantiate a compatible spectrum list.
**    We instantiate the form that contains the composite widget we're creating
**    and put inside a stacked Label, scrolled list and pushbutton.
** Formal Parameters:
**    char *name:
**       The name of the composite widget this will be given to the form.
**    XMWidget *parent:
**       Parent of the widget.
**    char *title_string:
**       A title string to be placed at the top of the list.
**    int spectrum_number:
**       The spectrum number that we're building compatible spectra of.
*/
CompatibleSpectrumList::CompatibleSpectrumList(char     *name,
					       XMWidget *parent,
					       char     *title_string,
					       int      spectrum_number) :
       XMForm(name, *parent) 
{
  /*  Fill in the attributes of the object: */

  spectrum   = spectrum_number;
  title      = new XMLabel("Title", *this, title_string);
  speclist   = new XMScrolledList("List", *this);
  speclist->SetSelectionPolicy(XmEXTENDED_SELECT);
  select_all = new XMPushButton("Button", *this, SelAll_relay, this);
  select_all->Label("Select All");

  speclist->SetRows(10);

  /* Glue the objects together on the form: */

  SetTopAttachment(*title,   XmATTACH_FORM);
  SetLeftAttachment(*title,  XmATTACH_FORM);
  SetRightAttachment(*title, XmATTACH_FORM);

  Widget sw = speclist->GetScrolledWindow();
  SetTopAttachment(sw,   XmATTACH_WIDGET);
  SetTopWidget(sw,       *title);
  SetLeftAttachment(sw,  XmATTACH_FORM);
  SetRightAttachment(sw, XmATTACH_FORM);

  SetTopAttachment(*select_all,   XmATTACH_WIDGET);
  SetTopWidget(*select_all,       sw);
  SetBottomAttachment(*select_all,XmATTACH_FORM);
  SetLeftAttachment(*select_all, XmATTACH_POSITION);
  SetLeftPosition(*select_all,   4);
  
}
/*
** Functional Description:
**   CompatibleSpectrumList::~CompatibleSpectrumList:
**    Destructor for the compatible spectrumlist compound widget.
*/
 CompatibleSpectrumList::~CompatibleSpectrumList()
{
  delete title;
  delete speclist;
  delete select_all;
}

/*
** Functional Description:
**   CompatibleSpectrumList::NewSpectrum:
**      Changes the spectrum that we're interested in.
** Formal Parameters:
**    int newspec:
**      New spectrum number.
*/
void CompatibleSpectrumList::NewSpectrum(int newspec)
{
  spectrum = newspec;
  Update();
}

/*
** Functional Description:
**   CompatibleSpectrumList::TitleString:
**     Sets a new title string.
** Formal Parameters:
**   char *newtitle:
**     new title string.
*/
void CompatibleSpectrumList::TitleString(char *newtitle)
{
  title->SetLabel(newtitle);
}

/*
** Functional Description:
**   CompatibleSpectrumList::Manage:
**     Put everything under geometry management:
*/
void CompatibleSpectrumList::Manage()
{

  /* Make the pieces visible */

  title->Manage();
  speclist->Manage();
  select_all->Manage();
  XMForm::Manage();
}

/*
 ** Functional Description:
 **  CompatibleSpectrumList::UnManage:
 **    Remove all from geometry management.
 */
void CompatibleSpectrumList::UnManage()
{
  title->UnManage();
  speclist->UnManage();
  select_all->UnManage();
   XMForm::UnManage();
}

/*
 ** Functional Description:
 **   CompatibleSpectrumList::GetSelections:
 **     Get the set of selections.  The set of selections are returned as
 **     a pointer to an array of character strings.  The caller is responsible
 **     for releasing both the character strings and the array of pointers
 **     when done with them.
 **     Allocation failures will result in NULL Pointers.
 ** Formal Parameters:
 **   char ***items:
 **     On return points to a table of string pointers each of which points
 **     to the ASCIIZ of a selected string.  Null values and entries imply
 **     storage allocation failure and should be checked for by the caller.
 **     it is also the caller's responsibility to release the strings and
 **     the table of string pointers after use.
 **   int *count:
 **     On return contains the number of items in the string table described
 **     above (number of selected items)
 */
void CompatibleSpectrumList::GetSelections(char ***items, int *count)
{
  *count = speclist->GetSelectedListCount();
  XmStringTable selections = speclist->GetSelectedItems();
  if(selections == NULL) {	/* Couldn't get the string table. */
    *items = NULL;
    return;
  }
  *items = new cstring[*count];
  char **item = *items;
  for(int i = 0; i < *count; i++) {
    XmStringGetLtoR(selections[i], XmSTRING_DEFAULT_CHARSET, item);
    item++;
  }
}
/*
 ** Functional Description:
 **   Xamine_FreeStrings:
 **      Free allocated string tables.
 ** Formal Parameters:
 **    char **items:
 **      The string table to release.
 **    int count:
 **      The number of items to release.
 */
void Xamine_FreeStrings(char **items, int count)
{
  /* Note, the individual strings are assumed not to have been
   ** new'd, but the table iteslef is new'd.
   */
  if(items != NULL) {
    for(int i = 0; i < count; i++) {
      if(items[i] != NULL) XtFree(items[i]);
    }
    delete []items;
  }
}

/*
** Functional Description:
**    CompatibleSpectrumList::Clear:
**      Clears the spectrum list.
*/
void CompatibleSpectrumList::Clear()
{
  speclist->ClearItems();
}
/*
 ** Functional Description:
 **   CompatibleSpectrumList::Update:
 **     This function updates the list of spectra in the scrolled list
 **     widget.  Spectra which survive the update and are selected will
 **     remain selected.
 ** Formal Parameters:
 **   NONE -- However the spectrum attribute is an implicit input to this
 **           method as is the pointer to the Xamine shared memory region.
 */
void CompatibleSpectrumList::Update()
{
  char **selected;
  int    selcount;
  char   specname[100];
  spec_title rawname;
  
  GetSelections(&selected, &selcount); /* Get selection list. */
  speclist->ClearItems();	       /* Empty the list. */
  
  /* Now run through the spectra available for compatibles:  */
  
  speclist->SetSelectionPolicy(XmMULTIPLE_SELECT);
  for(int i = 0; i < DISPLAY_MAXSPEC; i++) {
    if(Xamine_IsCompatible(spectrum, i) && (spectrum != i)) {
      sprintf(specname, "[%3d] %s",
	      i,
	      xamine_shared->getname(rawname, i));
      speclist->AddItem(specname); /* Add spectrum to list */
      if(selected) {
	for(int s = 0; s < selcount; s++) {
	  if(selected[s]) {
	    if(strcmp(specname, selected[s]) == 0) {
	      speclist->SelectItem();
	    }
	  }
	}
      }
    }
  }
  speclist->SetSelectionPolicy(XmEXTENDED_SELECT);
  
}

/*
 ** Functional Description:
 **    CompaitbleSpectrumList::SelectAll:
 **      This function selects all spectra in the compatible spectrum
 **      list.  No implicit update is done so what the user sees is
 **      what he/she gets.
 */
void CompatibleSpectrumList::SelectAll()
{
  int n = speclist->GetListCount();
  
  speclist->SetSelectionPolicy(XmMULTIPLE_SELECT);
  for(int i = 1; i <= n; i++) {
    speclist->SelectItem(i);
  }
  speclist->SetSelectionPolicy(XmEXTENDED_SELECT);
}

/*
 ** Functional Description:
 **  DisplayedCompatibleSpectra::DisplayedCompatibleSpectra:
 **     Constructor for a class which displays compatible spectra but only
 **     those currently displayed in a pane.
 **  Formal Parameters:
 **    char *name:
 **      Name of the top of the compound widget tree.
 **    XMWidget *parent:
 **      parent of the top of the compound widget tree.
 **    char *title_string:
 **      Title string displayed at top of pop up dialog shell.
 **    int spectrum_number:
 **      Number of the spectrum for which we're displaying compatible spectra.
 */
 DisplayedCompatibleSpectrumList::DisplayedCompatibleSpectrumList(char *name,
								  XMWidget *parent,
								  char *title_string,
								  int spectrum_number) :
  CompatibleSpectrumList(name, parent, title_string, spectrum_number)
{
}
/*
** Destructor:
*/
 DisplayedCompatibleSpectrumList::~DisplayedCompatibleSpectrumList()
{}				/* Just get the parent destructor called. */


/*
** Functional Description:
**   DisplayeCompatibleSpectrumList::Upate:
**     Updates the current spectrum list displayed.  Note that we try to keep
**     the currently selected set.
*/
void DisplayedCompatibleSpectrumList::Update()
{
  char **selected;
  int    selcount;
  char   specname[100];
  spec_title rawname;
  int    rows = Xamine_Panerows();
  int    cols = Xamine_Panecols();

  GetSelections(&selected, &selcount); /* Get selection list. */
  speclist->ClearItems();	       /* Empty the list. */


  /* Now run through the spectra available for compatibles:  */

  speclist->SetSelectionPolicy(XmMULTIPLE_SELECT);

  /*  Run through the panes:   */


  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      win_attributed *atr = Xamine_GetDisplayAttributes(r,c);
      int spec;
      if(atr != NULL) {
	spec = atr->spectrum();
	if(Xamine_IsCompatible(spec, spectrum)) {
	  sprintf(specname, "(%3d,%3d) [%3d] %s",
		  c,r,spec,
		  xamine_shared->getname(rawname, spec));
	  speclist->AddItem(specname); /* Add spectrum to list */
	  if(selected) {
	    for(int s = 0; s < selcount; s++) {
	      if(selected[s]) {
		if(strcmp(specname, selected[s]) == 0) {
		  speclist->SelectItem();
		}
	      }
	    }
	  }
	}
      }
    }
  }

  speclist->SetSelectionPolicy(XmEXTENDED_SELECT);
  Xamine_FreeStrings(selected, selcount);

}

/*
** Functional Description:
**   DisplayedCompatibleSpectrumList::NewSpectrum:
**      Changes the spectrum that we're interested in.
** Formal Parameters:
**    int newspec:
**      New spectrum number.
*/
void DisplayedCompatibleSpectrumList::NewSpectrum(int newspec)
{
  spectrum = newspec;
  Update();
}
