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
**   lblfont.h:
**     This file contains the definitions associated with the graphical
**     object font size dialog box.  That dialog box allows the user to
**     choose the size of graphical object labels.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**   @(#)lblfont.h	8.1 6/23/95 
*/

#ifndef LBLFONT_H
#define LBLFONT_H

#include "XMWidget.h"

void Xamine_SetGrobjLblFont(XMWidget *w, XtPointer ud, XtPointer cd);

#endif
