/*
** Facility:
**   Xamine -- NSCL Display program.
** Abstract:
**   ptlist.cc:
**     This file implements the nontrivial methods of the class
**     AcceptPointList.  This class accepts a point list which is not
**     exactly a first class graphical object in the sense that it can never
**     appear in the graphical object databases.  Point lists are usually used
**     to communicate with Xamine's client through client button presses.
** Author:
**   Ron FOx
**   NSCL
**   MIchigan State University
**   East Lansing, MI 48824-1321
*/

/*
** Include files:
*/
#include "panemgr.h"
#include "errormsg.h"
#include "dispgrob.h"
#include "ptlist.h"

/*
** The following is the default help text which is supplied if the
** client does not supply any help text.  It should be suitable for most 
** occasions.
*/

static char *default_help[] =
{
  "  You are being prompted to enter a list of points.  Points can be\n",
  "accepted either by clicking MB1 in the currently selected spectrum\n",
  "or by typing a point in to the next point text box. The point should be\n",
  "typed in in 'spectrum coordinates' (channel, counts for 1-ds and channel\n",
  "channel for 2-ds).  Other actions that are possible include:\n\n",
  "   Clicking the Delete Last button   -- Deletes the most recent point\n",
  "   Clicking MB2 or MB3 in the \n",
  "    selected pane                    -- Deletes the most recent pont\n",
  "   Clicking on a different pane      -- Deletes all points and starts\n",
  "                                        the point selection process\n",
  "                                        (if legal) in the new pane\n",
  "   Type text in the name text box    -- Sets a new name for the points\n\n",
  "  The buttons at the bottom perform the following functions:\n\n",
  "      Ok         - Accepts the current set of points and dismisses the\n",
  "                   dialog.\n",
  "      Apply      - Accepts the current set of points but continues to\n",
  "                   display the dialog.\n",
  "      Cancel     - Cancels the operation, dismissing the dialog.\n",
  "      Help       - As you must by now know brings up this help text\n",
  NULL
};



/*
** Functional Description:
**   AcceptPointList::AcceptPointList
**      Constructor for the AcceptPointList class.  The only action we
**      take besides calling the superclass' constructor is to 
**      set the values ofr minpts and maxpts.
** Formal Parameters:
**   char *name:
**     Name of the dialog.
**   XMWidget *parent:
**     parent of the widget tree created for the prompter.
**   int minpts:
**     Fewest points the user can enter for accept to be legal.
**   int maxpts:
**     Maximum number of points the user can enter for accept to be legal.
**   char **help_text:
**     User supplied help text list.. we supply default text if omitted.
*/
AcceptPointList::AcceptPointList(char *name, XMWidget *parent, 
				 int minpts, int maxpts,
				 char **help_text) :
		 AcceptSummingRegion(name, parent, 
				     help_text == NULL ? default_help :
				                         help_text)
{
  if(minpts > 0) min_pts = minpts;
  else           min_pts = 1;

  if(maxpts <= GROBJ_MAXPTS) max_pts = maxpts;
  else                       max_pts = GROBJ_MAXPTS;

}   


/*
** Functional Description:
**   AcceptPointList::DoAccept:
**     This function is called to perform the accept.  It can be called from
**     either the Ok or Apply method.  
** Formal Parameters:
**   NONE:
** Returns:
**   True   - If the points were properly accepted and Ok (if that's what 
**            called us) can pop down the requester dialog.
**   False  - If the points could not be accepted... and Ok (if that's
**            what called us) should not pop down the requester dialog.
*/
Boolean AcceptPointList::DoAccept()
{
  char error_msg[256];
  if(object == NULL) {
    Xamine_error_msg(Points, "You cannot Accept with no points, use Cancel\n");
    return False;
  }

  int npts = object->pointcount();
  if(npts < min_pts) {
    sprintf(error_msg,
	    "You must enter at least %d points to accept\n",
	    min_pts);
    Xamine_error_msg(Points, error_msg);
    return False;
  }

  if(npts > max_pts) {		// This should never be called due to AddPoint
    sprintf(error_msg,
	    "You can enter at most %d points to accept\n",
	    max_pts);
    Xamine_error_msg(Points, error_msg);
  }

  return True;			// Doing this lets the deriver use us to 
				// range check prior to acceptance.
}   


/*
** Functional Description:
**   AcceptPointList::ChooseDefaultName:
**     This function selects a default name for the object.  The default
**     name is put in the text box for the object name and will be used
**     to name the object in DoAccept if the user has not overridden it.
**     For point lists, we just use the generic string:
**      "Point List"
*/

void AcceptPointList::ChooseDefaultName()
{
  ObjectInput::SetText("Point List");
}   


/*
** Functional Description:
**   AcceptPointList::SelectChanged:
**     This method is called if the selected pane changes.  It is our 
**     responsibility to:
**      1)   Delete any points that have been accepted (includes undrawing)
**      2)   Destroy the old object.
**      3)   If the new pane has no spectrum, then unmanage else:
**      4)   Create a new object of the appropriate type.
**
** Formal Parameters:
**   int oldc, int oldr:
**      Prior row and column of the selection.
**   int newc, newr:
**      Row and column of the pane we're moving into.
**   
*/
void AcceptPointList::SelectChanged(int oldc, int oldr,
				    int newc, int newr)
{
  GraphicalInput::SelectChanged(oldc, oldr, // This will get rid of old points
				newc, newr); // etc.
  RebuildObject();			     // Build a new object.
}

/*
** Functional Description:
**   AcceptPointList::RebuildObject:
**     This method rebuilds the object.  If an old object exists, it is deleted
**     A new object is created of the appropriate type for the underlying
**     pane as follows:
**      Pane is 1-d:  Creates a grobj_ptlist1
**      Pane is 2-d:  Creates a grobj_ptlist2
**      Pane is empty: Leaves object NULL... Unmanages pane.
*/
void AcceptPointList::RebuildObject()
{
  if(object != NULL) {
    delete object;		// Get rid of any pending object.
    object = NULL;		// Leave pointer null in case no spectrum
  }
  
  win_attributed *att = Xamine_GetDisplayAttributes(row, col);
  if(att == NULL) {		// No underlying spectrum:
    UnManage();			//  So don't create a new object and pop down
    return;			//  the dialog.
  }

  if(att->is1d()) {		// Underlying spectrum is 1d so:
    object = new grobj_ptlist1(max_pts);
				//   Create the new 1-d point list object.
  }
  else {			// Underlying specturm is 2d so:
    object = new grobj_ptlist2(max_pts);
				//   Create the new 2-d point list object.
  }
				// And return.
}   


/*
** Functional Description:
**   AcceptPointList::AddPoint:
**     This function adds a point. We need to override the sumregion
**     point adder because we have different constraints on the number
**     of points allowed (especially on 1-d spectra).
** Formal Parameters:
**   point &pt:
**     The point to add.
*/

void AcceptPointList::AddPoint(point &pt)
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

  if(object->pointcount() == max_pts) {
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
