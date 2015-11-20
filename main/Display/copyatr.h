/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/*
** Facility:
**    Xamine -  NSCL display program.
** Abstract:
**    copyatr.h:
**       Defines entry point for a function that produces a dialog that
**       prompts for pane attribute copying.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)copyatr.h	8.1 6/23/95 
*/

#ifndef _COPYATTR_H
#define _COPYATTR_H

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

void Xamine_CopyPaneAttributes(XMWidget *b, XtPointer ud, XtPointer cd);


#endif
