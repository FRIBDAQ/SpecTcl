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

#ifndef _AXES_H
#define _AXES_H
#include "XMWidget.h"
#include "dispwind.h"
#include "refreshctl.h"

#define XAMINE_MARGINSIZE 0.10	/* Fraction of tube for left, bottom margins */
#define XAMINE_TICK_FRACTION 0.05 /* Fraction of display area for tick mark */
#define XAMINE_TICK_MINSPACE 40   /* Minimum tick spacing in pixels */
#define XAMINE_TICK_MAXCOUNT 10   /* Maximum number of ticks.       */
#define XAMINE_TICK_LABEL_INTERVALDIVISOR 2.3
void Xamine_DrawAxes(Xamine_RefreshContext *ctx, win_attributed *attribs);

#endif
