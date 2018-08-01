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
**   Xamine:   NSCL Display program.
** Abstract:
**    advprompt.h   - This file is a header file with definitions for
**                    clients of the advprompt.cc module.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**    @(#)advprompt.h	8.1 6/23/95 
*/
#ifndef ADVPROMPT_H
#define ADVPROMPT_H

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

void Xamine_PromptAdvance(XMWidget *w, XtPointer clientd, XtPointer calld);

#endif
