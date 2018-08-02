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


static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   expand.cc:
**     This file contains software which manages the Expand/UnExpand
**     part of the user interface.  The UnExpand function is pretty
**     trivial, it just removes any applied expansion from the currently
**     displayed spectrum.  The Expand Function is a bit more involved.
**     A dialog is displayed which allows the user to enter two points.
**     As the points are entered, they are displayed on the spectrum as
**     an expansion region (1d) or box (2d).  Points can be accepted either
**     by typing them in or by clicking them in with the mouse.  The points
**     can also be edited (e.g. removed) prior to acceptance.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files:
*/

#include <config.h>
#include "XMWidget.h"
#include "XMDialogs.h"
#include "errormsg.h"
#include "panemgr.h"
#include "helpmenu.h"
#include "expand.h"
#include "refreshctl.h"
#include "exit.h"
#include "select2.h"
#include "grafinput.h"
#include "dispshare.h"
#include "panemgr.h"
#include "convert.h"
#include "colormgr.h"
#include "gcmgr.h"
#include "mapcoord.h"
/* The declarations below define the help text for the custom dialog
** that we'll be building.
*/

static const char *help_text[] = {
  "  This dialog allows you to select an expansion region for the selected\n",
  "spectrum.  Points can be accepted by either clicking in the spectrum, or\n",
  "by typing them into the slots below.  If you click on a different spectrum\n",
  "than the selected spectrum, all points will be removed and subsequent\n",
  "points will be entered into the new selected spectrum\n\n",
  "  Points can be edited as follows:\n\n",
  "     Left mouse button         - Accepts a new point, replaces last point\n",
  "                                 if two points are already input\n",
  "     Middle mouse button       - Deletes most recently accepted point.\n",
  "     Retyping a point line     - Replaces that point.\n",
  "     Clicking a Delete button  - Deletes the point to the left of that \n",
  "                                 button\n\n",
  "  Buttons in the action area have the following functions:\n\n",
  "    Ok    - Accepts the current pair of points, completes the expansion\n",
  "            and dismisses the dialog\n",
  "    Apply - Accepts the current pair of points, completes the expansion\n",
  "            and leaves the dialog in place for a future expansion\n",
  "    Cancel- Dismisses the dialog without making any changes.\n",
  "    Help  - Displays this helpful message\n",
  NULL
  };



/*
** The class Expand is a locally defined class which allows the user
** to expand the display. It is built by specializing the GraphicalInput
** class, and inheriting the Xamine_Select2 class to get the graphical
** part of the dialog. We also add the state data that we need to keep
** track of the points.
*/

class Expand : public GraphicalInput, public Xamine_Select2 {
 protected:
  /* The protected elements are the state information required to keep
  ** track of which points have been accepted by now.
  */
  point pt1;			/* First point value. */
  Boolean pt1_accepted;		/* True if first point accepted. */
  point pt2;			/* Second point values. */
  Boolean pt2_accepted;		/* True if second point accepted. */
 public:
  /* Constructors and destructors: */

  Expand(XMWidget *parent, const char *name, const char **help_text = NULL);
  virtual ~Expand() { }			/* Get the hierarchy destroyed. */

  /* The following functions replace the virtual function set that
  ** are pure virtuals in GraphicalInput:
  */
  virtual void ClearState() {	/* Clear the internal state. */
    pt1_accepted = False;
    pt2_accepted = False;
  }
  virtual void DrawPoints(XMWidget *pane, /* Draw the current set of pts. */
		  Xamine_RefreshContext *ctx = NULL); 
  virtual void ClearPoints(XMWidget *pane,
		  Xamine_RefreshContext *ctx = NULL); /* Clear the current set of pts. */

  virtual void ClearDialog() {	/* Clear the typins for points */
    SetPoint1(0);
    SetPoint2(0);
  }

  virtual void AddPoint(point &pt);  /* Add the next point */
  virtual void DelPoint();           /* Remove the last accepted point */

  virtual Boolean DoAccept();	/* Peforms the expansion. */

  /* The functions below extend the methods of the base classes: */

  void DelPt1();		/* Delete point 1 */
  void DelPt2();		/* and 2.         */

  void AddPt1(point &pt);	/* Sets point 1 and */
  void AddPt2(point &pt);	/* point 2.         */
  Boolean Pt1() { return pt1_accepted; }
  Boolean Pt2() { return pt2_accepted; }
  void    Pt1(int *x, int *y)  { *x = pt1.x; *y = pt1.y; }
  void    Pt2(int *x, int *y)  { *x = pt2.x; *y = pt2.y; }
private:
  // Callback relay functions:

  static void CancelCallback_relay(XMWidget* w,
				   XtPointer u,
				   XtPointer c);
  static void ApplyCallback_relay(XMWidget* w,
				  XtPointer u,
				  XtPointer c);
  static void OkCallback_relay(XMWidget* w,
			       XtPointer u,
			       XtPointer c);
};

static Expand *dialog = NULL;

/*
** External references:
*/
extern spec_shared *xamine_shared;

/*
** The two line functions below are intended to relay the callback
** functions into class methods for Expand:
*/
void 
Expand::CancelCallback_relay(XMWidget *w, XtPointer u, XtPointer c)
{
  Expand *t = (Expand *)u;
  t->CancelCallback(c);
}
void 
Expand::ApplyCallback_relay(XMWidget *w, XtPointer u, XtPointer c)
{
  Expand *t = (Expand *)u;
  t->ApplyCallback(c);
}
void 
Expand::OkCallback_relay(XMWidget *w, XtPointer u, XtPointer c)
{
  Expand *t = (Expand *)u;
  t->OkCallback(c);
}
  

/*
** Functional Description:
**   Expand::Expand:
**    This method instantiates an Expand object.  The main work is done
**    by the parent constructors.  We just have to clear the internal state
**    and dialogs.
*/
Expand::Expand(XMWidget *parent, const char *name, const char **help_text) :
       GraphicalInput(parent, name, help_text),
       Xamine_Select2(this, *work_area)
{
  /* Our part is to just clear the input state variables and the type-in
  ** prompters.
  */
  ClearState();
  ClearDialog();

  /* Callbacks must be added here or else the wrong virtual functions will
  ** be associated
  */
  AddCancelCallback(CancelCallback_relay, this);
  AddOkCallback(OkCallback_relay, this);
  AddApplyCallback(ApplyCallback_relay, this);
}

/*
** Functional Description:
**   Expand::DrawPoints:
**     This method draws the expansion region on the spectrum.
**     The drawing is not performed if the current widget is in the
**     process of updating and the state indicates that we're not part of
**     the refresh callback action.  This is done to avoid the case where
**     we'd draw an even number of times over all or part of the lines.
**     Since we're drawing in XOR mode, that would cause parts of the line
**     to be invisible.  The lines are drawn dotted to distinguish them from
**     stuff that's already on the tube.
** Formal Parameters:
**   XMWidget *pane:
**     The drawing area widget that we must draw on.
**   Xamine_RefreshContext *ctx:
**     Pointer to the refresh context if there is a refresh**  
**     in progress.
*/
void Expand::DrawPoints(XMWidget *pane,
			Xamine_RefreshContext *ctx)
{
  /* If there are no points accepted, this is a no-op */

  if(!(pt1_accepted || pt2_accepted))
    return;

  /* If a draw is in progress, and we're not in the callback stage
  ** then we let the callback stage do the drawing to prevent any 
  ** partial lines that might result from double draws.
  **   The basic idea is that there are two cases that we can draw in:
  **    1. No other drawing is happening.
  **    2. The callback state is executing in which case this function
  **       is assumed to be the callback to draw the expansion points.
  **
  */
  pane_db *pdb  = Xamine_GetPaneDb();
  if( (pdb->refresh_state(row, col) != rfsh_idle) &&
      (pdb->refresh_state(row, col) != rfsh_callback))
    return;

  /* Our first job is to get things like the window, display and a skeleton
  ** graphical context:
  */
  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) return;	/* Spectrum is now gone.. */

  Window win = XtWindow(pane->getid());
  Drawable p = (Drawable)NULL;
  if(ctx != (Xamine_RefreshContext *)NULL) {
    p = ctx->pixmap;
  }
  else {
    p = Xamine_GetBackingStore(row, col);
  }

  Display *d = XtDisplay(pane->getid());

  XamineGrobjGC *xgc = Xamine_GetGrobjGC(*pane); // Create/get the drawing ctx.
  xgc->ClipToSpectrum(*pane, *att);
  xgc->SetProvisional();
  if(att->is1d()) {
    xgc->Set1DColors(*pane);
  }
  else {
    xgc->Set2DColors(*pane);
  }
  GC gc = xgc->gc;

  /* Now that the graphical context is created, we can do the draw. */
  /* What we draw depends on the dimensionality.                    */

  int x1,y1,x2,y2;		/* Line coordinates. */
  float xval, yval;
  Dimension nx;			/* Last X coordinate. */
  pane->GetAttribute(XmNwidth, &nx);
  if(att->is1d()) {		/* 1-d. */
    win_1d* a1 = (win_1d*)att;
    Xamine_Convert1d cvt(pane, att, xamine_shared);
    int zero = 0;
    if(att->hasfloor()) zero = att->getfloor();

    if(pt1_accepted) {	/* Draw point 1 */
      if(att->isflipped()) {
        cvt.SpecToScreen(&x1, &y1, pt1.x, 0);
        x2 = nx;
        y2 = y1;
      } else {
        cvt.SpecToScreen(&x1, &y1, pt1.x, 0);
        x2 = x1;
        y2 = 0;
      }
      XDrawLine(d, win, gc, x1,y1, x2,y2);
      if(p) XDrawLine(d, p, gc, x1,y1, x2,y2);
    }
    if(pt2_accepted) {	/* Draw Point 2  line...  */
      if(att->isflipped()) {
        cvt.SpecToScreen(&x1, &y1, pt2.x, 0);
        x2 = nx;
        y2 = y1;
      }  else {
        cvt.SpecToScreen(&x1, &y1, pt2.x, 0);
        x2 = x1;
        y2 = 0;
      }
      XDrawLine(d, win, gc, x1,y1, x2,y2);
      if(p) XDrawLine(d,p,gc, x1,y1, x2,y2);
    }
  }
  else {			/* 2-d.   */
    Xamine_Convert2d cvt(pane, att, xamine_shared);
    win_2d *at2 = (win_2d *)att;
    int xl = 0;
    int yl = 0;
    float fxl = 0;
    float fyl = 0;
    int x,y;
    int x0,y0;
    if(at2->isexpanded()) {
      xl = at2->xlowlim();
      yl = at2->ylowlim();
    }
    cvt.SpecToScreen(&x0, &y0, xl,yl);
    if(pt1_accepted) {	/* First point. */
      cvt.SpecToScreen(&x,&y, pt1.x, pt1.y);
      XDrawLine(d,win,gc, x, y0, x, 0);
      XDrawLine(d,win,gc, x0,y,  nx,y);
      if(p) {
        XDrawLine(d,p,gc, x, y0, x, 0);
        XDrawLine(d,p,gc, x0,y,  nx,y);
      }
       
    }				
    if(pt2_accepted) {	/* Second Point */
      cvt.SpecToScreen(&x,&y, pt2.x, pt2.y);
      XDrawLine(d,win,gc, x, y0, x, 0);
      XDrawLine(d,win,gc, x0,y,  nx,y);
      if(p) {
        XDrawLine(d,p,gc, x, y0, x, 0);
        XDrawLine(d,p,gc, x0,y,  nx,y);
      }
    }
  }
}

/*
** Functional Description:
**   Expand::ClearPoints:
**     Clears the current set of points from the screen.  Note that
**     at present, since we're XOR'ing we just invoke DrawPoints again.
*/
void Expand::ClearPoints(XMWidget *pane, 
			 Xamine_RefreshContext *ctx)
{
  DrawPoints(pane, ctx);
}

/*
** Functional Description:
**   Expand::AddPoint:
**     This function adds a point to the point list.
**     The point is added as pt1 only if pt1 has not yet been accepted.
**     Otherwise it modifies the value of pt2 and ensures that it's listed
**     as defined.
** Formal Parameters:
**   point &pt:
**     New point to accept.  This is already in spectrum coordinates.
*/
void Expand::AddPoint(point &pt)
{
  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {		/* If the window nulled out on us. */
    CancelCallback(NULL);	/* Treat it like the user cancelled. */
    return;
  }
  /* Get the display attributes since they determine if we need to 
  ** use a 1-d or 2-d format for the display
  */
  float fpx1, fpx2, fpy1, fpy2;
  int specno = att->spectrum();

  if(pt1_accepted) {
    pt2 = pt;
    pt2_accepted = True;
    if(att->is1d()) {
      if(att->isflipped()) {
	pt2.x = pt.y;
	pt2.y = pt.x;
	if(att->ismapped()) {
	  fpx2 = Xamine_XChanToMapped(specno, pt2.x);
	  fpy2 = Xamine_YChanToMapped(specno, pt2.y);
	  SetMappedPoint2(fpx2);
	} else {
	  SetPoint2(pt2.x);
	}
      }
      else {
	if(att->ismapped()) {
	  fpx2 = Xamine_XChanToMapped(specno, pt.x);
	  fpy2 = Xamine_YChanToMapped(specno, pt.y);
	  SetMappedPoint2(fpx2);
	} else {
	  SetPoint2(pt2.x);
	}
      }
    }
    else {
      if(att->ismapped()) {
	fpx1 = Xamine_XChanToMapped(specno, pt.x);
	fpy1 = Xamine_YChanToMapped(specno, pt.y);
	SetMappedPoint2(fpx1, fpy1);
      } else {
	SetPoint2(pt.x, pt.y);
      }
    }
  }
  else {
    pt1 = pt;
    pt1_accepted = True;
    if(att->is1d()) {
      if(att->isflipped()) {
	pt1.x = pt.y;
	pt1.y = pt.x;
      }
      if(att->ismapped()) {
	fpx1 = Xamine_XChanToMapped(specno, pt1.x);
	SetMappedPoint1(fpx1);
      } else {
	SetPoint1(pt1.x);
      }
    }
    else {
      if(att->ismapped()) {
	fpx1 = Xamine_XChanToMapped(specno, pt.x);
	fpy1 = Xamine_YChanToMapped(specno, pt.y);
	SetMappedPoint1(fpx1, fpy1);
      } else {
	SetPoint1(pt.x, pt.y);
      }
    }
  }
}

/*
** Functional Description:
**   Expand::DelPoint:
**     This function performs a point deletion.  The highest numbered
**     point is removed. 
*/
void 
Expand::DelPoint()
{
  if(pt2_accepted) {
    DelPt2();
    return;
  }
  DelPt1();
}

/*
** Functional Description:
**   Expand:DoAccept:
**     This function actually executes the expansion.
*/
Boolean Expand::DoAccept() 
{

  /* To accept , both points must have been supplied: */

  if( pt1_accepted && pt2_accepted ) {
    win_attributed *att;
    att = Xamine_GetDisplayAttributes(row, col);
    if(att == NULL) return False; /* Spectrum vanished. */
    if(att->is1d()) {		/* 1-d logic. */
      win_1d *a1 = (win_1d *)att;
      int low = pt1.x;
      int hi  = pt2.x;
      if(low > hi) {
	int temp = hi;
	hi       = low;
	low      = temp;
      }
      if(low == hi) {		/* Don't allow zero size expand... */
	Xamine_error_msg(this,
			 "The two expansion points must be different");
	return False;
      }
      a1->expand(low, hi);
    }
    else {			/* 2-d logic. */
      win_2d *a2 = (win_2d *)att;
      int xl = pt1.x;
      int xh = pt2.x;
      if(xl > xh) {
	int temp = xh;
	xh       = xl;
	xl       = temp;
      }
      int yl = pt1.y;
      int yh = pt2.y;
      if(yl > yh) {
	int temp = yh;
	yh       = yl;
	yl       = temp;
      }
      if( (xl == xh) || (yl == yh)) {
	Xamine_error_msg(this,
		 "The expansion rectangle must have nonzero height and width");
	return False;
      }
      a2->expand(xl,xh, yl,yh);
    }
    Xamine_ChangedWindows();
    Xamine_RedrawPane(col, row);
    return True;
  }
  else {
    Xamine_error_msg(this, 
		     "Not enough points have been entered to complete expand");
    return False;
  }

}

/*
** Functional Description:
**   Expand::DelPt1:
**     Deletes point 1
*/
void Expand::DelPt1()
{
  pt1_accepted = False;
  SetPoint1(0);
}
/*  And Delpt2 deletes point2  */
void Expand::DelPt2()
{
  pt2_accepted = False;
  SetPoint2(0);
}

/*
** Expand::Addpt1 
**    Defines point1:
*/
void Expand::AddPt1(point &pt)
{
  pt1_accepted = True;
  pt1          = pt;
}
/* And point 2 too: */

void Expand::AddPt2(point &pt)
{
  pt2_accepted = True;
  pt2          = pt;
}

/*
** Functional Description:
**   Xamine_UnExpand:
**      This function restores a spectrum to it's unexpanded state.
** Formal Parameters:
**   As for any XM callback, but all are ignored, since this operates on
**   the Selected spectrum.
*/
void Xamine_UnExpand(XMWidget *widget, XtPointer user_data, XtPointer call_data)
{
  win_attributed *win = Xamine_GetSelectedDisplayAttributes();

  if(win != NULL) {
    if(win->is1d()) {		/* Unexpand a 1-d. */
      win_1d *w = (win_1d *)win;
      w->unexpand();
    }
    else {			/* Unexpand a 2-d.  */
      win_2d *w = (win_2d *)win;
      w->unexpand();
    }
    Xamine_RedrawSelectedPane(); /* Update the pane to draw unexpanded. */
    Xamine_ChangedWindows();
  }
}

/* Functional Description:
**   DelPt1:
**     Delete point 1.  next routine deletes point 2.
**  Formal Parameters:
**     XMWidget *w:
**       Widget that caused the call.
**     Others are ignored.
*/
static void DelPt1(XMWidget *w, XtPointer user_d, XtPointer call_d)
{
  Expand *dlg = (Expand *)w;

  
  dlg->ClearPoints(Xamine_GetDrawingArea(dlg->inputrow(), /* Clear the lines */
					 dlg->inputcol())); 
  dlg->DelPt1();		/* Delete point 1. */
  dlg->DrawPoints(Xamine_GetDrawingArea(dlg->inputrow(), /* Redraw the lines */
					dlg->inputcol()));
}
/* Ditto for point 2: */
static void DelPt2(XMWidget *w, XtPointer user_d, XtPointer call_d)
{
  Expand *dlg = (Expand *)w;

  
  dlg->ClearPoints(Xamine_GetDrawingArea(dlg->inputrow(), /* Clear the lines */
					 dlg->inputcol())); 
  dlg->DelPt2();		/* Delete point 2. */
  dlg->DrawPoints(Xamine_GetDrawingArea(dlg->inputrow(), /* Redraw the lines */
					dlg->inputcol()));
}


/*
** Functional Description:
**   AddPt1:
**      This function is called to add a point to the 
**      selection set.  The first thing we do is kill off the
**      current set of points (done by redrawing them since they draw in
**      XOR mode).   Then we add the point and redraw the points.  Since
**      the points are drawn in XOR mode we need to do the initial undraw
**      for two reasons:
**        1. Pt1 could already be defined in which case we need to get rid
**           of the old point.
**        2. Pt2 could already be drawn in which case we want to make sure
**           it stays visible.
**      NOTE:  There is a difference of behavior for 2-d and 1-d displays.
**             a 2-d display requires both the X and Y coordinates of the
**             point to be gotten while the 1-d only requires the X coordinate
**       This function assumes that the points are comming from the text fld.
**       of the dialog widget.
** Formal Parameters:
**   XMWidget *wid:
**     Pointer to the dialog widget.
**   XtPointer user_d:
**     Ignored callback data.
**   XtPointer call_d:
**     Ignored callback data.
*/
static void AddPt1(XMWidget *wid, XtPointer user_d, XtPointer call_d)
{
  Expand *dlg = (Expand *)wid;
  int r = dlg->inputrow();
  int c = dlg->inputcol();
  XMWidget *pane = Xamine_GetDrawingArea(r,c);
  
  int   x,y;
  point pt;

  /* Make sure we don't veto the move: */

  XmTextVerifyCallbackStruct *cb = (XmTextVerifyCallbackStruct *)call_d;
  if( (cb->reason == XmCR_LOSING_FOCUS) ||
      (cb->reason == XmCR_MODIFYING_TEXT_VALUE) ||
      (cb->reason == XmCR_MOVING_INSERT_CURSOR)) {
    cb->doit = True;
  }

  dlg->ClearPoints(pane);

  /* Figure out what the spectrum number is.  That will determine what
  ** to do since we're dimension dependent.
  */
  win_attributed *att = Xamine_GetDisplayAttributes(r,c);
  if(att == NULL) {		/* Pane got blanked on us. */
    dlg->CancelCallback(NULL);		/* Treat like a cance.     */
    return;
  }

  int spec = att->spectrum();
  
  switch(xamine_shared->gettype(spec)) {
  case twodbyte:
  case twodword:
  case twodlong:
    {
      int xlo = 0; 
      int ylo = 0;
      int xhi = xamine_shared->getxdim(spec);
      int yhi = xamine_shared->getydim(spec);
      if(att->ismapped()) {
	float fx;
	float fy;
	try {
	  dlg->GetFloatPoint1(fx, fy);
	  float fxlo  = xamine_shared->getxmin_map(att->spectrum());
	  float fylo  = xamine_shared->getymin_map(att->spectrum());
	  float fxhi  = xamine_shared->getxmax_map(att->spectrum());
	  float fyhi  = xamine_shared->getymax_map(att->spectrum());
	  if((fx < fxlo) || (fx > fxhi)) {
	    throw "X coordinate out of range";
	  }
	  if((fy < fylo) || (fy > fyhi)) {
	    throw "Y coordiante out of range";
	  }
	  dlg->SetMappedPoint1(fx, fy);
	  pt.x = Xamine_XMappedToChan(att->spectrum(), fx);
	  pt.y = Xamine_YMappedToChan(att->spectrum(), fy);
	  dlg->AddPt1(pt);
	  break;
	}
	catch (...) {
	  goto failed2d;
	}
      }
      else {
	if(dlg->GetPoint1(&x, &y)) { /* Got the two points. */
	  if( (x < 0) || (x > xhi))
	    goto failed2d;
	  if( (y < 0) || (y > yhi))
	    goto failed2d;
	  /* At this point we have a good point. */
	  dlg->SetPoint1(x,y);
	  pt.x = x;
	  pt.y = y;
	  dlg->AddPt1(pt);
	  break;
	}
      }
    }
  failed2d:
    if(dlg->Pt1()) {
      dlg->Pt1(&x, &y);
      dlg->SetPoint1(x,y);
    }
    else
      dlg->SetPoint1(0,0);
    Xamine_error_msg(dlg, 
		     "Invalid point, must be a 2-d channel specification");
    break;
  case onedlong:
  case onedword:
    {
      win_1d* att1 = (win_1d*)att;
      int hi = xamine_shared->getxdim(att1->spectrum());
      int lo = 0;
      if(att1->ismapped()) {
	try {
	  float fx = dlg->GetFloatPoint1(); // Get the raw point.

	  // The point must be within the spectrum mapped limits.

	  float flow = xamine_shared->getxmin_map(att1->spectrum());
	  float fhi  = xamine_shared->getxmax_map(att1->spectrum());
	  if((fx < flow) || (fx > fhi)) {
	    throw "Point out of bounds";
	  }
	  // All is golden .. save the point.
	 
	  dlg->SetMappedPoint1(fx); // Write it back to the dialog..
	  pt.x = Xamine_XMappedToChan(att1->spectrum() ,fx);
	  pt.y = 0;
	  dlg->AddPt1(pt);
	  break;
	}
	catch(...) {
	  goto failed1d;
	}
      }
      else {
	if((x = dlg->GetPoint1()) >= 0) {
	  if( (x < lo) || (x > hi))
	    goto failed1d;
	  /* At this point we have a good point. */
	  dlg->SetPoint1(x);
	  pt.x = x;
	  pt.y = 0;
	  dlg->AddPt1(pt);
	  break;
	}
      }
    }
  failed1d:
    if(dlg->Pt1()) {
      dlg->Pt1(&x, &y);
      dlg->SetPoint1(x);
    }
    else
      dlg->SetPoint1(0);
    Xamine_error_msg(dlg,
		     "Invalid point, must be a channel specification");
    break;
  default:
    Xamine_error_msg(dlg,
		     "A spectrum must remain present to Expand");
    break;
  }

  dlg->DrawPoints(pane);		/* Do final point draw.   */
}

/*
** Functional Description:
**   AddPt2:
**      This function is called to add a point to the 
**      selection set.  The first thing we do is kill off the
**      current set of points (done by redrawing them since they draw in
**      XOR mode).   Then we add the point and redraw the points.  Since
**      the points are drawn in XOR mode we need to do the initial undraw
**      for two reasons:
**        1. Pt1 could already be defined in which case we need to get rid
**           of the old point.
**        2. Pt2 could already be drawn in which case we want to make sure
**           it stays visible.
**      NOTE:  There is a difference of behavior for 2-d and 1-d displays.
**             a 2-d display requires both the X and Y coordinates of the
**             point to be gotten while the 1-d only requires the X coordinate
**       This function assumes that the points are comming from the text fld.
**       of the dialog widget.
** Formal Parameters:
**   XMWidget *wid:
**     Pointer to the dialog widget.
**   XtPointer user_d:
**     User data, in this case a pointer to the input state.
**   XtPointer call_d:
**     Ignored callback data.
*/
static void AddPt2(XMWidget *wid, XtPointer user_d, XtPointer call_d)
{
  Expand *dlg = (Expand *)wid;
  int r = dlg->inputrow();
  int c = dlg->inputcol();
  XMWidget *pane = Xamine_GetDrawingArea(r,c);
  int   x,y;
  point pt;

  /* Make sure we don't veto the move: */

  XmTextVerifyCallbackStruct *cb = (XmTextVerifyCallbackStruct *)call_d;
  if( (cb->reason == XmCR_LOSING_FOCUS) ||
      (cb->reason == XmCR_MODIFYING_TEXT_VALUE) ||
      (cb->reason == XmCR_MOVING_INSERT_CURSOR)) {
    cb->doit = True;
  }

  dlg->ClearPoints(pane);

  /* Figure out what the spectrum number is.  That will determine what
  ** to do since we're dimension dependent.
  */
  win_attributed *att = Xamine_GetDisplayAttributes(r,c);
  if(att == NULL) {		/* Pane got blanked on us. */
    dlg->CancelCallback(NULL);		/* Treat like a cance.     */
    return;
  }
  int spec = att->spectrum();
  
  switch(xamine_shared->gettype(spec)) {
  case twodbyte:
  case twodword:
  case twodlong:
    {
      int xlo = 0; 
      int ylo = 0;
      int xhi = xamine_shared->getxdim(spec);
      int yhi = xamine_shared->getydim(spec);
      if(att->ismapped()) {
	float fx;
	float fy;
	try {
	  dlg->GetFloatPoint2(fx, fy);
	  float fxlo  = xamine_shared->getxmin_map(att->spectrum());
	  float fylo  = xamine_shared->getymin_map(att->spectrum());
	  float fxhi  = xamine_shared->getxmax_map(att->spectrum());
	  float fyhi  = xamine_shared->getymax_map(att->spectrum());
	  if((fx < fxlo) || (fx > fxhi)) {
	    throw "X coordinate out of range";
	  }
	  if((fy < fylo) || (fy > fyhi)) {
	    throw "Y coordiante out of range";
	  }
	  dlg->SetMappedPoint2(fx, fy);
	  pt.x = Xamine_XMappedToChan(att->spectrum(), fx);
	  pt.y = Xamine_YMappedToChan(att->spectrum(), fy);
	  dlg->AddPt2(pt);
	  break;
	}
	catch (...) {
	  goto failed2d;
	}
      }
      else {

	if(dlg->GetPoint2(&x, &y)) { /* Got the two points. */
	  if( (x < 0) || (x > xhi))
	    goto failed2d;
	  if( (y < 0) || (y > yhi))
	    goto failed2d;
	  /* At this point we have a good point. */
	  dlg->SetPoint2(x,y);
	  pt.x = x;
	  pt.y = y;
	  dlg->AddPt2(pt);
	  break;
	}
      }
    }
  failed2d:
    if(dlg->Pt2()) {
      dlg->Pt2(&x, &y);
      dlg->SetPoint2(x,y);
    }
    else
      dlg->SetPoint2(0,0);
    Xamine_error_msg(dlg, 
		     "Invalid point, must be a 2-d channel specification");
    break;
  case onedlong:
  case onedword:
    {
      win_1d* att1 = (win_1d*)att;
      int hi = xamine_shared->getxdim(att1->spectrum());
      int lo = 0;
      if(att1->ismapped()) {
	try {
	  float fx = dlg->GetFloatPoint2(); // Get the raw point.

	  // The point must be within the spectrum mapped limits.

	  float flow = xamine_shared->getxmin_map(att1->spectrum());
	  float fhi  = xamine_shared->getxmax_map(att1->spectrum());
	  if((fx < flow) || (fx > fhi)) {
	    throw "Point out of bounds";
	  }
	  // All is golden .. save the point.
	 
	  dlg->SetMappedPoint2(fx); // Write it back to the dialog..
	  pt.x = Xamine_XMappedToChan(att1->spectrum() ,fx);
	  pt.y = 0;
	  dlg->AddPt2(pt);
	  break;
	}
	catch(...) {
	  goto failed1d;
	}
      }
      else {
	if((x = dlg->GetPoint2()) >= 0) {
	  if( (x < lo) || (x > hi))
	    goto failed1d;
	  /* At this point we have a good point. */
	  dlg->SetPoint2(x);
	  pt.x = x;
	  pt.y = 0;
	  dlg->AddPt2(pt);
	  break;
	}
      }
    }
  failed1d:
    if(dlg->Pt2()) {
      dlg->Pt2(&x, &y);
      dlg->SetPoint2(x);
    }
    else
      dlg->SetPoint2(0);
    Xamine_error_msg(dlg,
		     "Invalid point, must be a channel specification");
    break;
  default:
    Xamine_error_msg(dlg,
		     "A spectrum must remain present to Expand");
    break;
  }

  dlg->DrawPoints(pane);		/* Do final point draw.   */
}
/*
** Functional Description:
**   Xamine_Expand:
**     This function sets up the user interface for the expand operation.
**     For a description of the user interface, see the help text and
**     comment descriptions towards the to of this file.
** Formal Parameters:
**  Are all ignored, but are the standard XM callback parameters
*/
void Xamine_Expand(XMWidget *w, XtPointer user_data, XtPointer call_data)
{
  panemgr_select_callback previous_cb;
  XtPointer               previous_cd;

  /* If some other input is in progress then we error message. */

  Xamine_GetSelectCallback(&previous_cb, &previous_cd);
  if(previous_cb != NULL) {
    Xamine_error_msg(w,
		     " Input already pending, complete or dismiss it first");
    return;
  }

  /* If the dialog does not exist, then instantiate it.  */

  if(dialog == NULL) {
    dialog = new Expand(w, "Expand", help_text);

    dialog->SetPoint1Label(const_cast<char*>("Limit 1"));
    dialog->SetPoint2Label(const_cast<char*>("Limit 2"));

    dialog->AddPt1Callback(AddPt1);
    dialog->AddPt2Callback(AddPt2);
    dialog->AddDeletePt1Callback(DelPt1);
    dialog->AddDeletePt2Callback(DelPt2);

    // In some systems destroy callbacks are called.
    // in others popdowns when the window manager X is hit.
    // so catch them both.

    dialog->AddCallback(XtNdestroyCallback, Xamine_DestroyGraphicalInput, 
			(XtPointer)&dialog);
    dialog->AddCallback(XtNpopdownCallback, Xamine_DestroyGraphicalInput, 
			(XtPointer)&dialog);

  }

  /* Set up the characteristics of the dialog, and our ancillary callbacks. */

  dialog->SetPoint1(0);
  dialog->SetPoint2(0);

  /* Manage the widget. */

  dialog->SetStandardCallbacks();
  dialog->setrow(Xamine_PaneSelectedrow());
  dialog->setcol(Xamine_PaneSelectedcol());
  dialog->Manage();
}
