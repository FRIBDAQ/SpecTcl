/*
** Facility:
**   C++ class support for motif widgets.
** Abstract:
**   This file defines the separator classes.  We define vertical and
**   horizontal separators for convenience.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** History:
**   @(#)XMSeparators.h	8.1 6/23/95 
*/

#ifndef _XMSEPARATORS_H
#define _XMSEPARATORS_H
#include <Xm/Separator.h>
#include "XMWidget.h"

/*
** The base class XMSeparator is used to derive the classes 
** XMVerticalSeparator and XMHorizontalSeparator
*/
class XMSeparator : public XMWidget {
 public:
  /* Constructors */
  XMSeparator(char *n) : XMWidget(n) {}
  XMSeparator(char *n, Widget parent,
	      ArgList l = NULL, Cardinal num_args=0) :
		XMWidget(n, xmSeparatorWidgetClass,
				parent, l, num_args) {}
  XMSeparator(char *n, XMWidget &parent,
	      ArgList l = NULL, Cardinal num_args = 0) :
		XMWidget(n, xmSeparatorWidgetClass,
				parent, l, num_args) {}
  XMSeparator(Widget w) : XMWidget(w) {}

  /* Manipulators: */

  void SetShadowType(unsigned char newtype) {
    SetAttribute(XmNseparatorType, newtype);
  }
  void SetOrientation(unsigned char orientation) {
    SetAttribute(XmNorientation, orientation);
  }

};
/*
** The horizontal Separator is just a widget with initial orientation set
** horizontal
*/
class XMHorizontalSeparator : public XMSeparator {
 public:
  /* Constructors: */
  XMHorizontalSeparator(char *n) : XMSeparator(n) { Manage(); }
  XMHorizontalSeparator(char *n, Widget parent,
			ArgList l = NULL, Cardinal num_args = 0) :
			  XMSeparator(n, parent, l, num_args) {
			    SetOrientation(XmHORIZONTAL);
			    Manage(); }
  XMHorizontalSeparator(char *n, XMWidget &parent,
			ArgList l = NULL, Cardinal num_args = 0) :
			  XMSeparator(n, parent, l, num_args) {
			    SetOrientation(XmHORIZONTAL);
			    Manage();
			  }
};
/*
** The Vertical separator is just a widget with an initial orientation set
**  vertical
*/
class XMVerticalSeparator : public XMSeparator {
 public:
  /* Constructors: */
  XMVerticalSeparator(char *n) : XMSeparator(n) { Manage(); }
  XMVerticalSeparator(char *n, Widget parent,
			ArgList l = NULL, Cardinal num_args = 0) :
			  XMSeparator(n, parent, l, num_args) {
			    SetOrientation(XmVERTICAL);
			    Manage(); }
  XMVerticalSeparator(char *n, XMWidget &parent,
			ArgList l = NULL, Cardinal num_args = 0) :
			  XMSeparator(n, parent, l, num_args) {
			    SetOrientation(XmVERTICAL);
			    Manage();
			  }
};

#endif
