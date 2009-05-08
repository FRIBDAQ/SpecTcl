/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xm C++ Support.
** Abstract:
**   XMWlist.cc This file implements the out of line methods defined for
**   the various widget list classes supported by XMWlist
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
*/

/*
** Include files required:
*/
#include <config.h>
#include <stdio.h>
#include "XMWlist.h"

/*
** Method description:
**    XMWidgetList::SetAttribute
**       Set an attribute of all of the widgets in the list to the
**       same value.
** Formal Parameters:
**    String attribute:
**      The name of the attribute (e.g. XmNwidth).
**    XtArgVal value:
**    void     *value:
**      The value to which the attribute is to be set.
*/
void XMWidgetList::SetAttribute(const char* attribute, XtArgVal value)
{
  XMWidget *current;
  InitIteration();
  while(Exists()) {
    current = (XMWidget *)this->Next();
    current->SetAttribute(attribute, value);
  }

}
void XMWidgetList::SetAttribute(const char*  attribute, void *value)
{
  XMWidget *current;
  
  InitIteration();
  while(Exists()) {
    current = (XMWidget *)this->Next();
    current->SetAttribute(attribute, value);
  }

}

/*
** These functions are implemented here because inlining them caused linking
** problems on Tru64. These functions are for class XMWidgetList
*/

XMWidgetList::XMWidgetList(int num) : 
  Generic_List<XMWidget>(num) 
{}

/*
** These functions are for class XMButtonList
*/

void
XMButtonList::Enable() {
  SetAttribute(XmNsensitive, (XtArgVal)True);
}

void
XMButtonList::Disable() {
  SetAttribute(XmNsensitive, (XtArgVal)False);
}
