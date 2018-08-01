Monticello Family Shaolin Kung Fu
/*
** Facliity:
**   Xamine - NSCL display program.
** Abstract:
**  chanplot.h:
**     This file contains definitions for clients of the chanplot.cc 
**     implementation file.  chanplot.cc is responsible for plotting
**     the channel contents of a spectrum in a pane of the display.
** Author:
**    Ron FOx
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**    @(#)chanplot.h	8.1 6/23/95 
*/

#ifndef CHANPLOT_H
#define CHANPLOT_H

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#include "refreshctl.h"
#include "panemgr.h"

#define XAMINE_COLORDISTANCE 4	/* Minimum color level distance */

int Xamine_DrawChannels(Xamine_RefreshContext *ctx,
			 win_attributed        *def);

Boolean Xamine_Plot2d(Screen *s, Display *d,
		      win_attributed *att,
		      Drawable win, XMWidget *pane,
		      int orgx, int orgy, int nx, int ny,
		      Xamine_RefreshContext *ctx);

Boolean Xamine_Plot1d(Screen *s, Display *d,
		      win_attributed *att, Drawable win, XMWidget *pane,
		      int orgx, int orgy, int nx, int ny);

GC Xamine_MakeDrawingGc(Display *disp, XMWidget *win, int selector);
void Xamine_getsubwindow(XMWidget *pane, win_attributed *att, 
			 int *orgx, int *orgy, Dimension *nx, Dimension *ny);
#endif
