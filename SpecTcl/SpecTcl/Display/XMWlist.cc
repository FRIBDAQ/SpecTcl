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
static char *sccsinfo="@(#)XMWlist.cc	8.1 6/23/95 ";


/*
** Include files required:
*/
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
void XMWidgetList::SetAttribute(String attribute, XtArgVal value)
{
  XMWidget *current;
  InitIteration();
  while(Exists()) {
    current = (XMWidget *)this->Next();
    current->SetAttribute(attribute, value);
  }

}
void XMWidgetList::SetAttribute(String attribute, void *value)
{
  XMWidget *current;
  
  InitIteration();
  while(Exists()) {
    current = (XMWidget *)this->Next();
    current->SetAttribute(attribute, value);
  }

}
