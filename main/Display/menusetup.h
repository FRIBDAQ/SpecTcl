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
**   Xamine - NSCL Display program.
** Abstract:
**   menusetup.h  - Defines external entry points to the menu handling module
**                  of Xamine.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**  @(#)menusetup.h	8.2 5/21/96 
*/


#ifndef MENUSETUP_H
#define MENUSETUP_H

#include "XMWidget.h"
#include "XMMenus.h"

XMMenuBar *Xamine_setup_menus(XMWidget *parent); /* set up the menus. */

/*  Control the state of the zoom toggle */

void Xamine_SetZoomToggleButtonState(Boolean state);
void Xamine_EnableUnsuperimpose();
void Xamine_DisableUnsuperimpose();

#endif
