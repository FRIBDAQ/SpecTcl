/*
** Facility:
**   Xamine   - NSCL Display program.
** Abstract:
**   grafinput.cc:
**      This file implements the GraphicalInput class.  This class is a
**      base class application framework for Xamine operations which require
**      graphical input.  This includes selecting expansion regions, accepting
**      gating objects and local graphical objects.
** Author:
**     Ron Fox
**     NSCL
**     Michigan State University
**     East Lansing, MI 48824-1321
*/

/*
** Include files:
*/
#include <X11/keysym.h>
#include <X11/keysymdef.h>

#include "XMDialogs.h"
#include "grafinput.h"
#include "helpmenu.h"
#include "panemgr.h"
#include "refreshctl.h"
#include "convert.h"
#include "dispshare.h"

/*
** External references:
*/
extern spec_shared *xamine_shared;

/*
** Functional Description:
**   GraphicalInput::GraphicalInput:
*      The constructor must instantiate the custom dialog and set up
**     the help for the dialog.  If no help text is supplied, then the
**     help button is left greyed out.  The help dialog is given the
**     name "Help_Popup".
**     We also fill in the current row and column
** Formal Parameters:
**    XMWidget *parent:  
**      Parent widget of the dialog widget class it will be the 
**      head of the custom dialog widget tree.
**    char *name:
**      Name to be given to the custom dialog widget.
**    char **help_text:
**      Possibly null pointer to the help text strings.
*/
 GraphicalInput::GraphicalInput(XMWidget *parent, char *name, 
				char **help_text) :
  XMCustomDialog(name, *parent, "Graphical Input")
{
  /* Set up the help button */

  if(help_text) {
    help.name = "Help_Popup";
    help.dialog = NULL;
    help.text   = help_text;
    AddHelpCallback(Xamine_display_help, &help);
  }
  else {
    Help->Disable();
  }
  /*
  **  Get the row/column of the entity we're working with.
  **  Note, this couples strongly to the panemgr.
  */
  row = Xamine_PaneSelectedrow();
  col = Xamine_PaneSelectedcol();

}

/*
** Functional Description:
**   GraphicalInput::~GraphicalInput
**      This function is only present to ensure that the dialog destructor
**      is also called.
*/
GraphicalInput::~GraphicalInput()
{
  
}

/*
** Functional Description:
**   GraphicalInput::ClearStandardCallbacks:
**     There are a set of event callbacks that are associated with graphical
**     input.  These allow the graphical input object to be informed of
**     events which can affect it's state.  In Particular, these are
**       SelectionChanged  - If the pane has changed then this is called.
**       PaneInput         - If the pane has had some input then this is
**                           called.
**       Refresh           - If the pane is being redrawn then this event
**                           is triggered.
**     Part of cleaning up following input is to remove the event handlers
**     for these functions.  This method does that:
*/
void GraphicalInput::ClearStandardCallbacks()
{
  Xamine_ClearSelectCallback();
  Xamine_ClearPaneInputCallback();
  Xamine_RemoveRefreshCallback();
}

/*
** The functions here convert a standard event callback into the approprate
** method invocation.  User data is always a pointer to the object.
** All other arguments are passed in when appropriate.
*/

/*
**  Selection changed:
*/
static void SelectCallback_Relay(int oldc, int oldr, int newc, int newr,
				 XtPointer user_d)
{
  GraphicalInput *object = (GraphicalInput *)user_d;
  object->SelectChanged(oldc, oldr, newc, newr);
}
/*
** Mouse or keyboard input:
*/
static void PaneInput_Relay(XMWidget *w, XtPointer user_d, XtPointer call_d)
{
  GraphicalInput *object = (GraphicalInput *)user_d;
  XmDrawingAreaCallbackStruct *cbs  = (XmDrawingAreaCallbackStruct *)call_d;

  /* If input was mouse hit then call the mouse hit function */

  if(cbs->reason != XmCR_INPUT) return;	/* Not an input event... */
  switch(cbs->event->type) {
  case ButtonRelease:
    object->MouseHit(w, call_d);
    break;
  case KeyPress:
    object->KeyboardInput(w, call_d);
    break;
  default:
    return;
  }
}

static void Refresh_Relay(Xamine_RefreshContext *ctx, XtPointer user_d)
{
  GraphicalInput *object = (GraphicalInput *)user_d;
  object->Refresh(ctx);
}
/*
** Functional Description:
**   GraphicalInput::SetStandardCallbacks:
**     This function sets up the callback relay functions that we
**     defined above.  This in effect connects the appropriate methods
**     of the object to the appropriate events in the outside world.
**     This allows the object to function more or less autonomously
**     for some events.
*/
void GraphicalInput::SetStandardCallbacks()
{
  Xamine_SetPaneInputCallback(PaneInput_Relay, (XtPointer)this);
  Xamine_SetSelectCallback(SelectCallback_Relay, (XtPointer)this);
  Xamine_AddRefreshCallback(Refresh_Relay, (XtPointer)this);
}

/*
** Functional Description:
**   GraphicalInput::MouseHit
**     This function is called whenever a pane receives input.
**     We process mouse hits only.  A mouse hit is defined as what
**     happens when a mouse button is released inside the active pane.
**     There are two kinds of hits.  The first is when the left button
**     is pressed.  That's a request to accept a point.  The second is
**     when another button is pressed.  That's a request to delete a point.
** Formal Parameters:
**   XtPointer call_d:
**     Pointer to the callback data.  This is actually a pointer to an
**     XmDrawingAreaCallbackStruct.
*/
void GraphicalInput::MouseHit(XMWidget *wid, XtPointer call_d)
{
  XmDrawingAreaCallbackStruct *cbs  = (XmDrawingAreaCallbackStruct *)call_d;
  int xpix, ypix;
  int button;
  win_attributed   *attrib;
  spec_location    loc;

  /* Get the window attributes.  It's possible that the window has changed
  ** to a NULL spectrum at some point in all of this:
  */
  attrib = Xamine_GetDisplayAttributes(row, col); /* Get attribs. */
  if(attrib == NULL) {
    CancelCallback(NULL);
    return;			/* giving up on the input function.  */
  }
  


  /* The event was a button release so get the X,Y coordinates and the */
  /* button number:                                                    */

  XButtonEvent *event = (XButtonEvent *)cbs->event;
  xpix = event->x;
  ypix = event->y;
  button = event->button;

  /* The first branch depends on the button.  If not button 1, then delete
  ** the most recent point... otherwise accept the 'most recent' point
  */
  ClearPoints(wid);		/* Un draw the current points. */
  if(button != Button1) {	/* Delete most recent point */
    DelPoint();
  }
  else {			/* Accept Point */
    Xamine_Convert1d *cvt1;
    Xamine_Convert2d *cvt2;

  
    /* Handling differs depending on the spectrum type: */

    switch(xamine_shared->gettype(attrib->spectrum())) {
    case twodword:
    case twodbyte:
      cvt2 = new Xamine_Convert2d(wid, attrib, xamine_shared);
      cvt2->ScreenToSpec(&loc, xpix, ypix);
      delete cvt2;
      break;
    case onedlong:
    case onedword:
      cvt1 = new Xamine_Convert1d(wid, attrib, xamine_shared);
      cvt1->ScreenToSpec(&loc, xpix, ypix);
      delete cvt1;
      break;
    default:			/* No such spectrum... pop down etc. */
      CancelCallback(NULL);
      return;
    }
 
    /* If control passes here, then loc contains the spectrum location 
     ** which corresponds to the mouse hit.
     */
    point pt;
    pt.x = loc.xpos;
    pt.y = loc.ypos;
    
    AddPoint(pt);
  }
  DrawPoints(wid);		/* Draw the current point set. */
}

/*
** Functional Description:
**   GraphicalInput::SelectChanged:
**     This callback is invoked when the selection has changed.  There
**     are several possibilities:
**      1) A different pane is selected by clicking.  In that case the
**         accepted points are deleted and erased from the tube.
**      2) A new geometry was selected.  In that case old row, col are -1,
**         new row,col are probably 0,0.  In this case we cancel the dialog.
**      3) A new spectrum was put in the current pane.  In that case,
**         the old and new row and column are the same.  In this case
**         we treat things as if we had moved to a different pane.
** Formal Parameters:
**   int oldc,oldr:
**     Old row and column coordinates of the pane (the pane being left).
**   int newc, newr:
**     New row and column pane coordinates (pane being entered).
*/
void GraphicalInput::SelectChanged(int oldc, int oldr, int newc, int newr)
{
  
  /* Check for new geometry which requires us to dismiss the dialog: */

  if( (oldc == -1) || (oldr == -1)) {
    CancelCallback(NULL);
    return;
  }

  /* If the spectrum changed, then just delete the points without draws. */

  if( (oldc == newc) && (oldr == newr)) {
    ClearState();		/* Clear input state. */
    ClearDialog();		/* Clear the input dialog */
    ClearPoints(Xamine_GetSelectedDrawingArea());
    return;
  }
  /* If the selection changed, then we work just a bit harder... we
  ** clear the lines and then clear the state and null out the points.
  */
  ClearPoints(Xamine_GetDrawingArea(row, col));
  ClearState();
  ClearDialog();
  row = newr;
  col = newc;
  return;

}

/*
** Functional Description:
**   GraphicalInput::Refresh:
**     This function is called when a refresh is in progress on a pane.
**     If the pane matches the one that we're accepting input on then
**     we need to redraw the points that have been accepted so far.
** Formal Parameters:
**    Xamine_RefreshContext *ctx:
**      refresh context associated with this pane and update.
*/
void GraphicalInput::Refresh(Xamine_RefreshContext *ctx)
{
  if(ctx == NULL) 
    return;

  if( (ctx->column == col) && (ctx->row == row))
    DrawPoints(ctx->pane, ctx);
}

/*
** Functional Description:
**   GraphicalInput::CancelCallback:
**     This function cancels the input of a set of points.
**     The default (Possibly overriden or augmented) behavior is to
**     clear the points, clear the dialog, clear the state and unmanage
**     the dialog.
** Formal Parameters:
**   XtPointer call_d:
**     call data from the callback (XmButtonCallbackStruct).
*/
void GraphicalInput::CancelCallback(XtPointer call_d)
{
  ClearPoints(Xamine_GetSelectedDrawingArea());
  ClearState();
  ClearDialog();
  ClearStandardCallbacks();
  UnManage();
}

/*
** Functional Description:
**  GraphicalInput::Apply:
**    This function applies the set of points.  Application is
**    accepting the points but keeping the dialog alive. We call
**    DoAccept.  If the accept was successful, then we clear the
**    state and dialog (We assume DoAccept has made the screen look ok or
**    set up a refresh that will make it look ok.
*/
void GraphicalInput::ApplyCallback(XtPointer call_d)
{
  if(DoAccept()) {
    ClearDialog();
    ClearState();
  }
}

/*
** Functional Description:
**   GraphicalInput::Ok:
**      This function performs identically to Apply, however on success,
**      the dialog is also unmanaged, and all callbacks are cleared..
*/
void GraphicalInput::OkCallback(XtPointer call_d)
{
  if(DoAccept()) {
    ClearDialog();
    ClearState();
    ClearStandardCallbacks();
    UnManage();
  }
}

/*
** Functional Description:
**   KeyboardInput:
**     Provides the default behavior of keyboard input for graphical input:
**     Return -> Ok, and Enter -> Apply
** Formal Parameters:
**    XMWidget *wid:
**       Target Widget.
**    XtPointer d:
**       Actually a pointer to a drawing area callback structure.
*/
 void GraphicalInput::KeyboardInput(XMWidget *wid, XtPointer d)
{
  XmDrawingAreaCallbackStruct *cbs = (XmDrawingAreaCallbackStruct *)d;
  char printable_string[100];
  KeySym key;
  XComposeStatus  s;

  XKeyEvent *evt = (XKeyEvent *)cbs->event;
  XLookupString(evt, printable_string, sizeof(printable_string), &key, &s);

  switch(key) {
  case XK_k:
  case XK_K:
    OkCallback(d);
    return;
  case XK_KP_Enter:
  case XK_a:
  case XK_A:
    ApplyCallback(d);
    return;
  case XK_c:
  case XK_C:
    CancelCallback(d);
    return;
  default:
    return;
  }
  
}
