/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

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

#ifndef XMMANAGERS_H
#define XMMANAGERS_H

/*
** Include files required:
*/
#include <Xm/BulletinB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PanedW.h>
#include <Xm/MainW.h>
#include <Xm/Frame.h>

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

/*
** The XMManager widget is the superclass of all manager classes.
** The manager is built as an unmanaged widget to allow the user
** control over when the manager is managed and realized.
*/

class XMManager : public XMWidget {
 public:
  /* Constructors */

  XMManager(const char *n);
  XMManager(Widget w);
  XMManager(const char *n,
	    WidgetClass cl,
	    XMApplication &parent,
	    ArgList l= NULL,Cardinal num_args = 0);
  XMManager(const char *n, WidgetClass cl, Widget parent,
	    ArgList l=NULL, Cardinal num_args = 0);
  XMManager(const char *n, WidgetClass cl, XMWidget &parent,
	     ArgList l = NULL, Cardinal num_args = 0);
  
  /*  The following functions manipulate generic manager resources: */

  Callback_data *AddHelpCallback(void (*callback)(XMWidget *, XtPointer, XtPointer),
		  XtPointer client_data);
  void SetNavigationType(XmNavigationType navtype = XmNONE);
  void TraversalOn();
  void TraverseOff();
  
  /*  The function below is a convenience function to hide the fact that
  **  management is actually modified via constraint resources applied to
  **  the widget itself rather than by creating, and managing with constraints
  */

  void SetConstraint(XMWidget &w, String constraint, XtArgVal value);
  void SetConstraint(XMWidget &w, String constraint, void *value);
  void SetConstraint(Widget w, String constraint, XtArgVal value);
  void SetConstraint(Widget w, String constraint, void *value);
};
/*
**  The Bulletin board manager allows users to pin items up on arbitrary
**  positions of the bulletin board.  No actual geometry management is 
**  performed by the bulletin board.
*/
class XMBulletinBoard : public XMManager {
 protected:
  XMBulletinBoard(const char *n, WidgetClass cl, Widget parent,
		  ArgList l, Cardinal num_args);

 public:
  /* Constructors: */

  XMBulletinBoard(const char *n);
  XMBulletinBoard(Widget w);
  XMBulletinBoard(const char *n, XMApplication &parent,
		  ArgList l = NULL, Cardinal num_args = 0);
  XMBulletinBoard(const char *n, Widget parent, 
		  ArgList l = NULL, Cardinal num_args=0);
  XMBulletinBoard(const char *n, XMWidget &parent,
		  ArgList l = NULL, Cardinal num_args = 0);

  /* The following modify attributes of the bulletin board itself: */

  void AllowOverlap(Boolean allow);
  void Margins(Dimension ymargin, Dimension xmargin = 0);
  void SetShadowType(unsigned char type);

  /* The following constraints locate the widget: */

  void SetAbsPosition(XMWidget &w, int x, int y);
};
/*
** The frame manager is just a picture frame that can be wrapped around a
** single widget.  Often this is used around a manager to put a box around
** some related set of controls.
*/
class XMFrame : public XMManager {
 public:
  /* Constructors: */

  XMFrame(const char *name);
  XMFrame(Widget id);
  XMFrame(const char *n, XMApplication &parent, 
	  ArgList l = NULL, Cardinal num_args = 0);
  XMFrame(const char *n, XMWidget &parent,
	  ArgList l = NULL, Cardinal num_args = 0);
  XMFrame(const char *n, Widget parent, ArgList l = NULL, Cardinal num_args = 0);
  
  /* Set frame attributes: */
  
  void Margins(Dimension height, Dimension width = 0);
  void SetShadowType(unsigned char type);
};
/*
 ** A form manager is subclassed from the Bulletin board.  It provides
 ** a highly flexible layout policy set and automatically positions and
 ** repositions widgets as needed top make things work.
 */

class XMForm : public XMBulletinBoard  {
 public:
  /* Constructors: */
  XMForm(const char *name);
  XMForm(Widget id);
  XMForm(const char *n, XMApplication &parent, 
	 ArgList l = NULL, Cardinal num_args = 0);
  XMForm(const char *n, Widget parent, 
	 ArgList l=NULL, Cardinal num_args = 0);
  XMForm(const char *n, XMWidget &parent, 
	 ArgList l = NULL, Cardinal num_args = 0);
  
  /* Methods  which  set attributes of the form as a whole */

  void SetFractionBase(int n);
  void SetHorizontalSpacing(Dimension n);
  void SetRubberPositioning(Boolean on);
  void SetVerticalSpacing(Dimension n);

  /* Constraint methods.  */

  void SetBottomAttachment(XMWidget &wid, unsigned char method);
  void SetBottomAttachment(Widget wid, unsigned char method);
  void SetBottomOffset(XMWidget &wid, int off);
  void SetBottomPosition(XMWidget &wid, int position);
  void SetBottomWidget(XMWidget &wid, XMWidget &bound);
  void SetBottomWidget(XMWidget &wid, Widget bound);

  void SetLeftAttachment(XMWidget &wid, unsigned char method);
  void SetLeftAttachment(Widget wid, unsigned char method);
  void SetLeftOffset(XMWidget &wid, int offset);
  void SetLeftPosition(XMWidget &wid, int position);
  void SetLeftWidget(XMWidget &wid, XMWidget &bound);
  void SetLeftWidget(XMWidget &wid, Widget bound);

  void SetRightAttachment(XMWidget &wid, unsigned char method);
  void SetRightAttachment(Widget wid, unsigned char method);
  void SetRightOffset(XMWidget &wid, int offset);
  void SetRightPosition(XMWidget &wid, int position);
  void SetRightWidget(XMWidget &wid, XMWidget &bound);
  void SetRightWidget(XMWidget &wid, Widget bound);

  void SetTopAttachment(XMWidget &wid, unsigned char method);
  void SetTopAttachment(Widget wid, unsigned char method);
  void SetTopOffset(XMWidget &wid, int offset);
  void SetTopPosition(XMWidget &wid, int position);
  void SetTopWidget(XMWidget &wid, XMWidget &bound);
  void SetTopWidget(XMWidget &wid, Widget bound);
  void SetTopWidget(Widget wid, Widget bound);
  void SetTopWidget(Widget wid, XMWidget &bound);
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

  XMRowColumn(const char *name);
  XMRowColumn(Widget id);
  XMRowColumn(const char *name, XMApplication &parent, 
	      ArgList l = NULL, Cardinal num_args = 0);
  XMRowColumn(const char *name, XMWidget &parent,
	      ArgList l = NULL, Cardinal num_args = 0);
  XMRowColumn(const char *name, Widget parent,
	      ArgList l = NULL, Cardinal num_args = 0);

  /* Methods to set the major attributes of a row column widget */

  void SetAlignment(unsigned char alignment);
  void SetEntryClass(WidgetClass type);
  void Align(Boolean align);
  void SetHomogenous(Boolean homog);
  void Margins(Dimension vertical, Dimension horizontal);

  void SetRowColumns(int numcols);
  void SetOrientation(unsigned char orientation);
  void SetPacking(unsigned char packmethod);
  void RadioMenu();      /* Request toggles be radio buttons */
  void RadioForceOne();
  void NoRadioMenu();
  void RadioNoForceOne();
  void SetSpacing(Dimension spacing);

  
  /* Callback methods: */

  Callback_data *AddEntryCallback(void (*callback)(XMWidget *,XtPointer, XtPointer),
			XtPointer client_data);
};

class XMPanedWindow : public XMManager {
 public:
  /* Constructors: */
  XMPanedWindow(const char *name);
  XMPanedWindow(Widget id);
  XMPanedWindow(const char *n, XMWidget &parent,
		 ArgList l = NULL, Cardinal num_args = 0);
  XMPanedWindow(const char *n, Widget parent,
		ArgList l = NULL, Cardinal num_args = 0);

  /* Set attributes of the manager itself. */

  void Margins(Dimension vert, Dimension hor);
  void SetSashHeight(Dimension height);
  void SetSashIndent(Position indent);
  void SetSeparator(Boolean enabled);
  void SetPaneSpacing(Dimension spacing);

  /* Constraint setting methods            */

  void AllowResize(XMWidget &wid, Boolean allow);
  void MaxPaneSize(XMWidget &wid, Dimension max);
  void MinPaneSize(XMWidget &wid, Dimension min);
  void SkipAdjust(XMWidget &wid, Boolean skip);
};
/*
** The Main window while not in the Manager classes discussed in O'Reilly's
** is actually a manager widget which is subclassed from the Manager widget
** class.  It managers a fixed set of subwidgets in an organization which
** is similar to that of a Paned Window.
*/
class XMMainWindow : public XMManager {
 public:
  XMMainWindow(const char *name);
  XMMainWindow(Widget id);
  XMMainWindow(const char *name, XMApplication &parent,
	       ArgList l = NULL, Cardinal arg_count = 0);
  XMMainWindow(const char *name, XMWidget &parent, 
	       ArgList l = NULL, Cardinal arg_count  = 0);
  XMMainWindow(const char *name, Widget &parent,
	       ArgList l = NULL, Cardinal arg_count = 0);

  /*  Modify attributes of the main window: */

  void SetAreas(XMWidget *menubar = NULL, XMWidget *commandwindow = NULL,
		XMWidget *horizontalscroller = NULL,
		XMWidget *verticalscroller = NULL,
		XMWidget *workregion = NULL, XMWidget *messagewidget = NULL);

  void ShowSeparator(Boolean show);
  void CommandWindowLocation(unsigned char where);
};
#endif
