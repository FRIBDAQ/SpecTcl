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

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   C++ library support for Motif.
** Abstract:
**   XMManagers.cc  - This file contains code for methods in the manager
**                    class which is not suitable for in-lining.
** Author:
**   Ron FOx
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

/*
** Include files:
*/
#include <config.h>

#include "XMManagers.h"

/*
**  These functions are implemented for the class XMManager
*/

XMManager::XMManager(const char *n) : XMWidget(n) {}

XMManager::XMManager(Widget w) : XMWidget(w) {}

XMManager::XMManager(const char *n,
		     WidgetClass cl,
		     XMApplication &parent,
		     ArgList l,Cardinal num_args) : 
  XMWidget(n, cl, parent, l, num_args) {}


XMManager::XMManager(const char *n, WidgetClass cl, Widget parent,
		     ArgList l, Cardinal num_args) :
  XMWidget(n, cl, parent, l, num_args) {}

XMManager::XMManager(const char *n, WidgetClass cl, XMWidget &parent,
		     ArgList l, Cardinal num_args) :
  XMWidget(n, cl, parent, l, num_args) { }

Callback_data*
XMManager::AddHelpCallback(void (*callback)(XMWidget *, XtPointer, XtPointer),
			   XtPointer client_data) {
  return AddCallback(XmNhelpCallback, callback, client_data);
}

void
XMManager::SetNavigationType(XmNavigationType navtype) {
  SetAttribute(XmNnavigationType, navtype);
}

void
XMManager::TraversalOn() {
  SetAttribute(XmNtraversalOn, True);
}

void
XMManager::TraverseOff() {
  SetAttribute(XmNtraversalOn, (XtArgVal)False);
}

void
XMManager::SetConstraint(XMWidget &w, String constraint, XtArgVal value) {
  w.SetAttribute(constraint, value);
}

void
XMManager::SetConstraint(XMWidget &w, String constraint, void *value) {
  w.SetAttribute(constraint, value);
}

void
XMManager::SetConstraint(Widget w, String constraint, XtArgVal value) {
  XtVaSetValues(w, constraint, value, NULL);
}

void
XMManager::SetConstraint(Widget w, String constraint, void *value) {
  XtVaSetValues(w, constraint, value, NULL);
}

/*
** These functions are implementations for class XMBulletinBoard
*/

XMBulletinBoard::XMBulletinBoard(const char *n, WidgetClass cl, Widget parent,
				 ArgList l, Cardinal num_args) : 
  XMManager(n, cl, parent, l, num_args) 
{}

XMBulletinBoard::XMBulletinBoard(const char *n) : XMManager(n) {}

XMBulletinBoard::XMBulletinBoard(Widget w): XMManager(w) {}

XMBulletinBoard::XMBulletinBoard(const char *n, XMApplication &parent,
				 ArgList l, Cardinal num_args) :
  XMManager(n, xmBulletinBoardWidgetClass, parent, l, num_args) {}

XMBulletinBoard::XMBulletinBoard(const char *n, Widget parent, 
				 ArgList l, Cardinal num_args) : 
  XMManager(n, xmBulletinBoardWidgetClass, parent, l, num_args) {}

XMBulletinBoard::XMBulletinBoard(const char *n, XMWidget &parent,
				 ArgList l, Cardinal num_args) : 
  XMManager(n, xmBulletinBoardWidgetClass, parent, l, num_args) {}

void
XMBulletinBoard::AllowOverlap(Boolean allow) {
  SetAttribute(XmNallowOverlap, allow);
}

void
XMBulletinBoard::Margins(Dimension ymargin, Dimension xmargin) {
  SetAttribute(XmNmarginHeight, ymargin);
  SetAttribute(XmNmarginWidth, xmargin);
}

void
XMBulletinBoard::SetShadowType(unsigned char type) {
  SetAttribute(XmNshadowType, type);
}

void
XMBulletinBoard::SetAbsPosition(XMWidget &w, int x, int y) {
  SetConstraint(w, XmNx, x);
  SetConstraint(w, XmNy, y);
}


/*
** These functions implement those in class XMFrame
*/

XMFrame::XMFrame(const char *name) : XMManager(name) {}

XMFrame::XMFrame(Widget id)  : XMManager(id)   {}

XMFrame::XMFrame(const char *n, XMApplication &parent, 
		 ArgList l, Cardinal num_args) : 
  XMManager(n, xmFrameWidgetClass, parent, l, num_args) {}

XMFrame::XMFrame(const char *n, XMWidget &parent,
		 ArgList l, Cardinal num_args) :
  XMManager(n, xmFrameWidgetClass, parent, l, num_args) {}

XMFrame::XMFrame(const char *n, Widget parent, ArgList l, 
		 Cardinal num_args) :
  XMManager(n, xmFrameWidgetClass, parent, l, num_args) {}

void
XMFrame::Margins(Dimension height, Dimension width) {
  SetAttribute(XmNmarginWidth, width);
  SetAttribute(XmNmarginHeight, height);
}

void
XMFrame::SetShadowType(unsigned char type) {
  SetAttribute(XmNshadowType, type);
}


/*
** These functions are implemented for class XMForm
*/

XMForm::XMForm(const char *name) : XMBulletinBoard(name) {}

XMForm::XMForm(Widget id)  : XMBulletinBoard(id)   {}

XMForm::XMForm(const char *n, XMApplication &parent, 
	       ArgList l, Cardinal num_args) :
  XMBulletinBoard(n, xmFormWidgetClass, parent.getid(), l, num_args)
{}

XMForm::XMForm(const char *n, Widget parent, 
	       ArgList l, Cardinal num_args) :
  XMBulletinBoard(n, xmFormWidgetClass, parent, l, num_args)
{}

XMForm::XMForm(const char *n, XMWidget &parent, 
	       ArgList l, Cardinal num_args) :
  XMBulletinBoard(n, xmFormWidgetClass, parent.getid(), l, num_args) 
{}

void
XMForm::SetFractionBase(int n) {
  SetAttribute(XmNfractionBase, n);
}

void
XMForm::SetHorizontalSpacing(Dimension n) {
  SetAttribute(XmNhorizontalSpacing, n);
}

void
XMForm::SetRubberPositioning(Boolean on) {
  SetAttribute(XmNrubberPositioning, on);
}

void
XMForm::SetVerticalSpacing(Dimension n) {
  SetAttribute(XmNverticalSpacing, n);
}

void
XMForm::SetBottomAttachment(XMWidget &wid, unsigned char method) {
  SetConstraint(wid, XmNbottomAttachment, method);
}

void
XMForm::SetBottomAttachment(Widget wid, unsigned char method) {
  SetConstraint(wid, XmNbottomAttachment, method);
}

void
XMForm::SetBottomOffset(XMWidget &wid, int off) {
  SetConstraint(wid, XmNbottomOffset, off);
}

void
XMForm::SetBottomPosition(XMWidget &wid, int position) {
  SetConstraint(wid, XmNbottomPosition, position);
}

void
XMForm::SetBottomWidget(XMWidget &wid, XMWidget &bound) {
  SetConstraint(wid, XmNbottomWidget, bound.getid());
}

void
XMForm::SetBottomWidget(XMWidget &wid, Widget bound) {
  SetConstraint(wid, XmNbottomWidget, bound);
}

void
XMForm::SetLeftAttachment(XMWidget &wid, unsigned char method) {
  SetConstraint(wid, XmNleftAttachment, method);
}

void
XMForm::SetLeftAttachment(Widget wid, unsigned char method) {
  SetConstraint(wid, XmNleftAttachment, method);
}

void
XMForm::SetLeftOffset(XMWidget &wid, int offset) {
  SetConstraint(wid, XmNleftOffset, offset);
}

void
XMForm::SetLeftPosition(XMWidget &wid, int position) {
  SetConstraint(wid, XmNleftPosition, position);
}

void
XMForm::SetLeftWidget(XMWidget &wid, XMWidget &bound) {
  SetConstraint(wid, XmNleftWidget, bound.getid());
}

void
XMForm::SetLeftWidget(XMWidget &wid, Widget bound) {
  SetConstraint(wid, XmNleftWidget, bound);
}

void
XMForm::SetRightAttachment(XMWidget &wid, unsigned char method) {
  SetConstraint(wid, XmNrightAttachment, method);
}

void
XMForm::SetRightAttachment(Widget wid, unsigned char method) {
  SetConstraint(wid, XmNrightAttachment, method);
}

void
XMForm::SetRightOffset(XMWidget &wid, int offset) {
  SetConstraint(wid, XmNrightOffset, offset);
}

void
XMForm::SetRightPosition(XMWidget &wid, int position) {
  SetConstraint(wid, XmNrightPosition, position);
}

void
XMForm::SetRightWidget(XMWidget &wid, XMWidget &bound) {
  SetConstraint(wid, XmNrightWidget, bound.getid());
}

void
XMForm::SetRightWidget(XMWidget &wid, Widget bound) {
  SetConstraint(wid, XmNrightWidget, bound);
}

void
XMForm::SetTopAttachment(XMWidget &wid, unsigned char method) {
  SetConstraint(wid, XmNtopAttachment, method);
}

void
XMForm::SetTopAttachment(Widget wid, unsigned char method) {
  SetConstraint(wid, XmNtopAttachment, method);
}

void
XMForm::SetTopOffset(XMWidget &wid, int offset) {
  SetConstraint(wid, XmNtopOffset, offset);
}

void
XMForm::SetTopPosition(XMWidget &wid, int position) {
  SetConstraint(wid, XmNtopPosition, position);
}

void
XMForm::SetTopWidget(XMWidget &wid, XMWidget &bound) {
  SetConstraint(wid, XmNtopWidget, bound.getid());
}

void
XMForm::SetTopWidget(XMWidget &wid, Widget bound) {
  SetConstraint(wid, XmNtopWidget, bound);
}

void
XMForm::SetTopWidget(Widget wid, Widget bound) {
  SetConstraint(wid, XmNtopWidget, bound);
}

void
XMForm::SetTopWidget(Widget wid, XMWidget &bound) {
  SetConstraint(wid, XmNtopWidget, bound.getid());
}






/*
** These are implementations of functions from class XMRowColumn
*/

XMRowColumn::XMRowColumn(const char *name) : XMManager(name) {}

XMRowColumn::XMRowColumn(Widget id)  : XMManager(id)   {}

XMRowColumn::XMRowColumn(const char *name, XMApplication &parent, 
			 ArgList l, Cardinal num_args) :
  XMManager(name, xmRowColumnWidgetClass, parent, l, num_args) {}

XMRowColumn::XMRowColumn(const char *name, XMWidget &parent,
			 ArgList l, Cardinal num_args) :
  XMManager(name, xmRowColumnWidgetClass, parent, l, num_args) {}

XMRowColumn::XMRowColumn(const char *name, Widget parent,
			 ArgList l, Cardinal num_args) :
  XMManager(name, xmRowColumnWidgetClass, parent, l, num_args) {}

void
XMRowColumn::SetAlignment(unsigned char alignment) {
  SetAttribute(XmNentryAlignment, alignment);
}

void
XMRowColumn::SetEntryClass(WidgetClass type) {
  SetAttribute(XmNentryClass, type);
}

void
XMRowColumn::Align(Boolean align) {
  SetAttribute(XmNisAligned, align);
}

void
XMRowColumn::SetHomogenous(Boolean homog) {
  SetAttribute(XmNisHomogeneous, homog);
}

void
XMRowColumn::Margins(Dimension vertical, Dimension horizontal) {
  SetAttribute(XmNmarginHeight, vertical);
  SetAttribute(XmNmarginWidth, horizontal);
}

void
XMRowColumn::SetRowColumns(int numcols) {
  SetAttribute(XmNnumColumns, numcols);
}

void
XMRowColumn::SetOrientation(unsigned char orientation) {
  SetAttribute(XmNorientation, orientation);
}

void
XMRowColumn::SetPacking(unsigned char packmethod) {
  SetAttribute(XmNpacking, packmethod);
}

void
XMRowColumn::RadioMenu()      /* Request toggles be radio buttons */
{ SetAttribute(XmNradioBehavior, (XtArgVal)True); }

void
XMRowColumn::RadioForceOne() {
  SetAttribute(XmNradioAlwaysOne, (XtArgVal)True); }

void
XMRowColumn::NoRadioMenu() {
  SetAttribute(XmNradioBehavior, (XtArgVal)False); }

void
XMRowColumn::RadioNoForceOne() {
  SetAttribute(XmNradioAlwaysOne, (XtArgVal)False); }

void
XMRowColumn::SetSpacing(Dimension spacing) {
  SetAttribute(XmNspacing, spacing);
}

Callback_data*
XMRowColumn::AddEntryCallback(void(*callback)(XMWidget *,XtPointer, XtPointer),
			      XtPointer client_data) {
  return AddCallback(XmNentryCallback, callback, client_data);
}

/*
** The following are implementations of functions for class XMPanedWindow
*/

XMPanedWindow::XMPanedWindow(const char *name) : XMManager(name) {}

XMPanedWindow::XMPanedWindow(Widget id)  : XMManager(id)   {}

XMPanedWindow::XMPanedWindow(const char *n, XMWidget &parent,
			     ArgList l, Cardinal num_args) :
  XMManager(n, xmPanedWindowWidgetClass, parent, l, num_args) {}

XMPanedWindow::XMPanedWindow(const char *n, Widget parent,
			     ArgList l, Cardinal num_args) :
  XMManager(n, xmPanedWindowWidgetClass, parent, l, num_args) {}

void
XMPanedWindow::Margins(Dimension vert, Dimension hor) {
  SetAttribute(XmNmarginHeight, vert);
  SetAttribute(XmNmarginWidth, hor);
}

void
XMPanedWindow::SetSashHeight(Dimension height) {
  SetAttribute(XmNsashHeight, height);
}

void
XMPanedWindow::SetSashIndent(Position indent) {
  SetAttribute(XmNsashIndent, indent);
}

void
XMPanedWindow::SetSeparator(Boolean enabled) {
  SetAttribute(XmNseparatorOn, enabled);
}

void
XMPanedWindow::SetPaneSpacing(Dimension spacing) {
  SetAttribute(XmNspacing, spacing);
}

void
XMPanedWindow::AllowResize(XMWidget &wid, Boolean allow) {
  wid.SetAttribute(XmNallowResize, allow);
}

void
XMPanedWindow::MaxPaneSize(XMWidget &wid, Dimension max) {
  wid.SetAttribute(XmNpaneMaximum, max);
}

void
XMPanedWindow::MinPaneSize(XMWidget &wid, Dimension min) {
  wid.SetAttribute(XmNpaneMinimum, min);
}

void
XMPanedWindow::SkipAdjust(XMWidget &wid, Boolean skip) {
  wid.SetAttribute(XmNskipAdjust, skip);
}

/*
** The following are implemented for class XMMainWindow
*/

XMMainWindow::XMMainWindow(const char *name) : XMManager(name) {}

XMMainWindow::XMMainWindow(Widget id)  : XMManager(id  ) {}

XMMainWindow::XMMainWindow(const char *name, XMApplication &parent,
			   ArgList l, Cardinal arg_count) :
  XMManager(name, xmMainWindowWidgetClass, parent, l, arg_count) {}

XMMainWindow::XMMainWindow(const char *name, XMWidget &parent, 
			   ArgList l, Cardinal arg_count) :
  XMManager(name, xmMainWindowWidgetClass, parent, l, arg_count) {}

XMMainWindow::XMMainWindow(const char *name, Widget &parent,
			   ArgList l, Cardinal arg_count) :
  XMManager(name, xmMainWindowWidgetClass, parent, l, arg_count) {}

void
XMMainWindow::ShowSeparator(Boolean show) {
  SetAttribute(XmNshowSeparator, show);
}

void
XMMainWindow::CommandWindowLocation(unsigned char where) {
  SetAttribute(XmNcommandWindowLocation, where);
}




/*
** Method Description:
**   XMMainWindow::SetAreas
**      This function tells a main window object which widgets are which.
**      The Main Window widget supports the following windows:
**         menubar        -  A menu bar at the top of the window.
**         commandwindow  -  A widget for taking commands.
**         scrollbars     -  Horizontal and vertical scrollbars for the
**                           work region.
**         workregion     -  A region in which real work gets done.
**         message        - A region for displaying messages.
** Formal Parameters:
**    NOTE: Any parameter can be null indicating that there is not a
**          corresponding widget.
**  XMWidget *menubar:
**      The menu bar widget.
**  XMWidget *commandwindow:
**      The window containing command data.
**  XMWidget *verticalscroller:
**  XMWidget *horizontalscroller:
**       The scroll bars for the work region.
*/
void XMMainWindow::SetAreas(XMWidget *menubar, XMWidget *commandwindow,
			    XMWidget *horizontalscroller,
			    XMWidget *verticalscroller,
			    XMWidget *workregion,
			    XMWidget *messagewidget) {
  Widget menu, command, horiz, vertic, work;

  menu = command = horiz = 
    vertic = work = NULL;	/* Assume for now none are present */

  /* Setup for and call the XmMainWindowSetAreas function by getting */
  /* all of the supplied widget ids.                                 */

  if(menubar)            menu     = menubar->getid();
  if(commandwindow)      command  = commandwindow->getid();
  if(horizontalscroller) horiz    = horizontalscroller->getid();
  if(verticalscroller)   vertic   = verticalscroller->getid();
  if(workregion)         work     = workregion->getid();

  XmMainWindowSetAreas(this->getid(), menu, command, horiz, vertic, work);

  /* If necessary, set the XmNmessageWindow resource: */

  if(messagewidget) SetAttribute(XmNmessageWindow, messagewidget->getid());
}


