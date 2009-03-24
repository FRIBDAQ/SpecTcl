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
*//*
** Facility:
**   Xamine -- NSCL display program C++ widget support.
** Abstract:
**   XMList.h:
**     This file provides a class definition for the XMList class.  The
**     XMList class implements list box widget, and a Scrolled list box
**     widget.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS info:
**    @(#)XMList.h	8.1 6/23/95 
*/
#ifndef XMLIST_H
#define XMLIST_H

/*
** Include files:
*/
#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#include <Xm/List.h>
#include <Xm/ScrolledW.h>

/*
**    XMListBaseClass encapsulates all the common behavior of 
**    XmList widgets and XmScrolledList combo widgets.
*/
class XMListBaseClass : public XMWidget {
 public:
  /* Constructors: */

  XMListBaseClass(char *n, WidgetClass cl, XMWidget &parent, 
		  ArgList l = NULL, Cardinal num_args = 0);
  XMListBaseClass(char *n, WidgetClass cl, Widget parent, 
		  ArgList l = NULL, Cardinal num_args = 0);
  XMListBaseClass(char *n);
  XMListBaseClass(Widget w);

  /* Manage the attributes: */
  void AutoSelect(Boolean enable=True);
  void SetDoubleClickTime(int ms = 100);
  void SetRows(int rows);
  void SetScrollPolicy(int policy = XmAS_NEEDED);
  void SetSelectionPolicy(int policy = XmSINGLE_SELECT);

  int GetListCount();
  XmStringTable GetListValues();
  int GetSelectedListCount();
  XmStringTable GetSelectedItems();

  /* Callbacks: */  
  Callback_data *AddbrowseSelectionCallback(void (*callback)(XMWidget *,
							     XtPointer,
							     XtPointer),
					    XtPointer user_data = NULL);
  Callback_data *AddDefaultActionCallback(void (*callback)(XMWidget *,
							   XtPointer,
							   XtPointer),
					  XtPointer user_data =NULL);
  Callback_data *AddExtendedSelectionCallback(void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer),
					      XtPointer user_data = NULL);
  Callback_data *AddMultipleSelectionCallback(void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer),
					      XtPointer user_data = NULL);
  Callback_data *AddSingleSelectionCallback(void (*callback)(XMWidget *,
							     XtPointer ,
							     XtPointer),
					    XtPointer user_data = NULL);

  /*  Behavior that is actually convenience functions for list widget: */

  void AddItem(char *item, int position = 0);
  void ClearItems();
  void DeleteItem(char *item);
  void DeleteItem(int loc = 0);
  void DeleteItems(int loc, int count = 1);
  void DeselectAll();
  void DeselectItem(char *item);
  void DeselectItem(int pos = 0);
  void SetBottomItem(int position = 0);
  void SelectItem(int pos = 0);
};
/*
**  XMList is an unscrolled list.
*/
class XMList : public XMListBaseClass {
 protected:
 public:
  /* Constructors and Destructors: */

  XMList(char *n, XMWidget &parent, int rows = 10,
	 ArgList args = NULL, Cardinal arg_count = 0);
  XMList(char *n, Widget parent, int rows = 10,
	 ArgList args =NULL, Cardinal arg_count = 0);
  XMList(char *n);
  XMList(Widget w);
};
/*
** XMScrolledList  - A scrolled list widget.  This constructs a scrolled
**                   window widget with a list widget inside.  An additional
**                   attribute keeps track of the scrolled window widget.
**                   The management routines (un)manage both.
*/
class XMScrolledList : public XMListBaseClass {
 protected:
  Widget scrolled_widget;

 public:
  XMScrolledList(char *n, XMWidget &parent, int rows = 10,
		 ArgList args = NULL, Cardinal arg_count = 0);
  XMScrolledList(char *n, Widget parent, int rows = 10,
		 ArgList args = NULL, Cardinal arg_count = 0);

  /* Get the scrolled widget id:  */
  Widget GetScrolledWindow();

  /* Manage from scrolled_widget: */

  void Manage();
  void UnManage(); 
};    

#endif
