/*
** Facility:
**    Motif/C++ class support.
** Abstract:
**    XMManagers.h   - This include file is intended for use by clients
**                     of Motif manager classes.
**
** Author:
**    Ron Fox
**    NSCl
**    Michigan State University
**    East Lansing, MI 48824-1321
**    @(#)XMManagers.h	8.1 6/23/95 
*/

#ifndef _XMMANAGERS_H
#define _XMMANAGERS_H

#ifdef unix
#pragma interface
#endif

/*
** Include files required:
*/
#include <Xm/BulletinB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PanedW.h>
#include <Xm/MainW.h>
#include <Xm/Frame.h>

#include "XMWidget.h"

/*
** The XMManager widget is the superclass of all manager classes.
** The manager is built as an unmanaged widget to allow the user
** control over when the manager is managed and realized.
*/

class XMManager : public XMWidget {
 public:
  /* Constructors */

  XMManager(char *n) : XMWidget(n) {}
  XMManager(Widget w) : XMWidget(w) {}
  XMManager(char *n,
	    WidgetClass cl,
	    XMApplication &parent,
	    ArgList l= NULL,Cardinal num_args = 0) : XMWidget(n, cl, parent,
						       l, num_args) {}
  XMManager(char *n, WidgetClass cl, Widget parent,
	    ArgList l=NULL, Cardinal num_args = 0) :
	      XMWidget(n, cl, parent, l, num_args) {}

  XMManager(char *n, WidgetClass cl, XMWidget &parent,
	     ArgList l = NULL, Cardinal num_args = 0) :
	      XMWidget(n, cl, parent, l, num_args) { }
  
  /*  The following functions manipulate generic manager resources: */

  Callback_data *AddHelpCallback(void (*callback)(XMWidget *, XtPointer, XtPointer),
		  XtPointer client_data) {
    return AddCallback(XmNhelpCallback, callback, client_data);
  }
  void SetNavigationType(XmNavigationType navtype = XmNONE) {
    SetAttribute(XmNnavigationType, navtype);
  }

  void TraversalOn() {
    SetAttribute(XmNtraversalOn, True);
  }
  void TraverseOff() {
    SetAttribute(XmNtraversalOn, (XtArgVal)False);
  }

  
  /*  The function below is a convenience function to hide the fact that
  **  management is actually modified via constraint resources applied to
  **  the widget itself rather than by creating, and managing with constraints
  */

  void SetConstraint(XMWidget &w, String constraint, XtArgVal value) {
    w.SetAttribute(constraint, value);
  }
  void SetConstraint(XMWidget &w, String constraint, void *value) {
    w.SetAttribute(constraint, value);
  }
  void SetConstraint(Widget w, String constraint, XtArgVal value) {
    XtVaSetValues(w, constraint, value, NULL);
  }
  void SetConstraint(Widget w, String constraint, void *value) {
    XtVaSetValues(w, constraint, value, NULL);
  }
};
/*
**  The Bulletin board manager allows users to pin items up on arbitrary
**  positions of the bulletin board.  No actual geometry management is 
**  performed by the bulletin board.
*/
class XMBulletinBoard : public XMManager {
 protected:
  XMBulletinBoard(char *n, WidgetClass cl, Widget parent,
		  ArgList l, Cardinal num_args) : 
		    XMManager(n, cl, parent, l, num_args) {}
 public:
  /* Constructors: */

  XMBulletinBoard(char *n) : XMManager(n) {}
  XMBulletinBoard(Widget w): XMManager(w) {}
  XMBulletinBoard(char *n, XMApplication &parent,
		  ArgList l = NULL, Cardinal num_args = 0) :
		    XMManager(n, xmBulletinBoardWidgetClass, parent, 
			     l, num_args) {}
  XMBulletinBoard(char *n, Widget parent, 
		  ArgList l = NULL, Cardinal num_args=0) : 
		    XMManager(n, xmBulletinBoardWidgetClass, parent,
			     l, num_args) {}
  XMBulletinBoard(char *n, XMWidget &parent,
		  ArgList l = NULL, Cardinal num_args = 0) : 
		    XMManager(n, xmBulletinBoardWidgetClass, parent,
			     l, num_args) {}
  /* The following modify attributes of the bulletin board itself: */

  void AllowOverlap(Boolean allow) {
    SetAttribute(XmNallowOverlap, allow);
  }
  void Margins(Dimension ymargin, Dimension xmargin = 0) {
    SetAttribute(XmNmarginHeight, ymargin);
    SetAttribute(XmNmarginWidth, xmargin);
  }
  void SetShadowType(unsigned char type) {
    SetAttribute(XmNshadowType, type);
  }
  /* The following constraints locate the widget: */

  void SetAbsPosition(XMWidget &w, int x, int y) {
    SetConstraint(w, XmNx, x);
    SetConstraint(w, XmNy, y);
  }
    
};
/*
** The frame manager is just a picture frame that can be wrapped around a
** single widget.  Often this is used around a manager to put a box around
** some related set of controls.
*/
class XMFrame : public XMManager {
 public:
  /* Constructors: */

  XMFrame(char *name) : XMManager(name) {}
  XMFrame(Widget id)  : XMManager(id)   {}
  XMFrame(char *n, XMApplication &parent, 
	  ArgList l = NULL, Cardinal num_args = 0) : 
	    XMManager(n, xmFrameWidgetClass, parent, l, num_args) {}
  XMFrame(char *n, XMWidget &parent,
	  ArgList l = NULL, Cardinal num_args = 0) :
	    XMManager(n, xmFrameWidgetClass, parent, l, num_args) {}
  XMFrame(char *n, Widget parent, ArgList l = NULL, Cardinal num_args = 0) :
    XMManager(n, xmFrameWidgetClass, parent, l, num_args) {}
  
  /* Set frame attributes: */
  
  void Margins(Dimension height, Dimension width = 0) {
    SetAttribute(XmNmarginWidth, width);
    SetAttribute(XmNmarginHeight, height);
  }
  void SetShadowType(unsigned char type) {
    SetAttribute(XmNshadowType, type);
  }
  
};
/*
 ** A form manager is subclassed from the Bulletin board.  It provides
 ** a highly flexible layout policy set and automatically positions and
 ** repositions widgets as needed top make things work.
 */

class XMForm : public XMBulletinBoard  {
 public:
  /* Constructors: */
  XMForm(char *name) : XMBulletinBoard(name) {}
  XMForm(Widget id)  : XMBulletinBoard(id)   {}
  XMForm(char *n, XMApplication &parent, 
	 ArgList l = NULL, Cardinal num_args = 0) :
	   XMBulletinBoard(n, xmFormWidgetClass, parent.getid(), l, num_args)
	     {}
  XMForm(char *n, Widget parent, 
	 ArgList l=NULL, Cardinal num_args = 0) :
	   XMBulletinBoard(n, xmFormWidgetClass, parent, l, num_args)
	     {}
  XMForm(char *n, XMWidget &parent, 
	 ArgList l = NULL, Cardinal num_args = 0) :
	   XMBulletinBoard(n, xmFormWidgetClass, parent.getid(), l, num_args) 
	   {}
  
  /* Methods  which  set attributes of the form as a whole */

  void SetFractionBase(int n) {
    SetAttribute(XmNfractionBase, n);
  }
  void SetHorizontalSpacing(Dimension n) {
    SetAttribute(XmNhorizontalSpacing, n);
  }
  void SetRubberPositioning(Boolean on) {
    SetAttribute(XmNrubberPositioning, on);
  }
  void SetVerticalSpacing(Dimension n) {
    SetAttribute(XmNverticalSpacing, n);
  }

  /* Constraint methods.  */

  void SetBottomAttachment(XMWidget &wid, unsigned char method) {
    SetConstraint(wid, XmNbottomAttachment, method);
  }
  void SetBottomAttachment(Widget wid, unsigned char method) {
    SetConstraint(wid, XmNbottomAttachment, method);
  }

  void SetBottomOffset(XMWidget &wid, int off) {
    SetConstraint(wid, XmNbottomOffset, off);
  }
  void SetBottomPosition(XMWidget &wid, int position) {
    SetConstraint(wid, XmNbottomPosition, position);
  }
  void SetBottomWidget(XMWidget &wid, XMWidget &bound) {
    SetConstraint(wid, XmNbottomWidget, bound.getid());
  }
  void SetBottomWidget(XMWidget &wid, Widget bound) {
    SetConstraint(wid, XmNbottomWidget, bound);
  }
  

  void SetLeftAttachment(XMWidget &wid, unsigned char method) {
    SetConstraint(wid, XmNleftAttachment, method);
  }
  void SetLeftAttachment(Widget wid, unsigned char method) {
    SetConstraint(wid, XmNleftAttachment, method);
  }
  void SetLeftOffset(XMWidget &wid, int offset) {
    SetConstraint(wid, XmNleftOffset, offset);
  }
  void SetLeftPosition(XMWidget &wid, int position) {
    SetConstraint(wid, XmNleftPosition, position);
  }
  void SetLeftWidget(XMWidget &wid, XMWidget &bound) {
    SetConstraint(wid, XmNleftWidget, bound.getid());
  }
  void SetLeftWidget(XMWidget &wid, Widget bound) {
    SetConstraint(wid, XmNleftWidget, bound);
  }


  void SetRightAttachment(XMWidget &wid, unsigned char method) {
    SetConstraint(wid, XmNrightAttachment, method);
  }
  void SetRightAttachment(Widget wid, unsigned char method) {
    SetConstraint(wid, XmNrightAttachment, method);
  }
  void SetRightOffset(XMWidget &wid, int offset) {
    SetConstraint(wid, XmNrightOffset, offset);
  }
  void SetRightPosition(XMWidget &wid, int position) {
    SetConstraint(wid, XmNrightPosition, position);
  }
  void SetRightWidget(XMWidget &wid, XMWidget &bound) {
    SetConstraint(wid, XmNrightWidget, bound.getid());
  }
  void SetRightWidget(XMWidget &wid, Widget bound) {
    SetConstraint(wid, XmNrightWidget, bound);
  }

  void SetTopAttachment(XMWidget &wid, unsigned char method) {
    SetConstraint(wid, XmNtopAttachment, method);
  }
  void SetTopAttachment(Widget wid, unsigned char method) {
    SetConstraint(wid, XmNtopAttachment, method);
  }
  void SetTopOffset(XMWidget &wid, int offset) {
    SetConstraint(wid, XmNtopOffset, offset);
  }
  void SetTopPosition(XMWidget &wid, int position) {
    SetConstraint(wid, XmNtopPosition, position);
  }
  void SetTopWidget(XMWidget &wid, XMWidget &bound) {
    SetConstraint(wid, XmNtopWidget, bound.getid());
  }
  void SetTopWidget(XMWidget &wid, Widget bound) {
    SetConstraint(wid, XmNtopWidget, bound);
  }
  void SetTopWidget(Widget wid, Widget bound) {
    SetConstraint(wid, XmNtopWidget, bound);
  }
  void SetTopWidget(Widget wid, XMWidget &bound) {
    SetConstraint(wid, XmNtopWidget, bound.getid());
  }
};

/*
** The Row column widget is subclassed from the XMManager class.  It
** implements a manager which lays out a regular grid of managed widgets.
** While it is implicitly used in menu handling, the client should see
** XMMenus.h for a better way to handle menus than manual construction.
*/
class XMRowColumn : public XMManager {
 public:
  /*  Constructors:  */

  XMRowColumn(char *name) : XMManager(name) {}
  XMRowColumn(Widget id)  : XMManager(id)   {}
  XMRowColumn(char *name, XMApplication &parent, 
	      ArgList l = NULL, Cardinal num_args = 0) :
		XMManager(name, xmRowColumnWidgetClass, parent, l, num_args) {}
  XMRowColumn(char *name, XMWidget &parent,
	      ArgList l = NULL, Cardinal num_args = 0) :
		XMManager(name, xmRowColumnWidgetClass, parent, l, num_args) {}
  XMRowColumn(char *name, Widget parent,
	      ArgList l = NULL, Cardinal num_args = 0) :
		XMManager(name, xmRowColumnWidgetClass, parent, l, num_args) {}

  /* Methods to set the major attributes of a row column widget */

  void SetAlignment(unsigned char alignment) {
    SetAttribute(XmNentryAlignment, alignment);
  }
  void SetEntryClass(WidgetClass type) {
    SetAttribute(XmNentryClass, type);
  }
  void Align(Boolean align) {
    SetAttribute(XmNisAligned, align);
  }
  void SetHomogenous(Boolean homog) {
    SetAttribute(XmNisHomogeneous, homog);
  }
  void Margins(Dimension vertical, Dimension horizontal) {
    SetAttribute(XmNmarginHeight, vertical);
    SetAttribute(XmNmarginWidth, horizontal);
  }

  void SetRowColumns(int numcols) {
    SetAttribute(XmNnumColumns, numcols);
  }
  void SetOrientation(unsigned char orientation) {
    SetAttribute(XmNorientation, orientation);
  }
  void SetPacking(unsigned char packmethod) {
    SetAttribute(XmNpacking, packmethod);
  }
  void RadioMenu()      /* Request toggles be radio buttons */
    { SetAttribute(XmNradioBehavior, (XtArgVal)True); }
  void RadioForceOne() {
    SetAttribute(XmNradioAlwaysOne, (XtArgVal)True); }
  void NoRadioMenu() {
    SetAttribute(XmNradioBehavior, (XtArgVal)False); }
  void RadioNoForceOne() {
    SetAttribute(XmNradioAlwaysOne, (XtArgVal)False); }
  void SetSpacing(Dimension spacing) {
    SetAttribute(XmNspacing, spacing);
  }

  
  /* Callback methods: */

  Callback_data *AddEntryCallback(void (*callback)(XMWidget *,XtPointer, XtPointer),
			XtPointer client_data) {
    return AddCallback(XmNentryCallback, callback, client_data);
  }

};

class XMPanedWindow : public XMManager {
 public:
  /* Constructors: */
  XMPanedWindow(char *name) : XMManager(name) {}
  XMPanedWindow(Widget id)  : XMManager(id)   {}
  XMPanedWindow(char *n, XMWidget &parent,
		 ArgList l = NULL, Cardinal num_args = 0) :
		   XMManager(n, xmPanedWindowWidgetClass, parent, 
			     l, num_args) {}
  XMPanedWindow(char *n, Widget parent,
		 ArgList l = NULL, Cardinal num_args = 0) :
		   XMManager(n, xmPanedWindowWidgetClass, parent,
			     l, num_args) {}

  /* Set attributes of the manager itself. */

  void Margins(Dimension vert, Dimension hor) {
    SetAttribute(XmNmarginHeight, vert);
    SetAttribute(XmNmarginWidth, hor);
  }
  void SetSashHeight(Dimension height) {
    SetAttribute(XmNsashHeight, height);
  }
  void SetSashIndent(Position indent) {
    SetAttribute(XmNsashIndent, indent);
  }
  void SetSeparator(Boolean enabled) {
    SetAttribute(XmNseparatorOn, enabled);
  }
  void SetPaneSpacing(Dimension spacing) {
    SetAttribute(XmNspacing, spacing);
  }

  /* Constraint setting methods            */

  void AllowResize(XMWidget &wid, Boolean allow) {
    wid.SetAttribute(XmNallowResize, allow);
  }
  void MaxPaneSize(XMWidget &wid, Dimension max) {
    wid.SetAttribute(XmNpaneMaximum, max);
  }
  void MinPaneSize(XMWidget &wid, Dimension min) {
    wid.SetAttribute(XmNpaneMinimum, min);
  }
  void SkipAdjust(XMWidget &wid, Boolean skip) {
    wid.SetAttribute(XmNskipAdjust, skip);
  }
};
/*
** The Main window while not in the Manager classes discussed in O'Reilly's
** is actually a manager widget which is subclassed from the Manager widget
** class.  It managers a fixed set of subwidgets in an organization which
** is similar to that of a Paned Window.
*/
class XMMainWindow : public XMManager {
 public:
  XMMainWindow(char *name) : XMManager(name) {}
  XMMainWindow(Widget id)  : XMManager(id  ) {}
  XMMainWindow(char *name, XMApplication &parent,
	       ArgList l = NULL, Cardinal arg_count = 0) :
		 XMManager(name, xmMainWindowWidgetClass, parent,
			   l, arg_count) {}
  XMMainWindow(char *name, XMWidget &parent, 
	       ArgList l = NULL, Cardinal arg_count  = 0) :
		 XMManager(name, xmMainWindowWidgetClass, parent,
			   l, arg_count) {}
  XMMainWindow(char *name, Widget &parent,
	       ArgList l = NULL, Cardinal arg_count = 0) :
		 XMManager(name, xmMainWindowWidgetClass, parent, 
			   l, arg_count) {}

  /*  Modify attributes of the main window: */

  void SetAreas(XMWidget *menubar = NULL, XMWidget *commandwindow = NULL,
		XMWidget *horizontalscroller = NULL,
		XMWidget *verticalscroller = NULL,
		XMWidget *workregion = NULL, XMWidget *messagewidget = NULL);

  void ShowSeparator(Boolean show) {
    SetAttribute(XmNshowSeparator, show);
  }
  void CommandWindowLocation(unsigned char where) {
    SetAttribute(XmNcommandWindowLocation, where);
  }


};
#endif
