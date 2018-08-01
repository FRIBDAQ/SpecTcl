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
**    sumregion.h:
**      This file defines datatypes and procedures used by the summing region
**      handling software.
** Author:
**    Ron FOx
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**    December 13, 1993
** SCCS info:
**    @(#)sumregion.h	8.1 6/23/95 
*/

/*
  Change Log:
  $Log$
  Revision 5.3  2007/05/29 14:50:24  ron-fox
  Fix channel to pixel conversion errors in Xamine.. was misplacing positions of summing regions etc.

  Revision 5.2  2005/06/03 15:18:57  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:15  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:55:37  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.3.4.1  2004/10/20 15:47:42  ron-fox
  Misc changes resulting from update merges and pedantic finds.

  Revision 4.3  2003/08/25 16:25:31  ron-fox
  Initial starting point for merge with filtering -- this probably does not
  generate a goo spectcl build.

  Revision 4.2  2003/04/04 17:44:21  ron-fox
  Catch dialog destruction for cached widgets so that the destroyed dialog is re-created when it's needed.  Prior behavior would usually crash Xamine because deleted widgets would be referenced.

*/

#ifndef SUMREGION_H_INSTALLED
#define SUMREGION_H_INSTALLED

#include "XMWidget.h"
#include "XMList.h"
#include "XMPushbutton.h"
#include "XMLabel.h"
#include "XMText.h"

#include "convert.h"
#include "dispgrob.h"
#include "grobjinput.h"
/*
** We export the class that accepts summing regions because accepting
** contours and cuts is almost identical and can therefore be derived via
** inheritance:
*/
/*
**      The  AcceptSummingRegion class is a specialization of the 
**   ObjectInput class adapted to accepting summing regions.
**   In order to simplify the input, the prompting region is the same
**   for 1-d and 2-d.  A list box is used to list the set of points that
**   are already accepted (2 max if a 1-d).  A button allows the most recent
**   point to be deleted. 
**   A typein line allows a point to be entered from the keyboard.
**   The layout is as follows:
**                   +---------------+
**                   |               |
**                   |  List box     |  +---------------+
**                   |               |  | Delete Last   |
**                       ...            +---------------+
**                   +---------------+
**                    Next Point:
**                   |______________
*/
class AcceptSummingRegion : public ObjectInput {
 protected:
  XMScrolledList *Points;
  XMPushButton   *DeleteLast;
  XMLabel        *NextLabel;
  XMTextField    *NextPoint;
  grobj_generic  *object;
 public:

  /* Constructors and destructors: */

  AcceptSummingRegion(const char *name, XMWidget *parent, 
		      const char **help_text = NULL);
  virtual ~AcceptSummingRegion();

  /* Virtual functions which are pure in the base class: */

  virtual void ClearState();
  virtual void DrawPoints(XMWidget *pane, 
       		  Xamine_RefreshContext *ctx = NULL);
  virtual void ClearPoints(XMWidget *pane,
		   Xamine_RefreshContext *ctx = NULL);
  virtual void ClearDialog();
  virtual void AddPoint(point &pt);
  virtual void DelPoint();
  virtual Boolean DoAccept();

  /*  The following functions override base class virtuals: */

  virtual void SelectChanged(int oldc, int oldr, int newc, int newr);
  
  /* Additional behavior: */

  virtual void ChooseDefaultName();	/* Select default name for object.        */
  virtual void RebuildObject();    /* Build grobj depending on current spec. */
  void TextPoint();		/* Accept Typed in point. */

  /* Widget management:  */

  virtual void Manage() {
    NextPoint->Manage();
    NextLabel->Manage();
    DeleteLast->Manage();
    Points->Manage();
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
    Points->UnManage();
    DeleteLast->UnManage();
    NextLabel->UnManage();
    NextPoint->UnManage();
    ObjectInput::UnManage();
  }
  // Callback relay functionsl

private:
  static void CancelRelay(XMWidget *w, XtPointer ud, XtPointer cd);
  static void ApplyRelay(XMWidget *w, XtPointer ud, XtPointer cd);
  static void OkRelay(XMWidget *w, XtPointer ud, XtPointer cd);
  static void TextPointRelay(XMWidget *w, XtPointer ud, XtPointer cd);
  static void Delete_relay(XMWidget *w, XtPointer ud, XtPointer cd);

};


/* Public exported functions: */


void Xamine_AcceptSummingRegion(XMWidget *w, XtPointer ud, XtPointer cd);
void Xamine_DrawSumRegion(Display *d,
			  Drawable w,
			  GC ctx,
			  XMWidget *wid,
			  Xamine_Converter *cvt,
			  Boolean flipped,
			  Boolean labelit,
			  grobj_generic *object);
void Xamine_Draw2dContour(Display *d, Drawable w, GC gc,
			  XMWidget *wid, Xamine_Converter *cvt,
			  Boolean flipped, grobj_generic *object);
void Xamine_Draw1dCut(Display *d, Drawable win, GC ctx, XMWidget *wid,
		      Xamine_Converter *cvt, Boolean flipped, 
		      grobj_generic *object);
#endif
