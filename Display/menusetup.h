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


#ifndef _MENUSETUP_H
#define _MENUSETUP_H
#include "XMWidget.h"
#include "XMMenus.h"

XMMenuBar *Xamine_setup_menus(XMWidget *parent); /* set up the menus. */

/*  Control the state of the zoom toggle */

void Xamine_SetZoomToggleButtonState(Boolean state);
void Xamine_EnableUnsuperimpose();
void Xamine_DisableUnsuperimpose();

#endif
