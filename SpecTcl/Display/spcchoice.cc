/*
** Facility:
**    Xamine -- NSCL display program.
** Abstract:
**    spcchoice.cc:
**      This file contains the methods required to implement the
**      spectrum chooser object.  The spectrum chooser is built on top of
**      the XMSelector class to provide a list of spectra for the user to
**      select from.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS history:
**   @(#)spcchoice.cc	8.1 6/23/95 
*/

/*
** Include files:
*/
#include <stdio.h>
#include <string.h>
#include <Xm/Xm.h>
#include <assert.h>

#include "errormsg.h"
#include "dispshare.h"
#include "spcchoice.h"

/*
** Externals:
*/
extern spec_shared *xamine_shared;

/*
** Help text for the widgets:
*/
static char *ChooserHelp[] =
{
  " This dialog is prompting you for a spectrum.  There are several ways to\n",
  "specify a spectrum: \n\n",
  "   1. Click, or double click on a spectrum in the scrolling list\n",
  "   2. Type in the entire name of the spectrum as it appears in the\n",
  "      scrolling list in the text box.\n",
  "   3. Type in the number of the spectrum (the number is bracketed by\n",
  "      squaer brackets in the list) in the text box.\n\n",
  "  Once you have made your selection, you can click on any of:\n\n",
  "     Ok    - To accept the selection and dismiss the chooser dialog\n",
  "     Apply - To accept the selection and keep the chooser displayed\n",
  "     Cancel- To dismiss the chooser display without accepting anything\n",
  "     Help  - To display this text\n",
  NULL
};

/*
** Functional Description:
**   SpectrumChooser::SpectrumChooser:
**     Instantiates a spectrum chooser.  Not really much to do except
**     to invoke the parent class constructors.
** Formal Parameters:
**   char *name:
**      Name of the dialog widget to be created.
**   Widget parent:
**      widget id of the parent of the dialog.
**   XMWidget &parent:
**      Reference to the parent widget object.
**   char *prompt:
**     The prompt string for the text type in box.
**   XtPointer cbd:
**     Callback data that's passed in to the perform functions.
*/
SpectrumChooser::SpectrumChooser(char *name, Widget parent, char *prompt,
				 XtPointer cbd) :
		 XMSelector(name, parent, prompt, cbd)
{
  SetupList();
  RestrictChoices();
  SetHelpText(ChooserHelp);
}
SpectrumChooser::SpectrumChooser(char *name, XMWidget &parent, char *prompt,
				 XtPointer cbd) :
                 XMSelector(name, parent, prompt, cbd)
{
  SetupList();
  RestrictChoices();
  SetHelpText(ChooserHelp);
}

/*
** Destructor is pretty well gutted too: 
*/

SpectrumChooser::~SpectrumChooser() 
{}

/*
** Functional Descrption:
**   SpectrumChooser::SetupList:
**      This method builds the list of spectra to display.  It is an override
**      of SetupList from the base class.  We call the boolean method
**      FilterList for each in-use spectrum, only adding a spectrum to the
**      list if FilterList approves.  This makes the FilterList virtual 
**      function the hook on which to hang fancy conditioning of the list
**      of spectra presented to the user.
*/
void SpectrumChooser::SetupList()
{
  char **speclist;
  char *(dispspec[DISPLAY_MAXSPEC]);
  Cardinal    accepted;
  int         lsize;
  int         spno;

  assert((sizeof(dispspec) == DISPLAY_MAXSPEC * sizeof(char *)));
  lsize = Xamine_GetSpectrumList(&speclist); // Get the formatted/sorted list.
  assert((sizeof(dispspec) >= (lsize * sizeof(char*))));

  accepted = 0;
  for(int i = 0; i < lsize; i++) { // For each spectrum in the list:
    sscanf(speclist[i], "[%03d]", &spno); // Pull out the spectrum number.
    if(FilterList(spno)) {
      dispspec[accepted] = speclist[i];	// Accept one spectrum.
      accepted++;
    }
  }

  /* Set the final list: */

  SetSelectionList(accepted, dispspec);
}

/*
** Functional Description:
**   SpectrumChooser::FilterList:
**     This function is an overridable function which determines which
**     spectrum names in fact appear in the chooser.  For this base class
**     we always return True.  Derivers override this method to produce
**     partial spectrum lists.
** Formal Parameters:
**    int specno:
**      Number of the spectrum to which the filter is applied.
** Returns:
**    True - if that spectrum should appear in the list.
**    False- If that spectrum should not appear in the list.
*/
Boolean SpectrumChooser::FilterList(int specno)
{
  return (xamine_shared->gettype(specno) != undefined);
}

/*
** This page is devoted to application specific code and templates.
** The Perform processing of the XMSelector class is divided into two parts.
** Preprocessing extracts the spectrum number from the user selection and
** does appropriate error checking.  If necessary, error dialogs are popped
** up as needed.  If a legal spectrum number is selected, then the
** method AppPerform is called to do detailed application formatting.
** This whole thing allows us to get a sort of layered application specific
** behavior out of this.  The Perform processing is appropriate to almost all
** typical spectrum choosers, while the AppPerform function does only
** application specific processing and is intended to be overridden:
*/
Boolean SpectrumChooser::AppPerform(XMWidget *wid, // Trigger widget.
				    XtPointer userd, // User data 
				    int reason,	     // Reason for call.
				    int specno	     // Decoded spectrum #.
				    )
{
  return True;
}
/*
** Functional Description:
**   SpectrumChooser::Perform:
**     This function has spectrum choice dependent processing of a user's
**     selection.  The user could have selected an entry from the list or
**     typed in a number.  If a number was typed in then we decode it as
**     a spectrum selector.
** Formal Parameters:
**   XMWidget *wid:
**     Pointer to the activating widget.  In this case it's the 'this' pointer
**   XtPointer userd:
**     user data, passed without interpretation to AppPerform.
**   int reason:
**     The reason we were called:
**       XmCR_OK    - Ok pressed... decode spectrum # from [] string.
**       XmCR_APPLY - Apply pressed... decode spectrum # from [] string.
**       XmCR_NO_MATCH - No match with anything in the list... so 
**                       try to decode a raw number from the input text.
**   XmString value:
**      The compound string representation of the user's selection.
**   int length:
**      length of the compound string representation of the user's selection.
** Returns:
**   False  - If an error was detected.
**   Otherwise returns the value of the AppPerform() method.
*/
Boolean SpectrumChooser::Perform(XMWidget *wid, XtPointer userd,
				 int reason, XmString value, int length)
{
  char *text;	       	// ASCII string version of value.
  int  nonb;		// First non whitespace char of ASCII value.
  Boolean status;
  int     spno;

  /* First extract the ASCII version of the value string: */

  if(!XmStringGetLtoR(value, XmSTRING_DEFAULT_CHARSET, &text)) {
    status = False;
    Xamine_error_msg(this,
		   "SpectrumChooser::Perform, Could not extract ASCII choice");
    return status;
  }
  else {
    switch(reason) {
    case XmCR_OK:		// Legitimate match from string list so
    case XmCR_APPLY:		// fmt is [nnn] Name of spectrum.
    case XmCR_NO_MATCH:
      if(sscanf(text, "[%03d]", &spno) != 1) {
	nonb = strspn(text, " \t\v\r\n");
	if(nonb < strlen(text)) {
	  if(sscanf(&text[nonb], "%d", &spno) != 1) {
	    status = False;
	    Xamine_error_msg(this,
		     "SpectrumChooser::Perform -- Bad Spectrum number");
	  }
	  else {
	    if(FilterList(spno)) {
	      status = AppPerform(wid, userd, reason, spno);
	    }
	    else {
	      status = False;
	      Xamine_error_msg(this,
		       "SpectrumChooser::Perform -- Unsuitable spectrum");
	    }
	  }
	}
	else {
	  status = False;
	  Xamine_error_msg(this,
		   "SpectrumChooser::Perform -- Blank spectrum number");
	}
	break;
	
      }
      else {
	if(FilterList(spno)) {
	  status = AppPerform(wid, userd, reason, spno);
	}
	else {
	  status = False;
	  Xamine_error_msg(this,
			   "SpectrumChooser::Perform - unsuitable spectrum");
	}
      }
      break;
    default:
      Xamine_error_msg(this,
	       "SpectrumChooser::Perform unrecognized callback reason");
      status = False;
    }
  }
  XtFree(text);
  return status;
}
