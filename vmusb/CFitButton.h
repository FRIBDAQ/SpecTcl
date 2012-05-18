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

#ifndef __CFITBUTTON_H
#define __CFITBUTTON_H

#ifndef __XAMINEEVENTHANDLER_H
#include <XamineEventHandler.h>	// For base class.
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

class CButtonEvent;

/*!
   This class defines a subclass of CXamineEventHandler::CButtonHandler.
   The constructor creates a set of buttons in the Xamine client
   button box, which is assumed to already have been made.
   - Fit Gammas - requests a 1-d point prompter and creates a fit
     named <spectrumname>-gammas
   - Fit Neutrons - requests a 1-d point prompter and creates a fit
     named <specturmname>-neutrons
   - FOM - Performs the figure of merit computation on the currently
     selected spectrum (no prompter).
   - FOM All - Performs the figure of merit computation on all spectra
     for which it is possible and pops it up in a new toplevel so that
     the user can inspect or write to file.
   - Project - allows you to create a projection of the current 2-d spectrum.

   Note that this class makes use of several helper Tcl scripts that
   are assumed to already be defined:

   - ShowFOM spectrumname
     Shows the figure of merit computation for the currently selected spectrum.
   - ShowFOMAll 
     shows the figure of merit computation for all spectra that are elligible for
     this.
   - Project2D manages the gui for 2-d projections.

*/
class CFitButton : public CXamineEventHandler::CButtonHandler
{
  // Member data.
private:
  int   m_FitButtonId;		// Ids assigned to buttons.
  int   m_GammaFitButtonId;
  int   m_NeutronFitButtonId;
  int   m_FOMButtonId;
  int   m_FOMAllButtonId;
  int   m_projectButtonId;

  // constructors and canonicals.. note that copy-like ops are forbidden.
public:
  CFitButton(CXamineEventHandler* pHandler);
  virtual ~CFitButton();
private:
  CFitButton(const CFitButton& rhs);
  CFitButton& operator=(const CFitButton& rhs);
  int operator==(const CFitButton& rhs) const;
  int operator!=(const CFitButton& rhs) const;

  // The following is called when the button is successfully used:

  virtual Bool_t operator()(CButtonEvent& event);

  STD(string) spectrumName(CButtonEvent& event);
  void        invokeScript(STD(string) script);

};



#endif
