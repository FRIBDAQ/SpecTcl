/*
** Facility:
**   Xamine -- NSCL display program C++ widget support.
** Abstract:
**   XMList.h:
**     This file provides a classs definition for the XMList class.  The
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
#ifndef XMLIST_H_INSTALLED
#define XMLIST_H_INSTALLED
/*
** Include files:
*/
#include "XMWidget.h"
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
		  ArgList l = NULL, Cardinal num_args = 0) :
		    XMWidget(n, cl, parent, l, num_args) {}
  XMListBaseClass(char *n, WidgetClass cl, Widget parent, 
		  ArgList l = NULL, Cardinal num_args = 0) :
		    XMWidget(n, cl, parent, l, num_args) {}

  XMListBaseClass(char *n)  : XMWidget(n) {}
  XMListBaseClass(Widget w) : XMWidget(w) {}

  /* Manage the attributes: */

  void AutoSelect(Boolean enable=True) {
    SetAttribute(XmNautomaticSelection, enable);
  }
  void SetDoubleClickTime(int ms = 100) {
    SetAttribute(XmNdoubleClickInterval, ms);
  }
  
  void SetRows(int rows) {
    SetAttribute(XmNvisibleItemCount, rows);
  }
  void SetScrollPolicy(int policy = XmAS_NEEDED) {
    SetAttribute(XmNscrollBarDisplayPolicy, policy);
  }
  void SetSelectionPolicy(int policy = XmSINGLE_SELECT) {
    SetAttribute(XmNselectionPolicy, policy);
  }

  int GetListCount() {
    int cnt;
    GetAttribute(XmNitemCount, &cnt);
    return cnt;
  }
  XmStringTable GetListValues() {
    XmStringTable list;
    GetAttribute(XmNitems, &list);
    return list;
  }

  int GetSelectedListCount() {
    int cnt;
    GetAttribute(XmNselectedItemCount, &cnt);
    return cnt;
  }

  XmStringTable GetSelectedItems() {
    XmStringTable list;
    GetAttribute(XmNselectedItems, &list);
    return list;
  }
  /* Callbacks: */
  
  Callback_data *AddbrowseSelectionCallback(void (*callback)(XMWidget *,
							     XtPointer,
							     XtPointer),
					    XtPointer user_data = NULL) {
    return AddCallback(XmNbrowseSelectionCallback, callback, user_data);
  }
  Callback_data *AddDefaultActionCallback(void (*callback)(XMWidget *,
							   XtPointer,
							   XtPointer),
					  XtPointer user_data =NULL)
    {
      return AddCallback(XmNdefaultActionCallback, callback, user_data);
    }
  Callback_data *AddExtendedSelectionCallback(void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer),
					      XtPointer user_data = NULL)
    {
      return AddCallback(XmNextendedSelectionCallback, callback, user_data);
    }
  Callback_data *AddMultipleSelectionCallback(void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer),
					      XtPointer user_data = NULL)
    {
      return AddCallback(XmNmultipleSelectionCallback, callback, user_data);
    }
  Callback_data *AddSingleSelectionCallback(void (*callback)(XMWidget *,
							     XtPointer ,
							     XtPointer),
					    XtPointer user_data = NULL)
    {
      return AddCallback(XmNsingleSelectionCallback, callback, user_data);
    }

  /*  Behavior that is actually convenience functions for list widget: */

  void AddItem(char *item, int position = 0) {
    XmString s = XmStringCreateLtoR(item, XmSTRING_DEFAULT_CHARSET);
    XmListAddItem(id, s, position);
    XmStringFree(s);
  }
  void ClearItems() { XmListDeleteAllItems(id); }
  void DeleteItem(char *item) {
    XmString s = XmStringCreateLtoR(item, XmSTRING_DEFAULT_CHARSET);
    XmListDeleteItem(id, s);
    XmStringFree(s);
  }
  void DeleteItem(int loc = 0) {
    XmListDeletePos(id, loc);
  }
  void DeleteItems(int loc, int count = 1) {
    XmListDeleteItemsPos(id, count, loc);
  }
  void DeselectAll() {
    XmListDeselectAllItems(id);
  }
  void DeselectItem(char *item) {
    XmString s = XmStringCreateLtoR(item, XmSTRING_DEFAULT_CHARSET);
    XmListDeselectItem(id, s);
    XmStringFree(s);

  }
  void DeselectItem(int pos = 0) {
    XmListDeselectPos(id, pos); 
  }
  void SetBottomItem(int position = 0) {
    XmListSetBottomPos(id, position);
  }

  void SelectItem(int pos = 0) {
    XmListSelectPos(id, pos, False);
  }

};
/*
**  XMList is an unscrolled list.
*/
class XMList : public XMListBaseClass {
 protected:
 public:
  /* Constructors and Destructors: */

  XMList(char *n, XMWidget &parent, int rows = 10,
	 ArgList args = NULL, Cardinal arg_count = 0) :
	   XMListBaseClass(n, xmListWidgetClass, parent, args, arg_count) {
	     SetRows(rows);
	     Manage();
	   }
  XMList(char *n, Widget parent, int rows = 10,
	 ArgList args =NULL, Cardinal arg_count = 0) :
	   XMListBaseClass(n, xmListWidgetClass, parent, args, arg_count) {
	     SetRows(rows);
	     Manage();
	   }
  XMList(char *n) : XMListBaseClass(n) { Manage(); }
  XMList(Widget w) : XMListBaseClass(w) { Manage(); }
  
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
		 ArgList args = NULL, Cardinal arg_count = 0) :
		   XMListBaseClass(n) {	/* Cheat. */
		     id = XmCreateScrolledList(parent.getid(), 
					       n, args, arg_count);
		     scrolled_widget = XtParent(id);
		     SetRows(rows);
		     Manage();
		     XtManageChild(scrolled_widget);
		   }
  XMScrolledList(char *n, Widget parent, int rows = 10,
		 ArgList args = NULL, Cardinal arg_count = 0) :
		   XMListBaseClass(n)  { /* Cheat. */
		     id = XmCreateScrolledList(parent, n, args, arg_count);
		     scrolled_widget = XtParent(id);
		     SetRows(rows);
		     Manage();
		     }
  /* Get the scrolled widget id:  */

  Widget GetScrolledWindow() { return scrolled_widget; }

  /* Manage from scrolled_widget: */

  void Manage()   { 
                     XtManageChild(id);
                     XtManageChild(scrolled_widget);
		  }
  void UnManage() { 
                    XtManageChild(id);
                    XtUnmanageChild(scrolled_widget); 
		  }

};    

#endif
