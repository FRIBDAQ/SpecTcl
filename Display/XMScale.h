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
#ifndef _XMSCALE_H_INSTALLED
#define _XMSCALE_H_INSTALLED

#include <Xm/Scale.h>
#include "XMWidget.h"

class XMScale : public XMManagedWidget {
 public:

  /* Constructors: */

  XMScale(char *n, XMWidget &parent, 
	  ArgList args = NULL, Cardinal arg_count = 0) :
	    XMManagedWidget(n, xmScaleWidgetClass, parent, args, arg_count) {
	    }
  
  XMScale(char *n, Widget parent,
	  ArgList args = NULL, Cardinal arg_count = 0) : 
	    XMManagedWidget(n, xmScaleWidgetClass, parent, args, arg_count) {
	    }
  
  XMScale(char *n) : XMManagedWidget(n) { }
  XMScale(Widget w): XMManagedWidget(w) { }
  
  /* manipulators:  */
  
  void SetRange(int hi, int lo = 0) {
    XtVaSetValues(id, 
		  XmNmaximum, hi,
		  XmNminimum, lo,
		  NULL);
  }
  int Value() {
    int i;
    XmScaleGetValue(id, &i);
    return i;
  }
  void Value(int value) {
    XmScaleSetValue(id, value);
  }
  
  /* Routines to add callbacks:  */
  
  Callback_data *AddDragCallback(void (*callback)(XMWidget *, 
						  XtPointer, XtPointer),
				 XtPointer user_data = NULL) {
    return AddCallback(XmNdragCallback, callback, user_data);
  }
  Callback_data *AddChangedCallback(void (*callback)(XMWidget *,
						     XtPointer, XtPointer),
				    XtPointer user_data = NULL) {
    return AddCallback(XmNvalueChangedCallback, callback, user_data);
  }
  
};
#endif

