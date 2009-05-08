
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

#ifndef XMSEPARATORS_H
#define XMSEPARATORS_H
#include <Xm/Separator.h>

#ifndef XMWIDGET_H
#include "XMWidget.h"
#endif

/*
** The base class XMSeparator is used to derive the classes 
** XMVerticalSeparator and XMHorizontalSeparator
*/
class XMSeparator : public XMWidget {
 public:
  /* Constructors */
  XMSeparator(const char *n);
  XMSeparator(const char *n, Widget parent,
	      ArgList l = NULL, Cardinal num_args=0);
  XMSeparator(const char *n, XMWidget &parent,
	      ArgList l = NULL, Cardinal num_args = 0);
  XMSeparator(Widget w);

  /* Manipulators: */

  void SetShadowType(unsigned char newtype);
  void SetOrientation(unsigned char orientation);
};
/*
** The horizontal Separator is just a widget with initial orientation set
** horizontal
*/
class XMHorizontalSeparator : public XMSeparator {
 public:
  /* Constructors: */
  XMHorizontalSeparator(const char *n);
  XMHorizontalSeparator(const char *n, Widget parent,
			ArgList l = NULL, Cardinal num_args = 0);
  XMHorizontalSeparator(const char *n, XMWidget &parent,
			ArgList l = NULL, Cardinal num_args = 0);
};
/*
** The Vertical separator is just a widget with an initial orientation set
**  vertical
*/
class XMVerticalSeparator : public XMSeparator {
 public:
  /* Constructors: */
  XMVerticalSeparator(const char *n);
  XMVerticalSeparator(const char *n, Widget parent,
		      ArgList l = NULL, Cardinal num_args = 0);
  XMVerticalSeparator(const char *n, XMWidget &parent,
		      ArgList l = NULL, Cardinal num_args = 0);
};

#endif
