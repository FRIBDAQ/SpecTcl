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
**   Xamine - NSCL display program.
** Abstract:
**   axes.h:
**     This file contains definitions for clients of the Xamine axis
**     drawing routines
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** @(#)axes.h	8.1 6/23/95 
*/

#ifndef AXES_H
#define AXES_H

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#include "dispwind.h"
#include "refreshctl.h"

#define XAMINE_MARGINSIZE 0.10	/* Fraction of tube for left, bottom margins */
#define XAMINE_MAPPED_MARGINSIZE 0.15 /* Fraction for margins if mapped */
#define XAMINE_TICK_FRACTION 0.05 /* Fraction of display area for tick mark */
#define XAMINE_TICK_MINSPACE 40   /* Minimum tick spacing in pixels */
#define XAMINE_MAPPED_TICK_MINSPACE 80 /* Min. tick spacing if mapped */
#define XAMINE_TICK_MAXCOUNT 10   /* Maximum number of ticks.       */
#define XAMINE_MAPPED_TICK_MAXCOUNT 7 /* Max. number of ticks if mapped */
#define XAMINE_TICK_LABEL_INTERVALDIVISOR 2.3
void Xamine_DrawAxes(Xamine_RefreshContext *ctx, win_attributed *attribs);

#endif
