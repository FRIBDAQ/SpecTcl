/*
** Facility:
**    Xamine C++ support for motif.
** Abstract:
**    XMSCale.h  - This file defines the XMScale widget class.  The XMScale
**                 widget class implements the Motif Scale widget which is
**                 essentially a value slider.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)XMScale.h	8.1 6/23/95 
*/
/*
** Facility:
**    Xamine C++ support for motif.
** Abstract:
**    XMSCale.cc - This file implements the XMScale widget class.  The XMScale
**                 widget class implements the Motif Scale widget which is
**                 essentially a value slider.
** Author:
**   Jason Venema
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)XMScale.h	8.1 6/23/95 
*/
#include <config.h>
#include "XMScale.h"

XMScale::XMScale(const char *n, XMWidget &parent, 
		 ArgList args, Cardinal arg_count) :
  XMManagedWidget(n, xmScaleWidgetClass, parent, args, arg_count)
{}

XMScale::XMScale(const char *n, Widget parent,
		 ArgList args, Cardinal arg_count) : 
  XMManagedWidget(n, xmScaleWidgetClass, parent, args, arg_count) 
{ }

XMScale::XMScale(const char *n) : XMManagedWidget(n) { }

XMScale::XMScale(Widget w): XMManagedWidget(w) { }

void
XMScale::SetRange(int hi, int lo) {
  XtVaSetValues(id, 
		XmNmaximum, hi,
		XmNminimum, lo,
		NULL);
}

int
XMScale::Value() {
  int i;
  XmScaleGetValue(id, &i);
  return i;
}

void
XMScale::Value(int value) {
  XmScaleSetValue(id, value);
}

Callback_data*
XMScale::AddDragCallback(void (*callback)(XMWidget *, 
					  XtPointer, XtPointer),
			 XtPointer user_data) {
  return AddCallback(XmNdragCallback, callback, user_data);
}

Callback_data*
XMScale::AddChangedCallback(void (*callback)(XMWidget *,
					     XtPointer, XtPointer),
			    XtPointer user_data) {
  return AddCallback(XmNvalueChangedCallback, callback, user_data);
}

