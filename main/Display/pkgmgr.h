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


/*
** Facility:
**    Xamine  - NSCL Display program.
** Abstract:
**    pkgmgr.h  - This file is intended for clients of the Xamine package manager
**                The package manager is responsible for maintaining lists
**                of widgets which may be disabled or enabled to turn on
**                and off the availability of various packages of functions.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**      @(#)pkgmgr.h	8.1 6/23/95 
*/
#ifndef PKGMGR_H
#define PKGMGR_H
#include "XMPushbutton.h"
/* Manipulate the base package.  In theory this is always present, but enable
** and disable functions are provided for completeness.
*/

void Xamine_AddtoBasePackage(XMButton *button);
void Xamine_EnableBasePackage();
void Xamine_DisableBasePackage();

/*
** Manipulate the package that's present when there's more than one window.
*/
void Xamine_AddtoMultiwindowPackage(XMButton *button);
void Xamine_EnableMultiwindowPackage();
void Xamine_DisableMultiwindowPackage();

/*
**  Manipulate the package that's present when there's at least one spectrum
**  in the set of windows.
*/

void Xamine_AddtoSpectrumPresentPackage(XMButton *button);
void Xamine_EnableSpectrumPresentPackage();
void Xamine_DisableSpectrumPresentPackage();

/*
** Manipulate the package that's present when the selected window has a spectrum.
*/

void Xamine_AddtoSpectrumSelectedPackage(XMButton *button);
void Xamine_EnableSpectrumSelectedPackage();
void Xamine_DisableSpectrumSelectedPackage();

/*
** Manipulate the selected spectrum is 1-d package
*/

void Xamine_Addto1dSelectedPackage(XMButton *button);
void Xamine_Enable1dSelectedPackage();
void Xamine_Disable1dSelectedPackage();


void Xamine_Addto2dSelectedPackage(XMButton *button);
void Xamine_Enable2dSelectedPackage();
void Xamine_Disable2dSelectedPackage();


#endif
