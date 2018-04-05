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

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";

/*
** Facility:
**   Display program graphical object subsystem.
** Abstract:
**   dispgrob.cc   - This file contains code for the graphical object
**                   methods which are not in-line methods.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**    @(#)dispgrob.cc	2.1 12/22/93 
*/

/*
** Include files required:
*/
#include <config.h>

#include <stdio.h>
#include <string.h>
#include <tcl.h>
#include "dispgrob.h"
#include "XMWidget.h"
#include "XBatch.h"
#include "dispwind.h"
#include "dispshare.h"
#include "convert.h"
#include "sumregion.h"
#include "acceptgates.h"
#include "grobjdisplay.h" 
#include "marker.h"
#include "gcmgr.h"
#include "xaminegc.h"
#include "gc.h"
#include "chanplot.h"
#include "colormgr.h"

#include <iostream>

#include <math.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
/*
** Local storage:
**
*/

/*
** External references:
*/
extern volatile spec_shared *xamine_shared;

/*
** Functional Description:
**    Swap:
**      Template function to swap two arbitrarily typed items:
** Formal Parameters:
**    T   - Type of parameters.   Must have a copy constructor or an
**          assignment operator defined, null constructor must also be defined.
**
**    T& item1,item2:
**        The items to swap.
*/
template <class T>
static void Swap(T& item1, T& item2)
{
  T temp;
  temp  = item1;
  item1 = item2;
  item2 = temp;
}

/*
** Functional Description:
**    SetClipRegion:
**      Set the clipping region for drawing a graphical object... the
**      clip region is set to be the piece of the pane which contains
**      the drawn part of the spectrum.
** Formal Paramters:
**   XamineGrobjGC *gc:
**      Pointer to the graphical context object whose clip region will be set.
**   XMWidget *wid:
**      Widget which the gc is in.
**    win_attributed *attributes:
**       Attributes of the spectrum.
*/
static void SetClipRegion(XamineGrobjGC *gc, XMWidget *wid, 
			  win_attributed *attributes)
{
  gc->ClipToSpectrum(*wid, *attributes);
}



/*
** Method Description:
**   addpt   - Adds a point to the end of the graphical object's point list.
**             Ensures that the point buffer does not overflow.
** Formal Parameters:
**   int x:
**      X coordinates of the point (defaults to zero).
**   int y:
**     Y coordinates of the point (defaults to zero).
** Returns:
**     0   - If failed.
**    npts - now in graphical object if success.
*/
int grobj_generic::addpt(int x, int y)
{
  if( (npts < GROBJ_MAXPTS) ) {
    pts[npts].set(x,y);
    npts++;
    return npts;
  }
  return 0;
}

/*
** Method Description:
**   delpt    -- Delete points from the point list of the graphical object.
**               This function has two instances, one with a location
**               and one without a location.  The function with a location
**               specifies which point is to be deleted (numbered from zero).
**               The function without a location deletes the last point.
**               The location less function is in-line.
** Formal Parameters:
**    int loc:
**       Optional parameter which specifies which point (numbered from zero)
**       should be deleted.
** Returns:
**   -1 if invalid point or nonexistent point.
**    0 If point deleted was the last one.
**  npts Number of points remaining.
*/
int grobj_generic::delpt(int loc)
{
  if(loc < npts) {
    npts--;
    for(int i = loc; i < npts ; i++)
      pts[i] = pts[i+1];	/* Slide pts down into hole if needed. */
    return npts;
  }
  return -1;
}

/*
** Method Description:
**   insertpt    - This function inserts a point into the middle of
**                 a graphical object's point list.  
** Formal Parameters:
**   int loc:
**     Point number (numbered from zero) at which to insert the point.
**     The point at that point number and all subsequent points are bumped
**     foreward one position.
**  int x,y:
**     The value of the point to add.
** Returns:
**    -1  - Location is not a valid location.
**     0  - Point list is already full.
**   npts - Number of points in list after insertion.
**/
int grobj_generic::insertpt(int loc,int x, int y)
{
  if(npts < GROBJ_MAXPTS) {
    if(loc < npts) {
      for(int i = npts; i > loc; i--)
	pts[i] = pts[i-1];	/* Slide everything up */
      pts[loc].set(x,y);	/* Insert the new point */
      npts++;
      return npts;
    }
    else
      return -1;
  }
  else
    return 0;
}

/*
** Method Description:
**   getpt     -  Returns the specified point from the point list.
** Formal Parameters:
**   int loc:
**     Specifies the point to get (numbered from zero).
** Returns:
**   A pointer to the selected point.
**   NULL if no such point.
*/
grobj_point *grobj_generic::getpt(int loc)
{
  if(loc < npts)
    return &pts[loc];
  return NULL;
}
/*
** Method description:
**  nextpt   - Retrieves the next point in a traversal context set up
**             by firstpt.
** Returns:
**   Pointer to the next point or NULL if there is no next point.
*/
grobj_point *grobj_generic::nextpt()
{
  where++;
  if(where < npts) {
    return &pts[where];
  }
  where--;
  return NULL;
}
/*
** Method Description:
**   *::type   - Returns the type of the graphical object.
**/
grobj_type grobj_generic::type() { return generic; }
grobj_type grobj_cut1d::type()   { return cut_1d;  }
grobj_type grobj_sum1d::type()   { return summing_region_1d; }
grobj_type grobj_mark1d::type()  { return marker_1d; }
grobj_type grobj_contour::type() { return contour_2d; }
grobj_type grobj_band::type()    { return band; }
grobj_type grobj_sum2d::type()   { return summing_region_2d; }
grobj_type grobj_mark2d::type()  { return marker_2d; }








/*
** The pages below provide drawing methods for all of the
** graphical object types.  Each method draws the object on a widget which
** is assumed to be a drawing area widget.
*/

/*
** Functional Description:
**   DrawPlusses:
**     This function draws a set of + marks at various points in a drawable.
** Formal Parameters:
**   Display *d:
**     Display Connection identifier.
**   Drawable win:
**     Id of the drawable, could be a window or a pixmap.
**   GC gc:
**     Handle to the graphical context to use for drawing.
**   Xamine_Converter *cvt:
**     Pointer to a coordinates convesion routine.
**   Boolean flipped:
**     True if the spectrum is displayed in an axis flipped representation.
**   grobj_generic: *o:
**     The object to draw.
*/
static void DrawPlusses(Display *d, Drawable win, GC gc,
		    Xamine_Converter *cvt, Boolean flipped, grobj_generic *o)
{
  XSegmentBatch crosses(d, win, gc); /* Instantiate a segment drawer. */

  grobj_point *pt = o->firstpt();

  while(pt != NULL) { 
    int sx,sy;			/* Spectrum points. */
    int x,y;			/* Screen points.   */

    sx = pt->getx(); sy = pt->gety();

    if(flipped) {
       int t;
       switch(o->type()) {
         case contour_2d:
	 case summing_region_2d:	/* If necessary flip the axes */
	 case marker_2d:
	 case pointlist_2d:
	    t = sx;
	    sx= sy;
	    sy= t;
            break;
	 default:
	    break;
       }
    }
    cvt->SpecToScreen(&x, &y, sx, sy);

    crosses.draw((short)(x - 4), (short)(y), 
		 (short)(x + 4), (short)(y));
    crosses.draw((short)(x), (short)(y + 4), 
		 (short)(x), (short)(y - 4));

    pt = o->nextpt();
  }
}
/*
** Functional Description:
**   DrawXes:
**     This is a local function which draws X-crosses at the vertices
**     of a graphical object.  It is used typically while drawing objects to 
**     show where the points are in high relief.
** Formal Parameters:
**   Display *d:
**      Pointer to the X11 display connection id.
**  Drawable win:
**     Drawable Id.  May be a window or a pixmap.
**  GC gc:
**     Graphical context which describes how the drawing should be done.
**  Xamine_Converter *cvt:
**     Pointer to the coordinate conversion for this window.
**  Boolean flipped:
**     True if the spectrum is displayed in axis flipped representation.
**  grobj_generic *o:
**     The graphical object to draw.
*/
static void DrawXes(Display *d, Drawable win, GC gc,
		    Xamine_Converter *cvt, Boolean flipped, grobj_generic *o)
{
  XSegmentBatch crosses(d, win, gc); /* Instantiate a segment drawer. */

  grobj_point *pt = o->firstpt();

  while(pt != NULL) { 
    int sx,sy;			/* Spectrum points. */
    int x,y;			/* Screen points.   */

    sx = pt->getx(); sy = pt->gety();
    if(flipped) {
       int t;
       switch(o->type()) {
       case contour_2d:
       case summing_region_2d:	/* If necessary flip the axes */
       case marker_2d:
       case pointlist_2d:
       case band:
	    t = sx;
	    sx= sy;
	    sy= t;
            break;
	 default:
	    break;
       }
    }

    cvt->SpecToScreen(&x, &y, sx, sy);

    crosses.draw((short)(x - 4), (short)(y - 4), 
		 (short)(x + 4), (short)(y + 4));
    crosses.draw((short)(x - 4), (short)(y + 4), 
		 (short)(x + 4), (short)(y - 4));

    pt = o->nextpt();
  }

  

}    

/*
** functional Description:
**   grobj_sum1d::draw:
**      This function draws a 1-d summing region on a given pane.
**      In final mode, the summing region uses a dashed line, while in
**      non-final mode a dotted line is used.  The pane is used
**      to derive everything else we need.
** Formal Parameters:
**   XMWidget *pane:
**     A drawing area widget into which to draw the pane.
**  win_attributed *at:
**    The attributes of the pane.
**  Boolean final:
**    True if the plot should be in final form... in which case we'll used
**    dashed lines, and non xor drawing.  Otherwise we'll use dotted lines
**    and Xor drawing.  If final is on, then labels can also be drawn.
*/
void grobj_sum1d::draw(XMWidget *pane, win_attributed *at, Boolean final)
{
  Display *d = XtDisplay(pane->getid());
  Window  win= XtWindow(pane->getid());
  Drawable pm= (Drawable)NULL;

  /* Determine if there's a backing store too.. we rely on the fact that
  ** the pane has encoded in it the widget index:
  */
  long index, row, col;
  pane->GetAttribute(XmNuserData, &index);
  col = index % WINDOW_MAXAXIS;
  row = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  /* Next construct a converter for the drawing operation to convert
  ** channel coords into pixels. Since the object is a 1-d we're assured
  ** that we want a 1-d converter
  */
  Xamine_Convert1d cvt(pane, at, xamine_shared);
  
  /* Next figure out the remaining arguments for Xamine_DrawSumRegion
  ** These are flipped, labelit, and the graphical context.  labelit
  ** and the graphical context depend on the final argument
  */

  Boolean flipped = 
    at->isflipped();	/* Flipped however is just a spectrum attrib */

  Boolean labelit = False;
  if(final && at->showlbl()) 
    labelit = True;

  /* Finally construct the graphical context.  The line style depends on 
  ** the final argument... dashed if final otherwise solid
  ** as does function Set if final otherwise Xor
  */
  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pane);
  SetClipRegion(xgc, pane, at);
  xgc->Set1DColors(*pane);
  GC gc;
  if(final) {
    xgc->SetSumRegion();
  }
  else {
    xgc->SetProvisional();
  }
  gc = xgc->gc;

  /* Draw the lines.. n both the window and the pixmap (if present). */

  Xamine_DrawSumRegion(d, win, gc, pane, &cvt, flipped, labelit,
		       this);
  if(pm) {
    Xamine_DrawSumRegion(d, pm, gc, pane, &cvt, flipped, labelit,
			 this);
  }

}

/*
** Functional Description:
**   grobj_cut1d::draw:
**     Draws a 1-d cut on the current window pane.
** Formal Parameters:
**   XMWidget *pane:
**     The pane to draw on.
**   win_attriburted *at:
**     The attributes of the pane
**  Boolean Final:
**     True if the object should be drawn in final attributes.
*/
void grobj_cut1d::draw(XMWidget *pane, win_attributed *at, Boolean final)
{
  Display *d = XtDisplay(pane->getid());
  Window  win= XtWindow(pane->getid());
  Drawable pm= (Drawable)NULL;

  /* Determine if there's a backing store too.. we rely on the fact that
  ** the pane has encoded in it the widget index:
  */
  long index, row, col;
  pane->GetAttribute(XmNuserData, &index);
  col = index % WINDOW_MAXAXIS;
  row = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  /* Next construct a converter for the drawing operation to convert
  ** channel coords into pixels. Since the object is a 1-d we're assured
  ** that we want a 1-d converter
  */
  Xamine_Convert1d cvt(pane, at, xamine_shared);
  
  /* Next figure out the remaining arguments for Xamine_DrawSumRegion
  ** These are flipped, labelit, and the graphical context.  labelit
  ** and the graphical context depend on the final argument
  */

  Boolean flipped = 
    at->isflipped();	/* Flipped however is just a spectrum attrib */

  Boolean labelit = False;
  if(final && at->showlbl()) 
    labelit = True;

  /* Finally construct the graphical context.  The line style depends on 
  ** the final argument... dashed if final otherwise solid
  ** as does function Set if final otherwise Xor
  */

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pane);
  SetClipRegion(xgc, pane, at);
  xgc->Set1DColors(*pane);
  GC gc;
  if(final) {
    xgc->SetPermanent();
    
  }
  else {
    xgc->SetProvisional();
  }
  gc = xgc->gc;

  /* Draw the lines.. in both the window and the pixmap (if present). */

  Xamine_DisplayCut(d, win, gc, pane, &cvt, flipped, labelit, this);
  if(pm)
    Xamine_DisplayCut(d, pm, gc, pane, &cvt, flipped, labelit,this);
}


/*
** Functional Description:
**   grobj_mark1d:draw:
**     Method to draw a marker.  Final markers are drawn as filled
**     diamonds.  Tentative markers are drawn as dashed diamonds.
** Formal Parameters:
**   XMWidget *pane:
**     The pane to draw on.
**   win_attriburted *at:
**     The attributes of the pane
**  Boolean Final:
**     True if the object should be drawn in final attributes.
*/
void grobj_mark1d::draw(XMWidget *pane, win_attributed *at, Boolean final)
{
  Display *d = XtDisplay(pane->getid());
  Window  win= XtWindow(pane->getid());
  Drawable pm= (Drawable)NULL;


  /* Determine if there's a backing store too.. we rely on the fact that
  ** the pane has encoded in it the widget index:
  */
  long index, row, col;
  pane->GetAttribute(XmNuserData, &index);
  col = index % WINDOW_MAXAXIS;
  row = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  /* Next construct a converter for the drawing operation to convert
  ** channel coords into pixels. Since the object is a 1-d we're assured
  ** that we want a 1-d converter
  */
  Xamine_Convert1d cvt(pane, at, xamine_shared);
  
  /* Next figure out the remaining arguments for Xamine_DrawSumRegion
  ** These are flipped, labelit, and the graphical context.  labelit
  ** and the graphical context depend on the final argument
  */

  Boolean labelit = False;
  if(final && at->showlbl()) 
    labelit = True;

  /* Finally construct the graphical context.  The line style depends on 
  ** the final argument... dashed if final otherwise solid
  ** as does function Set if final otherwise Xor
  */

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pane);
  SetClipRegion(xgc, pane, at);
  xgc->Set1DColors(*pane);
  GC gc;
  if(final) {
    gc = xgc->gc;
    Xamine_PlotMarker(d, win, gc, pane, labelit, &cvt, this, at);
    if(pm)
      Xamine_PlotMarker(d,pm,gc, pane, labelit, &cvt, this, at);
  }
  else {
     
    grobj_point *pt = getpt(0);
    int x = pt->getx(),
        y = pt->gety();
    int sx,sy;
    
    cvt.SpecToScreen(&sx, &sy, x,y);
    xgc->SetProvisional();
    gc = xgc->gc;
    Xamine_DrawMarker(d, win, gc, sx, sy);
    if(pm)
      Xamine_DrawMarker(d, pm, gc, sx, sy);
  }
 
}

/*
** Functional Description:
**   grobj_contour::draw:
**     This method tells a contour to draw itself in either accepted or
**     tentative form.
** Formal Parameters:
**     XMWidget *pane:
**        Pane in which the contour should draw.
**     win_attributed *at:
**        Attributes of the pane.
**     Boolean final:
**        True if to draw in final form or not.
*/
void grobj_contour::draw(XMWidget *pane, win_attributed *at, Boolean final)
{
  Display *d = XtDisplay(pane->getid());
  Window  win= XtWindow(pane->getid());
  Drawable pm= (Drawable)NULL;

  /* Determine if there's a backing store too.. we rely on the fact that
  ** the pane has encoded in it the widget index:
  */
  long index, row, col;
  pane->GetAttribute(XmNuserData, &index);
  col = index % WINDOW_MAXAXIS;
  row = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  /* Next construct a converter for the drawing operation to convert
  ** channel coords into pixels. Since the object is a 1-d we're assured
  ** that we want a 1-d converter
  */
  Xamine_Convert2d cvt(pane, at, xamine_shared);
  
  /* Next figure out the remaining arguments for Xamine_DrawSumRegion
  ** These are flipped, labelit, and the graphical context.  labelit
  ** and the graphical context depend on the final argument
  */

  Boolean flipped = 
    at->isflipped();	/* Flipped however is just a spectrum attrib */

  Boolean labelit = False;
  if(final && at->showlbl()) 
    labelit = True;

  /* Finally construct the graphical context.  The line style depends on 
  ** the final argument... dashed if final otherwise solid
  ** as does function Set if final otherwise Xor
  */

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pane);
  SetClipRegion(xgc, pane, at);
  xgc->Set2DColors(*pane);
  GC gc = xgc->gc;
  if(final) {
    xgc->SetPermanent();
  }
  else {
    xgc->SetProvisional();

    /* Draw the x'es which mark the points */

    DrawXes(d,win,gc, &cvt, flipped,  this);
    if(pm)
      DrawXes(d,pm,gc, &cvt, flipped,  this);

  }
  Xamine_DisplayContour(d, win, gc, pane, &cvt,
			flipped, labelit, this);
  if(pm)
    Xamine_DisplayContour(d,pm,gc, pane, &cvt,
			  flipped, labelit, this);

}


/*
** Functional Description:
**     grobj_band::draw:
**        This function tells the object to draw itself.
**        The object can either be drawn tentatively (with XOR attributes and
**        dashed lines) or it can be drawn finally with solid lines and
**        XOR attributes.
** Formal Parameters:
**    XMWidget *pane:
**      The pane to draw in.
**    win_attributed *at:
**      Pane rendering attributes
**    Boolean final:
**      True if to be drawn in final form.
*/
void grobj_band::draw(XMWidget *pane, win_attributed *at, Boolean final)
{
  Display *d = XtDisplay(pane->getid());
  Window  win= XtWindow(pane->getid());
  Drawable pm= (Drawable)NULL;

  /* Determine if there's a backing store too.. we rely on the fact that
  ** the pane has encoded in it the widget index:
  */
  long index, row, col;
  pane->GetAttribute(XmNuserData, &index);
  col = index % WINDOW_MAXAXIS;
  row = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  /* Next construct a converter for the drawing operation to convert
  ** channel coords into pixels. Since the object is a 1-d we're assured
  ** that we want a 1-d converter
  */
  Xamine_Convert2d cvt(pane, at, xamine_shared);
  
  /* Next figure out the remaining arguments for Xamine_DrawSumRegion
  ** These are flipped, labelit, and the graphical context.  labelit
  ** and the graphical context depend on the final argument
  */

  Boolean flipped = 
    at->isflipped();	/* Flipped however is just a spectrum attrib */

  Boolean labelit = False;
  if(final && at->showlbl()) 
    labelit = True;

  /* Finally construct the graphical context.  The line style depends on 
  ** the final argument... dashed if final otherwise solid
  ** as does function Set if final otherwise Xor
  */

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pane);
  SetClipRegion(xgc, pane, at);
  xgc->Set2DColors(*pane);
  GC gc = xgc->gc;
  if(final) {
    xgc->SetPermanent();
  }
  else {
    xgc->SetProvisional();


    /* Draw the x'es which mark the points */

    DrawXes(d,win,gc, &cvt, flipped,  this);
    if(pm)
      DrawXes(d,pm,gc, &cvt, flipped,  this);

  }
  Xamine_DisplayBand(d, win, gc, pane, &cvt,
			flipped, labelit, this);
  if(pm)
    Xamine_DisplayBand(d,pm,gc, pane, &cvt,
			  flipped, labelit, this);

}

/*
** Functional Description:
**   grobj_sum2d:draw:
**     This method tells a summing region to draw itself into a pane.
** Formal Parameters:
**   XMWidget *pane:
**     The pane into which we should draw ourselves.
**   win_attributed *at:
**     The attributes of the pane.
**   Boolena final:
**     True if the object is being drawn in final form.
*/
void grobj_sum2d::draw(XMWidget *pane, win_attributed *at, Boolean final)
{
  Display *d = XtDisplay(pane->getid());
  Window  win= XtWindow(pane->getid());
  Drawable pm= (Drawable)NULL;

  /* Determine if there's a backing store too.. we rely on the fact that
  ** the pane has encoded in it the widget index:
  */
  long index, row, col;
  pane->GetAttribute(XmNuserData, &index);
  col = index % WINDOW_MAXAXIS;
  row = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  /* Next construct a converter for the drawing operation to convert
  ** channel coords into pixels. Since the object is a 1-d we're assured
  ** that we want a 1-d converter
  */
  Xamine_Convert2d cvt(pane, at, xamine_shared);
  
  /* Next figure out the remaining arguments for Xamine_DrawSumRegion
  ** These are flipped, labelit, and the graphical context.  labelit
  ** and the graphical context depend on the final argument
  */

  Boolean flipped = 
    at->isflipped();	/* Flipped however is just a spectrum attrib */

  Boolean labelit = False;
  if(final && at->showlbl()) 
    labelit = True;

  /* Finally construct the graphical context.  The line style depends on 
  ** the final argument... dashed if final otherwise solid
  ** as does function Set if final otherwise Xor
  */

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pane);
  SetClipRegion(xgc, pane, at);
  xgc->Set2DColors(*pane);
  GC gc = xgc->gc;
  if(final) {

    /* Need to set dashed lines: */

    xgc->SetSumRegion();
  }
  else {
    xgc->SetProvisional();


    /* Draw the x'es which mark the points */

    DrawXes(d,win,gc, &cvt, flipped,  this);
    if(pm)
      DrawXes(d,pm,gc, &cvt, flipped,  this);


  }
  Xamine_DrawSumRegion(d, win, gc, pane, &cvt,
			flipped, labelit, this);
  if(pm)
    Xamine_DrawSumRegion(d,pm,gc, pane, &cvt,
			 flipped, labelit, this);
}

/*
** Functional Description:
**   grobj_mark2d::draw:
**     Tells the marker to draw itself in a given pane.
**  Formal Parameters:
**     XMWidget *pane:
**       The pane into which the marker should be drawn.
**     win_attributed *at:
**       The pane drawing attributes.
**     Boolean final:
**       True if we're to draw the marker in final form.
*/
void grobj_mark2d::draw(XMWidget *pane, win_attributed *at, Boolean final)
{
  Display *d = XtDisplay(pane->getid());
  Window  win= XtWindow(pane->getid());
  Drawable pm= (Drawable)NULL;

  /* Determine if there's a backing store too.. we rely on the fact that
  ** the pane has encoded in it the widget index:
  */
  long index, row, col;
  pane->GetAttribute(XmNuserData, &index);
  col = index % WINDOW_MAXAXIS;
  row = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  /* Next construct a converter for the drawing operation to convert
  ** channel coords into pixels. Since the object is a 1-d we're assured
  ** that we want a 1-d converter
  */
  Xamine_Convert2d cvt(pane, at, xamine_shared);
  
  /* Next figure out the remaining arguments for Xamine_DrawSumRegion
  ** These are flipped, labelit, and the graphical context.  labelit
  ** and the graphical context depend on the final argument
  */

  Boolean flipped = 
    at->isflipped();	/* Flipped however is just a spectrum attrib */

  Boolean labelit = False;
  if(final && at->showlbl()) 
    labelit = True;

  /* Finally construct the graphical context.  The line style depends on 
  ** the final argument... dashed if final otherwise solid
  ** as does function Set if final otherwise Xor
  */

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pane);
  SetClipRegion(xgc, pane, at);
  xgc->Set2DColors(*pane);
  GC gc = xgc->gc;
  if(final) {
    xgc->SetPermanent();

    Xamine_PlotMarker(d, win, gc, pane, labelit, &cvt, this, at);
    if(pm)
      Xamine_PlotMarker(d,pm,gc, pane, labelit, &cvt, this, at);
  }
  else {
    xgc->SetProvisional();
    grobj_point *pt = getpt(0);
    int x = pt->getx(),
        y = pt->gety();
    if(flipped) {
      int t = x;
      x = y;
      y = t;
    }
    int sx,sy;
    
    cvt.SpecToScreen(&sx, &sy, x,y);
    Xamine_DrawMarker(d, win, gc, sx, sy);
    if(pm)
      Xamine_DrawMarker(d, pm, gc, sx, sy);
  }

}

/*
** Functions on the following pages are methods for the grobj_ptlist1 class.
** A grobj_ptlist1 is used to keep track of the points being accepted by a
** user point list.  It is convenient to use a grobj like object to do this
** since all of the plotting routines etc. are formulated with grobj in mind.
** User point lists are essentially temporary graphical objects anyway.
** grobj_ptlist1 is used for 1-d point lists.  See also grobj_ptlist2 used for
** 2-d point lists.
*/


/*
** Functional Description:
**   grobj_ptlist1::addpt:
**     adds a point to the end of a point list.
** Formal Parameters:
**   int x:
**     The x coordinate of the new point (channel number).
**     The y coordinate of the new point (counts if supplied).
** Returns:
**   0    - If failed (e.g. too many points) 
**  npts  - Number of points in the point list.
*/

int grobj_ptlist1::addpt(int x, int y)
{
  if(pointcount() >= maxpts)
    return 0;
  else
    return grobj_generic::addpt(x, y);
}   


/*
** Functional Description:
**   grobj_ptlist1::insertpt:
**     Insert a point at an arbitrary position of the point list.
** Formal Parameters:
**   int loc:
**     Location of inserted point.
**   int x,y:
**     Coordinates of the inserted point (channels, counts).
** Returns:
**   0  - If failed (e.g. too many points... or too few)
**  npts- Number of points in the new point list.
*/

int grobj_ptlist1::insertpt(int loc, int x, int y)
{
  if(pointcount() >= maxpts)
    return 0;
  else
    return grobj_generic::insertpt(loc, x, y);
}   


/*
** Functional Description:
**   grobj_ptlist1::clone:
**     Cones an instance of a point list for a 1-d spectrum.
** Returns:
**   pointer to the new point list which must be delete'd when no longer
**   needed.
*/

grobj_generic *grobj_ptlist1::clone()
{
  grobj_ptlist1 *pl = new grobj_ptlist1(maxpts);
  *pl       = *this;
  return (grobj_generic *)pl;

}   


/*
** Functional Description:
**   grobj_ptlist1::draw:
**     This function draws a point list on a widget which is assumed to be
**     a drawing area widget.  The points are drawn as little plus symbols to
**     distinguish them from Xamine's X points... although X points are not
**     used in 1-d grobj's.   We use +'s to preserve the height information
**     of the grobj points.
** Formal Parameters:
**   XMWidget *p:
**     The widget in which to draw.
**   win_attributed *at:
**     The window attributes list.
**   Boolean final:
**     True if drawing the final version... note there is no final version
**     for point lists, so this argument is ignored.
*/

void grobj_ptlist1::draw(XMWidget *p, win_attributed *at, Boolean final)
{
  Display   *d = XtDisplay(p->getid());
  Window   win = XtWindow(p->getid());
  Drawable  pm = (Drawable)NULL; // Possible backing pixmap.

  /*  If the pane has a backing pixmap, then we want to draw into that too
  **  so that if the window is contaiminated while accepting points, the 
  **  partial point set will be visible.
  */
  long index, row, col;
  p->GetAttribute(XmNuserData, &index); // This is the pane number.
  row = index % WINDOW_MAXAXIS;
  col = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  /*
  ** Construct a coordinate converter.  It's a 1-d converter since
  ** we're drawing in a 1-d pane.
  */

  Xamine_Convert1d cvt(p, at, xamine_shared);
  
  /* Now we need to figure out the remaining argument for DrawPlusses:
  */

  Boolean flipped = at->isflipped();

  /* We always create tentative graphical contexts: */

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*p);
  SetClipRegion(xgc, p,at);
  xgc->Set1DColors(*p);
  xgc->SetProvisional();
  GC gc = xgc->gc;

  /* Draw the plusses which mark the point:  */

  DrawPlusses(d, win, gc, &cvt, flipped, this);
  if(pm)
    DrawPlusses(d, pm, gc, &cvt, flipped, this);

}   


/*
**  The pages which follow contain method functions for the grahpical
**  object grobj_ptlist2.  Thist type of grahpical object is used to 
**  store a point list that is being accumulated for a client button request
**  prompter in a 2-d spectrum pane.  See also grobj_ptlist2 above.
*/


/*
** Functional Description:
**   grobj_ptlist2::addpt:
**     adds a point to the end of a point list.
** Formal Parameters:
**   int x:
**     The x coordinate of the new point (channel number).
**     The y coordinate of the new point (counts if supplied).
** Returns:
**   0    - If failed (e.g. too many points) 
**  npts  - Number of points in the point list.
*/

int grobj_ptlist2::addpt(int x, int y)
{
  if(pointcount() >= maxpts)
    return 0;
  else
    return grobj_generic::addpt(x, y);
}   


/*
** Functional Description:
**   grobj_ptlist2::insertpt:
**     Insert a point at an arbitrary position of the point list.
** Formal Parameters:
**   int loc:
**     Location of inserted point.
**   int x,y:
**     Coordinates of the inserted point (channels, counts).
** Returns:
**   0  - If failed (e.g. too many points... or too few)
**  npts- Number of points in the new point list.
*/

int grobj_ptlist2::insertpt(int loc, int x, int y)
{
  if(pointcount() >= maxpts)
    return 0;
  else
    return grobj_generic::insertpt(loc, x, y);
}   


/*
** Functional Description:
**   grobj_ptlist2::clone:
**     Clones an instance of a point list for a 1-d spectrum.
** Returns:
**   pointer to the new point list which must be delete'd when no longer
**   needed.
*/

grobj_generic *grobj_ptlist2::clone()
{

  grobj_ptlist2 *pl = new grobj_ptlist2(maxpts);
  *pl = *this;
  return (grobj_generic *)pl;
}   


/*
** Functional Description:
**   grobj_ptlist2::draw:
**     This function draws a point list on a widget which is assumed to be
**     a drawing area widget.  The points are drawn as little plus symbols to
**     distinguish them from Xamine's X points... although X points are not
**     used in 1-d grobj's.   We use +'s to preserve the height information
**     of the grobj points.
** Formal Parameters:
**   XMWidget *p:
**     The widget in which to draw.
**   win_attributed *at:
**     The window attributes list.
**   Boolean final:
**     True if drawing the final version... note there is no final version
**     for point lists, so this argument is ignored.
*/

void grobj_ptlist2::draw(XMWidget *p, win_attributed *at, Boolean final)
{
  Display   *d = XtDisplay(p->getid());
  Window   win = XtWindow(p->getid());
  Drawable  pm = (Drawable)NULL; // Possible backing pixmap.

  /*  If the pane has a backing pixmap, then we want to draw into that too
  **  so that if the window is contaiminated while accepting points, the 
  **  partial point set will be visible.
  */
  long index, row, col;
  p->GetAttribute(XmNuserData, &index); // This is the pane number.
  row = index % WINDOW_MAXAXIS;
  col = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  /*
  ** Construct a coordinate converter.  It's a 1-d converter since
  ** we're drawing in a 1-d pane.
  */

  Xamine_Convert2d cvt(p, at, xamine_shared);
  
  /* Now we need to figure out the remaining argument for DrawPlusses:
  */

  Boolean flipped = at->isflipped();

  /* We always create tentative graphical contexts: */

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*p);
  SetClipRegion(xgc, p, at);
  xgc->Set2DColors(*p);
  xgc->SetProvisional();
  GC gc = xgc->gc;

  /* Draw the plusses which mark the point:  */

  DrawPlusses(d, win, gc, &cvt, flipped, this);
  if(pm)
    DrawPlusses(d, pm, gc, &cvt, flipped, this);

}   

//////////////////////////////////////////////////////////////////////
// The methods below implement the grobj_Peak1d class.  That class is 
// responsible for maintaining and displaying 1-d peak objects on a
// spectrum.
//
// Functional Description:
//    grobj_Peak1d:
//          Constructs a 1d peak graphical object.
// Formal Parameters:
//    int nSpectrum:
//          Identifies the spectrum on which the object is applied.
//    int nId:
//          A unique Identifying number which the client can use to refer to
//          the object.  This is the number which is also displayed when the
//          labels are turned on.
//    grobj_name pszName:
//          Points to a name/comment string which is associated with the grobj.
//          The INFO button or menu entry will display this text as the comment
//          field for the object.
//    float fCentroid:
//          The centroid of the peak being identified.
//    float fWidth:
//          The width of the peak being identifies.
//  NOTE:
//      The point list is not used by this graphical object... yet another
//      reason to make it a dynamic (e.g. STL) list.
//
grobj_Peak1d::grobj_Peak1d(int nSpectrum,
			   int nId,
			   grobj_name pszName,
			   float fCentroid,
			   float fWidth) :
  grobj_generic(), 
  m_fFWHM(fWidth), 
  m_fPosition(fCentroid)
{
  setspectrum(nSpectrum);
  setid(nId);
  if(name != NULL) {
    setname(pszName);
  }
}
//
// Functional Description:
//   draw:
//     This member function draws the object on a window of the Xamine display.
// Formal Parameters:
//    XMWidget* pWindow:
//           Pointer to the widget containing the window in which the object
//           should be drawn.
//    win_attributed* pAttributes:
//           Pointer to the window attributes block which specifies display
//           attributes/modifications for the spectra.
//    Boolean fFinal:
//           True if the draw is for a final grobj, rather than a tentative
//           one.  There are no tentative peak markers, so this parameter is
//           ignored.
//
void grobj_Peak1d::draw(XMWidget* pWindow,
			win_attributed* pAttributes,
			Boolean fFinal)
{
  Display*  d = XtDisplay(pWindow->getid());
  Window  win = XtWindow(pWindow->getid());
  Drawable pm = (Drawable)NULL;

  // If there's a pixmap to accelerate refresh, then get it into pm.
  // we rely on the fact that the pane widget has the pane number in it's
  // user data field:

  long index, row, col;
  pWindow->GetAttribute(XmNuserData, &index);
  col = index % WINDOW_MAXAXIS;
  row = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  // We need a converter to convert the spectrum coordinates of the centroid
  // and width as well as heights into pixels.  The underlying spectrum is
  // gaurenteed to be 1d:

  Xamine_Convert1d cvt(pWindow, pAttributes, xamine_shared);
  cvt.NoClip();

  // Extract the flip and label flag fromt the spectrum attributes:

  Boolean flipped = pAttributes->isflipped();
  Boolean labelit = pAttributes->showlbl();

  // Create a graphical context which will be used for the drawing.

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pWindow);
  SetClipRegion(xgc, pWindow, pAttributes);
  xgc->Set1DColors(*pWindow);
  xgc->SetPermanent();
  GC gc = xgc->gc;

  // Now we draw the peak position.  Since this is a new grobj type and we
  // don't have to recycle old code. the draw is done here.  We do have some
  // helper functions.  The helpers determine in spectrum coordinates 
  // (floating) how high the horizontal line is, and where and how high the
  // vertical lines are to be drawn.  These are expressed in terms of 
  // channel value percentages, to ensure that the object will 'float' above
  // the peak.  This also allows the peak marked while spectra are still
  // incrementing.

  unsigned nBarHeight = ComputeHeight(m_fPosition); // Height of horizontal bar
  float fBarLeft   = m_fPosition  - m_fFWHM/2.0;   // left bound of peak.
  if(fBarLeft < 0.0) fBarLeft = 0;                  // For peaks close to edge.
  float fBarRight  = m_fPosition + m_fFWHM/2.0;   // right bound of peak.
  if(fBarRight >= (float)xamine_shared->getxdim(getspectrum()))
    fBarRight = (float)xamine_shared->getxdim(getspectrum())-1.0;
  unsigned nDropLeft  = ComputeBarHeight(fBarLeft);  // Where to drop left bar.
  unsigned nDropRight = ComputeBarHeight(fBarRight); // Where to drop right bar.
  unsigned nPeakDelta = ComputeDeltaHeight(m_fPosition); // Center spike height.
  
  //
  //  Convert the channel coordinates to pixels... including interpolation.
  //
  short npBarLeft;		// Pixel coordinates of left part of bar.
  short npBarLBottom;		// Pixel coords of bottom of Left bar.
  short npBarHeight;		// Pixel coords of top of bar.
  short npBarRight;		// Pixel coords of right of bar.
  short npBarRBottom;		// Pixel coords of bottom of right bar
  short npPeakHt;			// Pixel coords of peak spike.
  short npBarMid;			// Middle of the bar.
  short npUnused;

  // Figure out all the points we need to draw:
  //
  ComputeLocation(cvt, fBarLeft, nDropLeft, npBarLeft, npBarLBottom);
  if(flipped) Swap(npBarLeft, npBarLBottom);
  ComputeLocation(cvt, 0.0,      nBarHeight, npUnused, npBarHeight);
  if(flipped) npBarHeight = npUnused;
  ComputeLocation(cvt, fBarRight, nDropRight, npBarRight, npBarRBottom);
  if(flipped) Swap(npBarRight, npBarRBottom);
  ComputeLocation(cvt, m_fPosition, nPeakDelta,
		  npBarMid, npPeakHt);
  if(flipped) Swap(npBarMid, npPeakHt);

  XSegmentBatch XSPeak(d, win, gc);  // Polyline for drawing the peak.
  DrawMarker(XSPeak, flipped,
	     npBarLeft, npBarLBottom, npBarHeight, 
	     npBarRight, npBarRBottom,
	     npBarMid, npPeakHt);
  char label[10];
  sprintf(label, "P%d", getid());
  if(labelit) Xamine_LabelGrobj(d, win, gc, pWindow,
				label,
				flipped ? npBarHeight : npBarMid, 
				flipped ? npBarMid    : npBarHeight);

  // If there's a pixmap, we need to draw into it too:

  if(pm) {
    XSegmentBatch XSpmPeak(d, pm, gc);
    DrawMarker(XSpmPeak, flipped,
	       npBarLeft, npBarLBottom, npBarHeight, 
	       npBarRight, npBarRBottom,
	       npBarMid, npPeakHt);
    if(labelit) {
      Xamine_LabelGrobj(d, pm, gc, pWindow,
			label,
			flipped ? npBarHeight : npBarMid, 
			flipped ? npBarMid    : npBarHeight);

    }
  }

}

//
// Functional Description:
//    clone:
//      This function produces a copy of *this.  
//      The copy is dynamically allocated, so it's the
//      caller's responsibility to:
//      a. Delete when done.
//      b. Modify the id or spectrum number as appropriate.
//      c. Modify the comment if appropriate.
//  Returns:
//     Pointer to the clone of the grobj cast to grobj_generic*
//
grobj_generic*
grobj_Peak1d::clone()
{
  grobj_name    name;
  grobj_Peak1d* copy = new grobj_Peak1d(getspectrum(),
					getid(), 
					getname(name),
					m_fPosition,
					m_fFWHM);
  return (grobj_generic*)copy;
}
// Functional Description:
//    grobj_Peak1d::DrawMarker
//       Draws the peak marker and if necesary places a label on the marker.
//     The object is drawn to look like the picture below:
//
//                    |
//              +----[l]-----+
//              |           |
//              |           |
//              |           |
//
//     The vertical bars are m_fWidth channels apart, and extend down to
//     a bit above the height of the spectrum at that width.  The middle bar
//     is at the centroid and extends downward to just a bit above the peak.
//     The label, if enabled, is placed at the intersection of the middle bar
//     and the horizontal bar.
//
//     The lable will be placed at the intersection of the horizontal bar and
//     the vertical peak marker; [l] in the figure above..
//
// Formal Parameters:
//    XSegmentBatch &XSseg:
//       A reference to an X-11 polyline segment which will be used to draw
//       the marker.
//    Boolean flipped:
//        True if coordinate swap in force.
//    short npLeft, npLBottom:
//        Coordinates of the bottom right corner of the bar marker.
//    short npHeight:
//        Height of the horizontal bar.
//    short npRight, npRBottom:
//        Coordinates of the Lowr right corner of the bar marker.
//    short npMid:
//        X coordinate of the bar midpoint.
//    short npPeakht:
//        Y coordinate of the top of the peak center marker.
//
void
grobj_Peak1d::DrawMarker(XSegmentBatch &XSseg, Boolean flipped,
			 short npLeft, short npLBottom,
			 short npHeight, 
			 short npRight, short npRBottom,
			 short npMid, 
			 short npPeakht)
{
  if(flipped) {
    XSseg.draw(npLBottom, npLeft, npHeight, npLeft);   // Bottom horiz. bar.
    XSseg.draw(npHeight,  npLeft, npHeight, npRight);  // Vertical bar.
    XSseg.draw(npRBottom, npRight, npHeight, npRight); // Top horiz. bar.
    XSseg.draw(npHeight,  npMid,   npPeakht, npMid);   // Peak horiz bar.
    
  }
  else {
    XSseg.draw(npLeft, npLBottom, npLeft, npHeight);   // Left vertical bar.
    XSseg.draw(npLeft, npHeight, npRight, npHeight);   // Horizontal bar.
    XSseg.draw(npRight, npRBottom, npRight, npHeight); // Right vertical bar.
    XSseg.draw(npMid, npHeight, npMid, npPeakht);      // center vertical bar.
  }

}
//
//  Functional Description:
//     grobj_Peak1d::ComputeHeight
//       This function accepts a floating point channel number and determines
//       an appropriate height for a horizontal bar.
//       The bar is placed at a fixed percentage above the
//       spectrum.  In order to ensure that the bar is above the spectrum
//       regardless of the representation (remember connect the dots mode), we
//       use linear interpolation to figure out a 'virtual' spectrum height at
//       the position of the bar.
// Formal Parameters:
//     float fChan:
//       Floating point spectrum coordinates of the bar.
// Returns:
//       Integer height in counts describing where the bar should
//       go.
//
unsigned 
grobj_Peak1d::ComputeHeight(float fChan)
{
  // Need to get the height of the two channels above and below fChan They'll
  // be put into fLowh and fHih

  int nLow = (int) fChan;	// These are the limits of the interpolation.
  int nHi  = (int) (fChan + 1.0);
  int nSpec= getspectrum();     // Number of spectrum on which we're set.
  float fLowh= (float)xamine_shared->getchannel(nSpec, nLow);
  float fHih = (float)xamine_shared->getchannel(nSpec, nHi);

  // Now figure out the height at fChan via linear interpolation.
  
  float fFrac = (fChan - (float)nLow); // 1 channel span so this is easy.
  unsigned   nHt   = (unsigned)(fLowh + fFrac * (fHih - fLowh));

  // The proper height is GROBJ_MINDIST*nHt above nHt, but at least one count.

  int nDelta = (unsigned)((GROBJ_MINDIST)*(float)nHt);
  if(nDelta == 0) nDelta = 1;

  return( nHt + nDelta);

}
//
// Functional Description:
//   grobj_Peak1d::ComputeBarHeight:
//     This computes the clearance a vertical bar has.. For now, that's the 
//     same as ComputeHeight:
// Formal Parameters:
//    float fChan:
//      Where the bar is.
// Returns:
//   int height in counts at which bar bottom is:
//
unsigned 
grobj_Peak1d::ComputeBarHeight(float fChan)
{
  return ComputeHeight(fChan);
}
//
// Functional Description:
//   grobj_Peak1d::ComputeDeltaHeight:
//     Determines how tall the peak marker vertical bar should be.
//     We want it to be visible regardless of the height of the peak, or the
//     display scaling (log/linear). 100*peak ht. should be sufficient.
// Formal Parameters:
//    float fChan:
//      Channel at which we want the height.  We pass this in in case later
//      we want to try to be more clever about setting the height of this
//      bar.
// Returns:
//      The height of the top of the peak bar.
//
unsigned 
grobj_Peak1d::ComputeDeltaHeight(float fChan)
{
  return (unsigned)ComputeHeight(fChan)*100;
}
//
// Functional Description:
//   grobj_Peak1d::ComputeLocation:
//     This function computes the location in pixels of a point consisting of
//     a floating point channel number and an unsigned height.
// Formal Parameters:
//   Xamine_Convert1d &cvt:
//     Refers to the conversion block which determines the mapping of pixels to
//     spectrum coordinates.
//   float fchannel:
//     Floating point channel number.
//   unsigned height:
//     Number of counts of the point.
//   short &xpix, &ypix:
//     Output of the function... the pixel coords.
//
void 
grobj_Peak1d::ComputeLocation(Xamine_Convert1d& cvt,
			      float fChannel, unsigned nCounts,
			      short& xpix, short& ypix)
{
  //
  // The conversion procedes in two steps.  The first step gets the
  // pixel coordinates for (int)fChannel and (int)fChannel+1 
  // (nCounts is fine with the first one).
  // The second step does a linear interpolation betweent he two x-pixel values
  // to determine the closest pixel to fChannel.
  //
  int xpix1, xpix2, yp;
  cvt.SpecToScreen(&xpix1, &yp, (int)fChannel, nCounts);
  cvt.SpecToScreen(&xpix2, &yp, (int)(fChannel) + 1, nCounts);
  ypix = (short)yp;		// We're all set on the height...

  // Do the interpolation:

  xpix1 += (int)((fChannel - (float((int)fChannel))) * (float)(xpix2 - xpix1));
  xpix  = (short)xpix1;
}

////////////////////////////////////////////////////////////////////////////
//////////////// Implementation of grobj_Fitline ///////////////////////////
////////////////////////////////////////////////////////////////////////////

/*!
   Construct a fitline (parameterized):
   \param nSpectrum : int
      Number of the spectrum this fitline is defined on. That is the slot
      in shared memory of the spectrum descriptor.
   \param nId : int
      A unique id for the object.  This is used by clients e.g. to select
      this object from among all objects defined on a spectrum.
   \param pszName : grobj_name
      Name of the gate... could be null for an anonymous gate.
   \param low, high : int
      Defines the region of interest over which the fitline is defined.
      this is in terms of spectrum channel numbers... irrespective of mapping.
   \param evalProc : const char*
      Points to a Tcl script that must define the proc 'fitline'  this proc
      returns the height of the fitline at all points between low and high.
   
*/
grobj_Fitline::grobj_Fitline(int nSpectrum, int nId, grobj_name pszName,
			     int low, int high, const char* evalProc) : 
  grobj_generic(),
  m_low(low),
  m_high(high),
  m_evalProc(evalProc)
{
  // Initialize base class.. I know this is ugly  but it's what we're left with
  // at this point in time:

  if (pszName) {
    setname(pszName);
  }
  id       = nId;
  spectrum = nSpectrum;
  npts     = 0;
  
}
/*!
  Copy constructor.  Construct into ourselves to make a duplicate of some
  other object:

  \param rhs : const grob_Fitline&
     Reference to the object we are duplicating inside ourselves.

*/
grobj_Fitline::grobj_Fitline(const grobj_Fitline& rhs)
{
  copyIn(rhs);
}

/*!
   Destructor is a place holder since the only dynamic storage
   associated with us is a string that knows how to deal with itself on
   default destruction
*/
grobj_Fitline::~grobj_Fitline()
{
}
/*!
   Assignment - Assign to ourselves from another object the result
                is an object that has all the same member data as the rhs.
   \param rhs : const grobj_Fitline&
      Reference to the object that we will become a duplicate of.
   \return grobj_Fitline&
   \retval  -- A reference to *this to support chaining.
*/
grobj_Fitline&
grobj_Fitline::operator=(const grobj_Fitline& rhs)
{
  if (this != &rhs) {
    copyIn(rhs);
  }
  return *this;
}
/*!
   Return true if all the member data we care about is the same as the
   rhs:
   \param rhs : const grobj_Fitline& 
      Object we are comparing *this to.
   \return int
   \retval 0   Not equal
   \retval 1   Equal
*/
int
grobj_Fitline::operator==(const grobj_Fitline& rhs) const
{
  //  Name comparison is a bit tricky since one or both objects
  // may not have a name:

  bool namesame;
  if (hasname == rhs.hasname) {
    if (hasname) {		// Can compare
      namesame = (strcmp(name, rhs.name) == 0);
    } 
    else {			// Both have no name
      namesame = true;
    }
  }
  else {			// Namedness is different.
    namesame = false;
  }

  // Now we can do the 'grand return' typical of ==:

  return (namesame                          &&
	  (id == rhs.id)                    &&
	  (spectrum == rhs.spectrum)        &&
	  (m_low  == rhs.m_low)             &&
	  (m_high == rhs.m_high)            &&
	  (m_evalProc == rhs.m_evalProc));
}

/*!
  Inequality is defined as the logical negation of equality:
*/
int
grobj_Fitline::operator!=(const grobj_Fitline& rhs) const
{
  return !(*this == rhs);
}

/*!
  Return the type of the object:
*/
grobj_type
grobj_Fitline::type()
{
  return fitline;
}

/*!
  Clone the object:
*/
grobj_generic*
grobj_Fitline::clone()
{
  return new grobj_Fitline(*this); // Good thing we have a coyp consructor.
}


/*!
   Draw the object on a window.
  \param pWindow  : XMWidget*
     Pointer to the object wrapped widget in which we will draw.
    This is a drawing area widget.
  \param pAttributes : win_attributed*
    Pointer to the spectrum attributes object.
  \param final : Boolean
    Draw in final or tentative for.  Tentative draws are done with an xor blt
    to promote easy editing.
*/
void
grobj_Fitline::draw(XMWidget* pWindow, win_attributed* pAttributes,
		    Boolean final)
{
  // Get the boilerplate stuff done
  Display*  d = XtDisplay(pWindow->getid());
  Window  win = XtWindow(pWindow->getid());
  Drawable pm = (Drawable)NULL;

  // If there's a pixmap to accelerate refresh, then get it into pm.
  // we rely on the fact that the pane widget has the pane number in it's
  // user data field:

  long index, row, col;
  pWindow->GetAttribute(XmNuserData, &index);
  col = index % WINDOW_MAXAXIS;
  row = index / WINDOW_MAXAXIS;
  pm  = Xamine_GetBackingStore(row, col);

  // We need a converter to convert the spectrum coordinates of the centroid
  // and width as well as heights into pixels.  The underlying spectrum is
  // gaurenteed to be 1d:

  Xamine_Convert1d cvt(pWindow, pAttributes, xamine_shared);
  cvt.NoClip();

  // Extract the flip and label flag fromt the spectrum attributes:

  Boolean flipped = pAttributes->isflipped();
  Boolean labelit = pAttributes->showlbl();

  // Create a graphical context which will be used for the drawing.

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pWindow);
  SetClipRegion(xgc, pWindow, pAttributes);
  //  xgc->Set2DColors(*pWindow);	// A bit dirty to get contrast on the draw.
  xgc->SetForeground(Xamine_PctToPixel(50));
  xgc->SetPermanent();
  GC gc = xgc->gc;

  // We're now going to make a vector of points in spectrum coordinates
  // which we will then draw...the vector is in channel/floating height
  // pairs.

  vector<pair<int, float> > points = computePoints(pAttributes);
  vector<pair<int, int> >   pts;
  for (int i=0; i < points.size(); i++) {
    pair<int, int> p = computePosition(cvt, points[i].first, points[i].second);
    pts.push_back(p);
  }
  // Draw the fitline/label in the window and, if necessary, the backing
  // store pixmap


  drawFitline(d, win, gc, pWindow,
	      flipped, labelit,
	      pts);
  if(pm) {
    drawFitline(d, pm, gc, pWindow,
		flipped, labelit,
		pts);
  }


}



/*
  Copy in private member of the grobj_Fitline class
  Copies member data from rhs to *this:
*/
void
grobj_Fitline::copyIn(const grobj_Fitline& rhs)
{
  // Our stuff first.

  m_low   = rhs.m_low;
  m_high  = rhs.m_high;
  m_evalProc = rhs.m_evalProc;

  // Now the base class stuff:

  id       = rhs.id;
  spectrum = rhs.spectrum;
  npts     = rhs.npts;

  // Name if we have one:

  hasname = rhs.hasname;
  if (hasname) {
    strcpy(name, rhs.name);
  }
}
/*
  Private member to compute the points for the fitline.
  This involves:
   - Creating an interpreter, 
   - Sourcing into it the fitline definition.
   - Foreach point in the range [m_low, m_high] construct
     a script string of the form fitline channel
     evaluate it
   - add the point and result (as a float) to the vector of points
     being returned.
   - Destroy the interpreter
   - return the points.
*/
vector<pair<int, float> >
grobj_Fitline::computePoints(win_attributed* pAtr)
{
  vector<pair<int, float> > result;
  pair<int, float>          point;

  Tcl_Interp* interp = Tcl_CreateInterp();
  Tcl_GlobalEval(interp, m_evalProc.c_str()); // Define the fitline proc.

  for (int x = m_low; x <= m_high; x++) {
    char script[1000];
    sprintf(script, "fitline %d", x);
    Tcl_GlobalEval(interp, script);
    const char* value = Tcl_GetStringResult(interp);
    float y;
    sscanf(value, "%f", &y);


    point.first  = x;
    point.second = y;
    result.push_back(point);
  }
  Tcl_DeleteInterp(interp);

  return result;
}
/*
   Compute a position from a channel number and floating point height.
   Linear interpolation is used to figure out a pixel position more precise
   than an integerized height.
*/
pair<int,int>
grobj_Fitline::computePosition(Xamine_Convert1d& cvt,
			       int               channel,
			       float             height)
{
  int htlow = (int)height;
  int hthigh= htlow+1;
  int xpix;
  int ypix1;
  int ypix2;

  cvt.SpecToScreen(&xpix, &ypix1, channel, htlow);
  cvt.SpecToScreen(&xpix, &ypix2, channel, hthigh);

  // Interpolate here.

  int ypix = ypix1 + (int)((height - htlow)*(float)(ypix2-ypix1));

  return pair<int,int>(xpix, ypix);

}


/*
   Draw the polyline that makes up the fitline in a drawable
   (could be either the window or its manually maintained backing store
   pixmap
   Display*    d               - Display id of X11 connection.
   Drawabele   w               - Id of drawable to write in.
   GC&          gc             - Reference to graphical context to use when drawing
   XMWidget*   pWindow         - Pointer to the XMWidget we're drawing into (for
                                 the label).
   Boolean    flipped          - True if axes are 'backwards'.
   Boolean    labelit          - True if the object should be labelled (fn).
   std::vector<std::pair<int, int> >& points - pixel coordinates of polyline.


*/
void
grobj_Fitline::drawFitline(Display* d, Drawable w, GC& gc, XMWidget* pWindow,
			   Boolean flipped, Boolean labelit,
			   vector<pair<int, int> >& points)
{
  // We're going to draw into an XSegmentBatch for efficiency:
  // This is all inside a block so that the XSegmentBatch is destroyed,
  // flushing the line to the drawable after the end of the block.
 
  {

    XLineBatch polyline(d, w, gc);
    for(int i = 0; i < points.size(); i++) {
      int x = points[i].first;
      int y = points[i].second;
      if (flipped) {
	x = points[i].second;
	y = points[i].first;
      }
      polyline.draw(x, y);
    
    }
  }                         // Flush the line to drawable

  // If labelling is turned on, label the grob at its first point:

  if (labelit) {
    char label[100];
    sprintf(label, "f%d", getid());
    int labelx = points[0].first;
    int labely = points[0].second;
    if (flipped) {
      labelx = points[0].second;
      labely = points[0].first;
    }
    Xamine_LabelGrobj(d, w, gc, pWindow,
		      label, labelx, labely);
  }
  
}
