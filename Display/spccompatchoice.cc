/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   spccompatchoice.cc:
**     This file contains implementation code for the specific methods of
**     the CompatibleSpectrumChooser class. This class is really a subclass
**     of the SpectrumChooser class with a filter function that enforces
**     compatibility between the selectable spectra and the the currently
**     displayed spectrum.

** Author:
**   Ron Fox
**   NSCL 
**   Michigan State University
**   East Lansing, MI 48824-1321
**   [], 1994
** SCCS information:
**   @(#)spccompatchoice.cc	8.1 6/23/95 
*/
#if 0
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
#endif

/*
** Include files
*/
#include "dispshare.h"
#include "spcchoice.h"
#include "dispwind.h"
#include "panemgr.h"

/*
** Externals:
*/
extern spec_shared *xamine_shared;


/*
** Functional Description:
**   CompatibleSpectrumChooser::CompatibleSpectrumChooser:
**      This instantiates a spectrum chooser.  All the work is done by the
**      base class constructors.
** Formal Parameters:
**    char *name:
**      Name of the dialog widget.
**    Widget parent:
**      Parent widget id.
**    XMWidget &parent:
**      Reference to a parent object.
**    char *prompt:
**      Prompt to apply.
**    XtPointer cbd:
**      Callback data passed without interpretation to the AppPerform Method.
*/
CompatibleSpectrumChooser::CompatibleSpectrumChooser(char *name,
						     Widget parent,
						     char *prompt,
						     XtPointer cbd)
                          : SpectrumChooser(name, parent, prompt, cbd)
{ SetupList(); }
CompatibleSpectrumChooser::CompatibleSpectrumChooser(char *name,
						     XMWidget &parent,
						     char *prompt,
						     XtPointer cbd)
                          : SpectrumChooser(name, parent, prompt, cbd)
{ SetupList(); }

/*
** The destructor also is pretty well just a placeholder: 
*/

CompatibleSpectrumChooser::~CompatibleSpectrumChooser()
{
}
/*
** Default behavior of the AppPerform function is also essentially NULL
*/
Boolean CompatibleSpectrumChooser::AppPerform(XMWidget *wid,
					      XtPointer userd,
					      int reason,
					      int specno)
{
  return True;
}


/*
** Functional Description:
**   CompatibleSpectrumChooser::FilterList:
**     This function is at the heart of the specialization.
**     We take as input a spectrum number and determine if it can be
**     displayed on the chooser list.  To be displayed it must be:
**       Not the currently displayed spectrum.
**       Have the same number of dimensions and channels in each axis as
**       the currently displayed spectrum.
**  NOTE:  Nothing is compatible with a null (empty) pane.
** Formal Parameters:
**   int specno:
**     Number of the candidate spectrum.
** Returns:
**   True if comptable.
**   False if not.
*/
Boolean CompatibleSpectrumChooser::FilterList(int specno)
{
  win_attributed *pane = Xamine_GetSelectedDisplayAttributes();

  if(pane == NULL) 
    return False; // Null pane.
  int spec_sel = pane->spectrum();
  
  /* Return False if either spectrum undefined. */
  
  if( (xamine_shared->gettype(spec_sel) == undefined) ||
     (xamine_shared->gettype(specno)   == undefined))
    return False;
  
  /* Require dimensional matching:  */

  if( pane->is1d() ) {
    return ( ((xamine_shared->gettype(specno) == onedlong) ||
	      (xamine_shared->gettype(specno) == onedword))   &&
	    (xamine_shared->getxdim(spec_sel) == xamine_shared->getxdim(specno)
	     ));
  }
  else {
    return ( ((xamine_shared->gettype(specno) == twodword) ||
	      (xamine_shared->gettype(specno) == twodbyte))  &&
	    (xamine_shared->getxdim(spec_sel) == xamine_shared->getxdim(specno))
	                                     &&
	    (xamine_shared->getydim(spec_sel) == xamine_shared->getydim(specno))
	    );
  }
}

