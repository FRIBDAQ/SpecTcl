/*
** Facility:
**   Xamine - NSCL Display program.
** Abstract:
**   sumregion.cc:
**      This file contains code which accepts and draws summing regions.
**      We handle both 1-d and 2-d summing regions.
** Author:
**      Ron FOx
**      NSCL
**      Michigan State Universty
**      East Lansing,  MI 48824-1321
*/
#ifdef unix
#pragma implementation "XMCallback.h"
#endif
static char *sccsinfo = "@(#)sumregion.cc	8.1 6/23/95 ";


/*
** Include files:
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "XMWidget.h"
#include "XMPushbutton.h"
#include "XMLabel.h"
#include "XMText.h"
#include "XMList.h"
#include "XBatch.h"
#include "dispgrob.h"
#include "dispwind.h"
#include "dispshare.h"
#include "grobjmgr.h"
#include "grobjdisplay.h"
#include "sumregion.h"
#include "panemgr.h"
#include "convert.h"
#include "errormsg.h"
#include "grobjinput.h"
#include "colormgr.h"
#include "chanplot.h"

#ifdef unix
#if !defined(ultrix) && !defined(CYGWIN)
#define strerror(err) (sys_errlist[(err)])
#endif
#endif
/*
** Constant definitions:
*/

#define XAMINE_LABEL_OFFSET 3

/*
** The text below is the help text for the dialog that accepts summing
** regions:
*/
static char *help_text[] = {
  "  This dialog prompts you to enter the points that make up a summing\n",
  "region.  A summing region consists of either two points (1-d spectra)\n",
  "which describe a lower and upper limit pair or a coutable set of points\n",
  "which describe a closed polygon.  You can enter points by clicking with\n",
  "the mouse MB1 or by typing in the coordinates on the Next Point type-in line\n",
  "  Summing regions are stored as graphical objects associated with a \n",
  "spectrum.  Graphical objects have names which are intended to be used\n",
  "by you to distinguish between several objects that might be defined\n",
  "on the same spectrum.  A default name is provided in the Object Name\n",
  "type-in.  You can and probably should change this name.\n\n",
  "   The set of points which make up a graphical object are shown in the\n",
  "list box above the Next Point type-in.  You may delete the most recently\n",
  "accepted point either by clicking in the spectrum with any button other\n",
  "than MB1 or by clicking the Delete Point button on the dialog.  If you\n",
  "switch to a different spectrum, all points are deleted\n\n",
  "   When you have edited the graphical object to your satisfaction, you\n",
  "click on one of the buttons in the action area.  These buttons have the\n",
  "following meanings:\n\n",
  "     Ok           - Accept the object and dismiss the dialog.\n",
  "     Apply        - Accept the object and leave the dialog up so that\n",
  "                    you can enter another summing region.\n",
  "     Cancel       - Do not accept the most recently entered object and\n",
  "                    dismiss the dialog.\n",
  "     Help         - Display this dialog.",
  NULL
  };


/*
** Below is the pointer to the object used to accept summing regions:
*/
AcceptSummingRegion *dialog = NULL;
/*
** Below is a pointer to the shared spectra region:
*/
extern spec_shared *xamine_shared;

/*
** This page contains several little functions which serve to relay 
** Motif++ callbacks to appropriate method functions.
*/
inline AcceptSummingRegion *mkobject(XtPointer ud) /* Create object pointer */
{
  AcceptSummingRegion *dialog = (AcceptSummingRegion *)ud;
  return dialog;
}

void CancelRelay(XMWidget *w, XtPointer ud, XtPointer cd) /* Cancel relay: */
{
  mkobject(ud)->CancelCallback(cd);
}
void ApplyRelay(XMWidget *w, XtPointer ud, XtPointer cd) /* Apply relay: */
{
  mkobject(ud)->ApplyCallback(cd);
}
void OkRelay(XMWidget *w, XtPointer ud, XtPointer cd)  /* OK relay: */
{
  mkobject(ud)->OkCallback(cd);
}
void TextPointRelay(XMWidget *w, XtPointer ud, XtPointer cd) /* Txt in relay */
{
  mkobject(ud)->TextPoint();
}
void Delete_relay(XMWidget *w, XtPointer ud, XtPointer cd) /* Delete last pt */
{
  AcceptSummingRegion *o = mkobject(ud);
  
  XMWidget *pane = Xamine_GetSelectedDrawingArea();
  o->ClearPoints(pane);
  o->DelPoint();
  o->DrawPoints(pane);
}

/*
** Functional Description:
**   AcceptSummingRegion:
**     This is a constructor for the Accept Summing region class.
**     We build the widget hierarchy for the point prompter and
**     set up the initial state of the prompter.
** Formal Parameters:
**   char *name:
**      Name given to the widgets.
**   XMWidget *parent:
**      Parent widget of this whole thing.
**   char **help_text:
**      Pointer to the list of help strings.
*/
AcceptSummingRegion::AcceptSummingRegion(char *name, XMWidget *parent,
					 char **help_text) :
       ObjectInput(parent, name, help_text) 
{
  /* Build the widgets that will live in the point_prompts form: */

  Points         = new XMScrolledList(name, *point_prompts);
  DeleteLast     = new XMPushButton(name, *point_prompts, Delete_relay,
				    this);
  DeleteLast->Label("Delete Last");
  NextLabel      = new XMLabel(name, *point_prompts, "Next Point:");
  NextPoint      = new XMTextField(name, *point_prompts);

  object = NULL;		/* Null out the object so that Rebuild works.  */

  /* Lay out the widgets within the form.   */

  /*  The points list box is placed in the upper left corner of the form: */
  /*  To paste the list box we actually need the widget corresponding to  */
  /*  the parent of the list box:                                         */
  
  Widget scroller = Points->GetScrolledWindow();
  

  point_prompts->SetTopAttachment(scroller,      XmATTACH_FORM);
  point_prompts->SetLeftAttachment(scroller,     XmATTACH_FORM);
 
  /* Glue the delete last button to the list box on the left side, form on
  ** the right and top of form on the  top.  Bottom hangs free
  */

  point_prompts->SetTopAttachment(*DeleteLast,    XmATTACH_NONE);
  point_prompts->SetRightAttachment(*DeleteLast,  XmATTACH_FORM);
  point_prompts->SetBottomAttachment(*DeleteLast, XmATTACH_NONE);
  point_prompts->SetLeftAttachment(*DeleteLast,   XmATTACH_WIDGET);
  point_prompts->SetLeftWidget(*DeleteLast,      scroller);

  /*
  ** Glue the Next Point label to the form on the left and right and the
  ** list widget on the top.
  */
  point_prompts->SetLeftAttachment(*NextLabel,  XmATTACH_FORM);
  point_prompts->SetRightAttachment(*NextLabel, XmATTACH_FORM);
  point_prompts->SetTopAttachment(*NextLabel,   XmATTACH_WIDGET);
  point_prompts->SetTopWidget(*NextLabel,      scroller);

  /* Glue the point input text field to the form on the left right and bottom
  ** and to it's label on the top:
  */
  point_prompts->SetLeftAttachment(*NextPoint,   XmATTACH_FORM);
  point_prompts->SetRightAttachment(*NextPoint,  XmATTACH_FORM);
  point_prompts->SetBottomAttachment(*NextPoint, XmATTACH_FORM);
  point_prompts->SetTopAttachment(*NextPoint,    XmATTACH_WIDGET);
  point_prompts->SetTopWidget(*NextPoint,       *NextLabel);

  /* Set up the constant set of callbacks that drive the widget behavior */

  AddCancelCallback(CancelRelay, this);
  AddApplyCallback(ApplyRelay, this);
  AddOkCallback(OkRelay, this);
  NextPoint->AddActivateCallback(TextPointRelay, this);

}

/*
** Functional Description:
**   AcceptSummingRegion::ClearState:
**      This function clears the current input state.
**      This consists of removing all points from the list widget,
**      clearing the object (if it exists or unmanaging and going away if
**      it doesn't).
*/
void AcceptSummingRegion::ClearState()
{

  /* Clear the graphical object.    */

  if(object == NULL) {
    UnManage();
    return;
  }
  object->clrpts();

  /* Clear the points from the list widget:  */

  Points->ClearItems();
}

/*
** Functional Description:
**   AcceptSummingRegion::DrawPoints:
**     This function draws the summing represented by the object.
**     basically this is just a matter of setting up the call to
**     Xamine_DrawSumRegion.
** Formal Parameters:
**     XMWidget *pane:
**        Pointer to the widget to draw in.
**     Xamine_RefreshContext *ctx:
**        If non-null, the refresh context of the update
**        that's in progress. Used to locate the appropriate
**        drawing pixmap.
*/
void AcceptSummingRegion::DrawPoints(XMWidget *pane,
			     Xamine_RefreshContext *ctx)
{
  /* We need to get the display attributes of the spectrum associated with
  ** this pane, since it has the flipped flag and labelling flags.  If it
  ** Does not exist, then we cancel the input function.
  */

  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {
    UnManage();
    return;
  }

  /* We need to check to see if the object is still alive.  If not then we
  ** also unmanage and vanish.   If there are no points, then we just
  ** return without drawing.
  */

  if(object == NULL) {
    UnManage();
    return;
  }
  if(object->pointcount() == 0) {
    return;
  }

  /* If a draw is in progress, and we are not in the callback stage of the
  ** draw, then any drawing we do now will at best be wasted and at worst
  ** be partly overwritten, causing funny (even number of draws) output.
  ** Therefore we just return in that case and let the callback time deal
  ** with the draw when it happens:
  */
  pane_db *pdb = Xamine_GetPaneDb();
  if( (pdb->refresh_state(row, col) != rfsh_idle)   &&
      (pdb->refresh_state(row, col) != rfsh_callback))
    return;
  /*
  ** draw the object:
  */

  object->draw(pane, Xamine_GetDisplayAttributes(row,col), False);
}
/*  Clear points... this is just draw points in disguise since we
**  are using XOR drawing.  The implicit assumption is that the 
**  lines are already visible.
*/
void AcceptSummingRegion::ClearPoints(XMWidget *pane,
			      Xamine_RefreshContext *ctx)
{
  DrawPoints(pane, ctx);
}

/*
** Functional Description:
**   AcceptSummingRegion::ClearDialog:
**    This method clears all the points from the dialog.
**    This is done by calling the ClearItems method of the Points object.
*/
void AcceptSummingRegion::ClearDialog()
{
  Points->ClearItems();
}

/*
** Functional Description:
**   AcceptSummingRegion::AddPoint:
**      This function adds a point  The point has been transformed to
**      spectra coordinates but flips are still missing.  This is a consequence
**      of the way that the Converter's work.  The points are represented on
**      the list widget using the order given in the input point itself.
** Formal Parameters:
**   point &pt:
**     The point to accept.
*/
void AcceptSummingRegion::AddPoint(point &pt)
{

  /*  The object must be defined and the attributes must still exist: */

  if(object == NULL) {
    UnManage();
    return;
  }
  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {
    UnManage();
    return;
  }

  /* If the spectrum is 1-d, and if there are already 2 points, then we
  ** first delete the last point since we're only allowed 2 points in a
  ** summing region.
  */

  if(att->is1d() && (object->pointcount() == 2)) {
    DelPoint();			/* This does both the object and the list.  */
  }

  /* Now we add the point to the list, taking into account the flip: */

  point p;			/* This will be the grobj point. */
  if(att->isflipped()) {
    p.x = pt.y;
    p.y = pt.x;
  }
  else {
    p.x = pt.x;
    p.y = pt.y;
  }
  object->addpt(p.x, p.y);	/* Add the properly ordered point. */
  char pstring[80];
  sprintf(pstring, "%d %d", pt.x, pt.y);
  Points->AddItem(pstring);
  Points->SetBottomItem();	/* Scroll up if needed to show last item.  */
  NextPoint->SetText("");  /* Clear the input point.             */

}

/*
** Functional Description:
**   AcceptSummingRegion::DelPoint:
**     This function deletes a point from the summing region. Two things
**     must be done (we assume the caller takes care of things like
**     displaying the points).  First the most recent point must be removed
**     from the object.  Second the most recent point must be removed from
**     the list box.
*/
void AcceptSummingRegion::DelPoint()
{
  /* First the usual check to make sure the object is still defined. */

  if(object == NULL)  {
    UnManage();
    return;
  }

  if(object->pointcount() <= 0) return;	/* Return if no points left. */

  /* Delete the point from the list box: */

  Points->DeleteItem();

  /* Now delete the most recent point from the graphical object's point list */

  object->delpt();
}

/*
** Functional Description:
**   AcceptSummingRegion::DoAccept:
**     This function performs the mechanics of accepting a graphical object.
**     A 1-d summing region is exactly two points.  A 2-d summing region is
**     at least 3 points (user's responsible for making them non-colinear).
*/
Boolean AcceptSummingRegion::DoAccept()
{
  XMWidget *pane = Xamine_GetDrawingArea(row, col);

  /* IF the object is NULL, then we unmanage and exit: */

  if(object == NULL) {
    UnManage();
    return False;
  }
  /* If the attributes block is null, then return too: */

  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {
    UnManage();
    return False;
  }
  /* We first make sure the number of points is correct: */

  if(att->is1d() && (object->pointcount() != 2)) {
    Xamine_error_msg(Xamine_Getpanemgr(),
		     "1-d Summing regions require 2 points to be accepted");
    return False;
  }
  if(!att->is1d() && (object->pointcount() < 3)) {
    Xamine_error_msg(Xamine_Getpanemgr(),
	     "2-d Summing regions require at least 3 points to be accepted");
    return False;
  }
  /* Now we clear the point echoes and the list box.  */

  ClearPoints(pane);
  ClearDialog();

  /* Finally, accept the object by calling Xamine_EnterGrobj... after setting
  ** the spectrum number and object name.
  */
  object->setspectrum(att->spectrum());
  char *s = ObjectInput::GetText();
  object->setname(s);
  XtFree(s);
  grobj_generic *o;

  if(!(o = Xamine_EnterGrobj(object))) {
    char errormsg[80];
    strcpy(errormsg, "Could not enter graphical object\n");
    strcat(errormsg, strerror(errno));
    Xamine_error_msg(Xamine_Getpanemgr(), errormsg);
    return False;
  }
  else {
    ChooseDefaultName();
  }

  o->draw(pane, att, True);

  /* Cleanup:  Clear the points and redraw the pane to show the 
  ** object in permanent form. Also return True to indicate success.
  */

  object->clrpts();
  return True;

}

/*
** Functional Description:
**   AcceptSummingRegion::SelectChanged:
**     This function adds to the behavior for SelectChanged for the
**     base class.  In particular, we must rebuild the graphical object since
**     the type of spectrum might have changed.
** Formal Parameters:
**   int oldc, oldr:
**     Prior row and column.
**   int newc, newr:
**     New row and column to work with.
*/
void AcceptSummingRegion::SelectChanged(int oldc, int oldr, int newc, int newr)
{
 GraphicalInput::SelectChanged(oldc, oldr, newc, newr);
 RebuildObject();
}

/*
** Functional Description:
**   AcceptSummingRegion::ChooseDefaultName:
**     This function sets the graphical object's default name.  We get that
**     from the next graphical object id number
*/
void AcceptSummingRegion::ChooseDefaultName()
{
  char namestr[80];
  sprintf(namestr, "Summing Region %03d", Xamine_GetNextObjectId());
  ObjectInput::SetText(namestr);
}

/*
** Functional Description:
**   AcceptSummingRegion::RebuildObject:
**      This function deletes any existing graphical object and 
**      builds a new one as appropriate to the current row and column.
*/
void AcceptSummingRegion::RebuildObject()
{
  /* First get rid of any prior graphical object: */

  if(object != NULL) {
    delete object;
    object = NULL;
  }
  /* Next see what we should be building: */

  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {		/* Pane is empty so... */
    UnManage();
    return;			/* Give up on accepting stuff. */
  }
  if(att->is1d()) {
    object = new grobj_sum1d;
  }
  else {
    object = new grobj_sum2d;
  }
}

/*
** Functional Description:
**    AcceptSummingRegion::TextPoint:
**      This function is called when it is necessary to accept a typed in
**      point.  The point is validated, the text is cleared and the
**      point is entered in both the graphical object and the list box.
*/
void AcceptSummingRegion::TextPoint()
{
  char *s = NextPoint->GetText();
  int    x,y;

  /*  Get the spectrum attributes.. needed to validate point values. */

  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {
    UnManage();
    return;
  }

  /* Convert the point to numerical equivalents and complain if it won't. */

  if(sscanf(s,"%d %d", &x,&y) != 2) {
    XtFree(s);
    Xamine_error_msg(Xamine_Getpanemgr(),
		     "Please type in a valid pair of coordinates");
    return;
  }
  XtFree(s);

  /* Validate the points.  We don't require that the points be inside the
  ** displayed region.  Only that they be inside the spectrum itself.
  */

  point pt;
  int spec = att->spectrum();
  pt.x = x;			/* Point orientation doesn't change with */
  pt.y = y;			/* flip. */

  if(att->isflipped()) {	/* But for checking we must flip x/y. */
    x = pt.y;
    y = pt.x;
  }
  /* regardles... x chans must be checked: */

  if( (x < 0) || (x >= xamine_shared->getxdim(spec))) {
    Xamine_error_msg(Xamine_Getpanemgr(),
		     "Coordinate must be inside spectrum bounds");
    return;
  }
  if( !att->is1d()) {		/* Check y dim if 2-d */
    if( (y < 0) || (y >= xamine_shared->getydim(spec))) {
      Xamine_error_msg(Xamine_Getpanemgr(),
		       "Coordinate must be inside spectrum bounds");
      return;
    }
  }

  /* Now we can add the point: */

  XMWidget *pane = Xamine_GetDrawingArea(row, col);
  ClearPoints(pane);
  AddPoint(pt);
  DrawPoints(pane);
     

}

/*
** Functional Description:
**   Xamine_AcceptSummingRegion:
**     This function initiates the editing and acceptance of a summing
**     region graphical object.  This is done by popping up (instantiating
**     if need be) an AcceptSummingRegion object.
** Formal Parameters: (most ignored).
**    XMWidget *w:
**      The function is a callback.  This is the widget which initiated the
**      summing region acceptance.
**    XtPointer ud:
**      This is a pointer to user data associated with the callback.
**      At present, no user data is expected.
**    XtPointer cd:
**      This is a pointer to a callback type specific data structure
**      which is not used.
*/
void Xamine_AcceptSummingRegion(XMWidget *w, XtPointer ud, XtPointer cd)
{
  /* If there is already some graphical input in progress, we indicate this
  ** and return without doing anything.
  */
  panemgr_select_callback cb;
  XtPointer userd;
 
  Xamine_GetSelectCallback(&cb, &userd);
  if(cb != NULL) {
    Xamine_error_msg(w,
       "Graphical input is already in progress. Please finish or cancel it");
    return;
  }

  /* If the dialog does not exist, then we build it. */

  if(dialog == NULL) {
    dialog = new AcceptSummingRegion("Summing region",
				     w, help_text);
  }
  /* Set the current row/column: */

  dialog->setrow(Xamine_PaneSelectedrow());
  dialog->setcol(Xamine_PaneSelectedcol());

  /* Manage the dialog */

  dialog->Manage();
}

/*
** Functional Description:
**   Xamine_Draw1dCut:
**     This local function draws a 1-d cut.  A 1-d cut
**     is drawn as a pair of lines (much like a 1-d expansion area prior to
**     acceptance.  The lines will be either vertically or horizontally oriented.
**     Note that clipping may make one or both lines
**     invisible.  We rely on the clipping region to be set to eliminate
**     off window lines and lines that would be in the margins.
** Formal Parameters:
**   Display *d:
**     Pointer to the display connection.
**   Drawable win:
**     The thing we're drawing into.
**   GC ctx:
**     The graphical context to use when drawing.
**   XMWidget *wid:
**     The widget we're drawing in.
**   Xamine_Converter *cvt:
**     The converter to use to convert spectrum coordinates to tube coordinates.
**     (inputs will need to be flipped if flip is on).
**   Boolean flipped:
**      True if orientation is flipped.
**   grobj_sum1d *object:
**      Points to the object.
*/
void 
Xamine_Draw1dCut(Display *d, Drawable win, GC ctx, 
		 XMWidget *wid,
		 Xamine_Converter *cvt, 
		 Boolean flipped, 
		 grobj_generic *object)
{
  Dimension nx;			/* Need the X extent of window for flipped */

  wid->GetAttribute(XmNwidth, &nx);

  /* Construct the label in case labelit is true: */

  char label[10];
  sprintf(label, " S%d", object->getid());

  /* While summing regions are supposed to only consist of a pair of points,
  ** we display all points in the graphical object just in case this definition
  ** gets generalized later.
  */

  for(int i = 0; i < object->pointcount(); i++) {
    grobj_point *pt = object->getpt(i);	/* Get a point. */
    int xpix, ypix;
    if(flipped) {		/* Flipped -> horizontal line */
      cvt->SpecToScreen(&xpix, &ypix, pt->getx());
      XDrawLine(d,win, ctx,	/* This is a horizontal line from the axis */
		xpix, ypix,	/* out to the right side of the picture. */
		(int)nx, ypix);
    }
    else {			/* Unflipped -> vertical line */
      cvt->SpecToScreen(&xpix, &ypix, pt->getx());
      XDrawLine(d, win, ctx,
		xpix, ypix,
		xpix, 0);
    }
  }

}

/*
** Functional Description:
**   Xamine_Draw2dContour:
**      This function is called to draw a 2-d contour.  A 2-d contour
**      is a closed polygon of points.
** Formal Parameters:
**   Display *d:
**     Connection id for the display.
**   Drawable w:
**     Drawable identifier that we're drawing into.
**   GC gc:
**     Grahpical context which describes the visual attributes of the draw.
**   XMWidget *wid:
**     Widget that we draw into.  I don't think this function uses it but
**     we require it to be passed for call uniformity with the 1d.
**   Xamine_Converter *cvt:
**     A coordinate converter which will convert the X,Y spectrum coordinates
**     to screen coordinates.
**   Boolean flipped:
**     True if the axes of the pane are flipped.
**   grobj_sum2d *object:
**      Pointer to the object to be drawn.
*/
void 
Xamine_Draw2dContour(Display *d, Drawable w, GC gc,
			    XMWidget *wid, Xamine_Converter *cvt,
			    Boolean flipped, 
			    grobj_generic *object)
{
  /* Summing regions are closed polygons.  They are drawn by 
  ** using an XLineBatch object.   The polygon is closed by drawing the
  ** first point first and last as well.
  */
  XLineBatch poly(d, w, gc);
  for(int i = 0; i < object->pointcount(); i++) {
    grobj_point *pt = object->getpt(i);	/* Get a point and... */
    int xpix, ypix;		        /* convert from spectrum to  */
    if(flipped)			        /* screen/pixel coordinates. */
      cvt->SpecToScreen(&xpix, &ypix, pt->gety(), pt->getx());
    else
      cvt->SpecToScreen(&xpix, &ypix, pt->getx(), pt->gety());
    poly.draw((short)xpix, (short)ypix);
  }
  /* Now close the polygon by drawing the last point. (if there are at least
  ** 3 points.
  */
  grobj_point *pt = object->getpt(0); /* Get first point to close the poly */
  int xpix, ypix;		        /* convert from spectrum to  */
  if(flipped)			        /* screen/pixel coordinates. */
    cvt->SpecToScreen(&xpix, &ypix, pt->gety(), pt->getx());
  else
    cvt->SpecToScreen(&xpix, &ypix, pt->getx(), pt->gety());

  if(object->pointcount() >= 3) {
    poly.draw((short)xpix, (short)ypix);
  }
  poly.flush();			/* Draw the lines. */
}

/*
** Functional Description:
**   Xamine_DrawSumRegion:
**     This function draws a summing region in a drawable given all the stuff
**     necessary to show how to draw it (See parameters below).  The
**     action is to determine first if the spectrum is 1-d or 2-d.  If 1-d,
**     then Draw1dSumRegion is called.  If 2-d then Draw2dSumRegion is called.
**     The drawing region is assumed to be defined by the clipping region in
**     The graphical context which is in turn assumed to cover only the
**     spectrum region of the display.
** Formal Parameters:
**   Display *d:
**     Identifies the X-11 connection to the display.
**   Drawable win:
**     Identifies the X-11 drawable that the graphics will be drawn in.
**   GC ctx:
**     Graphical context to do the draws with.
**   XMWidget *wid:
**     Widget in which the draw is done (needed for Xamine_LabelObject
**   Xamine_Converter *cvt:
**     Points to a converter that will convert the spectrum coordinates to
**     screen coordinates. Note that the clipping state will be temporarily
**     turned off for 2-d's to make drawing easy.
**   Boolean flipped:
**     True if the spectrum coordinate axes are flipped.
**   Boolean labelit:
**     True if graphical objects are to be labelled.
**   grobj_generic *object:
**     The object itself.
*/
void 
Xamine_DrawSumRegion(Display *d, Drawable win, GC ctx, XMWidget *wid,
		     Xamine_Converter *cvt,
		     Boolean flipped, Boolean labelit,
		     grobj_generic *object)
{
  /* The methods for drawing a 1-d and 2-d object are so different, that
  ** we branch out at this early stage to draw the objects appropriately.
  */
  cvt->NoClip();	   /* use the clip region to reject off window lines  */

  switch(object->type()) {
  case summing_region_1d:
    Xamine_Draw1dCut(d, win, ctx, wid,
		     cvt, flipped,
		     object);
    if(labelit) {
      char label[10];
      Dimension nx;

      sprintf(label, "S%d", object->getid());
      wid->GetAttribute(XmNwidth, &nx);
      for(int i = 0; i < object->pointcount(); i++) {
	int xpix, ypix;
	grobj_point *pt = object->getpt(i);
	cvt->SpecToScreen(&xpix, &ypix, pt->getx());
	if(flipped) {
	  Xamine_LabelGrobj(d, win, ctx, wid,
			    label, 
			    (xpix + (int)nx)/2, 
			    ypix + XAMINE_LABEL_OFFSET);
	}
	else {
	  Xamine_LabelGrobj(d,win,ctx,wid,
			    label,
			    xpix, ypix/2);
	}
      }		  
    }
    break;
  case summing_region_2d:
    Xamine_Draw2dContour(d, win, ctx, wid,
		    cvt, flipped, 
		    object);

    /* If labeling is enabled, then we draw the label text at the position of
    ** of the first point in the object.
     */
    if(labelit) {
      char labeltxt[10];
      int xpix, ypix;
      grobj_point *pt = object->getpt(0);

      if(flipped) {
	cvt->SpecToScreen(&xpix, &ypix, pt->gety(), pt->getx());
      }
      else {
	cvt->SpecToScreen(&xpix, &ypix, pt->getx(), pt->gety());
      }
      sprintf(labeltxt, " S%d", object->getid());
      Xamine_LabelGrobj(d, win, ctx, wid, labeltxt, 
			xpix, ypix + XAMINE_LABEL_OFFSET);
    }
    break;
  default:
    Xamine_error_msg(Xamine_Getpanemgr(),
			 "Xamine_DrawSumRegion -- not given a summing region");
    break;
  }
  cvt->Clip();
}
