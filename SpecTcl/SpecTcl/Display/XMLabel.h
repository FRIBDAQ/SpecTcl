/*
** Facility:
**   Motif C++ support.  
** Abstract:
**   XMLabel.h   - This file contains a class definition for the XMLabel class.
**                 this class is derived from the XMManagedWidget class.
**                 It implements an object oriented encapsulation of the Motif
**                 Label widget.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**   @(#)XMLabel.h	8.1 6/23/95 
*/

#ifndef _XMLABEL_H
#define _XMLABEL_H
#include <Xm/Label.h>
#include "XMWidget.h"

class XMLabel :public  XMManagedWidget
   {
   public:
     XMLabel(char *n, XMWidget &parent, char *text,
	     ArgList args = NULL, Cardinal arg_count = 0) :
	       XMManagedWidget(n, xmLabelWidgetClass, parent,
			       args, arg_count) {
		 SetLabel(text);
	       }
     XMLabel(char *n, Widget parent, char *text,
	     ArgList args = NULL, Cardinal arg_count = 0) :
	       XMManagedWidget(n, xmLabelWidgetClass, parent,
			       args, arg_count) {
		 SetLabel(text);
	       }
     XMLabel(char *n) : XMManagedWidget(n) 
       { }
     XMLabel(Widget w) : XMManagedWidget(w)
       { }
     void SetLabel(char *text) {
       XmString label = XmStringCreateSimple(text);
       SetAttribute(XmNlabelString, label);
       XmStringFree(label);
     }
     void SetLabel(XmString lbl) { /* Set from compound string. */
       SetAttribute(XmNlabelString, lbl);
     }
   };
#endif
