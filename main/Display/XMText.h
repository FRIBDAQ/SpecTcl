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

#ifndef XMTEXT_H
#define XMTEXT_H

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <string.h>
class XMText : public XMManagedWidget
   {
   public:

     /* Constructors: */

     XMText(const char *n, XMWidget &parent, int rows = 1, int columns = 30,
	    ArgList args = NULL, Cardinal arg_count = 0);
     XMText(const char *n, Widget parent, int rows = 1, int columns = 30,
	    ArgList args = NULL, Cardinal arg_count = 0);

     XMText(const char *n);
     XMText(Widget w);

     /* Set attributes: */

     void SetColumns(int columns);
     void SetRows(int rows);
     void SetMaxLength(int len);
     void EnableWordWrap();
     void DisableWordWrap();
     char *GetText();

     void SetText(const char *txt);
     void SetEditing(Boolean enable);
   };


class XMTextField : public XMManagedWidget
   {
   public:

     /* Constructors: */

     XMTextField(const char *n, XMWidget &parent, int columns = 30,
	    ArgList args = NULL, Cardinal arg_count = 0);
     XMTextField(const char *n, Widget parent,  int columns = 30,
	    ArgList args = NULL, Cardinal arg_count = 0);

     XMTextField(const char *n);
     XMTextField(Widget w);

     /* Set attributes: */

     void SetColumns(int columns);
     void SetMaxLength(int len);
     void EnableWordWrap();
     char *GetText();

     void SetText(const char *txt);
     Callback_data *AddActivateCallback(void (*callback)(XMWidget *, 
							 XtPointer, 
							 XtPointer), 
					XtPointer user_data=NULL);
   };

class XMScrolledText : public XMText {
 protected:
  int    text_length;		/* Keeps track of length of text. */
  int    max_text_length;       /* Longest allowed text           */
  Widget scroller;		/* Scroller widget id.                */
 public:

  /* Constructors: */

  XMScrolledText(const char* n, XMWidget &parent, int rows = 20, int columns = 40,
		 ArgList args = NULL, Cardinal arg_count = 0);
  XMScrolledText(const char* n, Widget parent, int rows = 20, int columns = 40,
		 ArgList args = NULL, Cardinal arg_count = 0);

  /* Manipulators:  */

  void SetMaxLength(int maxlen);
  void ClearText();
  void AddText(const char* string);
  Widget Scroller();
};
#endif





