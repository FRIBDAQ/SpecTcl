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
#ifndef XMSCALE_H
#define XMSCALE_H

#include <Xm/Scale.h>

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

class XMScale : public XMManagedWidget {
 public:

  /* Constructors: */

  XMScale(const char *n, XMWidget &parent, 
	  ArgList args = NULL, Cardinal arg_count = 0);
  XMScale(const char *n, Widget parent,
	  ArgList args = NULL, Cardinal arg_count = 0);
  
  XMScale(const char *n);
  XMScale(Widget w);
  
  /* manipulators:  */
  
  void SetRange(int hi, int lo = 0);
  int Value();
  void Value(int value);
  
  /* Routines to add callbacks:  */
  
  Callback_data *AddDragCallback(void (*callback)(XMWidget *, 
						  XtPointer, XtPointer),
				 XtPointer user_data = NULL);
  Callback_data *AddChangedCallback(void (*callback)(XMWidget *,
						     XtPointer, XtPointer),
				    XtPointer user_data = NULL);
};
#endif
