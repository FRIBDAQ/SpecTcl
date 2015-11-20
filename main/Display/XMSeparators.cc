
/*
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
**   C++ class support for motif widgets.
** Abstract:
**   This file implements the separator classes.  We define vertical and
**   horizontal separators for convenience.
** Author:
**   Jason Venema
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** History:
**   @(#)XMSeparators.cc
*/
#include <config.h>
#include "XMSeparators.h"

XMSeparator::XMSeparator(const char *n) : XMWidget(n) {}

XMSeparator::XMSeparator(const char *n, Widget parent,
			 ArgList l, Cardinal num_args) :
  XMWidget(n, xmSeparatorWidgetClass, parent, l, num_args) {}

XMSeparator::XMSeparator(const char *n, XMWidget &parent,
			 ArgList l, Cardinal num_args) :
  XMWidget(n, xmSeparatorWidgetClass,
	   parent, l, num_args) {}

XMSeparator::XMSeparator(Widget w) : XMWidget(w) {}

void
XMSeparator::SetShadowType(unsigned char newtype) {
  SetAttribute(XmNseparatorType, newtype);
}

void
XMSeparator::SetOrientation(unsigned char orientation) {
  SetAttribute(XmNorientation, orientation);
}

/* 
** These functions are for class XMHorizontalSeparator
*/

XMHorizontalSeparator::XMHorizontalSeparator(const char *n) : 
  XMSeparator(n) { Manage(); }

XMHorizontalSeparator::XMHorizontalSeparator(const char *n, Widget parent,
					     ArgList l, 
					     Cardinal num_args) :
  XMSeparator(n, parent, l, num_args) 
{
  SetOrientation(XmHORIZONTAL);
  Manage(); 
}

XMHorizontalSeparator::XMHorizontalSeparator(const char *n, XMWidget &parent,
					     ArgList l , 
					     Cardinal num_args) :
  XMSeparator(n, parent, l, num_args) 
{
  SetOrientation(XmHORIZONTAL);
  Manage();
}


/* 
** These functions are for class XMVerticalSeparator
*/

XMVerticalSeparator::XMVerticalSeparator(const char *n) : 
  XMSeparator(n) 
{ Manage(); }

XMVerticalSeparator::XMVerticalSeparator(const char *n, Widget parent,
					 ArgList l, 
					 Cardinal num_args ) :
  XMSeparator(n, parent, l, num_args) 
{
  SetOrientation(XmVERTICAL);
  Manage();
}

XMVerticalSeparator::XMVerticalSeparator(const char *n, XMWidget &parent,
					 ArgList l, 
					 Cardinal num_args) :
  XMSeparator(n, parent, l, num_args) 
{
  SetOrientation(XmVERTICAL);
  Manage();
}
