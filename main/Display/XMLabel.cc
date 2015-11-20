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

#include <config.h>
#include "XMLabel.h"

XMLabel::XMLabel(const char* n, XMWidget &parent, const char* text,
		 ArgList args, Cardinal arg_count) :
  XMManagedWidget(n, xmLabelWidgetClass, parent, args, arg_count) 
{
  SetLabel(text);
}

XMLabel::XMLabel(const char* n, Widget parent, const char* text,
		 ArgList args, Cardinal arg_count) :
  XMManagedWidget(n, xmLabelWidgetClass, parent, args, arg_count)
{
  SetLabel(text);
}

XMLabel::XMLabel(const char* n) : 
  XMManagedWidget(n) 
{ }

XMLabel::XMLabel(Widget w) : 
  XMManagedWidget(w)
{ }

void
XMLabel::SetLabel(const char* text)
{
  XmString label = XmStringCreateSimple(const_cast<char*>(text));
  SetAttribute(XmNlabelString, label);
  XmStringFree(label);
}

void
XMLabel::SetLabel(XmString lbl) 
{ /* Set from compound string. */
  SetAttribute(XmNlabelString, lbl);
}
