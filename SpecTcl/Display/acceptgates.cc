/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   acceptgates.cc:
**     This file contains the code responsible for accepting user gate
**     inputs, drawing them as their entered, after they've been entered,
**     and passing accepted gates to the histogrammer program to do with
**     as it pleases.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/
#ifndef VMS
#pragma implementation "XMCallback.h"
#endif
static char *sccsinfo="@(#)acceptgates.cc	8.2 10/3/95 ";

/*
** Include files:
*/
#include "XMWidget.h"
#include "XBatch.h"
#include "convert.h"
#include "dispgrob.h"
#include "acceptgates.h"
#include "sumregion.h"
#include "grobjdisplay.h"
#include "panemgr.h"
#include "XMDialogs.h"
#include "grafinput.h"
#include "grobjinput.h"
#include "colormgr.h"
#include "chanplot.h"
#include "dispshare.h"
#include "grobjmgr.h"
#include "errormsg.h"
#include "gateio.h"
/*
** Constant definitions:
*/
#define XAMINE_LABEL_OFFSET  3

/*
** We reference the shared memory region:
*/
extern spec_shared *xamine_shared;
/*
** The text below is the help text for the dialog that accepts summing
** regions:
*/
static char *help_text[] = {
  "  This dialog prompts you to enter the points that make up a gate.\n",
  "Gates are graphical objects which are passed to Xamine's client program\n",
  "and interpreted by the client. \n\n",
  "  You can enter points by clicking with the mouse MB1 or by\n",
  "typing in the coordinates on the Next Point type-in line\n",
  "  Gates are stored as graphical objects associated with a \n",
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
** Classes defined for use in this module.
** All classes are subclassed from AcceptSummingRegion which contains the
** bulk of the code/methods we need.
*/

  /* Accept a cut gate.  A cut gate is a pair of upper and lower limits */
  /* Display is just like a 1-d summing region.                         */

class AcceptCut : public AcceptSummingRegion
{
 public:

  /* Constructors and destructors: */

  AcceptCut(char *name, XMWidget *parent, char **help_text = NULL);
  virtual ~AcceptCut();

  /* Replacements for virtual functions in AcceptSummingRegion: */

  virtual void DrawPoints(XMWidget *pane,
			  Xamine_RefreshContext *ctx = NULL);
  virtual Boolean DoAccept();
  virtual void SelectChanged(int oldc, int oldr, int newc, int newr);
  virtual void ChooseDefaultName();
  virtual void Manage();
};
  /* AcceptContour -- A counter is a polygon of points.  It is displayed */
  /* Exactly like a 2-d summing region                                   */

class AcceptContour : public AcceptSummingRegion
{
 public:

  /* Constructors and destructors: */

  AcceptContour(char *name, XMWidget *parent, char **help_text = NULL);
  virtual ~AcceptContour();

  /* Replacements for virtual functions in AcceptSummingRegion: */

  virtual void DrawPoints(XMWidget *pane,
			  Xamine_RefreshContext *ctx = NULL);
  virtual Boolean DoAccept();
  virtual void SelectChanged(int oldc, int oldr, int newc, int newr);
  virtual void ChooseDefaultName();
  virtual void Manage();

};

class AcceptBand : public AcceptSummingRegion
{
 public:

  /* Constructors and destructors: */

  AcceptBand(char *name, XMWidget *parent, char **help_text = NULL);
  virtual ~AcceptBand();

  /* Replacements for virtual functions in AcceptSummingRegion: */

  virtual void DrawPoints(XMWidget *pane,
			  Xamine_RefreshContext *ctx = NULL);
  virtual void ClearPoints(XMWidget *pane,
			   Xamine_RefreshContext *ctx = NULL) 
                                   { DrawPoints(pane,ctx); }
  virtual Boolean DoAccept();
  virtual void SelectChanged(int oldc, int oldr, int newc, int newr);
  virtual void ChooseDefaultName();
  virtual void Manage();
};
/*
** Finally we have pointers to dynamically created
** instances of these objects.  These pointers are initialized NULL so that
** it's possible to tell if they've been created yet.
*/
AcceptCut      *cutin  = NULL;
AcceptContour  *contin = NULL;
AcceptBand     *bandin = NULL;


/*
** Functional Description:
**   AcceptCut::AcceptCut:
**      This is the constructor for the Accept cut class.  Essentially
**      we can just use the constructor for our parent class.  We also
**      instantiate the object.
** Formal Parameters:
**   char *name:
**     Name used for dialog component widgets.
**   XMWidget *parent:
**     parent of the widget subtree.
**   char **help_text:
**     Help text pointer, or NULL If help is not available for the dialog.
*/
AcceptCut::AcceptCut(char* name, XMWidget* parent, char** help_text) :
       AcceptSummingRegion(name, parent, help_text) 
{
  object = (grobj_generic *)new grobj_cut1d;
}
/*
** Functional Description:
**   AcceptContour::AcceptContour:
**      This is the constructor for the Accept Contour class.  Essentially
**      we can use the constructor for our parent class. We also
**      instantiate the graphical object.
** Formal Parameters:
**    char *name:
**      Name used for dialog component widgets.
**    XMWidget *parent:
**      Parent for the widget hierarchy of the dialog.
**    char **help_text:
**      Help text if present or NULL if there is no help.
*/
AcceptContour::AcceptContour(char *name, XMWidget *parent, char **help_text) :
       AcceptSummingRegion(name, parent, help_text)
{
  object = (grobj_generic *) new grobj_contour;
}
/*
** AcceptBand::AcceptBand:
**   This is the constructor for the AcceptBand class. After invoking our
**   parent class constructor, we'll  instantiate a grobj_band as the
**   'object' instance variable.
** Formal Parameters:
**   char *name:
**      Name used for dialog component widget names.
**   XMWidget *parent:
**      Parent widget for subtree.
**   char **help_text:
**      Help text if any, NULL If none.
*/
AcceptBand::AcceptBand(char *name, XMWidget *parent, char **help_text) :
     AcceptSummingRegion(name, parent, help_text)
{
  object = (grobj_generic *)new grobj_band;
}

/*
** Functional Description:
**   AcceptCut::~AcceptCut:
**    This is the destructor for the AcceptCut class. The AcceptSummingRegion
**    destructor does all the work:
*/
AcceptCut::~AcceptCut() {}
/*
** Functional Description:
**   AcceptContour::~AcceptContour:
**     This is the destructor for the AcceptContour class.  The 
**    AcceptSummingRegion class does all the real work.
*/
AcceptContour::~AcceptContour() {}
/*
** Functional Description:
**   AcceptBand::~AcceptBand:
**      Destructor for the AcceptBand class.
*/
AcceptBand::~AcceptBand() {}

/*
** Functional Description:
**   AcceptCut::DrawPoints
**    This function draws the echo points of a cut being accepted.  This
**    essentially duplicates the summing region draw for 1-d's.
** Formal Parameters:
**   XMWidget *pane:
**     The pane to draw the widget it.
**   Xamine_RefreshContext *ctx:
**     A refresh context passed in if the spectrum is in the midst of an
**     update.  
** NOTE:
**   For now we simplify and draw in both the window and the pixmap if both
**   exist.  Even though this is not necessary during a refresh.
*/
void AcceptCut::DrawPoints(XMWidget *pane,
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


  /*  Now draw the object:  */

  object->draw(pane, att, False);

}
/*
** Functional Description:
**   AcceptContour::DrawPoints:
**     This function draws the echo points of a contour being accepted.
**     For closed contours, this duplicates the summing region draw for 2-d's.
** Formal Parameters:
**    XMWidget *pane:
**      Pane to draw in.
**    Xamine_RefreshContext *ctx:
**      If refresh in progress, then this is a pointer to the refresh context.
*/
void 
AcceptContour::DrawPoints(XMWidget *pane, Xamine_RefreshContext *ctx) 
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

  /* Draw the graphical object: */

  object->draw(pane, att, False);
}

/*
** Funtional Description:
**   AcceptBand::DrawPoints:
**      This function draws a band.  Bands are drawn exactly like 
**      2-d summing regions, but the first and last points are not connected.
** Formal Parameters:
**   XMWidget *pane:
**     The pane on which we're to draw the lines.
**   Xamine_RefreshContext *ctx:
**      The refresh context if a refresh is in progress or NULL if not.
*/
void
AcceptBand::DrawPoints(XMWidget *pane,
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

  /* Now draw the object */

  object->draw(pane, att, False);
}

/*
** Functional Description:
**   Boolean AcceptCut::DoAccept:
**     This function is called to process attempts to accept a gate.
**     Most of the action is like that of AcceptSummingRegion::DoAccept,
**     however instead of entering the gate in the graphical object database,
**     we pass it over to the client program.
** Returns:
**      True - If the gate could be properly accepted.
**      False- If there was some problem accepting the gate.
*/
Boolean AcceptCut::DoAccept()
{

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

  if((object->pointcount() != 2)) {
    Xamine_error_msg(Xamine_Getpanemgr(),
		     "Cuts require 2 points to be accepted");
    return False;
  }

  /* Now we clear the point echoes and the list box.  */

  ClearPoints(Xamine_GetDrawingArea(row,col));
  ClearDialog();

  
  /*
  ** Pass the points on to the consumer process:
  */
  object->setspectrum(att->spectrum()); /* +1 allows for f77 old style #s. */
  char *s = ObjectInput::GetText();
  object->setname(s);
  XtFree(s);
  Xamine_ReportGate(object);

  ChooseDefaultName();


  /* Clear the points and return True indicating success. */

  object->clrpts();

  return True;

}

/*
** Functional Description:
**    AcceptContour::DoAccept:
**      This function accepts a contour.
** Returns:
**    True if the contour could be accepted or False if it could not be.
*/
Boolean
AcceptContour::DoAccept()
{

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

  if((object->pointcount() <= 2)) {
    Xamine_error_msg(Xamine_Getpanemgr(),
		     "Contours require at least 3 points to be accepted");
    return False;
  }

  /* Now we clear the point echoes and the list box.  */

  ClearPoints(Xamine_GetDrawingArea(row,col));
  ClearDialog();

  
  /*
  ** Pass the points on to the consumer process:
  **  BUGBUGBUG  - at present do nothing.
  */
  object->setspectrum(att->spectrum());
  char *s = ObjectInput::GetText();
  object->setname(s);
  XtFree(s);
  Xamine_ReportGate(object);

  ChooseDefaultName();


  /* Clear the points and return True indicating success. */

  object->clrpts();

  return True;
  
}

/*
** Functional Description:
**   AcceptBand::DoAccept:
**     This function is called when the user attempts to accept a band.
** Returns:
**   True if the band was accepted and False otherwise.
*/
Boolean
AcceptBand::DoAccept()
{

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

  if((object->pointcount() < 2)) {
    Xamine_error_msg(Xamine_Getpanemgr(),
		     "bands require at least 2 points to be accepted");
    return False;
  }

  /* Now we clear the point echoes and the list box.  */

  ClearPoints(Xamine_GetDrawingArea(row,col));
  ClearDialog();

  
  /*
  ** Pass the points on to the consumer process:
  **  BUGBUGBUG  - at present do nothing.
  */
  object->setspectrum(att->spectrum());
  char *s = ObjectInput::GetText();
  object->setname(s);
  XtFree(s);
  Xamine_ReportGate(object);

  ChooseDefaultName();


  /* Clear the points and return True indicating success. */

  object->clrpts();

  return True;
  
}

/*
** Functional Description:
**   AcceptCut::SelectChanged:
**     This function  deals with a selection change.  We remove the old
**     points from the screen and delete them from the object being entered.
**     If the spectrum is not 1-d or not defined, then the dialog is dismissed.
**     Otherwise the user gets to start all over again on a new spectrum.
** Formal Parameters:
**   int oldc, oldr:
**      row and column of prior selection.
**   int newc, newr:
**      row and column of new selection.
*/
void 
AcceptCut::SelectChanged(int oldc, int oldr, int newc, int newr)
{
 win_attributed *att;

 GraphicalInput::SelectChanged(oldc, oldr, newc, newr);	/* Almost does all */


 delete object;
 object = (grobj_generic *)new grobj_cut1d;

 att = Xamine_GetDisplayAttributes(newr, newc);
 if(att == NULL) {		/* Switched to an empty pane. */
   UnManage();
   return;
 }

 if(!att->is1d()) {		/* Switched to a 2-d */
   UnManage();
   return;
 }


}
/*
** Functional Descriptoin:
**   AcceptContour::SelectChanged:
**     This function is called when the user selects a different pane.
**     The bulk of the work is done by the GraphicalInput::SelectChanged()
**     method.  However, there are cases where we will unmanage the dialog
**     since the selected pane cannot have a contour accepted in it.
** Formal Parameters:
**   int oldc, oldr:
**      Previous pane coordinates.
**   int newc, newr:
**      Next pane coordinates.
*/
void 
AcceptContour::SelectChanged(int oldc, int oldr, int newc, int newr)
{
 win_attributed *att;

 GraphicalInput::SelectChanged(oldc, oldr, newc, newr);	/* Almost does all */

 delete object;
 object = (grobj_generic *) new grobj_contour;

 att = Xamine_GetDisplayAttributes(newr, newc);
 if(att == NULL) {		/* Switched to an empty pane. */
   UnManage();
   return;
 }

 if(att->is1d()) {		/* Switched to a 2-d */
   UnManage();
   return;
 }
  
}
/*
** Functional Description:
**   AcceptBand::SelectChanged:
**     This function processes changes in the pane selection.  The
**     bulk of the work is done by GraphicalInput::SelectChanged.  The
**     rest is figuring out if we need to remain visible.
** Formal Parameters:
**   int oldc, oldr:
**     Prior pane coordinates.
**   int newc, newr:
**     New pane coordinates.
*/
void 
AcceptBand::SelectChanged(int oldc, int oldr, int newc, int newr)
{
 win_attributed *att;

 GraphicalInput::SelectChanged(oldc, oldr, newc, newr);	/* Almost does all */
 delete object;
 object = (grobj_generic *)new grobj_band;

 att = Xamine_GetDisplayAttributes(newr, newc);
 if(att == NULL) {		/* Switched to an empty pane. */
   UnManage();
   return;
 }

 if(att->is1d()) {		/* Switched to a 2-d */
   UnManage();
   return;
 }
}

/*
** Functional Description:
**   AcceptCut::ChooseDefaultName:
**     Selects a default name for an object and sets it in the name window
**     of the dialog.
**     The name given Cut nnn where n is the 'next id' number
*/
void
AcceptCut::ChooseDefaultName()
{
  char namestr[80];
  sprintf(namestr, "Cut %03d", Xamine_GetNextGateId());
  ObjectInput::SetText(namestr);
}
/*
** Functional Description:
**   AcceptContour::ChooseDefaultName:
**     Selects a default name for an objet and sets it in the name window
**    of the dialog.
*/
void
AcceptContour::ChooseDefaultName() {
  char namestr[80];
  sprintf(namestr, "Contour %03d", Xamine_GetNextGateId());
  ObjectInput::SetText(namestr);
}
/*
** Functional Description:
**   AcceptBand::ChooseDefaultName:
**     Selects a default name for an objet and sets it in the name window
**    of the dialog.
*/
void
AcceptBand::ChooseDefaultName() {
  char namestr[80];
  sprintf(namestr, "Band %03d", Xamine_GetNextGateId());
  ObjectInput::SetText(namestr);
}

/*
** Functional Description:
**   AcceptCut::Manage:
**     Makes the dialog widget visible.
*/
void AcceptCut::Manage()
{
  delete object;
  object = (grobj_generic *)new grobj_cut1d;
  NextPoint->Manage();
  NextLabel->Manage();
  DeleteLast->Manage();
  Points->Manage();
  ChooseDefaultName();
  SetStandardCallbacks();
  ObjectInput::Manage();

  setrow(Xamine_PaneSelectedrow());
  setcol(Xamine_PaneSelectedcol());
}
/*
** Functional Description:
**   AcceptContour::Manage:
**     Makes the dialog widget visible.
*/
void AcceptContour::Manage()
{
  delete object;
  object = (grobj_generic *)new grobj_contour;
  NextPoint->Manage();
  NextLabel->Manage();
  DeleteLast->Manage();
  Points->Manage();
  ChooseDefaultName();
  SetStandardCallbacks();
  ObjectInput::Manage();

  setrow(Xamine_PaneSelectedrow());
  setcol(Xamine_PaneSelectedcol());
}
/*
** Functional Description:
**   AcceptBand::Manage:
**     Makes the dialog widget visible.
*/
void AcceptBand::Manage()
{
  delete object;
  object = (grobj_generic *)new grobj_band;
  NextPoint->Manage();
  NextLabel->Manage();
  DeleteLast->Manage();
  Points->Manage();
  ChooseDefaultName();
  SetStandardCallbacks();
  ObjectInput::Manage();

  setrow(Xamine_PaneSelectedrow());
  setcol(Xamine_PaneSelectedcol());
}

/*
** Functional Description:
**  Xamine_AcceptGate:
**     This function accepts a gate.  We instantiate an acceptance object which
**     depends on the type of gate required.
** Formal Parameters:
**   XMWidget *w:
**     The handle to the widget which called us.  We'll only use this to 
**     provide parents for other widgets created during the interaction.
**   XtPointer clientd:
**     This is actually the grobj_type of the object that we're being asked
**     to produce.
**  XtPointer calld:
**     An XmPushButtonCallbackStruct containing the call data.
*/
void Xamine_AcceptGate(XMWidget *w, XtPointer clientd, XtPointer calld)
{
  unsigned long cd         = (unsigned long)clientd;
  grobj_type object_type   = (grobj_type)cd;

  /* First verify that there is no graphical input in progress: */

  panemgr_select_callback cb;
  XtPointer userd;
  Xamine_GetSelectCallback(&cb, &userd);
  if(cb != NULL) {
    Xamine_error_msg(w, 
       "Graphical input is already in progress. Please finish or cancel it");

    return;
  }

  /* If the appropriate dialog type does not exist, then we build it: */

  AcceptSummingRegion *prompter;

  switch (object_type) {
  case cut_1d:
    if(cutin == NULL) {
      cutin = new AcceptCut("Cut_Prompt", w, help_text);
    }
    prompter = (AcceptSummingRegion *)cutin;
    break;
  case contour_2d:
    if(contin == NULL) {
      contin = new AcceptContour("Contour_prompt", w, help_text);
    }
    prompter = (AcceptSummingRegion *)contin;
    break;
  case band:
    if(bandin == NULL) {
      bandin = new AcceptBand("Band input", w, help_text);
    }
    prompter = (AcceptSummingRegion *)bandin;
    break;
  default:
    Xamine_error_msg(Xamine_Getpanemgr(),
		     "Xamine_AcceptGate -- Invalid gate type proposed");
    return;
  }
  /* Set the current row and columen and manage the dialog: */

  prompter->setrow(Xamine_PaneSelectedrow());
  prompter->setcol(Xamine_PaneSelectedrow());

  prompter->Manage();
}

/*
** Functional Description:
**   DrawBand:
**     This static function actually draws the lines associated with a band.
**     Bands are drawn as unclosed polylines.
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
static
void 
DrawBand(Display *d, Drawable w, GC gc,
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
  poly.flush();			/* Draw the lines. */
}


/*
** Functional Description:
**   Xamine_DisplayCut:
**     This function draws a cut.  A cut is a pair of points in the channel
**     number coordinates of the spectrum.  The cut is drawn as a pair
**     of lines parallel to the counts axis. If labeling is enabled, then
**     the identifier of the cut is shown at the mid point of the lines.
** Formal Parameters:
**   Display *d:
**     Identifies the X-11 display to which we're connected.
**   Drawable w:
**     The window or pixmap into which we're supposed to draw the lines.
**   GC gc:
**     A graphical context which determines how the lines are drawn.
**   XMWidget *wid:
**     Is the handle to the widget object corresponding to the drawable.
**   Xamine_Converter *cvt:
**     Is the object which can convert coordinate systems.
**   int flipped:
**     True if the spectrum is flipped.
**   int label:
**     True if we're supposed to label the object.
**   grobj_generic *object:
**     The object we're supposed to draw.
*/
void 
Xamine_DisplayCut(Display *d, Drawable w, GC gc,
                  XMWidget *wid, Xamine_Converter *cvt,
                  int flipped, int label, 
                  grobj_generic *object)
{
  assert(object->type() == cut_1d);

  /* Draw the object: */

  cvt->NoClip();		/* Use the clipping region to clip cvt */
  Xamine_Draw1dCut(d, w, gc, wid, cvt, flipped, object);

  /* If labelling is turned on then draw them too:  */

  if(label) {
    char label[20];
    Dimension nx;
 
    sprintf(label, "C%d", object->getid()); /* Construct the label. */
    wid->GetAttribute(XmNwidth, &nx);
    for(int i = 0; i < object->pointcount(); i++) {
      int xpix, ypix;
      grobj_point *pt = object->getpt(i);
      cvt->SpecToScreen(&xpix, &ypix, pt->getx());
      if(flipped) {
	Xamine_LabelGrobj(d,w,gc,wid,
			  label, (xpix + (int)nx)/2,
			  ypix + XAMINE_LABEL_OFFSET);
      }
      else {
	Xamine_LabelGrobj(d,w,gc,wid,
			  label, xpix, ypix/2);
      }
    }
  }
    cvt->Clip();
}

/*
** Functional Description:
**   Xamine_DisplayContour:
**     This function Displays a contour.  A contour is displayed as a closed
**     polyline (polygon).  
** Formal Parameters:
**  Display *d:
**    Determines the X-11 connection to the display surface.
**  Drawable win:
**    Is the id of the drawable (window or pixmap) into which the contour
**    is drawn.
**  GC gc:
**    Is the graphical context to use when drawing the object. The graphical
**    context contains the drawing attributes.
**  XMWidget *wid:
**    The widget which corresponds to win.
**  Xamine_Converter *cvt:
**    The coordinate converter object.
**  int flipped:
**    True if x/y axes are to be flipped.
**  int label:
**    True if object labels should be drawn.  For contours, 
**    the labels are drawn at the first vertex.
*/
void
Xamine_DisplayContour(Display *d, Drawable w, GC gc,
		      XMWidget *wid, Xamine_Converter *cvt,
		      int flipped, int label,
		      grobj_generic *object)
  {
  assert(object->type() == contour_2d);

  cvt->NoClip();		/* Let the X-11 clipping region do clipping */

  /* Draw the contour: */

  Xamine_Draw2dContour(d,w,gc, wid, cvt, flipped, object);

  /* If the labels are turned on, then draw them:  */

  if(label) {
    char label[10];
    int xpix, ypix;
    grobj_point *pt = object->getpt(0);

    sprintf(label, "C%d", object->getid());
    if(flipped) {
      cvt->SpecToScreen(&xpix, &ypix, pt->gety(), pt->getx());
    }
    else {
      cvt->SpecToScreen(&xpix, &ypix, pt->getx(), pt->gety());
    }
    Xamine_LabelGrobj(d,w,gc,wid, label,
		      xpix, ypix + XAMINE_LABEL_OFFSET);
  }

  /* Set converter clipping back to default state: */

  cvt->Clip();
}
			  

/*
** Functional Description:
**   Xamine_DisplayBand:
**    This function displays a band.  Like contours, bands are 2-d gating
**    objects.  In fact the only difference between bands and contours is
**    that bands don't close (They're just polylines).
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
Xamine_DisplayBand(Display *d, Drawable win, GC gc, XMWidget *wid, 
		   Xamine_Converter *cvt, 
		   int flipped, int labelit, 
		   grobj_generic *object)
{
  assert(object->type() == band);
  cvt->NoClip();		/* Let X11 clip region do the clipping. */
  /*
  ** Draw the band.
  */
  DrawBand(d,win, gc, wid, cvt, flipped, object);
  /* 
  ** If labeling is turned on then draw a label at the first point position
  */

  if(labelit) {
    char label[10];
    int xpix, ypix;
    grobj_point *pt = object->getpt(0);

    sprintf(label, "B%d", object->getid());
    if(flipped) {
      cvt->SpecToScreen(&xpix, &ypix, pt->gety(), pt->getx());
    }
    else {
      cvt->SpecToScreen(&xpix, &ypix, pt->getx(), pt->gety());
    }
    Xamine_LabelGrobj(d,win,gc,wid, label,
		      xpix, ypix + XAMINE_LABEL_OFFSET);
  }


  cvt->Clip();			/* Return to default clipping state. */
}
