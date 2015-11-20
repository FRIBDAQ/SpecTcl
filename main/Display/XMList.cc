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

XMListBaseClass::XMListBaseClass(const char *n, WidgetClass cl, XMWidget &parent, 
				 ArgList l, Cardinal num_args) :
  XMWidget(n, cl, parent, l, num_args) 
{}

XMListBaseClass:: XMListBaseClass(const char *n, WidgetClass cl, Widget parent, 
				  ArgList l, Cardinal num_args) :
  XMWidget(n, cl, parent, l, num_args) 
{}

XMListBaseClass::XMListBaseClass(const char *n)  : 
  XMWidget(n)
{}

XMListBaseClass::XMListBaseClass(Widget w) : 
  XMWidget(w)
{}

void
XMListBaseClass::AutoSelect(Boolean enable) 
{
  SetAttribute(XmNautomaticSelection, enable);
}

void
XMListBaseClass::SetDoubleClickTime(int ms) 
{
  SetAttribute(XmNdoubleClickInterval, ms);
}

void
XMListBaseClass::SetRows(int rows) 
{
  SetAttribute(XmNvisibleItemCount, rows);
}

void
XMListBaseClass::SetScrollPolicy(int policy) 
{
  SetAttribute(XmNscrollBarDisplayPolicy, policy);
}

void
XMListBaseClass::SetSelectionPolicy(int policy)
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
					    XtPointer user_data)
{
  return AddCallback(XmNbrowseSelectionCallback, callback, user_data);
}

Callback_data*
XMListBaseClass::AddDefaultActionCallback(void (*callback)(XMWidget *,
							   XtPointer,
							   XtPointer),
					  XtPointer user_data )
{
  return AddCallback(XmNdefaultActionCallback, callback, user_data);
}

Callback_data*
XMListBaseClass::AddExtendedSelectionCallback(void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer),
					      XtPointer user_data )
{
  return AddCallback(XmNextendedSelectionCallback, callback, user_data);
}

Callback_data*
XMListBaseClass::AddMultipleSelectionCallback(void (*callback)(XMWidget *,
							       XtPointer,
							       XtPointer),
					      XtPointer user_data)
{
  return AddCallback(XmNmultipleSelectionCallback, callback, user_data);
}

Callback_data*
XMListBaseClass::AddSingleSelectionCallback(void (*callback)(XMWidget *,
							     XtPointer ,
							     XtPointer),
					    XtPointer user_data )
{
  return AddCallback(XmNsingleSelectionCallback, callback, user_data);
}

void
XMListBaseClass::AddItem(const char *item, int position) 
{
  XmString s = XmStringCreateLtoR(const_cast<char*>(item), 
				  const_cast<char*>(XmSTRING_DEFAULT_CHARSET));
  XmListAddItem(id, s, position);
  XmStringFree(s);
}

void
XMListBaseClass::ClearItems() { XmListDeleteAllItems(id); }

void
XMListBaseClass::DeleteItem(const char *item) 
{
  XmString s = XmStringCreateLtoR(const_cast<char*>(item), 
				  const_cast<char*>(XmSTRING_DEFAULT_CHARSET));
  XmListDeleteItem(id, s);
  XmStringFree(s);
}

void
XMListBaseClass::DeleteItem(int loc ) {
  XmListDeletePos(id, loc);
}

void
XMListBaseClass::DeleteItems(int loc, int count ) {
  XmListDeleteItemsPos(id, count, loc);
}

void
XMListBaseClass::DeselectAll() {
  XmListDeselectAllItems(id);
}

void
XMListBaseClass::DeselectItem(const char *item) {
  XmString s = XmStringCreateLtoR(const_cast<char*>(item), 
				  const_cast<char*>(XmSTRING_DEFAULT_CHARSET));
  XmListDeselectItem(id, s);
  XmStringFree(s);
}

void
XMListBaseClass::DeselectItem(int pos ) {
  XmListDeselectPos(id, pos); 
}

void
XMListBaseClass::SetBottomItem(int position ) {
  XmListSetBottomPos(id, position);
}

void
XMListBaseClass::SelectItem(int pos ) {
  XmListSelectPos(id, pos, False);
}


/*
** These files are implemented for the class XMList
*/

XMList::XMList(const char *n, XMWidget &parent, int rows ,
	       ArgList args, Cardinal arg_count ) :
  XMListBaseClass(n, xmListWidgetClass, parent, args, arg_count) {
  SetRows(rows);
  Manage();
}

XMList::XMList(const char *n, Widget parent, int rows ,
	       ArgList args, Cardinal arg_count) :
  XMListBaseClass(n, xmListWidgetClass, parent, args, arg_count) {
  SetRows(rows);
  Manage();
}

XMList::XMList(const char *n) : XMListBaseClass(n) { Manage();}

XMList::XMList(Widget w) : XMListBaseClass(w) { Manage(); }


/* 
** These functions are implementations for the class XMScrolledList
*/

XMScrolledList::XMScrolledList(const char *n, XMWidget &parent, int rows ,
			       ArgList args , Cardinal arg_count) :
  XMListBaseClass(n) {	/* Cheat. */
  id = XmCreateScrolledList(parent.getid(), 
			    const_cast<char*>(n), args, arg_count);
  scrolled_widget = XtParent(id);
  SetRows(rows);
  Manage();
  XtManageChild(scrolled_widget);
}

XMScrolledList::XMScrolledList(const char *n, Widget parent, int rows ,
			       ArgList args , Cardinal arg_count ) :
  XMListBaseClass(n)  { /* Cheat. */
  id = XmCreateScrolledList(parent, const_cast<char*>(n), args, arg_count);
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

