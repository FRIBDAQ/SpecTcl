/*
** Facility:
**   Xamine C++ Support for motif.
** Abstract:
**   XMText.h   - This file defines the XMText widget class.  This is
**                a text editing widget.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**   @(#)XMText.h	8.1 6/23/95 
*/

#ifndef _XMTEXT_H
#define _XMTEXT_H
#include "XMWidget.h"
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <string.h>
class XMText : public XMManagedWidget
   {
   public:

     /* Constructors: */

     XMText(char *n, XMWidget &parent, int rows = 1, int columns = 30,
	    ArgList args = NULL, Cardinal arg_count = 0) :
	      XMManagedWidget(n, xmTextWidgetClass, parent, args, arg_count) {
		SetColumns(columns);
		SetRows(rows);
	      }

     XMText(char *n, Widget parent, int rows = 1, int columns = 30,
	    ArgList args = NULL, Cardinal arg_count = 0) : 
	      XMManagedWidget(n, xmTextWidgetClass, parent) {
		SetColumns(columns);
		SetRows(rows);
	      }

     XMText(char *n) : XMManagedWidget(n) { }
     XMText(Widget w): XMManagedWidget(w) { }

     /* Set attributes: */

     void SetColumns(int columns) {
       SetAttribute(XmNcolumns, columns);
     }
     void SetRows(int rows) {
       SetAttribute(XmNrows, rows);
     }
     void SetMaxLength(int len) {
       SetAttribute(XmNmaxLength, len);
     }
     void EnableWordWrap() {
       SetAttribute(XmNwordWrap, True);
     }
     void DisableWordWrap() {
       SetAttribute(XmNwordWrap, (XtArgVal)False);
     }
     char *GetText() { return XmTextGetString(id); }

     void SetText(char *txt) {
       XmTextSetString(id, txt);
     }
     void SetEditing(Boolean enable) {
       SetAttribute(XmNeditable, enable);
     }
   };


class XMTextField : public XMManagedWidget
   {
   public:

     /* Constructors: */

     XMTextField(char *n, XMWidget &parent, int columns = 30,
	    ArgList args = NULL, Cardinal arg_count = 0) :
	      XMManagedWidget(n, xmTextFieldWidgetClass, 
			      parent, args, arg_count) {
		SetColumns(columns);
	      }

     XMTextField(char *n, Widget parent,  int columns = 30,
	    ArgList args = NULL, Cardinal arg_count = 0) : 
	      XMManagedWidget(n, xmTextFieldWidgetClass, parent) {
		SetColumns(columns);
	      }

     XMTextField(char *n) : XMManagedWidget(n) { }
     XMTextField(Widget w): XMManagedWidget(w) { }

     /* Set attributes: */

     void SetColumns(int columns) {
       SetAttribute(XmNcolumns, columns);
     }
     void SetMaxLength(int len) {
       SetAttribute(XmNmaxLength, len);
     }
     void EnableWordWrap() {
       SetAttribute(XmNwordWrap, True);
     }
     char *GetText() { return XmTextFieldGetString(id); }

     void SetText(char *txt) {
       XmTextFieldSetString(id, txt);
     }
     Callback_data *AddActivateCallback(void (*callback)(XMWidget *, 
					       XtPointer, XtPointer), 
			      XtPointer user_data=NULL) {
       return AddCallback(XmNactivateCallback, callback, user_data);
     }
   };

class XMScrolledText : public XMText {
 protected:
  int    text_length;		/* Keeps track of length of text. */
  int    max_text_length;       /* Longest allowed text           */
  Widget scroller;		/* Scroller widget id.                */
 public:

  /* Constructors: */

  XMScrolledText(char *n, XMWidget &parent, int rows = 20, int columns = 40,
		 ArgList args = NULL, Cardinal arg_count = 0) :
     XMText(n) {		/* Use no-op constructor for base class */
       id       = XmCreateScrolledText(parent.getid(), n, args, arg_count);
       scroller = XtParent(id);
       SetColumns(columns);
       SetRows(rows);
       text_length = 0;
       max_text_length = 0;     /* No max length. */
     }

  XMScrolledText(char *n, Widget parent, int rows = 20, int columns = 40,
		 ArgList args = NULL, Cardinal arg_count = 0) :
     XMText(n) {		/* Use no-op constructor for base class */
       id       = XmCreateScrolledText(parent, n, args, arg_count);
       scroller = XtParent(id);
       SetColumns(columns);
       SetRows(rows);
       text_length = 0;
       max_text_length = 0;     /* No max length. */
     }


  /* Manipulators:  */

  void SetMaxLength(int maxlen) { max_text_length = maxlen; }
  void ClearText() {
    text_length = 0;
    XmTextSetString(id, "");	/* Set text value to empty string. */
  }
  void AddText(char *string) {
    XmTextInsert(id, text_length, string);
    text_length += strlen(string);
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
  Widget Scroller() { return scroller; }
};
#endif





