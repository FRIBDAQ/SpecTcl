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
#ifndef SUPERPROMPT_H_INSTALLED
#define SUPERPROMPT_H_INSTALLED

#include "XMWidget.h"

void    Xamine_GetSuperposition(XMWidget *w, XtPointer u, XtPointer c);
void    Xamine_UnSuperimpose(XMWidget *w, XtPointer u, XtPointer c);
#endif
