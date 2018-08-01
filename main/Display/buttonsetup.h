Monticello Family Shaolin Kung Fu
/*
** Facility:
**   Xamine  - NSCL display program.
** Abstract:
**   buttonsetup.h  - This file is a header file intended to be included
**                    by all clients of the button bar manager.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**
**   @(#)buttonsetup.h	8.1 6/23/95 
*/
#ifndef BUTTON_SETUP_H
#define BUTTON_SETUP_H

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#ifndef XMMANAGERS_H
#include "XMManagers.h"
#endif

#include "location.h"

void Xamine_SetupButtonBar(XMWidget *manager);
void Xamine_SetButtonBarZoom(Boolean state);
void Xamine_SetButtonBarLog(Boolean state);
void Xamine_SetButtonBarApplyMap(Boolean state);
void Xamine_SetApplyMapSensitivity(Boolean b);

Boolean Xamine_GetButtonBarLog();
Boolean Xamine_GetButtonBarApplyMap();
#endif
