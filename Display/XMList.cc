/*
  This file contains implementations of functions declared in the file 
  XMList.h. They are not inlined because that causes problems with Tru64.

  Author:
    Jason Venema
    NSCL
    Michigan State University
    East Lansing, MI 48824
    mailto:venema@nscl.msu.edu
*/

#include "XMList.h"

XMListBaseClass::XMListBaseClass(char *n, WidgetClass cl, XMWidget &parent, 
				 ArgList l = NULL, Cardinal num_args = 0) :
  XMWidget(n, cl, parent, l, num_args) 
{}

XMListBaseClass:: XMListBaseClass(char *n, WidgetClass cl, Widget parent, 
				  ArgList l = NULL, Cardinal num_args = 0) :
  XMWidget(n, cl, parent, l, num_args) 
{}

XMListBaseClass::XMListBaseClass(char *n)  : 
  XMWidget(n)
{}

XMListBaseClass::XMListBaseClass(Widget w) : 
  XMWidget(w)
{}

void
XMListBaseClass::AutoSelect(Boolean enable=True) 
{
  SetAttribute(XmNautomaticSelection, enable);
}

void
XMListBaseClass::SetDoubleClickTime(int ms = 100) 
{
  SetAttribute(XmNdoubleClickInterval, ms);
}

void
XMListBaseClass::SetRows(int rows) 
{
  SetAttribute(XmNvisibleItemCount, rows);
}

void
XMListBaseClass::SetScrollPolicy(int policy = XmAS_NEEDED) 
{
  SetAttribute(XmNscrollBarDisplayPolicy, policy);
}

void
XMListBaseClass::SetSelectionPolicy(int policy = XmSINGLE_SELECT)
{
  SetAttribute(XmNselectionPolicy, policy);
}

int
XMListBaseClass::GetListCount() 
{
  int cnt;
  GetAttribute(XmNitemCount, &cnt);
  return cnt;
}

XmStringTable
XMListBaseClass::GetListValues() 
{
  XmStringTable list;
  GetAttribute(XmNitems, &list);
  return list;
}

int
XMListBaseClass::GetSelectedListCount() 
{
  int cnt;
  GetAttribute(XmNselectedItemCount, &cnt);
  return cnt;
}

XmStringTable
XMListBaseClass::GetSelectedItems() 
{
  XmStringTable list;
  GetAttribute(XmNselectedItems, &list);
  return list;
}

Callback_data*
XMListBaseClass::AddbrowseSelectionCallback(void (*callback)(XMWidget *,
							     XtPointer,
							     XtPointer),
					    XtPointer user_data = NULL)
{
  return AddCallback(XmNbrowseSelectionCallback, callback, user_data);
}

Callback_data*
XMListBaseClass::AddDefaultActionCallback(void (*callback)(XMWidget *,
							   XtPointer,
							   XtPointer),
					  XtPointer user_data =NULL)
{
  return AddCallback(XmNdefaultActionCallback, callback, user_data);
}

Callback_data*
XMListBaseClass::AddExtendedSelectionCallback(void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer),
					      XtPointer user_data = NULL)
{
  return AddCallback(XmNextendedSelectionCallback, callback, user_data);
}

Callback_data*
XMListBaseClass::AddMultipleSelectionCallback(void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer),
					      XtPointer user_data = NULL)
{
  return AddCallback(XmNmultipleSelectionCallback, callback, user_data);
}

Callback_data*
XMListBaseClass::AddSingleSelectionCallback(void (*callback)(XMWidget *,
							     XtPointer ,
							     XtPointer),
					    XtPointer user_data = NULL)
{
  return AddCallback(XmNsingleSelectionCallback, callback, user_data);
}

void
XMListBaseClass::AddItem(char *item, int position = 0) 
{
  XmString s = XmStringCreateLtoR(item, XmSTRING_DEFAULT_CHARSET);
  XmListAddItem(id, s, position);
  XmStringFree(s);
}

void
XMListBaseClass::ClearItems() { XmListDeleteAllItems(id); }

void
XMListBaseClass::DeleteItem(char *item) 
{
  XmString s = XmStringCreateLtoR(item, XmSTRING_DEFAULT_CHARSET);
  XmListDeleteItem(id, s);
  XmStringFree(s);
}

void
XMListBaseClass::DeleteItem(int loc = 0) {
  XmListDeletePos(id, loc);
}

void
XMListBaseClass::DeleteItems(int loc, int count = 1) {
  XmListDeleteItemsPos(id, count, loc);
}

void
XMListBaseClass::DeselectAll() {
  XmListDeselectAllItems(id);
}

void
XMListBaseClass::DeselectItem(char *item) {
  XmString s = XmStringCreateLtoR(item, XmSTRING_DEFAULT_CHARSET);
  XmListDeselectItem(id, s);
  XmStringFree(s);
}

void
XMListBaseClass::DeselectItem(int pos = 0) {
  XmListDeselectPos(id, pos); 
}

void
XMListBaseClass::SetBottomItem(int position = 0) {
  XmListSetBottomPos(id, position);
}

void
XMListBaseClass::SelectItem(int pos = 0) {
  XmListSelectPos(id, pos, False);
}


/*
** These files are implemented for the class XMList
*/

XMList::XMList(char *n, XMWidget &parent, int rows = 10,
	       ArgList args = NULL, Cardinal arg_count = 0) :
  XMListBaseClass(n, xmListWidgetClass, parent, args, arg_count) {
  SetRows(rows);
  Manage();
}

XMList::XMList(char *n, Widget parent, int rows = 10,
	       ArgList args =NULL, Cardinal arg_count = 0) :
  XMListBaseClass(n, xmListWidgetClass, parent, args, arg_count) {
  SetRows(rows);
  Manage();
}

XMList::XMList(char *n) : XMListBaseClass(n) { Manage();}

XMList::XMList(Widget w) : XMListBaseClass(w) { Manage(); }


/* 
** These functions are implementations for the class XMScrolledList
*/

XMScrolledList::XMScrolledList(char *n, XMWidget &parent, int rows = 10,
			       ArgList args = NULL, Cardinal arg_count = 0) :
  XMListBaseClass(n) {	/* Cheat. */
  id = XmCreateScrolledList(parent.getid(), 
			    n, args, arg_count);
  scrolled_widget = XtParent(id);
  SetRows(rows);
  Manage();
  XtManageChild(scrolled_widget);
}

XMScrolledList::XMScrolledList(char *n, Widget parent, int rows = 10,
			       ArgList args = NULL, Cardinal arg_count = 0) :
  XMListBaseClass(n)  { /* Cheat. */
  id = XmCreateScrolledList(parent, n, args, arg_count);
  scrolled_widget = XtParent(id);
  SetRows(rows);
  Manage();
}

Widget
XMScrolledList::GetScrolledWindow() { return scrolled_widget; }

void
XMScrolledList::Manage()   { 
  XtManageChild(id);
  XtManageChild(scrolled_widget);
}

void
XMScrolledList::UnManage() { 
  XtManageChild(id);
  XtUnmanageChild(scrolled_widget); 
}

