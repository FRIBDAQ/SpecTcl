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
#ifndef _BUTTON_SETUP_H
#define _BUTTON_SETUP_H
#include "XMWidget.h"
#include "XMManagers.h"
#include "location.h"

void Xamine_SetupButtonBar(XMWidget *manager);
void Xamine_SetButtonBarZoom(Boolean state);
void Xamine_SetButtonBarLog(Boolean state);
Boolean Xamine_GetButtonBarLog();
#endif
