/*
** Facility:
**   Xamine  - NSCL Display Program.
** Abstract:
**   marker.cc:
**     This file contains source code to handle accepting marker graphical
**     objects from the user.  A graphical object is just a small
**     mark (Diamond shaped) that will be drawn on and stick to the
**     spectrum.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

#include <errno.h>
#include <string.h>
#include "XMText.h"
#include "XMLabel.h"
#include "XMPushbutton.h"
#include "XBatch.h"
#include "errormsg.h"
#include "marker.h"
#include "grafinput.h"
#include "grobjinput.h"
#include "dispgrob.h"
#include "panemgr.h"
#include "dispshare.h"
#include "colormgr.h"
#include "convert.h"
#include "grobjmgr.h"
#include "refreshctl.h"
#include "grobjdisplay.h"

/*
** Constant definitions:
*/
#define PROMPT_WIDTH 10		/* Characters in text field point prompt. */

/*
** External references:
*/
extern spec_shared *xamine_shared;
/*
** The text below is help text which describes how to use the dialog
** that we use to prompt for a marker.
*/

static char *help_text[] = {
  "  This dialog is prompting for a point which will be used to enter a\n",
  "marker graphical object into Xamine's graphical object database.  A\n",
  "marker is a reference point on a spectrum which can be used to gauge\n",
  "motion of spectrum features relative to some fixed point as conditions\n",
  "change.  For example you can use a marker to mark the position of a peak\n",
  "prior to altering gains or timing so that you can see how a peak shifts\n",
  "relative to the original position.\n",
  "  The marker consists of a single point on the spectrum and a name.\n",
  "The name can be entered into the name type-in of this dialog.  The point\n",
  "can either be entered using the mouse or typed in if you know the\n",
  "coordinates in advance.  Separate the coordinates with a space. For 1-d\n",
  "markers the second coordinate represents the marker height in counts\n",
  "For 2-d spectra, both coordinates are in channels.\n\n",
  "   The buttons at the bottom of the dialog have the following meanings:\n\n",
  "       Ok      --  Accept the marker and dismiss the dialog.\n",
  "       Accept  --  Accept the marker and keep the dialog open\n",
  "       Cancel  --  Dismiss the dialog without accepting a marker.  If\n",
  "                   you want to keep the dialog open, then just change or\n",
  "                   delete the point.\n",
  "       Help    --  Displays this text",
  NULL
  };


/*
**   The class below specializes the ObjectInput class.  The specialization
** creates a class which is suitable for accepting a marker input.
** The marker input prompter is a text typein for a single point.
** To the right of that is a delete button that allows you to delete any
** point that has been entered.
**  The class includes a pointer to a generic graphical object which will
** wind up being either a grobj_mark1d or a grobj_mark2d object, depending
** on the spectrum that is current. Some flashy footwork of deleting and
** new'ing is done on the SelectChanged method to handle this properly.
*/
class AcceptMarker : public ObjectInput {
 protected:
  grobj_generic *object;	/* Points to the graphical storage (state). */
  XMTextField   *point;		/* Points point input prompter. */
  XMLabel       *pt_label;	/* Label the point input prompter with this. */
  XMPushButton  *del_button;	/* Button to delete the point. */

  /* Private methods: */

  void ChooseDefaultName();

 public:

  /* Class constructors and destructors. */

  AcceptMarker(XMWidget *parent, char *name, char **hlp_text);
  ~AcceptMarker() {
    if(object != NULL) delete object;
    delete point;
    delete pt_label;
    delete del_button;
  }
  /* Overrides for the virtual functions that are implemented by
  ** the indirect base class GraphicalInput.
  */
  virtual void ClearState();
  virtual void DrawPoints(XMWidget *pane,
			  Xamine_RefreshContext *ctx = NULL);
  virtual void ClearPoints(XMWidget *pane,
		   Xamine_RefreshContext *ctx = NULL) 
                          { DrawPoints(pane, ctx); }
  virtual void ClearDialog();
  virtual void AddPoint(struct point &pt);
  virtual void DelPoint();
  virtual Boolean DoAccept();
  virtual void SelectChanged(int old, int oldr, int newc, int newr);
  virtual void ApplyCallback(XtPointer call_d) {
    GraphicalInput::ApplyCallback(call_d);
    ChooseDefaultName();
  }

  /* Additional behavior: */

  void TextPoint();		/* Accept typed in point. */
  void RebuildObject();		/* Build new graphical object. */

  /* Managing the widget causes us to do some configuring of the object: */

  void Manage() {
    RebuildObject();
    ChooseDefaultName();
    SetStandardCallbacks();
    ObjectInput::Manage();
  }
  void UnManage() {
     ClearStandardCallbacks();
     if(object != NULL) {
       delete object;
       object = NULL;
     }
     ObjectInput::UnManage();
  }

};

/*
** Local data:  in this case the marker acceptance object.
*/
static AcceptMarker *dialog = NULL;

/*
** Functional Description:
**    TextPoint:
**      This function is called when the user hits a return on the
**      text field which contains the point text.  We retrieve and decode
**      the text field.  If it decodes, then we will add a point.  If not,
**      then we pop-up an error message.
** Formal Parameters:
**       -- as for any XMotif callback, however the user data is a 
**       pointer to this object.
*/
static void TextPoint(XMWidget *w, XtPointer ud, XtPointer cd)
{
  AcceptMarker *d = (AcceptMarker *)ud;

  d->TextPoint();
}

/*
** Functional Description:
**   Delete:
**     This local function process the delete button and redirects to 
**     the DelPoint method.
**  Formal Parameters:
**    XMWidget *w:
**      Widget which called back... in this case the pointer to the
**      delete button itself.
**    XtPointer user_d:
**      User data, in this case a pointer to the AcceptMarker object.
**    XtPointer call_d:
**      Callback data (unused).
*/
void Delete(XMWidget *w, XtPointer user_d, XtPointer call_d)
{
  AcceptMarker *dlg = (AcceptMarker *)user_d;
  XMWidget *pane = Xamine_GetSelectedDrawingArea();
  if(pane == NULL) {
    dlg->UnManage();
    return;
  }
  
  dlg->ClearPoints(pane);
  dlg->DelPoint();
  dlg->DrawPoints(pane);
  
}

/** Functional Description:
**    AcceptMarker::TextPoint:
**      This function handles activations of the text field that contains
**      the marker prompt.
*/
void AcceptMarker::TextPoint()
{
  char *s = point->GetText();
  struct point pt;
  int x,y;

  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {
    UnManage();
    return;
  }

  /* Decode the text from the string. */

  if(  sscanf(s, "%d %d", &x, &y) != 2) {
    Xamine_error_msg(this, "Valid spectrum coordinates must be supplied");
    XtFree(s);
    return;
  }
  XtFree(s);

  /* Get the current drawing area: */

  XMWidget *pane = Xamine_GetDrawingArea(row, col);
  if(pane == NULL) {		/* If nonexistent, kill off dialog. */
    UnManage();
    return;
  }


  /* Validate the points.  We don't require that the points be inside the
  ** displayed region.  Only that they be inside the spectrum itself.
  */

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



  /* Add the point and make sure it's drawn. */
  ClearPoints(pane);
  AddPoint(pt);
  DrawPoints(pane);

}

/*
** Functional Description:
**   AcceptMarker::ChooseDefaultName
**     This function sets a default name in the name string of the marker.
*/
void AcceptMarker::ChooseDefaultName()
{
  char namestr[80];
  sprintf(namestr, "Marker %03d", Xamine_GetNextObjectId());
  ObjectInput::SetText(namestr);
}

/*
** Functional Description:
**    AcceptMarker::RebuildObject:
**      This function builds a marker object appropriate to the
**      type of spectrum in pane that we're operating in.  The current
**      pane is assumed to already be in the parent class instance variables
**      row and col.
*/
void AcceptMarker::RebuildObject()
{
  if(object != NULL) {		/* Can't hurt to do this in case I got */
    delete object;		/* careless somewhere else. */
  }
  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {		/* No spectrum in pane so... */
    UnManage();			/* give up.                  */
    return;
  }
  if(att->is1d()) {		/* 1-d spectrum... */
    object = new grobj_mark1d;
  }
  else {			/* 2-d spectrum... */
    object = new grobj_mark2d;
  }
}

/*
** Functional Description:
**  AcceptMarker:
**    This method is a constructor for the Accept Marker class.  We:
**      1) Format the client part of the work area.
**      2) initialize the object field by determining the type of
**         spectrum being represented and then allocating the appropriate
**         type of marker.
** Formal Parameters: (all passed to parent types).
*/
AcceptMarker::AcceptMarker(XMWidget *parent, char *name, char **hlp_text) :
       ObjectInput(parent, name, hlp_text)
{
  /* Build the appropriate graphical object: */

  object = NULL;
  RebuildObject();


  /* Create the prompting area for the point. */

  /* Build the widgets...  */

  point = new XMTextField("Point_prompter", *point_prompts,  PROMPT_WIDTH);
  pt_label = new XMLabel("Point_Label",     *point_prompts,
			 "Position");
  del_button = new XMPushButton("Delete_point", *point_prompts);

  /* Paste up the widgets left to right. inside the point_prompts form: */

  point_prompts->SetLeftAttachment(*pt_label,     XmATTACH_FORM);
  point_prompts->SetTopAttachment(*pt_label,      XmATTACH_FORM);
  point_prompts->SetBottomAttachment(*pt_label,   XmATTACH_FORM);

  point_prompts->SetLeftAttachment(*point,        XmATTACH_WIDGET);
  point_prompts->SetLeftWidget(*point,           *pt_label);
  point_prompts->SetTopAttachment(*point,         XmATTACH_FORM);
  point_prompts->SetBottomAttachment(*point,      XmATTACH_FORM);

  point_prompts->SetLeftAttachment(*del_button,   XmATTACH_WIDGET);
  point_prompts->SetLeftWidget(*del_button,      *point);
  point_prompts->SetTopAttachment(*del_button,    XmATTACH_FORM);
  point_prompts->SetBottomAttachment(*del_button, XmATTACH_FORM);
  point_prompts->SetRightAttachment(*del_button,  XmATTACH_FORM);

  /* Generate the default object name and pre-load it into the name prompt
  ** widget.
  */
  ChooseDefaultName();
  /*
  ** Set the delete callback to be the delpoint redirecter:  
  */
  del_button->AddCallback(Delete, this);

  point->AddActivateCallback(::TextPoint, this);
}
/*
** Functional Description:
**   AcceptMarker::ClearState:
**      This method clears the state information associated with the
**       marker.  In this case we just clear all the points from object.
*/
void AcceptMarker::ClearState()
{
  if(object != NULL) {
    object->clrpts();
  }
  else {
    UnManage();  /* Must be point state for the dialog to work */
  }
}

/*
** Functional Description:
**   AcceptMarker::DrawPoints:
**      This method is expected to draw the tentative marker on the
**      indicated pane.
** Formal Parameters:
**     XMWidget *pane:
**        Drawing area widget associated with the display.
**     Xamine_RefreshContext *ctx:
**        Refresh context if refresh in progress.  This
**        is used to locate the pixmap that's being built.
*/
void AcceptMarker::DrawPoints(XMWidget *pane,
			      Xamine_RefreshContext *ctx)
{
  /* If no points are accepted, then just return immediately. */

  if(object == NULL) {		/* Must be an object... */
    UnManage();
    return;
  }
  if(object->pointcount() == 0) {
    return;
  }

  /* If a draw is in progress but we're not in the callback stage, then
  ** we could make things look a bit funny by drawing now since we're doing
  ** XOR drawing.  (Could get some partial lines).  In that case we
  ** don't draw.  The idea is that eventually we will get into the callback
  ** stage and at that time the draw will occur.
  */

  pane_db *pdb = Xamine_GetPaneDb();
  if( (pdb->refresh_state(row,col) != rfsh_idle) &&
      (pdb->refresh_state(row,col) != rfsh_callback))
    return;


  object->draw(pane, Xamine_GetDisplayAttributes(row, col), False);
}

/*
** Functional Description:
**   AcceptMarker::ClearDialog:
**     This method clears the contents of the point area prompter.  We
**     never clear the name prompter.
*/
void AcceptMarker::ClearDialog()
{
  point->SetText("");
}

/*
** Functional Description:
**   AcceptMarker::AddPoint
**      This function is called when a point should be added to the marker.
**      A marker is only allowed to have a single point.  Therefore we just
**      clear the points and then add a point.
** Formal Parameters:
**    point &pt:
**      The Point to add.  The point has already been transformed to 
**      spectrum coordinates.
*/
void AcceptMarker::AddPoint(struct point &pt)
{

  /* The graphical object must still be defined: */

  if(object == NULL) {
    UnManage();
    return;
  }

  /* Now manipulate the points list: */

  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {
    UnManage();
    return;
  }
  object->clrpts();
  if(att->isflipped()) {
    object->addpt(pt.y, pt.x);
  }
  else {
    object->addpt(pt.x, pt.y);
  }

  char point_txt[80];
  sprintf(point_txt, "%d %d", pt.x, pt.y);
  point->SetText(point_txt);

}

/*
** Functional Description:
**   AcceptMarker::DelPoint
**     Deletes the most recent point.
**     (all points since that's all there is for us).
*/
void AcceptMarker::DelPoint() 
{
  if(object == NULL) {
    UnManage();
    return;
  }

  /* Clear the points if the marker is not already empty. */

  object->clrpts();
  point->SetText(" ");
}

/*
** Functional Description:
**   AcceptMarker::DoAccept:
**      This function performs the accept operation.  What we need to do
**      is check to make sure that a marker is in.  If not, then that's an
**      error.  If so, then we must enter the object into the graphical object
**      database.
*/
Boolean AcceptMarker::DoAccept()
{
  if(object == NULL) {		/* Must be an object.. */
    UnManage();
    return False;
  }
  /* Must also be a spectrum. */

  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {
    UnManage();
    return False;
  }

  if(object->pointcount() != 1) {
    Xamine_error_msg(this, 
		    "You must first indicate where the marker is to be placed");
    return False;
  }


  XMWidget *pane = Xamine_GetDrawingArea(row,col);
  if(pane != NULL) {
    ClearPoints(pane);
  } 

  /* Enter the object in the Xamine graphical object database: */

  object->setspectrum(att->spectrum());
  char *s = GetText();
  object->setname(GetText());
  XtFree(s);
  grobj_generic *o;

  if(!(o = Xamine_EnterGrobj(object))) {
    char errormsg[80];
    strcpy(errormsg, "Could not enter graphical object \n");
    strcat(errormsg, strerror(errno));
    Xamine_error_msg(this,
		     errormsg);
    
  }
  o->draw(pane, att, True);

  object->clrpts();

  return True;
}

/*
** Functional Description:
**   AcceptMarker::SelectChanged:
**      This function is called when the selected pane changes.  For the
**      The bulk of the work is done by the base class SelectChanged method.
**      We execute that first, but then we delete the object and instantiate
**      a new one depending on the shape of the spectrum in the pane.  If
**      the pane is empty then of course we exit the dialog.
** Formal Parameters:
**    int oldc,oldr:
**      Prior pane coordinates.
**    int newc,newr:
**      New row and column.
*/
void AcceptMarker::SelectChanged(int oldc, int oldr, int newc, int newr)
{
    GraphicalInput::SelectChanged(oldc,oldr, newc,newr);

    /* Delete the object and start fresh with a new one: */

    RebuildObject();

} 

/*
** Functional Description:
**   Xamine_DrawMarker:
**      This function draws marker given the screen coordinates of the
**      marker position.  The marker is a diamond centered on the
**      input coordinates. 
** Formal Parameters:
**   Display *d:
**      Connection id for the display to draw on.
**   Drawable win:
**      Drawable to draw in.
**   GC gc:
**      Graphical context providing the drawing attributes.
**   int x,y:
**      Screen coords of diamond center.
*/
void Xamine_DrawMarker(Display *d, Drawable w, GC gc, int x, int y)
{
  XLineBatch diamond(d, w, gc);
  
  diamond.draw(x, y+3);		/* Point below the center. */
  diamond.draw(x-3, y);		/* Point to the left of the center. */
  diamond.draw(x, y-3);		/* Point above the center. */
  diamond.draw(x+3,y);		/* Point to the right of center. */
  diamond.draw(x, y+3);		/* Close the curve. */
  
}

/*
** Callback relays:
*/
static void Marker_ok(XMWidget *w, XtPointer ud, XtPointer cd)
{
  AcceptMarker *d = (AcceptMarker *)ud;
  d->OkCallback(NULL);
}
static void Marker_cancel(XMWidget *w, XtPointer ud, XtPointer cd)
{
  AcceptMarker *d = (AcceptMarker *)ud;

  d->CancelCallback(NULL);
}
static void Marker_accept(XMWidget *w, XtPointer ud, XtPointer cd)
{
  AcceptMarker *d = (AcceptMarker *)ud;
 
  d->ApplyCallback(NULL);
}

void Xamine_AddMarker(XMWidget *wid, XtPointer ud, XtPointer cd)
{
  /* If there is already something involving graphical input, then
  ** we put up an error message and return.
  */
  panemgr_select_callback cb;
  XtPointer userd;

  Xamine_GetSelectCallback(&cb, &userd);
  if(cb != NULL) {
    Xamine_error_msg(wid,
      "Graphical input is already in progress.  Please finish or cancel it");
    return;
  }


  /* If the dialog has not yet been instantiated, then we 
  ** instantiate it now and set up the invariant behavior.
  */
  if(dialog == NULL) {
    dialog = new AcceptMarker(wid, "Define_Marker", help_text);

    dialog->AddOkCallback(Marker_ok, dialog);
    dialog->AddCancelCallback(Marker_cancel, dialog);
    dialog->AddApplyCallback(Marker_accept, dialog);

  }

  /* Set up the callbacks and manage the dialog. 
  */
  dialog->setrow(Xamine_PaneSelectedrow());
  dialog->setcol(Xamine_PaneSelectedcol());
  dialog->Manage();		/* Sets the callbacks too. */
}
