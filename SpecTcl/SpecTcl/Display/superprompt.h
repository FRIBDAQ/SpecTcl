/*
** Facility:
**   Xamine:
**     Nscl display progrma.
** Abstract:
**    superprompt.h:
**       This file contains definitions of functions etc. used by the
**       user interface for the dialogs that prompt for superposition.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
#ifndef _SUPERPROMPT_H_INSTALLED
#define _SUPERPROMPT_H_INSTALLED
#include "XMWidget.h"

void    Xamine_GetSuperposition(XMWidget *w, XtPointer u, XtPointer c);
void    Xamine_UnSuperimpose(XMWidget *w, XtPointer u, XtPointer c);
#endif
