/*
** Facility:
**   Xamine -- NSCL Display system.
** Abstract:
**   spcchoice.h:
**     This file contains some base class definitions for dialogs
**     that allow the user to choose a spectrum (one only) from a list.
**     The classes are derived from the XMSelector selfcontained Selection Box
**     dialog class. The class hierarchy we produce is as follows:
**                               SpectrumChooser
**                                      |
**                       +--------------+-----------------+
**                       |                                |
**             LimitedSpectrumChooser          CompatibleSpectrumChooser
**
**     To avoid duplicating the setup list code, we use the concept of a
**     filter method.  The base SpectrumChooser class passes each candidate
**     spectrum name to be added to the list through a filter virtual method
**     and only adds the spectrum to the list if the filter method approves.
**     Thus the derived classes merely replace the filter method with one
**     appropriate to the limitations which they wish to impose.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS:
**    @(#)spcchoice.h	8.1 6/23/95 
*/

#ifndef _SPCCHOICE_H
#define _SPCCHOICE_H

#include "XMDialogs.h"

/*
** The base class for this hierarchy is a SpectrumChooser class.
** This class displays a possibly filtered list of spectra in a SelectionBox
** dialog.  The dialog behavior is inherited from the XMSelector class.
** The extra behavior we supply is a SetupList function which sets the initial
** list to be a possibly filtered set of spectra.  Client is expected to
** specialize the AppPerform method.  Note that the Perform method extracts
** the spectrum number from the choice and runs it through FilterList to make
** certain it's still suitable before calling AppPerform.
**  NOTE/PROBLEM for implementors/derivers:
**    SetupList must be called from within the outermost constructor as
**    in at least one major C++ implementation the override of the virtual
**    functions does not occur until after the base class constructors have
**    run.
*/

class SpectrumChooser : public XMSelector 
{
  public:
    SpectrumChooser(char *name, Widget parent, char *prompt = "Spectrum: ",
		    XtPointer cbd = NULL);
    SpectrumChooser(char *name, XMWidget &parent, char *prompt = "Spectrum: ",
		    XtPointer cbd = NULL);
    virtual ~SpectrumChooser();

    void PopUp() { SetupList(); 
		   Manage();
		  }

    virtual void SetupList();
    
  protected:
    virtual Boolean FilterList(int specno); // Expected to be overidden.
    virtual Boolean Perform(XMWidget *wid,  // Initial perform function has
			    XtPointer userd, // default behavior of pulling out
			    int       reason, // spectrum no. from value or
			    XmString  value,  // popping up error dialogs etc.
			    int       length);

    virtual Boolean AppPerform(XMWidget *wid,	  // Specializer fills in this
			       XtPointer userd,	  // method.
			       int       reason,
			       int       specno
			       ); 
  private:
};


/*
**  The Compatible SpectrumChooser class limits the spectra displayed to the
**  compatible ones only.  This is done by supplying a FilterList override
**  function which knows what he current spectrum looks like and allows
**  only compatible spectra which are not equal to the current spectrum.
**  The current spectrum is defined a the one in the selected pane.
**  Compatible is defined as having the same number of dimensions and
**  having the same number of channels on each axis.
*/
class CompatibleSpectrumChooser : public SpectrumChooser
{
   public:
     CompatibleSpectrumChooser(char *name, Widget parent, 
			    char *prompt = "Spectrum: ",
			    XtPointer cbd = NULL);
     CompatibleSpectrumChooser(char *name, XMWidget &parent,
			    char *prompt = "Spectrum: ",
			    XtPointer cbd = NULL);
     virtual ~CompatibleSpectrumChooser();

     /* Use ancestor's SetupList() */

   protected:
    virtual Boolean AppPerform(XMWidget *wid,	  // Specializer fills in this
			       XtPointer userd,	  // method.
			       int       reason,
			       int       specno
			       ); 
     virtual Boolean FilterList(int specno); // Override parent's Filter .
};

/*
** The Limited Spectrum chooser class limits the spectra displayed to 
** a given class of spectra.  Spectrum classes include:
** oned, twod and any.
*/
enum SpectrumClass
{
  oned,
  twod,
  any				// Degenerates to SpectrumChooser
};

class LimitedSpectrumChooser : public SpectrumChooser
{
  public:
    LimitedSpectrumChooser(char *name, Widget parent, 
			   char *prompt = "Spectrum ",
			   SpectrumClass limitation = any,
			   XtPointer cbd= NULL);
    LimitedSpectrumChooser(char *name, XMWidget &parent, 
			   char *prompt = "Spectrum ",
			   SpectrumClass limitation = any,
			   XtPointer cbd= NULL);

    virtual ~LimitedSpectrumChooser();
    
    void SetLimitation(SpectrumClass newlimit);
  protected:
    virtual Boolean FilterList(int specno); // Override parent's filter.
    virtual Boolean AppPerform(XMWidget *wid,	  // Specializer fills in this
			       XtPointer userd,	  // method.
			       int       reason,
			       int       specno
			       ); 
  private:
    SpectrumClass limit;
};
#endif
