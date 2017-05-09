/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine - NSCL display program.
** Abstract:
**   spclimchoice.cc:
**     This file contains specific methods of the LimitedSpectrumChooser class.
**     This class is derived form a SpectrumChooser dialog class.
**     The derivation is mostly done by defining a new FilterList method
**     which will limit the user's choice of spectra to spectra in the
**     appropriate class.
** Author:
**   Ron Fox
**   NSCL 
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS information:
**   @(#)spclimchoice.cc	8.1 6/23/95 
*/

/*
** Include files
*/
#include <config.h>
#include "dispshare.h"
#include "spcchoice.h"

/*
** Externals:
*/
extern spec_shared *xamine_shared;

/*
** Functional Description:
**   LimitedSpectrumChooser::LimitedSpectrumChooser:
**      This method instantiates a spectrum chooser object.  The only
**      work we do is to set the limit attribute.  This has to be done with
**      constructor notation to be sure it's set by the time that
**      SpectrumChooser attempts to build the displayed spectrum choice list.
** Formal Parameters:
**   char *name:
**      Name of the dialog widget.
**   Widget parent:
**      Parent widget id.
**   XMWidget &parent:
**      Parent widget object.
**   char *prompt:
**      Prompt string to be displayed near the text input box.
**   SpectrumClass limitation:
**      Sets a limit on the kind of spectrum that will be displayed.
**   XtPointer cbd:
**      Callback data which will be passed uninterpreted to AppPerform().
*/
LimitedSpectrumChooser::LimitedSpectrumChooser(char *name, Widget parent, 
                                               const char *prompt, 
					       SpectrumClass limitation,
					       XtPointer cbd) :
			SpectrumChooser(name, parent, prompt, cbd),
			limit(limitation)
{ SetupList(); }
LimitedSpectrumChooser::LimitedSpectrumChooser(char *name, XMWidget &parent,
                                               const char *prompt, 
					       SpectrumClass limitation,
					       XtPointer cbd) :

			SpectrumChooser(name, parent, prompt, cbd),
			limit(limitation)
{ SetupList(); }

LimitedSpectrumChooser::~LimitedSpectrumChooser()
{}


/*
** Functional Description:
**   LimitedSpectrumChooser::SetLimitation:
**     Allows the client to modify the limitations on spectrum choice
**     dynamically.  This forces a list update.
** Formal Parameters:
**    SpectrumClass newlimit:
**      New class to limit by.
*/
void LimitedSpectrumChooser::SetLimitation(SpectrumClass newlimit)
{
  limit = newlimit;

  SetupList();			// Set up the new list.
}


/*
** Functional Description:
**   LimitedSpectrumChooser::FilterList:
**      Determines if a candidate spectrum number is legitimate for
**      inclusion in the list.
** Formal Parameters:
**   int specno:
**     Number of the spectrum to check
** Returns:
**   True   - Spectrum is in limitation.
**   False  - Spectrum is not in limitation.
*/
Boolean LimitedSpectrumChooser::FilterList(int specno)
{
  switch(limit) {
  case oned:
    return ((xamine_shared->gettype(specno) == onedlong) ||
	    (xamine_shared->gettype(specno) == onedword));
	    
  case twod:
    return ((xamine_shared->gettype(specno) == twodword) ||
	    (xamine_shared->gettype(specno) == twodbyte) ||
	    (xamine_shared->gettype(specno) == twodlong));
  case any:
  default:
    return (xamine_shared->gettype(specno) != undefined);
  }
}


/*
** And finally, we have the virtual function template for the
** AppPerform method.
*/
Boolean LimitedSpectrumChooser::AppPerform(XMWidget *wid, XtPointer userd, 
			       int reason, int specno)
{
  return True;
}
