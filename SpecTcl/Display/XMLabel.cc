/*
  The following implements functions declared in the file XMLabel.h. These 
  functions used to be inlined, but that caused problems on Tru64.

  Author:
    Jason Venema
    NSCL
    Michigan State University
    East Lansing, MI 48824
    mailto:venema@nscl.msu.edu
*/

#include "XMLabel.h"

XMLabel::XMLabel(char *n, XMWidget &parent, char *text,
		 ArgList args = NULL, Cardinal arg_count = 0) :
  XMManagedWidget(n, xmLabelWidgetClass, parent, args, arg_count) 
{
  SetLabel(text);
}

XMLabel::XMLabel(char *n, Widget parent, char *text,
		 ArgList args = NULL, Cardinal arg_count = 0) :
  XMManagedWidget(n, xmLabelWidgetClass, parent, args, arg_count)
{
  SetLabel(text);
}

XMLabel::XMLabel(char *n) : 
  XMManagedWidget(n) 
{ }

XMLabel::XMLabel(Widget w) : 
  XMManagedWidget(w)
{ }

void
XMLabel::SetLabel(char *text)
{
  XmString label = XmStringCreateSimple(text);
  SetAttribute(XmNlabelString, label);
  XmStringFree(label);
}

void
XMLabel::SetLabel(XmString lbl) 
{ /* Set from compound string. */
  SetAttribute(XmNlabelString, lbl);
}
