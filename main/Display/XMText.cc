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
**   Xamine C++ Support for motif.
** Abstract:
**   XMText.cc  - This file implements the XMText widget class.  This is
**                a text editing widget. This file exists because inlining
**                these functions caused linking errors on Tru64.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**   @(#)XMText.h	8.1 6/23/95 
*/
#include <config.h>
#include "XMText.h"

XMText::XMText(const char *n, XMWidget &parent, int rows , int columns,
	       ArgList args, Cardinal arg_count) :
  XMManagedWidget(n, xmTextWidgetClass, parent, args, arg_count) {
  SetColumns(columns);
  SetRows(rows);
}

XMText::XMText(const char *n, Widget parent, int rows, int columns,
	       ArgList args, Cardinal arg_count) : 
  XMManagedWidget(n, xmTextWidgetClass, parent) {
  SetColumns(columns);
  SetRows(rows);
}

XMText::XMText(const char *n) : XMManagedWidget(n) { }

XMText::XMText(Widget w): XMManagedWidget(w) { }

void
XMText::SetColumns(int columns) {
  SetAttribute(XmNcolumns, columns);
}

void
XMText::SetRows(int rows) {
  SetAttribute(XmNrows, rows);
}

void
XMText::SetMaxLength(int len) {
  SetAttribute(XmNmaxLength, len);
}

void
XMText::EnableWordWrap() {
  SetAttribute(XmNwordWrap, True);
}

void
XMText::DisableWordWrap() {
  SetAttribute(XmNwordWrap, (XtArgVal)False);
}

char*
XMText::GetText() { return XmTextGetString(id); }

void
XMText::SetText(const char *txt) {
  XmTextSetString(id, const_cast<char*>(txt));
}

void
XMText::SetEditing(Boolean enable) {
  SetAttribute(XmNeditable, enable);
}

/*
** The following functions are implementations for class XMTextField
*/

XMTextField::XMTextField(const char *n, XMWidget &parent, int columns,
			 ArgList args, Cardinal arg_count) :
  XMManagedWidget(n, xmTextFieldWidgetClass, 
		  parent, args, arg_count) {
  SetColumns(columns);
}

XMTextField::XMTextField(const char *n, Widget parent,  int columns,
			 ArgList args, Cardinal arg_count) : 
  XMManagedWidget(n, xmTextFieldWidgetClass, parent) {
  SetColumns(columns);
}

XMTextField::XMTextField(const char *n) : XMManagedWidget(n) { }

XMTextField::XMTextField(Widget w): XMManagedWidget(w) { }

void
XMTextField::SetColumns(int columns) {
  SetAttribute(XmNcolumns, columns);
}

void
XMTextField::SetMaxLength(int len) {
  SetAttribute(XmNmaxLength, len);
}

void
XMTextField::EnableWordWrap() {
  SetAttribute(XmNwordWrap, True);
}

char*
XMTextField::GetText() { return XmTextFieldGetString(id); }

void
XMTextField::SetText(const char *txt) {
  XmTextFieldSetString(id, const_cast<char*>(txt));
}

Callback_data*
XMTextField::AddActivateCallback(void (*callback)(XMWidget *, 
						  XtPointer, XtPointer), 
				 XtPointer user_data) {
  return AddCallback(XmNactivateCallback, callback, user_data);
}

/*
** These implementations are for class XMScrolledText
*/

XMScrolledText::XMScrolledText(const char *n, XMWidget &parent, int rows, 
			       int columns,
			       ArgList args, Cardinal arg_count) :
  XMText(n) {		/* Use no-op constructor for base class */
  id       = XmCreateScrolledText(parent.getid(),  const_cast<char*>(n), args, arg_count);
  scroller = XtParent(id);
  SetColumns(columns);
  SetRows(rows);
  text_length = 0;
  max_text_length = 0;     /* No max length. */
}

XMScrolledText::XMScrolledText(const char *n, Widget parent, int rows, 
			       int columns,
			       ArgList args, Cardinal arg_count) :
  XMText(n) {		/* Use no-op constructor for base class */
  id       = XmCreateScrolledText(parent,  const_cast<char*>(n), args, arg_count);
  scroller = XtParent(id);
  SetColumns(columns);
  SetRows(rows);
  text_length = 0;
  max_text_length = 0;     /* No max length. */
}

void
XMScrolledText::SetMaxLength(int maxlen) { max_text_length = maxlen; }

void
XMScrolledText::ClearText() {
  text_length = 0;
  XmTextSetString(id, 
		  const_cast<char*>(""));	/* Set text value to empty string. */
}

void
XMScrolledText:: AddText(const char *pstring) {
  XmTextInsert(id, text_length,  const_cast<char*>(pstring));
  text_length += strlen(pstring);
  if( (max_text_length > 0) && (text_length > max_text_length)) {
    int rmlen;
    rmlen = text_length - max_text_length;
    char *st = XmTextGetString(id);
    XmTextSetString(id, &st[rmlen]);
    text_length -= rmlen;
    XtFree(st);
  }
  XmTextSetInsertionPosition(id, text_length);
}

Widget
XMScrolledText::Scroller() { return scroller; }
