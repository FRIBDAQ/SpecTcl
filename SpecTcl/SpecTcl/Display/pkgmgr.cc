/*
** Facility:
**   Xamine  - NSCL Display program.
** Abstract:
**   pkgmgr.cc -- This file contains software which manipulates the 
**                Xamine function package widget lists.  A function package
**                widget list is a set of XMButton derived widgets which
**                can be enabled or disabled as a gang.
**                The following packages of functions currently exist and
**                have Addto, Enable and Disable functions:
**                Base Package
**                Multiwindow package
**                Spectrum Present package
**                Selected pane contains spectrum package.
**                Selected Pane is 1d package
**                Selected pane is 1d with grobs present package.
**                Selected pane is 2d package
**                Selected pane is 2d with grobs present package.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
static char *sccsinfo = "@(#)pkgmgr.cc	8.1 6/23/95 \n";


/*
** Include files required
*/
#include "XMWlist.h"
#include "pkgmgr.h"

static XMButtonList BasePackage;        /* Base package button list. */
static XMButtonList MultiwindowPackage;	/* Multiwindow button list */
static XMButtonList SpectrumPresentPackage; /* A pane has a spectrum package. */
static XMButtonList SpectrumSelectedPackage; /* Selected pane contains a spec. */
static XMButtonList SelectedIs1DPackage; /* Selected pane has a 1-d spectrum. */
static XMButtonList SelectedIs2DPackage; /* Selected is a 2d. */

/*
** Module global data:
*/


/*
** Functional Description:
**   Xamine_AddtoBasePackage:
**       Adds a button to the list of widgets that control base class
**       functions.
** Formal Parameters:
**   XMButton *button:
**      The button to add.
*/
void Xamine_AddtoBasePackage(XMButton *button)
{
  BasePackage.Add(button);
}
/*
** Functional Description:
**    Xamine_EnableBasePackage:
**      Turns on the buttons in the base package
*/
void Xamine_EnableBasePackage()
{
  BasePackage.Enable();
}
/*
** Functional Description:
**    Xamine_DisableBasePackage:
**      Turns off the buttons in the base package
*/
void Xamine_DisableBasePackage()
{
  BasePackage.Disable();
}

/*
** Functional Description:
**    Xamine_AddtoMultiwindowPackage:
**       This function adds a button to the multiple window function package
**       widget list allowing disable and enable to control it's state.
** Formal Parameter:
**    XMButton *button:
**      The button which controls a multiwindow package function
*/
void Xamine_AddtoMultiwindowPackage(XMButton *button)
{
  MultiwindowPackage.Add(button);
}
/*
** Functional Description:
**    Xamine_EnableMultiwindowPackage:
**       Turn on the functions associated with the multiwindow package.
*/
void Xamine_EnableMultiwindowPackage()
{
  MultiwindowPackage.Enable();
}
/*
** Functional Description:
**    Xamine_DisableMultiwindowPackage:
**      Turn off the functions associated with the multiwindow package.
*/
void Xamine_DisableMultiwindowPackage()
{
  MultiwindowPackage.Disable();
}

/*
** Functional Description:
**   Xamine_AddtoSpectrumPresentPackage:
**      Adds a button widget to the list of widgets which control functions in
**      the Spectrum Present package of functions.
** Formal Parameters:
**    XMButton *button:
**      The button to add.
*/
void Xamine_AddtoSpectrumPresentPackage(XMButton *button)
{
  SpectrumPresentPackage.Add(button);
}
/*
** Functional Description:
**   Xamine_EnableSpectrumPresentPackage:
**     Enables the buttons which have been added to the widget lists controlling
**     the spectrum present package.
*/
void Xamine_EnableSpectrumPresentPackage()
{
  SpectrumPresentPackage.Enable();
}
/*
** Functional Description:
**   Disables the buttons which have been added to the widget lists controlling
**   the spectrum present package.
*/
void Xamine_DisableSpectrumPresentPackage()
{
  SpectrumPresentPackage.Disable();
}


/*
** Functional Descriptions:
**   Xamine_AddtoSpectrumSelectedPackage:
**     This function allows the caller to add a widget to the list of widgets
**     which control functions that should only be possible if the selected
**     pane contains a spectrum.
** Formal Parameters:
**   XMButton *buton:
**     The button to add
*/
void Xamine_AddtoSpectrumSelectedPackage(XMButton *button)
{
  SpectrumSelectedPackage.Add(button);
}
/*
** Functional Description:
**   Xamine_EnableSpectrumSelectedPackage:
**     This function enables the buttons which make up the spectrum selected
**     package:
*/
void Xamine_EnableSpectrumSelectedPackage()
{
  SpectrumSelectedPackage.Enable();
}
/*
** Functional Description:
**    Xamine_DisbleSpectrumSelectedPackage:
** Formal Parameters:
**    This function disables the buttons which make up the spectrum selected
**    package.
*/
void Xamine_DisableSpectrumSelectedPackage()
{
  SpectrumSelectedPackage.Disable();
}

/*
** Functional Description:
**    Xamine_Addto1dSelectedPackage:
**      This function adds a button to the set of buttons which should only
**      be enabled if a 1-d spectrum is in the selected pane.
** Formal Parameters:
**    XMButton *button:
**      The button to add
*/
void Xamine_Addto1dSelectedPackage(XMButton *button)
{
  SelectedIs1DPackage.Add(button);
}
/*
** Functional Description:
**    Xamine_Enable1dSelectedPackage:
**       Enables the functions which are present only when a 1 d spectrum
**       is selected.
*/
void Xamine_Enable1dSelectedPackage()
{
  SelectedIs1DPackage.Enable();
}
/*
** Functional Description:
**   Xamine_Disable1dSelectedPackage:
**     Disable the functions which are present only when a 1 d spectrum
**     is selected.
*/
void Xamine_Disable1dSelectedPackage()
{
  SelectedIs1DPackage.Disable();
}

/*
** Functional Description:
**    Xamine_Addto2dSelectedPackage:
**       This function adds a button to the 2-d selected package of functions.
** Formal Parameters:
**    XMButton *button:
**       The button to add.
*/
void Xamine_Addto2dSelectedPackage(XMButton *button)
{
  SelectedIs2DPackage.Add(button);
}
/*
** Functional Description:
**    Xamine_Enable2dSelectedPackage:
**      This function enables the 2d is selected pane package of functions.
*/
void Xamine_Enable2dSelectedPackage()
{
  SelectedIs2DPackage.Enable();
}
/*
** Functional Description:
**   Xamine_Disable2dSelectedPackage:
**      Turn off the package of functions requiring a selected 2-d spectrum.
*/
void Xamine_Disable2dSelectedPackage()
{
  SelectedIs2DPackage.Disable();
}
