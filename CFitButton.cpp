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

#include <config.h>
#include "CFitButton.h"
#include "ButtonEvent.h"
#include <Xamine.h>
#include <CGaussianFit.h>
#include <CSpectrumFit.h>
#include <CFitDictionary.h>
#include <Histogrammer.h>
#include <SpecTcl.h>
#include <Spectrum.h>
#include <CFitCommand.h>
#include <TCLInterpreter.h>

#include <clientops.h>
#include <string>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

#include <iostream>		// For debugging.

/*!

   For now we assume we have the button box all to ourselves.

   \param pEventHandler : CXamineEventHandler*
      Pointer to the xamine event handler (we register ourselvse on this).

*/
CFitButton::CFitButton(CXamineEventHandler* pEventHandler) 
{
  // Generic Gauss fit button.


  m_FitButtonId          = 1;
  ButtonDescription myButton;
  myButton.button_code   = m_FitButtonId;
  strcpy(myButton.label, "Gauss Fit");
  myButton.type          = Push;
  myButton.sensitive     = T;
  myButton.prompter      = Points;
  myButton.whenavailable = In1dSpectrum;
  strcpy(myButton.promptstr, "Fit limits"); // Ingored?
  myButton.spectype      = Oned;            // I think this is ignored for this prompter
  myButton.minpts        = 2;
  myButton.maxpts        = 2;               // need exactly 2 pts.

  //  Xamine_DefineButton(0,0, &myButton); // Old button in case Tim needs to see.

  // The Fit Gammas button is about the same as the GaussFit button.. just
  // different Id and label:

  m_GammaFitButtonId   = 2;
  myButton.button_code = m_GammaFitButtonId;
  strcpy(myButton.label, "Fit Gammas");
  Xamine_DefineButton(0,0, &myButton);

  // The fit Neutrons button is also about the same as the guass fit button:

  m_NeutronFitButtonId  = 3;
  myButton.button_code = m_NeutronFitButtonId;
  strcpy(myButton.label, "Fit Neutrons");
  Xamine_DefineButton(0,2, &myButton);

  // The SHOW FOM  button has no prompter, and requires
  // we be in a 1-d spectrum... 

  m_FOMButtonId        = 4;
  myButton.button_code = m_FOMButtonId;
  strcpy(myButton.label, "FOM");
  myButton.prompter    = NoPrompt;
  Xamine_DefineButton(2,0, &myButton);

  // Show all FOM button has no prompter, and no requirements
  // about the current spectrum.

  m_FOMAllButtonId    = 5;
  myButton.button_code = m_FOMAllButtonId;
  strcpy(myButton.label, "FOM All");
  myButton.whenavailable =  Anytime;
  Xamine_DefineButton(2,1, &myButton);


  // Project button:

  m_projectButtonId  = 6;
  myButton.button_code = m_projectButtonId;
  strcpy(myButton.label, "Project");
  myButton.whenavailable = In2dSpectrum;
  Xamine_DefineButton(2,2, &myButton);

  pEventHandler->addButtonHandler(*this);
  
  
  
}
/*!
   The destructor does nothing for now, however note that a call to the
   destructor is really really bad since at present, there's no way to
   unregister us as a button event handler!!
*/
CFitButton::~CFitButton() {}
/*
   The button handler is called when \em any button in the button box
   sends its message back to SpecTcl.  We need to determine if we can
   successfully process the message.  If so return true if not,
   false so that other handlers can try.

   \param event : CButtonEvent&
      Reference to an object that describes the button event.

   \return Bool_t
   \retval  kfTRUE - we processed the event, no need for other handlers to fire.
   \retval  kfFALSE - we did not process the event, continue checking handlers.
*/

Bool_t 
CFitButton::operator()(CButtonEvent& event)
{
  // If this is not our button, return false right away:

  int buttonId = event.getId();

  // What we do depends a bit on the button that has been clicked:

  if (buttonId == m_FitButtonId      | 
      buttonId == m_GammaFitButtonId |
      buttonId == m_NeutronFitButtonId) {

    // Fits are all the same except for where the fit name comes from.


  // Before we can fit, we need to get:
  // - The name of the fit.
  // - The name of the spectrum.
  // - The fit points.

    PointArray     pts           = event.getPoints();
    string spectrum              = spectrumName(event);
    // Figure out the fit source depending on the actual button
    // id:



    string fitName;
    if (buttonId == m_FitButtonId) {
      fitName       = event.getm_sPromptedString();

    }
    if (buttonId == m_GammaFitButtonId) {
      fitName     = spectrum;
      fitName    += "-gammas";
    }
    if (buttonId == m_NeutronFitButtonId) {
      fitName     = spectrum;
      fitName    += "-neutrons";
    }
    
    // Now we can create the fit:
    
    CGaussianFit*  pFit  = new CGaussianFit(fitName, CFitCommand::id());
    int            low   = pts[0].X();
    int            high  = pts[1].X();

    // Danger Will Robinson.. if low==high, gsl will exit our
    // program.. not likely but need to catch it because the
    // game theoretcial payoff is soo sooo bad.
    //
    if (low == high) {
      invokeScript(string("tk_messageBox -icon error -message {Both fit points have the same X positions}"));
      return kfTRUE;
    }
    if (low > high) {
      int temp = low;
      low      = high;
      high     = temp;
    }
    // This is in a try/catch block in case the user did the truly 
    // pathalogical thing of deleting the spectrum just as they
    // accepted the fit ... kids these days.
    //
    CSpectrumFit*   pSpectrumFit;
    try {
      pSpectrumFit = new CSpectrumFit(spectrum,
				      pFit->getNumber(),
				      low, high, *pFit);
    }
    catch (...) {
      delete pFit;		// Just abort the operation silently.
      return kfTRUE;
    }
    
    // we use the addOrReplace function of the fit dictionary:
    
    CFitDictionary& dict(CFitDictionary::getInstance());
    dict.addOrReplace(*pSpectrumFit);
    
  } 
  else if (buttonId == m_FOMButtonId) {
    //
    // We're going to invoke ShowFOM spectrum name:
    // construc the script:

    string script = "ShowFOM ";
    script       += spectrumName(event);

    invokeScript(script);
  } 
  else if (buttonId == m_FOMAllButtonId) {
    // Report all the FOM's.
    //

    invokeScript(string("ShowFOMAll"));
  }
  else if (buttonId == m_projectButtonId) {
    //
    // Invoke the projection GUI:

    string script = "Project2D ";
    script       += spectrumName(event);
    invokeScript(script);
  }
  else {
    return kfFALSE;		// Not one of our buttons.
  }
  return kfTRUE;
}
//  Return the spectrum name associated with a button click.
// an empty string if spectrum is not defined.
//
string
CFitButton::spectrumName(CButtonEvent& event)
{
    int            bindId        = event.getPromptedSpectrum();
    SpecTcl*       pApi          = SpecTcl::getInstance();
    CHistogrammer* pHistogrammer = pApi->GetHistogrammer();
    CSpectrum*     pSpectrum     = pHistogrammer->DisplayBinding(bindId-1);
    string spectrumName;
    if (pSpectrum) {
      spectrumName  = pSpectrum->getName();
    } 
    else {
      spectrumName  = "";
    }
    return spectrumName;
}
// 
// Invoke a Tcl script ignoring any errors that may occur in it:
//
void
CFitButton::invokeScript(string script)
{
  SpecTcl*          api     = SpecTcl::getInstance();
  CTCLInterpreter*  pInterp = api->getInterpreter();
  try {
    pInterp->GlobalEval(script);
  }
  catch (...) {
  }
}
