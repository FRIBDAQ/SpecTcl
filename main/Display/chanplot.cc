/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:   chanplot.cc
 *  @brief:  Plot histograms. 
 */
static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   chanplot.cc:
**     This file contains an implementation of the software needed to plot
**     histogram channels. There are 2-d and 1-d histograms to deal with.
**     Each histogram has several different rendition types. This file contains
**     code which does some of the type independent computation and farms out
**     the work to the appropriate rendition software.
** AUthor:
**     Ron Fox
**     NSCL
**     Michigan State University
**     East Lansing, MI 48824-1321
*/


/*
** Include files:
*/

#include <config.h>
#include "XMWidget.h"
#include "refreshctl.h"
#include "dispwind.h"
#include "axes.h"
#include "scaling.h"
#include "chanplot.h"
#include "panemgr.h"
#include "colormgr.h"
#include "superpos.h"
#include "gc.h"
#include "xaminegc.h"
#include "gcmgr.h"
/*
** Static storage... the dash lists for the superposition graphical contexts
** are stored here.
*/

#define MAX_SUPERLEN 10
int dashlen[MAX_SUPERPOSITIONS] = { 2, /* Used cells in dashlists. */
				    2,
				    2,
				    4,
				    4,
				    4,
				    6,
				    4
				    };
struct color {
    int r,g,b;
};

struct color superpositionColors[MAX_SUPERPOSITIONS] = {
    {100, 0, 0},
    {0, 100, 0},
    {0, 0, 100},
    
    {100, 100, 0},
    {0, 100, 100},
    {100, 0, 100},
    
    {50, 50, 0},
    {50, 0, 50}
};

char dashlist[MAX_SUPERPOSITIONS][MAX_SUPERLEN] = { /* Dash lists.  */
  { 1,1, 0,0,0,0,0,0,0,0 },				/* 0 */
  { 2,2, 0,0,0,0,0,0,0,0 },				/* 1 */
  { 3,1, 0,0,0,0,0,0,0,0 },				/* 2 */
  { 3,2,1,2, 0,0,0,0,0,0 },				/* 3 */
  { 2,1,1,1, 0,0,0,0,0,0 },				/* 4 */
  { 3,3,3,1, 0,0,0,0,0,0 },				/* 5 */
  { 1,1,2,2,3,3, 0,0,0,0 },				/* 6 */
  { 3,2,2,3, 0,0,0,0,0,0 }				/* 7 */
};
/**
 * getSuperpositionColor
 *    Get the pixel color value for a superposition.
 *
 *  @param d  - display.
 *  @param wid - widget object (XMWidget*)
 *  @param n   - Superposition number - pre checked to be in range.
 *  @return unsigned long  - Pixel color value.
 */
static unsigned long
getSuperpositionColor(Display* d, XMWidget* wid, int n)
{
    // Get what we need to invoke Xamine_ComputeDirectColor which gives us what
    // we need:
    XVisualInfo       info;
    XStandardColormap map;
    Window            win = wid->getWindow();

    Xamine_GetVisualInfo(d, win, &info);
    Xamine_GetX11ColorMap(map, d, win, &info);
    
    struct color c = superpositionColors[n];
    return Xamine_ComputeDirectColor(&map, c.r, c.g, c.b);
}
/*
** Functional Description:
**   Xamine_getsubwindow:
**     This function gets a description of the subwindow in which spectrum
**     plotting should occur.  The description is give in terms of an origin
**     set of coordinates and a number of X and Y pixels.  The function
**     accounts for margins which may or may not be present for axis and
**     label information.
** Formal Parameters:
**   XMWidget *pane:
**      Widget corresponding to the pane within which we're trying to get 
**      the subwindow.
**    win_attributed *att:
**      The attributes of the pane.  Used to determine the actual margin sizes.
**    int *orgx, *orgy:
**       The origin coordinates of the subwindow. (returned)
**    int *nx,*ny:
**        The size of the subwindow (returned)
*/
void Xamine_getsubwindow(XMWidget *pane, win_attributed *att, 
			 int *orgx, int *orgy, Dimension *nx, Dimension *ny)
{
	Rectangle roi =  Xamine_GetSpectrumDrawingRegion(pane, att);
	*orgx = roi.xbase;
	*orgy = roi.ybase;
	*nx   = roi.xmax - roi.xbase;
	*ny   = roi.ybase;

  
}

/*
** Functional Description:
**   mkdrawinggc:
**      This function creates a graphical context suitable for drawing
**      histogram data.  In order to eventually support superposition spectra,
**      the function is built to return one of several graphical context
**      attributes.... the differences will be in line style and color.
** Formal Parameters:
**   Display *disp:
**     Display for which the graphic context is created.
**   XMWidget *win:
**     Widget in which drawing will take place.
**   int selector:
**     A 'small' integer which selects from among the set of attribute
**     modifiers for the default graphical context.  The selector is
**     currently ignored *BUGBUGBUG -- needs to change when we know how
**     to handle colors *BUGBUGBUG*
*/
GC Xamine_MakeDrawingGc(Display *disp, XMWidget *win, int selector)
{
  XGCValues modifiers;
  XamineSpectrumGC *gc = Xamine_GetSpectrumGC(*win);

  win->GetAttribute(XmNbackground, &modifiers.background);
  win->GetAttribute(XmNforeground, &modifiers.foreground);
  modifiers.line_style = LineSolid;

  if(selector < 0) {		/* Get default graphical context.  */
    gc->SetValues(&modifiers, GCLineStyle | GCForeground | GCBackground);
  }
  else {			/* Superposition graphical contexts.  */
    

    modifiers.foreground = getSuperpositionColor(disp, win,selector);

    //    modifiers.line_style = LineDoubleDash;
    // gc->SetValues(&modifiers, GCLineStyle | GCForeground | GCBackground);
    gc->SetValues(&modifiers, GCForeground | GCBackground);
    /// gc->SetDashes(0, dashlist[selector], dashlen[selector]);
  }

  return gc->gc;
}

/*
** Functional Description:
**   Xamine_DrawChannels:
**     This function is the top level drawing routine for the channels within
**     a spectrum.  It is called from the refresh action routine.  We determine
**     the size of the drawing rectangle, the type of spectrum, rendition,
**     and then simply transfer control to the
**     routine that actually does the drawing.
** Formal Parameters:
**   Xamine_RefreshContext *ctx: 
**      The refresh context block.  In some cases this is augmented (e.g. for
**      2-d plots).
**   win_attributed *def:
**      Pointer to the pane's window attributes object.
** Returns:
**    FALSE -- Necessary to re-call us.
**    TRUE  -- Not necessary to recall us.
*/
int Xamine_DrawChannels(Xamine_RefreshContext *ctx, win_attributed *def)
{

  Boolean cont;

  /* Get the pane database, pane widget id and a few other X-11 goodies that
  ** everyone else needs:
  */

  XMWidget *pane = ctx->pane;
  Screen   *scr  = XtScreen(pane->getid());
  Display  *disp = XtDisplay(pane->getid());
  Drawable win   = ctx->pixmap;

  /* Get the geometry of the window... in particular we get the origin for
  ** the histogram part of the window and the number of pixels available in 
  ** the x and y direction.
  */

  int orgx, orgy;
  Dimension nx, ny;

  Xamine_getsubwindow(pane, def, &orgx,&orgy, &nx,&ny);


  /*
  ** Depending on the type of spectrum we call the appropriate drawer:
  */

  if(def->is1d()) {
    cont =  Xamine_Plot1d(scr, disp, def, win, pane,
			      orgx, orgy, nx, ny);
  }
  else {
    cont = Xamine_Plot2d(scr, disp,  def, win, pane,
			     orgx, orgy, nx, ny, ctx);
  }


  return cont; 
}
