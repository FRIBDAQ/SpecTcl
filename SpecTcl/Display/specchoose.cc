/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**   specchoose.cc  - This file contains code which allows the user to choose 
**                    a spectrum from the list of available spectra.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccinfo="@(#)specchoose.cc	8.3 7/23/98 \n";


/*
** Include files:
*/
#include <stdio.h>
#include <Xm/Xm.h>
#include <ctype.h>
#include <stdlib.h>

#include "XMDialogs.h"

#include "dispshare.h"
#include "specchoose.h"
#include "helpmenu.h"
#include "errormsg.h"
#include "spcdisplay.h"
/*
** #define'd constants
*/
#define XAMINE_VISIBLE_SPECTRUM_CHOICES  50

/*
** External References:
*/

extern spec_shared *xamine_shared;

/*
**  LOCAL STORAGE:
*/

/*									    */
/* The storage cells below contain the spectrum list information	    */

static  int num_defined;
static  char **defined_list;

/*
** Storage cell below contain the callback handles for cancellation
*/
static Callback_data *cancel = NULL, 
                     *ok     = NULL,
                     *apply  = NULL,
                     *nomatch= NULL;

/*    The selection dialog widget which we generate and cache: */

static XMSelectionDialog *SpectrumChooser = NULL;

/*
** Help text and help structure for the HELP button on the chooser:
*/

static char *help_text[] = {
  "  This dialog allows you to choose a spectrum.  The list at the top of the\n",
  "chooser box lists all available spectra.  If you double click on one of the\n",
  "items in this list, the corresponding spectrum will be selected. If you know\n",
  "the name of the spectrum you want, then you can also type it in to the text\n",
  "input box in the middle section of the dialog. If you know the number\n", 
  "of the spectrum, that can also be typed in to the typein box.\n\n",
  "   The buttons at the bottom of the dialog have the following meanings:\n\n",
  "      OK      - Accept the spectrum name in the type-in box and pop-down\n",
  "                this dialog.\n",
  "      Apply   - Accept the spectrum name in the type-in box, and leave the\n",
  "                dialog up.\n",
  "      Cancel  - Pop this dialog down without making any selection\n",
  "      Help    - Display this message\n\n",
  "Typing a carriage return has the same effect as clicking on the highlighted\n",
  "button.",
  NULL
  };

static Xamine_help_client_data help = { "Spectrum_selection_help", NULL, help_text};


/*
** Functional Description:
**   StripName:
**     This function strips leading and trailing whitespace from a 
**     character string which is a spectrum name.
**     These strings must be handled carefully since they are not gaurenteed
**     to be NULL or blank terminated.
** Formal Parameters:
**    char **s, char **e:
**      will point to the first and last non-whitespace characters
**      on return.  If there are no non whitespace characters, then
**      both strings will be NULL.
**   spec_title t:
**      The text title.
*/
static void StripName(char **s, char **e, spec_title t)
{
  /* Hunt for the first non blank, when we find it, scan for NULL.
  */
  char *p = t;
  *s      = NULL;
  for(int i = 0; i < sizeof(spec_title); i++) {
    if(*p == '\0') 
      break;	                           /* Hit the end. */
    if(!isspace(*p) && *s == NULL) *s = p; /* Hit first non space. */
    p++;
  }
  /* At this point:
  **   p -> last character or last cell in the string.
  **  *s -> First non blank character or NULL If there isn't one.
  */
  if(*s == NULL) {		/* Non nonblank chars... */
    *e = NULL;
    return;
  }
  while(p != t) {		/* Scan Backwards for non space. */
    if(*p != '\0') {		/* Ignore the terminating zero */
      if(!isspace(*p)) {	/* Found non-whitespace.       */
	*e = p;			/* Set the end character.      */
	return;
      }
    }
    p--;
  }
}

/*
** Functional Description:
**  CaseBlindMatch:
**    This local function performs a case blind compare of two stripped strings.
**    Stripped strings are defined by start and end pointers.
**    The start pointer points to the first non blank character and
**    The end pointer points to the last non blank character.
**    The pointers are NULL for purely blank strings.
**    These pointers are returned from StripName.
** Formal Parameters:
**   char *s1, *e1:
**      Start and end pointers of first string.
**   char *s2, *e2:
**      Start and end ponters of second string.
** Returns:
**   TRUE for equality, FALSE otherwise.
*/
static Boolean CaseBlindMatch(char *s1, char *e1,
			      char *s2, char *e2)
{
  if(s1 == NULL)		/* First string NULL */
    return (s2 == NULL);	/* Only two null strings match then. */

  if(s2 == NULL) return FALSE;	/* Since we already know s1 is nonnull. */

  /* Print the strings for debugging purposes. */


  if( (e1-s1) != (e2-s2) )	/* Lengths must match. */
    return False;

  /* Strings are same length, so we must do the compare: */

  while( s1 <= e1 ) {
    if(toupper(*s1) != toupper(*s2) ) return False; /* Mismatch. */
    s1++; 
    s2++;
  }

  return True;			/* Match. */
}

/*
** Functional Description:
**   MatchSpecName:
**     This function matches a spectrum name against the set that's in Xamine
**     shared region.  The match is case blind, and ignores leading and
**     trailing blanks in both the source and target name.
**     Note that if there are several spectra in the Xamine shared region
**     with the same name we'll match the lowest numbered one.
** Formal Parameters:
**   char *name:
**     The name of the spectrum we'd like to match.
** Returns:
**    int:
**      Number of the matched spectrum or 0 if not found.
*/
int Xamine_MatchSpecName(char *name)
{
  int id;
  if((id = xamine_shared->getspecid(name)) > 0)
    return id;

  /* Locate the front and back character positions of the input name: */

  char *n1, *n2;
  StripName(&n1, &n2, name);

  /* Loop over all defined spectra looking for matches: */

  for(int i = 1; i <= DISPLAY_MAXSPEC; i++) {
    if(xamine_shared->gettype(i) != undefined) {
      spec_title spname;
      char *s1, *s2;
      StripName(&s1, &s2, (char*)xamine_shared->getname(spname, i));
      if(CaseBlindMatch(n1,n2, s1,s2)) 
	return i;
    }
  }
 
  return 0;			/* Undefined spectrum.  */

}

/*
** Functional Description:
**   Callback_handler:
**     This function handles the callbacks for the Ok, Apply, and Cancel buttons
**     of the spectrum chooser.  If the cancel button is pressed, we just
**     hide the dialog. If the OK or Apply button is pressed, then we 
**     determine which spectrum was selected and pass it on to the user's callback.
**     If the reason was OK, then the dialog is hidden.
** Formal Parameters:
**    XMWidget *w:
**      The widget object of the selection box.
**    XtPointer userd:
**       Actually a function pointer to the user's callback.
**    XtPointer calld:
**       Actually a pointer to an XmSelectionBoxCallbackStruct
*/
void Callback_handler(XMWidget *w, XtPointer userd, XtPointer calld)
{
  XMSelectionDialog *choice          = (XMSelectionDialog *)w;
  void  (*usercallback)(int id)      = (void (*)(int))userd;
  XmSelectionBoxCallbackStruct *info = (XmSelectionBoxCallbackStruct *)calld;
  char error_msg[512];
  
  char *spectrum_name;
  int  specid;

  /*
  ** Fan out depending on the callback reason:
  */

  switch(info->reason) {
  case XmCR_CANCEL:		/* Cancel button... hide the dialog, return */
    choice->Hide();
    return;
  case XmCR_NO_MATCH:		/* No match... Could be a numeric type-in   */
    if(!XmStringGetLtoR(info->value, XmSTRING_DEFAULT_CHARSET, &spectrum_name)) {
      Xamine_error_msg(choice, 
		       "Unable to determine spectrum name from selection");
      return;
    }
    sprintf(error_msg, 
	    "No such spectrum '%s'\nPlease choose one that is on the list\n",
	    spectrum_name);

   check_numeric:
    if(isdigit(spectrum_name[0])) {   /* Numeric type-in */
      specid = atoi(spectrum_name);
    numeric_spectrum:
      if((specid <= 0) || (specid > DISPLAY_MAXSPEC)) {
	Xamine_error_msg(choice,
			 "Invalid spectrum number\nPlease choose one from the list");
	choice->Show();
	XtFree(spectrum_name);
	return;
      }
      if(Xamine_SpectrumType(specid) == undefined) {
	Xamine_error_msg(choice,
			 error_msg);
	choice->Show();
	XtFree(spectrum_name);
	return;
      }
      (*usercallback)(specid);
      XtFree(spectrum_name);
      return;
    }
    else {
      specid = Xamine_MatchSpecName(spectrum_name); /* Match the spectrum name. */
      if(specid > 0)
	goto numeric_spectrum;	/* A bit spaghetti, but that code handles specid.  */
#ifdef LESSTIF_VERSION
      specid = Xamine_GetSpectrumId(spectrum_name);
      if(specid > 0) {
	XtFree(spectrum_name);
	(*usercallback)(specid);
	return;
      }
#endif
    }
    XtFree(spectrum_name);
    choice->Show();
    Xamine_error_msg(choice,
		     error_msg);
    return;
  case XmCR_OK:
  case XmCR_APPLY:
    if(!XmStringGetLtoR(info->value, XmSTRING_DEFAULT_CHARSET, &spectrum_name)) {
      Xamine_error_msg(choice, 
		       "Unable to determine spectrum name from selection");
      return;
    }
    specid = Xamine_GetSpectrumId(spectrum_name);
    if(specid == -1) {
      goto check_numeric;
    }
    (*usercallback)(specid);
    XtFree(spectrum_name);
    if(info->reason == XmCR_OK)
	choice->Hide();		/* OK will pop down the dialog... */
    return;
  }
}

/*
** Functional Description:
**   Xamine_ChooseSpectrum:
**      This function allows the user to select a spectrum for a generic operation.
**      The spectrum is chosen by presenting the user with a chooser dialog.
**      The dialog is cached to improve performance of subsequent activations.
** Formal Parameters:
**   XMWidget w:
**      The widget which resulted in this callback.. it will be used as the parent
**      widget for the dialog on first activation.
**   XtPointer clientd:
**      Client data.  In this case, the client data is actually a function which
**      will be called when the spectrum has been successfully chosen.  It will
**      be passed into the Ok/Apply callback routine.
**   XtPointer calld:
**      Callback data.  We ignore this since the call can come from any generic
**      sort of widget.
*/
void Xamine_ChooseSpectrum(XMWidget *w, XtPointer clientd, XtPointer calld)
{
  Xamine_ChooserCbData *callback_data = (Xamine_ChooserCbData *)clientd;

  num_defined = Xamine_GetSpectrumList(&defined_list);
  if(!num_defined) {
    Xamine_error_msg(w, "There are no spectra defined to choose from");
    return;
  }
  /*
  ** IF necessary, the spectrum chooser is created
  */

  if(!SpectrumChooser) {
    SpectrumChooser = new XMSelectionDialog("Spectrum_Choice",
					    *w, "Type Spectrum name Here:");
    SpectrumChooser->RestrictChoices();
    SpectrumChooser->SetVisibleItemCount(XAMINE_VISIBLE_SPECTRUM_CHOICES);
    SpectrumChooser->GetHelpButton()->Enable();
    SpectrumChooser->GetHelpButton()->AddCallback(Xamine_display_help, 
						  (XtPointer)&help);
    SpectrumChooser->SetAttribute(XmNautoUnmanage, (XtArgVal)False);
					    
  }
  if(callback_data->default_ok) {
    SpectrumChooser->DefaultToOk();
  }
  else {
    SpectrumChooser->DefaultToApply();
  }


  /* Set up the list of spectra: */

  SpectrumChooser->SetSelectionList(num_defined, defined_list);
  if(num_defined < XAMINE_VISIBLE_SPECTRUM_CHOICES) {
    SpectrumChooser->SetVisibleItemCount(num_defined);
  }
  else {
    SpectrumChooser->SetVisibleItemCount(XAMINE_VISIBLE_SPECTRUM_CHOICES);
  }

  /*  Remove the current list of Do, and cancel callbacks and set the new ones */

  if(cancel != NULL) {
    XMRemoveCallback(ok);
    XMRemoveCallback(apply);
    XMRemoveCallback(cancel);
    XMRemoveCallback(nomatch);
  }
typedef void (*cbfunc)(XMWidget *, XtPointer, XtPointer);
  ok = SpectrumChooser->AddDoCallback(Callback_handler, 
				      (XtPointer)callback_data->displayfunc,
				      &apply); /* New Ok/Apply */
  cancel = SpectrumChooser->AddCancelCallback(Callback_handler, 
				     (XtPointer)callback_data->displayfunc); /* New cancel */
  nomatch = SpectrumChooser->AddNoMatchCallback(Callback_handler, 
				      (XtPointer)callback_data->displayfunc);

  /* Show the dialog to the world. */

  SpectrumChooser->Show();
}
