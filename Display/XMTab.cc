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


#include <config.h>
#include "XMTab.h"

/*
  This file implements those functions found in class XMTab in file
  XMTab.h. This file exists because inlining these functions caused 
  linking to crash on Tru64

  Author:
    Jason Venema
    NSCL
    Michigan State University
    East Lansing, MI  48824
    mailto:venema@nscl.msu.edu
*/


XMTab::XMTab(const char *n, XMWidget &parent, 
	     ArgList args, Cardinal arg_count) :
  XMManagedWidget(n, xmTabWidgetClass, parent, args, arg_count) 
{ }

XMTab::XMTab(const char *n, Widget parent, char *text,
	     ArgList args, Cardinal arg_count) :
  XMManagedWidget(n, xmLabelWidgetClass, parent, args, arg_count) 
{ }

XMTab::XMTab(const char *n) : XMManagedWidget(n)
{ }

XMTab::XMTab(Widget w) : XMManagedWidget(w) 
{ }

Callback_data*
XMTab::AddCallback(void (*cb)(XMWidget *, XtPointer, XtPointer), 
		   XtPointer cd)
{ 
  return XMWidget::AddCallback(XmNactivateCallback, cb, cd);
}
