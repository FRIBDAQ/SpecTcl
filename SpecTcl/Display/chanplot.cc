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
static char *sccsinfo="@(#)chanplot.cc	8.2 10/3/95 ";

/*
** Include files:
*/
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
  /* Initial guess for size is gotten from the widget's size in the
  ** resource set:
  */

  pane->GetAttribute(XmNwidth, nx);
  pane->GetAttribute(XmNheight, ny);
  *orgx = 0;
  *orgy = *ny;

  /* This can be modified by the margins... if there is an axis set, then
  ** there is a full margin of  XAMINE_MARGINSIZE scale pixels.
  */

  if(att->showaxes()) {
    int newnx,newny;


    /* Factor axis margins into the size and origin of the channel region */

    newnx = (int)((float)*nx *(1.0 - XAMINE_MARGINSIZE));
    *orgx = (*nx - newnx);
    *nx   = newnx;

    newny = (int)((float)*ny *(1.0 - XAMINE_MARGINSIZE));
    *orgy = newny;
    *ny   = newny;
    return;
  }
  /* If there are only titles present, then the X axis margin is present in
  ** half size and the Y axis margin is absent.
  */
  if(att->showname()    ||  att->shownum()  ||
     att->showdescrip() ||  att->showpeak() ||
     att->showupdt()    ||  att->showlbl()) {
    int newny;

    newny = (int)((float)*ny * (1.0 - XAMINE_MARGINSIZE/2.0));
    *ny   = newny;
    *orgy = newny;
  }
  
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
    /* If color, then modify the foreground color appropriately */

    if(Xamine_ColorDisplay()) {
      modifiers.foreground = 
	Xamine_PctToPixel(100*(selector+1)/MAX_SUPERPOSITIONS);
      modifiers.background = Xamine_PctToPixel(0);

    }
    /* Now make the base graphics context and then modify it with the
    ** appropriate dashes:
    */
    modifiers.line_style = LineDoubleDash;
    gc->SetValues(&modifiers, GCLineStyle | GCForeground | GCBackground);
    gc->SetDashes(0, dashlist[selector], dashlen[selector]);
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
