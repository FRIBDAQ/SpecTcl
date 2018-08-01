/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

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
**    Xamine -- NSCL display program.
** Abstract:
**    colormgr.h:
**      This file contains templates and definitions used by the clients of
**      the Xamine/X11 color manager.  The color manager is responsible for
**      building determining, reporting color resources of the underlying
**      X-11 server and, if possible, reading in the color table for the
**      color level renditions.
**          @(#)colormgr.h	8.1 6/23/95 
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/
#ifndef COLORMGR_H
#define COLORMGR_H
#include <stdio.h>

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif
/*
** Constant definitions which control the color manager itself.
*/

#define XAMINE_MAXPLANES    6		/* Maximum number of free planes allowed. */
#define XAMINE_MAXCOLORS    (1 << XAMINE_MAXPLANES)   /* Maximum number of colors allowed */

#define XAMINE_MINPLANES    2		/* We need at least 4 colors for color */

/*
**   Environment variable/logical that points to color table directory
*/
#define XAMINE_COLOR_ENVIRONMENT "XAMINE_COLORTABLE_DIR"
/*
**    Location of default color tables.
*/
#ifndef HOME
#define XAMINE_DEFAULT_COLOR_DIR "/daq/etc"
#endif
/*
**   Color table filename string for use in sprintf:
*/


void Xamine_InitColors(XMWidget *w); /* Initialize the color manager. */
Boolean Xamine_ColorDisplay();	     /* True if color server.  */
unsigned int Xamine_NumColors();     /* Return number of color levels */
unsigned int Xamine_ColorDepth();    /* Return number of color planes. */
unsigned long Xamine_GetPixel(int idx); /* Get pixel number idx value */
unsigned long *Xamine_GetColorTable();  /* Return pointer to pixel list */
unsigned long Xamine_PctToPixel(int val); /* Return color for % of full scale */
Colormap Xamine_GetColormap();	/* Return color map id. */
Visual   *Xamine_GetVisual(Display *d, Window w); /* Return visual struct */
unsigned long Xamine_GetXorDrawingColor();
unsigned long Xamine_GetColorPlaneMask();
FILE *Xamine_OpenColorTable(unsigned int planes);

void Xamine_HtsToColor(unsigned int *hts, int full_scale, int nchan);
void Xamine_GetVisualInfo(Display* d, Window w, XVisualInfo* vis);
void Xamine_GetX11ColorMap(XStandardColormap& map, Display* d, Window w, XVisualInfo* vis);
unsigned long Xamine_ComputeDirectColor(XStandardColormap* map, int r, int g, int b);
#endif
